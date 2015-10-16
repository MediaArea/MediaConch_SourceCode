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
#include "Policy.h"
#include "SchematronPolicy.h"
#include "Schematron.h"
#include <iostream>
#include <sstream>
#include <string.h>

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// SchematronAssert
//***************************************************************************

//---------------------------------------------------------------------------
SchematronAssert::SchematronAssert(const SchematronAssert& r)
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
// SchematronRule
//***************************************************************************

//---------------------------------------------------------------------------
SchematronRule::SchematronRule(const SchematronRule& r)
{
    if (&r == this) {
        return;
    }

    for (size_t i = 0; i < r.asserts.size(); ++i)
    {
        SchematronAssert *a = new SchematronAssert(*r.asserts[i]);
        this->asserts.push_back(a);
    }
}

//---------------------------------------------------------------------------
SchematronRule::~SchematronRule()
{
    for (size_t i = 0; i < asserts.size(); ++i)
    {
        delete asserts[i];
    }
}

//***************************************************************************
// SchematronPattern
//***************************************************************************

//---------------------------------------------------------------------------
SchematronPattern::SchematronPattern(const SchematronPattern& p)
{
    if (&p == this) {
        return;
    }

    this->name = p.name;
    for (size_t i = 0; i < p.rules.size(); ++i)
    {
        SchematronRule *r = new SchematronRule(*p.rules[i]);
        this->rules.push_back(r);
    }
}

//---------------------------------------------------------------------------
SchematronPattern::~SchematronPattern()
{
    for (size_t i = 0; i < rules.size(); ++i)
    {
        delete rules[i];
    }
}

//---------------------------------------------------------------------------
SchematronPolicy::SchematronPolicy(const SchematronPolicy* s) : Policy(s)
{
    type = Policies::POLICY_SCHEMATRON;
    for (size_t i = 0; i < s->patterns.size(); ++i)
    {
        SchematronPattern *pattern = new SchematronPattern(*s->patterns[i]);
        this->patterns.push_back(pattern);
    }
}

//---------------------------------------------------------------------------
SchematronPolicy::~SchematronPolicy()
{
    for (size_t i = 0; i < patterns.size(); ++i)
        delete patterns[i];
}

//---------------------------------------------------------------------------
xmlNodePtr SchematronPolicy::write_ns()
{
    xmlNodePtr nodeNs = xmlNewNode(NULL, (const xmlChar *)"ns");
    xmlNewProp(nodeNs, (const xmlChar *)"prefix", (const xmlChar *)"ma");
    xmlNewProp(nodeNs, (const xmlChar *)"uri", (const xmlChar *)"https://mediaarea.net/mediaarea");
    xmlNewNs(nodeNs, NULL, (const xmlChar *)"sch");
    xmlNsPtr defNs = xmlNewNs(NULL, (const xmlChar*)"http://www.ascc.net/xml/schematron",
                           (const xmlChar *)"sch");
    nodeNs->ns = defNs;

    return nodeNs;
}

//---------------------------------------------------------------------------
xmlNodePtr SchematronPolicy::write_title(std::string& title)
{
    xmlNodePtr nodeTitle = xmlNewNode(NULL, (const xmlChar *)"title");
    xmlNewNs(nodeTitle, NULL, (const xmlChar *)"sch");
    xmlNsPtr defNs = xmlNewNs(NULL, (const xmlChar*)"http://www.ascc.net/xml/schematron",
                           (const xmlChar *)"sch");
    nodeTitle->ns = defNs;

    xmlNodeSetContent(nodeTitle, (const xmlChar *)title.c_str());
    return nodeTitle;
}

