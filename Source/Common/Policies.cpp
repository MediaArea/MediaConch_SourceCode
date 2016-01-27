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
#include "Core.h"
#include "Policy.h"
#include <iostream>
#include <sstream>
#include <string.h>
#include "SchematronPolicy.h"
#include "XsltPolicy.h"
#include "UnknownPolicy.h"
#include "ZenLib/Ztring.h"
#include "ZenLib/ZtringList.h"
//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// Policies
//***************************************************************************

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

    std::map<std::string, std::list<std::string> > Policies::existing_type = std::map<std::string, std::list<std::string> >();
    std::list<Policies::validatorType> Policies::existing_validator = std::list<Policies::validatorType>();
    std::list<std::string> Policies::existing_xsltOperator = std::list<std::string>();

//---------------------------------------------------------------------------
Policies::Policies(Core *c) : core(c)
{
}

Policies::~Policies()
{
    policies.clear();
}

int Policies::import_schema(const std::string& filename)
{
    if (!filename.length())
    {
        error = "The policy file does not exist";
        return -1;
    }

    Policy *p = NULL;
    int ret = -1;
    if (!filename.compare(filename.length() - 4, 4, ".sch"))
    {
        p = new SchematronPolicy;
        ret = p->import_schema(filename);
    }

    if (ret < 0)
    {
        if (p)
            delete p;
        p = new XsltPolicy;
        ret = p->import_schema(filename);
        if (ret < 0)
        {
            if (p)
                delete p;
            p = new UnknownPolicy;
            ret = p->import_schema(filename);
        }
    }

    if (ret >= 0)
        policies.push_back(p);
    else
        delete p;
    return ret;
}

int Policies::import_schema_from_memory(const std::string& filename, const char* buffer, int len)
{
    if (!buffer || !len)
    {
        error = "The policy does not exist";
        return -1;
    }

    Policy *p = NULL;
    int ret = -1;
    if (!filename.compare(filename.length() - 4, 4, ".sch"))
    {
        p = new SchematronPolicy;
        ret = p->import_schema_from_memory(filename, buffer, len);
    }

    if (ret < 0)
    {
        if (p)
            delete p;

        p = new XsltPolicy;
        ret = p->import_schema_from_memory(filename, buffer, len);
        if (ret < 0)
        {
            if (p)
                delete p;
            p = new UnknownPolicy;
            ret = p->import_schema_from_memory(filename, buffer, len);
        }
    }

    if (ret >= 0)
        policies.push_back(p);
    else
        delete p;
    return ret;
}

void Policies::export_schema(const char* filename, size_t pos)
{
    if (pos >= policies.size() || !policies[pos])
        return;

    if (filename == NULL)
        filename = policies[pos]->filename.c_str();

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
    ZenLib::ZtringList list;
    list.Separator_Set(0, __T(","));
    list.Write(core->Menu_Option_Preferences_Option(__T("MAXML_StreamKinds"), ZenLib::Ztring()));
    for (size_t i = 0; i < list.size(); ++i)
    {
        std::list<std::string> fields;
        fields.push_back("");

        ZenLib::ZtringList listField;
        listField.Separator_Set(0, __T(","));
        listField.Write(core->Menu_Option_Preferences_Option(__T("MAXML_Fields"), list[i]));
        for (size_t j = 0; j < listField.size(); ++j)
            fields.push_back(listField[j].To_UTF8());

        existing_type[list[i].To_UTF8()] = fields;
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

    std::string xsltOperators[] =
    {
        "is_equal",
        "is_not_equal",
        "is_greater_than",
        "is_less_than",
        "is_greater_or_equal_than",
        "is_less_or_equal_than",
        "exists",
        "does_not_exist",
        "contains_string",
    };

    for (size_t i=0; i < (sizeof(xsltOperators) / sizeof(*xsltOperators)); i++)
        existing_xsltOperator.push_back(xsltOperators[i]);
}

xmlDocPtr Policies::create_doc(size_t pos)
{
    if (pos >= policies.size() || !policies[pos])
        return NULL;

    return policies[pos]->create_doc();
}

bool Policies::policy_exists(const std::string& policy)
{
    for (size_t i =0; i < policies.size(); ++i)
        if (policies[i]->title == policy)
            return true;

    return false;
}

bool Policies::check_test_type(const std::string& type)
{
    std::map<std::string, std::list<std::string> >::iterator it = existing_type.begin();
    std::map<std::string, std::list<std::string> >::iterator ite = existing_type.end();

    for (; it != ite; ++it)
        if (!type.compare(it->first))
            return true;

    return false;
}

bool Policies::check_test_field(const std::string& field)
{
    std::map<std::string, std::list<std::string> >::iterator itType = existing_type.begin();
    std::map<std::string, std::list<std::string> >::iterator iteType = existing_type.end();

    for (; itType != iteType; ++itType)
    {
        std::list<std::string>::iterator it = itType->second.begin();
        std::list<std::string>::iterator ite = itType->second.end();

        for (; it != ite; ++it)
            if (!field.compare(*it))
                return true;
    }

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
