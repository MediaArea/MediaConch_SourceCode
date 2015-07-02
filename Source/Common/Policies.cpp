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
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------


//***************************************************************************
// Assert
//***************************************************************************

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Assert::Assert(const Assert& r)
{
    if (&r == this) {
        return;
    }

    this->description = r.description;
    this->use_free_text = r.use_free_text;
    this->type = r.type;
    this->field = r.field;
    this->validator = r.validator;
    this->value = r.value;
    this->text = r.text;
}

//***************************************************************************
// Policies
//***************************************************************************

//---------------------------------------------------------------------------

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Policies::Policies()
{
    create_values_from_csv();
}

Policies::~Policies()
{
    vector<pair<string, vector<Assert *> > >::iterator it = pattern.begin();
    vector<pair<string, vector<Assert *> > >::iterator ite = pattern.end();

    for (; it != ite; ++it) {
        for (size_t i = 0; i < it->second.size(); ++i)
        {
            delete it->second[i];
        }
    }
}

String Policies::import_schematron(const char* filename)
{
    if (!filename || !strlen(filename)) {
        return String(__T("The schematron file does not exist"));
    }

    xmlDocPtr doc = xmlParseFile(filename);

    if (!doc) {
        return String(__T("The schematron file cannot be parsed"));
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (!root) {
        return String(__T("No root node, leaving"));
    }

    pattern.clear();
    xmlNodePtr child = root->children;
    while (child) {
        find_pattern_node(child);
        child = child->next;
    }
    xmlFreeDoc(doc);
    return String();
}

string     Policies::serialize_assert_for_test(Assert *r)
{
    if (!r)
    {
        return string();
    }
    if (r->use_free_text)
    {
        return r->text;
    }
    stringstream ret;

    ret << "track[@type='" << r->type << "']";

    if (!r->field.length())
    {
        goto end;
    }
    ret << "/" << r->field;

    if (!r->validator.length())
    {
        goto end;
    }
    ret << " " << r->validator << " " << r->value;

end:
    return ret.str();
}

xmlNodePtr Policies::write_pattern(string name, vector<Assert *>& r)
{
    xmlNodePtr pattern = xmlNewNode(NULL, (xmlChar *)"pattern");
    xmlNewProp(pattern, (const xmlChar *)"name", (const xmlChar *)name.c_str());
    xmlNewNs(pattern, NULL, (const xmlChar *)"sch");
    xmlNsPtr defNs = xmlNewNs(NULL, (const xmlChar*)"http://www.ascc.net/xml/schematron",
                           (const xmlChar *)"sch");
    pattern->ns = defNs;

    for (size_t i = 0; i < r.size(); ++i)
    {
        xmlNodePtr node = write_rule(r[i]);
        xmlAddChild(pattern, node);
    }
    return pattern;
}

xmlNodePtr Policies::write_rule(Assert *r)
{
    xmlNodePtr rule = xmlNewNode(NULL, (xmlChar *)"rule");
    xmlNewProp(rule, (const xmlChar *)"context", (const xmlChar *)"/Mediainfo/File");
    xmlNewNs(rule, NULL, (const xmlChar *)"sch");
    xmlNsPtr defNs = xmlNewNs(NULL, (const xmlChar*)"http://www.ascc.net/xml/schematron",
                           (const xmlChar *)"sch");
    rule->ns = defNs;

    xmlNodePtr node = write_assert(r);
    xmlAddChild(rule, node);
    return rule;
}

xmlNodePtr Policies::write_assert(Assert *r)
{
    xmlNodePtr assert = xmlNewNode(NULL, (xmlChar *)"assert");
    xmlNewProp(assert, (xmlChar *)"test", (xmlChar *)serialize_assert_for_test(r).c_str());
    xmlNewNs(assert, NULL, (const xmlChar *)"sch");
    xmlNsPtr defNs = xmlNewNs(NULL, (const xmlChar*)"http://www.ascc.net/xml/schematron",
                           (const xmlChar *)"sch");
    assert->ns = defNs;

    xmlNodeSetContent(assert, (xmlChar *)r->description.c_str());
    return assert;
}

xmlDocPtr Policies::create_doc()
{
    xmlDocPtr doc = xmlNewDoc((xmlChar *)"1.0");
    xmlNodePtr root_node = xmlNewNode(NULL, (xmlChar *)"schema");
    xmlNsPtr ns = xmlNewNs(root_node, (const xmlChar*)"http://www.ascc.net/xml/schematron",
                           (const xmlChar *)"sch");

    root_node->ns = ns;
    xmlDocSetRootElement(doc, root_node);

    vector<pair<string, vector<Assert *> > >::iterator it = pattern.begin();
    vector<pair<string, vector<Assert *> > >::iterator ite = pattern.end();

    for (; it != ite; ++it)
    {
        xmlNodePtr node = write_pattern(it->first, it->second);
        xmlAddChild(root_node, node);
    }
    return doc;
}

void Policies::export_schematron(const char* filename)
{
    xmlDocPtr new_doc = create_doc();

    xmlSaveFormatFile(filename, new_doc, 2);
    xmlFreeDoc(new_doc);
}

void Policies::add_new_rule(string& name, Assert& rule)
{
    Assert *r = new Assert(rule);

    vector<pair<string, vector<Assert *> > >::iterator it = pattern.begin();
    vector<pair<string, vector<Assert *> > >::iterator ite = pattern.end();

    for (; it != ite; ++it)
    {
        if (it->first == name)
        {
            it->second.push_back(r);
            return;
        }
    }

    vector<Assert *> v;
    v.push_back(r);
    pattern.push_back(make_pair(name, v));
}

void Policies::erase_policy(int index)
{
    if (index < 0 || (unsigned int)index >= pattern.size())
    {
        return;
    }

    for (size_t i = 0; i < pattern[index].second.size(); ++i)
    {
        delete pattern[index].second[i];
    }
    pattern.erase(pattern.begin() + index);
}

void Policies::find_pattern_node(xmlNodePtr node)
{
    string def("pattern");
    if (!node || node->type != XML_ELEMENT_NODE ||
        !node->name || def.compare((const char*)node->name))
    {
        return;
    }

    xmlNodePtr next = node->children;
    while (next) {
        string name = (const char*)xmlGetNoNsProp(node, (const unsigned char*)"name");
        find_rule_node(next, name);
        next = next->next;
    }
}

void Policies::find_rule_node(xmlNodePtr node, string pattern_name)
{
    string def("rule");
    if (!node || node->type != XML_ELEMENT_NODE ||
        !node->name || def.compare((const char*)node->name))
    {
        return;
    }

    xmlNodePtr next = node->children;
    while (next) {
        find_assert_node(next, pattern_name);
        //find_report_node(next);
        next = next->next;
    }
}

void Policies::find_assert_node(xmlNodePtr node, string pattern_name)
{
    string def("assert");
    if (!node->name || def.compare((const char*)node->name))
    {
        return;
    }
    string description((const char*)xmlNodeGetContent(node));
    string test((const char*)xmlGetNoNsProp(node, (const unsigned char*)"test"));
    Assert r = create_rule_from_data(description, test);
    add_new_rule(pattern_name, r);
}

Assert Policies::create_rule_from_data(string descr, string data)
{
    Assert r;

    r.description = descr;
    if (!try_parsing_test(data, &r))
    {
        r.use_free_text = true;
        r.text = data;
    }
    return r;
}

bool Policies::check_test_type(const string& type)
{
    list<string>::iterator it = existing_type.begin();
    list<string>::iterator ite = existing_type.end();

    for (; it != ite; ++it)
    {
        if (!type.compare(*it))
        {
            return true;
        }
    }
    return false;
}

bool Policies::check_test_field(const string& field)
{
    list<string>::iterator it = existing_field.begin();
    list<string>::iterator ite = existing_field.end();

    for (; it != ite; ++it)
    {
        if (!field.compare(*it))
        {
            return true;
        }
    }
    return false;
}

bool Policies::check_test_validator(const string& validator)
{
    list<validatorType>::iterator it = existing_validator.begin();
    list<validatorType>::iterator ite = existing_validator.end();

    for (; it != ite; ++it)
    {
        if (!validator.compare(it->value))
        {
            return true;
        }
    }
    return false;
}

string Policies::parse_test_value(string& sub, const string& before, const string& after)
{
    if (sub.compare(0, before.length(), before))
    {
        return string();
    }
    sub = sub.substr(before.length());
    string::size_type pos = sub.find(after);
    if (pos == string::npos)
    {
        return string();
    }

    string ret = sub.substr(0, pos);
    sub = sub.substr(pos + after.length());

    return ret;
}

string Policies::parse_test_field(string& sub, const string& before)
{
    if (sub.compare(0, before.length(), before))
    {
        return string();
    }
    sub = sub.substr(before.length());
    if (!sub.length())
    {
        return string();
    }
    string::size_type pos = sub.find(" ");
    string ret = sub.substr(0, pos);
    if (pos == string::npos)
    {
        sub = sub.substr(sub.length());
        return ret;
    }
    sub = sub.substr(pos + 1);

    return ret;
}

bool Policies::try_parsing_test(string data, Assert *r)
{
    string sub = data;

    //Type
    string type = parse_test_value(sub, string("track[@type='"), string("']"));
    if (!check_test_type(type))
    {
        return false;
    }
    if (!sub.length())
    {
        r->use_free_text = false;
        r->type = type;
        return true;
    }

    //Field
    string field = parse_test_field(sub, string("/"));
    if (!check_test_field(field))
    {
        return false;
    }
    if (!sub.length())
    {
        r->use_free_text = false;
        r->type = type;
        r->field = field;
        return true;
    }

    //Validator
    string validator = parse_test_value(sub, string(""), string(" "));
    if (!check_test_validator(validator))
    {
        return false;
    }
    if (!sub.length())
    {
        return false;
    }
    r->use_free_text = false;
    r->type = type;
    r->field = field;
    r->validator = validator;

    //Value
    r->value = sub;

    return true;
}

void Policies::dump_pattern_to_stdout()
{
    vector<pair<string, vector<Assert *> > >::iterator it = pattern.begin();
    vector<pair<string, vector<Assert *> > >::iterator ite = pattern.end();

    for (size_t num = 0; it != ite; ++it, ++num) {
        cout << "#" << num;
        cout << " ==== Name:" << it->first << endl;
        for (size_t i = 0; i < it->second.size(); ++i)
        {
            Assert *r = it->second[i];

                if (r->use_free_text) {
                    cout << "Assert test:" << r->text;
                    cout << " ==== Desc:" << r->description;
                    cout << endl;
                } else {
                    //TODO when parsed
                    cout << "Type:" << r->type;
                    cout << " ==== Field:" << r->field;
                    cout << " ==== Validator:" << r->validator;
                    cout << " ==== Value:" << r->value;
                    cout << " ==== Desc:" << r->description;
                    cout << endl;
                }
        }
    }
}

void Policies::create_values_from_csv()
{
    string types[] = {
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

    string fields[] = {
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
    {
        existing_validator.push_back(validators[i]);
    }
}
