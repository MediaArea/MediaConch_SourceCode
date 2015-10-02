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
#include "Schematron.h"
#include <iostream>
#include <sstream>
#include <string.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// Assert
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
// Rule
//***************************************************************************

//---------------------------------------------------------------------------
Rule::Rule(const Rule& r)
{
    if (&r == this) {
        return;
    }

    for (size_t i = 0; i < r.asserts.size(); ++i)
    {
        Assert *a = new Assert(*r.asserts[i]);
        this->asserts.push_back(a);
    }
}

//---------------------------------------------------------------------------
Rule::~Rule()
{
    for (size_t i = 0; i < asserts.size(); ++i)
    {
        delete asserts[i];
    }
}

//***************************************************************************
// Pattern
//***************************************************************************

//---------------------------------------------------------------------------
Pattern::Pattern(const Pattern& p)
{
    if (&p == this) {
        return;
    }

    this->name = p.name;
    for (size_t i = 0; i < p.rules.size(); ++i)
    {
        Rule *r = new Rule(*p.rules[i]);
        this->rules.push_back(r);
    }
}

//---------------------------------------------------------------------------
Pattern::~Pattern()
{
    for (size_t i = 0; i < rules.size(); ++i)
    {
        delete rules[i];
    }
}

//***************************************************************************
// Policy
//***************************************************************************

//---------------------------------------------------------------------------
Policy::Policy(const Policy& p)
{
    if (&p == this) {
        return;
    }

    this->filename = p.filename;
    this->title = p.title;
    for (size_t i = 0; i < p.patterns.size(); ++i)
    {
        Pattern *pattern = new Pattern(*p.patterns[i]);
        this->patterns.push_back(pattern);
    }
}

//---------------------------------------------------------------------------
Policy::~Policy()
{
    for (size_t i = 0; i < patterns.size(); ++i)
    {
        delete patterns[i];
    }
}

//***************************************************************************
// Policies
//***************************************************************************

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
    policies.clear();
}

String Policies::import_schematron(const char* filename)
{
    if (!filename || !strlen(filename))
        return String(__T("The schematron file does not exist"));

    Schematron s;
    xmlSetGenericErrorFunc(&s, &s.manage_generic_error);

    xmlDocPtr doc = xmlParseFile(filename);
    if (!doc)
    {
        // maybe put the errors from s.errors
        return String(__T("The schematron file cannot be parsed"));
    }

    String ret = import_schematron_from_doc(filename, doc);
    xmlFreeDoc(doc);
    return ret;
}

String Policies::import_schematron_from_memory(const char* filename, const char* buffer, int len)
{
    if (!buffer || !len)
        return String(__T("The schematron does not exist"));

    Schematron s;
    xmlSetGenericErrorFunc(&s, &s.manage_generic_error);

    xmlDocPtr doc = xmlParseMemory(buffer, len);
    if (!doc)
    {
        // maybe put the errors from s.errors
        return String(__T("The schematron given cannot be parsed"));
    }

    String ret = import_schematron_from_doc(filename, doc);
    xmlFreeDoc(doc);
    return ret;
}

String Policies::import_schematron_from_doc(const char* filename, xmlDocPtr doc)
{
    if (!doc)
        return String(__T("The schematron doc is not valid"));

    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (!root)
        return String(__T("No root node, leaving"));

    Policy *p = new Policy;
    p->filename = filename;
    xmlNodePtr child = root->children;
    if (child)
    {
        while (child && !find_title_node(child, p->title))
            child = child->next;
        if (child == NULL)
            child = root->children;
    }
    if (!p->title.length())
    {
        p->title = string(filename);
        size_t start_index = p->title.find_last_of("\\/");
        if (std::string::npos != start_index)
            p->title = p->title.substr(start_index + 1);

        size_t end_index = p->title.find(".sch", p->title.length() - 5);
        if (end_index != std::string::npos)
            p->title = p->title.substr(0, end_index);
    }
    while (child)
    {
        find_patterns_node(child, p->patterns);
        child = child->next;
    }
    policies.push_back(p);
    xmlSetGenericErrorFunc(NULL, NULL);
    return String();
}

