/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "Policies.h"
#include <iostream>
#include <sstream>
#include <string.h>
#include "SchematronPolicy.h"
//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// Policies
//***************************************************************************

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

    std::list<std::string> Policies::existing_type      = std::list<std::string>();
    std::list<std::string> Policies::existing_field     = std::list<std::string>();
    std::list<Policies::validatorType> Policies::existing_validator = std::list<Policies::validatorType>();

//---------------------------------------------------------------------------
Policies::Policies()
{
    create_values_from_csv();
}

Policies::~Policies()
{
    policies.clear();
}

String Policies::import_schema(PolicyType type, const char* filename)
{
    if (!filename || !strlen(filename))
        return __T("The schematron file does not exist");

    Policy *p = new SchematronPolicy;
    String ret = p->import_schema(filename);

    if (ret.length())
        policies.push_back(p);
    return ret;
}

String Policies::import_schema_from_memory(PolicyType type, const char* filename, const char* buffer, int len)
{
    if (!buffer || !len)
        return __T("The schematron does not exist");

    Policy *p = new SchematronPolicy;
    String ret = p->import_schema_from_memory(filename, buffer, len);

    if (!ret.length())
        policies.push_back(p);
    return ret;
}

void Policies::export_schema(PolicyType type, const char* filename, size_t pos)
{
    if (pos >= policies.size() || !policies[pos])
        return;

    if (filename == NULL)
    {
        if (pos >= policies.size() || !policies[pos])
            return;

        filename = policies[pos]->filename.c_str();
    }

    policies[pos]->export_schema(filename);
}

void Policies::erase_policy(size_t index)
{
    if (index >= policies.size())
        return;

    if (policies[index])
        delete policies[index];
    policies.erase(policies.begin() + index);
}

void Policies::create_values_from_csv()
{
    std::string types[] = {
        "General",
        "Video",
        "Audio",
        "Text",
        "Image",
        "Other"
    };
    for (size_t i=0; i < (sizeof(types) / sizeof(*types)); i++)
    {
        existing_type.push_back(types[i]);
    }

    std::string fields[] = {
        "",
        "Format",
        "UniqueID",
        "FileExtension",
        "Duration",
        "FrameRate",
        "DisplayAspectRatio",
        "coder_type",
        "Width",
        "Height",
        "ColorSpace",
        "DisplayAspectRatio"
    };
    for (size_t i=0; i < (sizeof(fields) / sizeof(*fields)); i++)
    {
        existing_field.push_back(fields[i]);
    }

    validatorType validators[] = {
        { "", "" },
        { "=", "Equal" },
        { "!=", "Not Equal" },
        { ">", "Greater than" },
        { ">=", "Greater or equal" },
        { "<", "Less" },
        { "<=", "Less or equal" }
    };

    for (size_t i=0; i < (sizeof(validators) / sizeof(*validators)); i++)
        existing_validator.push_back(validators[i]);
}

xmlDocPtr Policies::create_doc(size_t pos)
{
    if (pos >= policies.size() || !policies[pos])
        return NULL;

    return policies[pos]->create_doc();
}

bool Policies::policy_exists(std::string policy)
{
    for (size_t i =0; i < policies.size(); ++i)
        if (policies[i]->title == policy)
            return true;

    return false;
}

bool Policies::check_test_type(const std::string& type)
{
    std::list<std::string>::iterator it = existing_type.begin();
    std::list<std::string>::iterator ite = existing_type.end();

    for (; it != ite; ++it)
        if (!type.compare(*it))
            return true;

    return false;
}

bool Policies::check_test_field(const std::string& field)
{
    std::list<std::string>::iterator it = existing_field.begin();
    std::list<std::string>::iterator ite = existing_field.end();

    for (; it != ite; ++it)
        if (!field.compare(*it))
            return true;

    return false;
}

bool Policies::check_test_validator(const std::string& validator)
{
    std::list<validatorType>::iterator it = existing_validator.begin();
    std::list<validatorType>::iterator ite = existing_validator.end();

    for (; it != ite; ++it)
        if (!validator.compare(it->value))
            return true;

    return false;
}

std::string Policies::parse_test_value(std::string& sub, const std::string& before, const std::string& after)
{
    if (sub.compare(0, before.length(), before))
        return std::string();

    sub = sub.substr(before.length());
    std::string::size_type pos = sub.find(after);
    if (pos == std::string::npos)
        return std::string();

    std::string ret = sub.substr(0, pos);
    sub = sub.substr(pos + after.length());

    return ret;
}

std::string Policies::parse_test_field(std::string& sub, const std::string& before)
{
    if (sub.compare(0, before.length(), before))
        return std::string();

    sub = sub.substr(before.length());
    if (!sub.length())
        return std::string();

    std::string::size_type pos = sub.find(" ");
    std::string ret = sub.substr(0, pos);
    if (pos == std::string::npos)
    {
        sub = sub.substr(sub.length());
        return ret;
    }
    sub = sub.substr(pos + 1);

    return ret;
}

bool Policies::try_parsing_test(std::string data, SchematronAssert *r)
{
    std::string sub = data;

    //Type
    std::string type = parse_test_value(sub, std::string("track[@type='"), std::string("']"));
    if (!check_test_type(type))
        return false;

    if (!sub.length())
    {
        r->use_free_text = false;
        r->type = type;
        return true;
    }

    //Field
    std::string field = parse_test_field(sub, std::string("/"));
    if (!check_test_field(field))
        return false;

    if (!sub.length())
    {
        r->use_free_text = false;
        r->type = type;
        r->field = field;
        return true;
    }

    //Validator
    std::string validator = parse_test_value(sub, std::string(""), std::string(" "));
    if (!check_test_validator(validator) || !sub.length())
        return false;

    r->use_free_text = false;
    r->type = type;
    r->field = field;
    r->validator = validator;

    //Value
    r->value = sub;

    return true;
}

std::string Policies::serialize_assert_for_test(SchematronAssert *r)
{
    if (!r)
        return std::string();

    if (r->use_free_text)
        return r->text;

    std::stringstream ret;

    ret << "track[@type='" << r->type << "']";

    if (!r->field.length())
        goto end;
    ret << "/" << r->field;

    if (!r->validator.length())
        goto end;

    ret << " " << r->validator << " " << r->value;

end:
    return ret.str();
}

}