//---------------------------------------------------------------------------
xmlNodePtr SchematronPolicy::write_pattern(SchematronPattern *p)
{
    xmlNodePtr pattern = xmlNewNode(NULL, (const xmlChar *)"pattern");
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

//---------------------------------------------------------------------------
xmlNodePtr SchematronPolicy::write_rule(SchematronRule *r)
{
    xmlNodePtr rule = xmlNewNode(NULL, (const xmlChar *)"rule");
    xmlNewProp(rule, (const xmlChar *)"context", (const xmlChar *)"/MediaArea/media/MediaInfo");
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

//---------------------------------------------------------------------------
xmlNodePtr SchematronPolicy::write_assert(SchematronAssert *a)
{
    xmlNodePtr assert = xmlNewNode(NULL, (const xmlChar *)"assert");
    xmlNewProp(assert, (const xmlChar *)"test", (const xmlChar *)Policies::serialize_assert_for_test(a).c_str());
    xmlNewNs(assert, NULL, (const xmlChar *)"sch");
    xmlNsPtr defNs = xmlNewNs(NULL, (const xmlChar*)"http://www.ascc.net/xml/schematron",
                           (const xmlChar *)"sch");
    assert->ns = defNs;

    xmlNodeSetContent(assert, (const xmlChar *)a->description.c_str());
    return assert;
}

//---------------------------------------------------------------------------
bool SchematronPolicy::find_schematron_header(xmlNodePtr node)
{
    if (!node || node->type != XML_ELEMENT_NODE || !node->ns)
        return false;

    std::string schPrefix("sch");
    if (schPrefix.compare((const char*)node->ns->prefix))
        return false;

    std::string schOldUrl("http://www.ascc.net/xml/schematron");
    std::string schNewUrl("http://purl.oclc.org/dsdl/schematron");
    if (schOldUrl.compare((const char*)node->ns->href) && schNewUrl.compare((const char*)node->ns->href))
        return false;

    return true;
}

//---------------------------------------------------------------------------
bool SchematronPolicy::find_title_node(xmlNodePtr node, std::string& title)
{
    std::string def("title");
    if (!node || node->type != XML_ELEMENT_NODE ||
        !node->name || def.compare((const char*)node->name))
        return false;

    const char* descr = (const char*)xmlNodeGetContent(node);
    if (descr)
        title = std::string(descr);
    return true;
}

//---------------------------------------------------------------------------
void SchematronPolicy::find_patterns_node(xmlNodePtr node, std::vector<SchematronPattern *>& patterns)
{
    std::string def("pattern");
    if (!node || node->type != XML_ELEMENT_NODE ||
        !node->name || def.compare((const char*)node->name))
        return;

    SchematronPattern* p = new SchematronPattern;
    xmlChar *name = xmlGetNoNsProp(node, (const unsigned char*)"name");
    if (name != NULL)
        p->name = std::string((const  char *)name);

    xmlNodePtr next = node->children;
    while (next)
    {
        find_rules_node(next, p->rules);
        next = next->next;
    }
    patterns.push_back(p);
}

//---------------------------------------------------------------------------
void SchematronPolicy::find_rules_node(xmlNodePtr node, std::vector<SchematronRule *>& rules)
{
    std::string def("rule");
    if (!node || node->type != XML_ELEMENT_NODE ||
        !node->name || def.compare((const char*)node->name))
        return;

    SchematronRule *r = new SchematronRule;
    xmlNodePtr next = node->children;
    while (next)
    {
        find_asserts_node(next, r->asserts);
        //find_report_node(next);
        next = next->next;
    }
    rules.push_back(r);
}

//---------------------------------------------------------------------------
void SchematronPolicy::find_asserts_node(xmlNodePtr node, std::vector<SchematronAssert *>& asserts)
{
    std::string def("assert");
    if (!node->name || def.compare((const char*)node->name))
        return;

    xmlNodePtr next = node->children;
    while (next)
    {
        std::string description((const char*)xmlNodeGetContent(node));
        xmlChar *test = xmlGetNoNsProp(node, (const unsigned char*)"test");
        std::string testStr;
        if (test != NULL)
            testStr = std::string((const char*)test);
        SchematronAssert* a = create_assert_from_data(description, testStr.c_str());
        asserts.push_back(a);
        next = next->next;
    }
}

//---------------------------------------------------------------------------
SchematronAssert* SchematronPolicy::create_assert_from_data(std::string descr, std::string data)
{
    SchematronAssert* a = new SchematronAssert;

    a->description = descr;
    if (!Policies::try_parsing_test(data, a))
    {
        a->use_free_text = true;
        a->text = data;
    }
    return a;
}

//---------------------------------------------------------------------------
String SchematronPolicy::import_schema_from_doc(const std::string& filename, xmlDocPtr doc)
{
    if (!doc)
        return __T("The schematron doc is not valid");

    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (!root)
        return __T("No root node, leaving");

    if (!find_schematron_header(root))
        return __T("Format not detected");

    this->filename = filename;
    xmlNodePtr child = root->children;
    if (child)
    {
        while (child && !find_title_node(child, title))
            child = child->next;
        if (child == NULL)
            child = root->children;
    }
    if (!title.length())
    {
        title = filename;
        size_t start_index = title.find_last_of("\\/");
        if (std::string::npos != start_index)
            title = title.substr(start_index + 1);

        size_t end_index = title.find(".sch", title.length() - 5);
        if (end_index != std::string::npos)
            title = title.substr(0, end_index);
    }
    while (child)
    {
        find_patterns_node(child, patterns);
        child = child->next;
    }
    xmlSetGenericErrorFunc(NULL, NULL);
    return String();
}

//---------------------------------------------------------------------------
xmlDocPtr SchematronPolicy::create_doc()
{
    xmlDocPtr doc = xmlNewDoc((const xmlChar *)"1.0");
    xmlNodePtr root_node = xmlNewNode(NULL, (const xmlChar *)"schema");
    xmlNsPtr ns = xmlNewNs(root_node, (const xmlChar*)"http://www.ascc.net/xml/schematron",
                           (const xmlChar *)"sch");

    root_node->ns = ns;
    xmlDocSetRootElement(doc, root_node);

    std::vector<SchematronPattern *>::iterator it = patterns.begin();
    std::vector<SchematronPattern *>::iterator ite = patterns.end();

    xmlNodePtr nodeTitle = write_title(title);
    xmlAddChild(root_node, nodeTitle);
    xmlNodePtr nodeNs = write_ns();
    xmlAddChild(root_node, nodeNs);
    for (; it != ite; ++it)
    {
        xmlNodePtr node = write_pattern(*it);
        xmlAddChild(root_node, node);
    }
    return doc;
}

}
