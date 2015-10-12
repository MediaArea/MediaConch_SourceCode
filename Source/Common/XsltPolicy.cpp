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
#include "XsltPolicy.h"
#include "Xslt.h"
#include <iostream>
#include <sstream>
#include <string.h>

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// XsltRule
//***************************************************************************

//---------------------------------------------------------------------------
XsltRule::XsltRule(const XsltRule& r)
{
    if (&r == this)
        return;

    this->title = r.title;
    this->use_free_text = r.use_free_text;
    this->type = r.type;
    this->field = r.field;
    this->occurence = r.occurence;
    this->ope = r.ope;
    this->value = r.value;
    this->text = r.text;
}

//***************************************************************************
// XsltPolicy
//***************************************************************************

//---------------------------------------------------------------------------
XsltPolicy::XsltPolicy(const XsltPolicy* s) : Policy(s)
{
    type = Policies::POLICY_XSLT;
    for (size_t i = 0; i < s->rules.size(); ++i)
    {
        XsltRule *rule = new XsltRule(*s->rules[i]);
        this->rules.push_back(rule);
    }
}

//---------------------------------------------------------------------------
XsltPolicy::~XsltPolicy()
{
    for (size_t i = 0; i < rules.size(); ++i)
        delete rules[i];
}

//---------------------------------------------------------------------------
xmlNodePtr XsltPolicy::write_title(std::string& title)
{
    xmlNodePtr nodeTitle = xmlNewNode(NULL, (xmlChar *)"title");
    xmlNewNs(nodeTitle, NULL, (const xmlChar *)"xsl");
    xmlNsPtr defNs = xmlNewNs(NULL, (const xmlChar*)"http://www.w3.org/1999/XSL/Transform",
                           (const xmlChar *)"xsl");
    nodeTitle->ns = defNs;

    xmlNodeSetContent(nodeTitle, (xmlChar *)title.c_str());
    return nodeTitle;
}

//---------------------------------------------------------------------------
xmlNodePtr XsltPolicy::write_rule(XsltRule *r)
{
    xmlNodePtr rule = xmlNewNode(NULL, (xmlChar *)"policy");
    /* TODO */
    (void)r;
    return rule;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_title_node(xmlNodePtr node, std::string& title)
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
void XsltPolicy::find_rules_node(xmlNodePtr node, std::vector<XsltRule *>& rules)
{
    std::string def("policy");
    if (!node->name || def.compare((const char*)node->name))
        return;

    /* TODO */
    (void)rules;
    // xmlNodePtr next = node->children;
    // while (next)
    // {
    //     std::string description((const char*)xmlNodeGetContent(node));
    //     xmlChar *test = xmlGetNoNsProp(node, (const unsigned char*)"test");
    //     std::string testStr;
    //     if (test != NULL)
    //         testStr = std::string((const char*)test);
    //     XsltAssert* a = create_assert_from_data(description, testStr.c_str());
    //     asserts.push_back(a);
    //     next = next->next;
    // }
}

//---------------------------------------------------------------------------
String XsltPolicy::import_schema_from_doc(const char* filename, xmlDocPtr doc)
{
    if (!doc)
        return String(__T("The XSL doc is not valid"));

    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (!root)
        return String(__T("No root node, leaving"));

    this->filename = filename;
    xmlNodePtr child = root->children;
    if (child)
    {
        while (child && !find_title_node(child, title))
            child = child->next;
        if (child == NULL)
            child = root->children;
    }
    // if (!title.length())
    // {
    //     title = std::string(filename);
    //     size_t start_index = title.find_last_of("\\/");
    //     if (std::string::npos != start_index)
    //         title = title.substr(start_index + 1);

    //     size_t end_index = title.find(".sch", title.length() - 5);
    //     if (end_index != std::string::npos)
    //         title = title.substr(0, end_index);
    // }
    while (child)
    {
        find_rules_node(child, rules);
        child = child->next;
    }
    xmlSetGenericErrorFunc(NULL, NULL);
    return String();
}

//---------------------------------------------------------------------------
xmlDocPtr XsltPolicy::create_doc()
{
    xmlDocPtr doc = xmlNewDoc((xmlChar *)"1.0");
    xmlNodePtr root_node = xmlNewNode(NULL, (xmlChar *)"stylesheet");
    xmlNsPtr ns = xmlNewNs(root_node, (const xmlChar*)"http://www.w3.org/1999/XSL/Transform",
                           (const xmlChar *)"xsl");

    root_node->ns = ns;
    xmlDocSetRootElement(doc, root_node);
    return doc;
}

}