string     Policies::serialize_assert_for_test(Assert *r)
{
    if (!r)
        return string();

    if (r->use_free_text)
        return r->text;

    stringstream ret;

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

xmlNodePtr Policies::write_title(string& title)
{
    xmlNodePtr nodeTitle = xmlNewNode(NULL, (xmlChar *)"title");
    xmlNewNs(nodeTitle, NULL, (const xmlChar *)"sch");
    xmlNsPtr defNs = xmlNewNs(NULL, (const xmlChar*)"http://www.ascc.net/xml/schematron",
                           (const xmlChar *)"sch");
    nodeTitle->ns = defNs;

    xmlNodeSetContent(nodeTitle, (xmlChar *)title.c_str());
    return nodeTitle;
}

xmlNodePtr Policies::write_pattern(Pattern *p)
{
    xmlNodePtr pattern = xmlNewNode(NULL, (xmlChar *)"pattern");
    xmlNewProp(pattern, (const xmlChar *)"name", (const xmlChar *)p->name.c_str());
    xmlNewNs(pattern, NULL, (const xmlChar *)"sch");
    xmlNsPtr defNs = xmlNewNs(NULL, (const xmlChar*)"http://www.ascc.net/xml/schematron",
                           (const xmlChar *)"sch");
    pattern->ns = defNs;

    for (size_t i = 0; i < p->rules.size(); ++i)
    {
        xmlNodePtr node = write_rule(p->rules[i]);
        xmlAddChild(pattern, node);
    }
    return pattern;
}

xmlNodePtr Policies::write_rule(Rule *r)
{
    xmlNodePtr rule = xmlNewNode(NULL, (xmlChar *)"rule");
    xmlNewProp(rule, (const xmlChar *)"context", (const xmlChar *)"/Mediainfo/File");
    xmlNewNs(rule, NULL, (const xmlChar *)"sch");
    xmlNsPtr defNs = xmlNewNs(NULL, (const xmlChar*)"http://www.ascc.net/xml/schematron",
                           (const xmlChar *)"sch");
    rule->ns = defNs;

    for (size_t i = 0; i < r->asserts.size(); ++i)
    {
        xmlNodePtr node = write_assert(r->asserts[i]);
        xmlAddChild(rule, node);
    }
    return rule;
}

xmlNodePtr Policies::write_assert(Assert *a)
{
    xmlNodePtr assert = xmlNewNode(NULL, (xmlChar *)"assert");
    xmlNewProp(assert, (xmlChar *)"test", (xmlChar *)serialize_assert_for_test(a).c_str());
    xmlNewNs(assert, NULL, (const xmlChar *)"sch");
    xmlNsPtr defNs = xmlNewNs(NULL, (const xmlChar*)"http://www.ascc.net/xml/schematron",
                           (const xmlChar *)"sch");
    assert->ns = defNs;

    xmlNodeSetContent(assert, (xmlChar *)a->description.c_str());
    return assert;
}

xmlDocPtr Policies::create_doc(size_t pos)
{
    xmlDocPtr doc = xmlNewDoc((xmlChar *)"1.0");
    xmlNodePtr root_node = xmlNewNode(NULL, (xmlChar *)"schema");
    xmlNsPtr ns = xmlNewNs(root_node, (const xmlChar*)"http://www.ascc.net/xml/schematron",
                           (const xmlChar *)"sch");

    root_node->ns = ns;
    xmlDocSetRootElement(doc, root_node);

    if (pos >= policies.size() || !policies[pos])
        return NULL;

    vector<Pattern *>::iterator it = policies[pos]->patterns.begin();
    vector<Pattern *>::iterator ite = policies[pos]->patterns.end();

    xmlNodePtr nodeTitle = write_title(policies[pos]->title);
    xmlAddChild(root_node, nodeTitle);
    for (; it != ite; ++it)
    {
        xmlNodePtr node = write_pattern(*it);
        xmlAddChild(root_node, node);
    }
    return doc;
}

void Policies::export_schematron(const char* filename, size_t pos)
{
    xmlDocPtr new_doc = create_doc(pos);

    if (filename == NULL)
    {
        if (pos >= policies.size() || !policies[pos])
            return;

        filename = policies[pos]->filename.c_str();
    }

    xmlSaveFormatFile(filename, new_doc, 2);
    xmlFreeDoc(new_doc);
}

void Policies::erase_policy(size_t index)
{
    if (index >= policies.size())
        return;

    if (policies[index])
        delete policies[index];
    policies.erase(policies.begin() + index);
}

bool Policies::find_title_node(xmlNodePtr node, string& title)
{
    string def("title");
    if (!node || node->type != XML_ELEMENT_NODE ||
        !node->name || def.compare((const char*)node->name))
        return false;

    const char* descr = (const char*)xmlNodeGetContent(node);
    if (descr)
        title = string(descr);
    return true;
}

void Policies::find_patterns_node(xmlNodePtr node, vector<Pattern *>& patterns)
{
    string def("pattern");
    if (!node || node->type != XML_ELEMENT_NODE ||
        !node->name || def.compare((const char*)node->name))
        return;

    Pattern* p = new Pattern;
    xmlChar *name = xmlGetNoNsProp(node, (const unsigned char*)"name");
    if (name != NULL)
        p->name = string((const  char *)name);

    xmlNodePtr next = node->children;
    while (next)
    {
        find_rules_node(next, p->rules);
        next = next->next;
    }
    patterns.push_back(p);
}

void Policies::find_rules_node(xmlNodePtr node, vector<Rule *>& rules)
{
    string def("rule");
    if (!node || node->type != XML_ELEMENT_NODE ||
        !node->name || def.compare((const char*)node->name))
        return;

    Rule *r = new Rule;
    xmlNodePtr next = node->children;
    while (next)
    {
        find_asserts_node(next, r->asserts);
        //find_report_node(next);
        next = next->next;
    }
    rules.push_back(r);
}

void Policies::find_asserts_node(xmlNodePtr node, vector<Assert *>& asserts)
{
    string def("assert");
    if (!node->name || def.compare((const char*)node->name))
        return;

    xmlNodePtr next = node->children;
    while (next)
    {
        string description((const char*)xmlNodeGetContent(node));
        xmlChar *test = xmlGetNoNsProp(node, (const unsigned char*)"test");
        string testStr;
        if (test != NULL)
            testStr = string((const char*)test);
        Assert* a = create_assert_from_data(description, testStr.c_str());
        asserts.push_back(a);
        next = next->next;
    }
}

Assert* Policies::create_assert_from_data(string descr, string data)
{
    Assert* a = new Assert;

    a->description = descr;
    if (!try_parsing_test(data, a))
    {
        a->use_free_text = true;
        a->text = data;
    }
    return a;
}

bool Policies::check_test_type(const string& type)
{
    list<string>::iterator it = existing_type.begin();
    list<string>::iterator ite = existing_type.end();

    for (; it != ite; ++it)
        if (!type.compare(*it))
            return true;

    return false;
}

bool Policies::check_test_field(const string& field)
{
    list<string>::iterator it = existing_field.begin();
    list<string>::iterator ite = existing_field.end();

    for (; it != ite; ++it)
        if (!field.compare(*it))
            return true;

    return false;
}

bool Policies::check_test_validator(const string& validator)
{
    list<validatorType>::iterator it = existing_validator.begin();
    list<validatorType>::iterator ite = existing_validator.end();

    for (; it != ite; ++it)
        if (!validator.compare(it->value))
            return true;

    return false;
}

string Policies::parse_test_value(string& sub, const string& before, const string& after)
{
    if (sub.compare(0, before.length(), before))
        return string();

    sub = sub.substr(before.length());
    string::size_type pos = sub.find(after);
    if (pos == string::npos)
        return string();

    string ret = sub.substr(0, pos);
    sub = sub.substr(pos + after.length());

    return ret;
}

string Policies::parse_test_field(string& sub, const string& before)
{
    if (sub.compare(0, before.length(), before))
        return string();

    sub = sub.substr(before.length());
    if (!sub.length())
        return string();

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
        return false;

    if (!sub.length())
    {
        r->use_free_text = false;
        r->type = type;
        return true;
    }

    //Field
    string field = parse_test_field(sub, string("/"));
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
    string validator = parse_test_value(sub, string(""), string(" "));
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

void Policies::dump_policies_to_stdout()
{
    for (size_t i = 0; i < policies.size(); ++i)
    {
        Policy *policy = policies[i];
        cout << "#" << i;
        cout << " ==== Title:" << policy->title << endl;

        for (size_t j = 0; j < policy->patterns.size(); ++j)
        {
            Pattern *pat = policy->patterns[j];

            cout << "\t==== Pattern name:" << pat->name << endl;
            for (size_t k = 0; k < pat->rules.size(); ++k)
            {
                Rule *r = pat->rules[k];
                for (size_t l = 0; l < r->asserts.size(); ++l)
                {
                    Assert *a = r->asserts[l];

                    if (a->use_free_text) {
                        cout << "\t\tAssert test:" << a->text;
                        cout << "' ==== Desc:" << a->description;
                        cout << endl;
                    } else {
                        cout << "\t\tAssert: Type:" << a->type;
                        cout << " ==== Field:" << a->field;
                        cout << " ==== Validator:" << a->validator;
                        cout << " ==== Value:" << a->value;
                        cout << " ==== Desc:" << a->description;
                        cout << endl;
                    }
                }
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
        existing_validator.push_back(validators[i]);
}

bool Policies::policy_exists(std::string policy)
{
    for (size_t i =0; i < policies.size(); ++i)
        if (policies[i]->title == policy)
            return true;

    return false;
}

}
