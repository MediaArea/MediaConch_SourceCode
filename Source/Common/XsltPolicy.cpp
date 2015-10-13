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
xmlNsPtr XsltPolicy::create_namespace_xsl(xmlNodePtr node)
{
    return xmlNewNs(node, (const xmlChar*)"http://www.w3.org/1999/XSL/Transform",
                    (const xmlChar *)"xsl");
}

//---------------------------------------------------------------------------
xmlNsPtr XsltPolicy::create_namespace_mc(xmlNodePtr node)
{
    return xmlNewNs(node, (const xmlChar*)"https://mediaarea.net/mediaarea",
                    (const xmlChar *)"mc");
}

//---------------------------------------------------------------------------
xmlNsPtr XsltPolicy::create_namespace_xsi(xmlNodePtr node)
{
    return xmlNewNs(node, (const xmlChar*)"http://www.w3.org/2001/XMLSchema-instance",
                    (const xmlChar *)"xsi");
}

//---------------------------------------------------------------------------
void XsltPolicy::create_xpath_from_rule(XsltRule *rule, std::string& xpath)
{
    std::stringstream ss;

    ss << "mc:MediaInfo/mc:track[@type='";

    if (!rule->type.length())
        return;
    ss << rule->type << "']";

    ss << "[" << rule->occurrence << "]";

    if (!rule->field.length())
    {
        xpath = ss.str();
        return;
    }
    ss << "/mc:" << rule->field;

    xpath = ss.str();
}

//---------------------------------------------------------------------------
void XsltPolicy::write_rule_title_child(xmlNodePtr node, XsltRule *rule)
{
    if (!rule->title.length())
        return;

    xmlNodePtr child = xmlNewNode(NULL, (xmlChar *)"with-param");
    child->ns = node->ns;
    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"title");
    xmlNodeSetContent(child, (xmlChar *)rule->title.c_str());

    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_rule_xpath_child(xmlNodePtr node, XsltRule *rule)
{
    std::string xpath;
    create_xpath_from_rule(rule, xpath);
    if (!xpath.length())
        return;

    xmlNodePtr child = xmlNewNode(NULL, (xmlChar *)"with-param");
    child->ns = node->ns;
    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"xpath");
    xmlNewProp(child, (const xmlChar *)"select", (xmlChar *)xpath.c_str());

    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_rule_value_child(xmlNodePtr node, XsltRule *rule)
{
    if (!rule->value.length())
        return;

    xmlNodePtr child = xmlNewNode(NULL, (xmlChar *)"with-param");
    child->ns = node->ns;
    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"value");
    xmlNodeSetContent(child, (xmlChar *)rule->value.c_str());

    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_rule_type_child(xmlNodePtr node, XsltRule *rule)
{
    if (!rule->type.length())
        return;

    xmlNodePtr child = xmlNewNode(NULL, (xmlChar *)"with-param");
    child->ns = node->ns;
    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"tracktype");
    xmlNodeSetContent(child, (xmlChar *)rule->type.c_str());

    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_rule_occurence_child(xmlNodePtr node, XsltRule *rule)
{
    std::stringstream ss;
    ss << rule->occurence;
    xmlNodePtr child = xmlNewNode(NULL, (xmlChar *)"with-param");
    child->ns = node->ns;
    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"occurence");
    xmlNodeSetContent(child, (xmlChar *)ss.str().c_str());

    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_rule_field_child(xmlNodePtr node, XsltRule *rule)
{
    if (!rule->field.length())
        return;

    xmlNodePtr child = xmlNewNode(NULL, (xmlChar *)"with-param");
    child->ns = node->ns;
    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"field");
    xmlNodeSetContent(child, (xmlChar *)rule->field.c_str());

    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_rule_child(xmlNodePtr node, XsltRule *rule)
{
    xmlNodePtr child = xmlNewNode(NULL, (xmlChar *)"call-template");
    child->ns = create_namespace_xsl(NULL);
    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)rule->ope.c_str());

    xmlAddChild(node, child);
    write_rule_title_child(child, rule);
    write_rule_xpath_child(child, rule);
    write_rule_value_child(child, rule);
    write_rule_type_child(child, rule);
    write_rule_occurence_child(child, rule);
    write_rule_field_child(child, rule);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_policy_childs(xmlNodePtr node, XsltRule *rule)
{
    xmlNodePtr child = xmlNewNode(NULL, (xmlChar *)"policy");
    xmlAddChild(node, child);

    write_rule_child(child, rule);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_media_attribute_value_child(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (xmlChar *)"value-of");
    child->ns = node->ns;

    xmlNewProp(child, (const xmlChar *)"select", (const xmlChar *)"./@ref");
    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_media_attribute_childs(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (xmlChar *)"attribute");
    child->ns = create_namespace_xsl(NULL);

    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"ref");
    xmlAddChild(node, child);
    write_media_attribute_value_child(child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_media_childs(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (xmlChar *)"media");
    xmlAddChild(node, child);

    write_media_attribute_childs(child);
    for (size_t i = 0; i < rules.size(); ++i)
        write_policy_childs(child, rules[i]);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_for_each_childs(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (xmlChar *)"for-each");
    child->ns = create_namespace_xsl(NULL);

    xmlNewNsProp(child, create_namespace_mc(NULL), (const xmlChar *)"select", (const xmlChar *)"media");
    xmlAddChild(node, child);
    write_media_childs(child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_policychecks_attribute_text_child(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (xmlChar *)"text");
    child->ns = node->ns;

    xmlNodeSetContent(child, (xmlChar *)"0.1");
    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_policychecks_attribute_childs(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (xmlChar *)"attribute");
    child->ns = create_namespace_xsl(NULL);

    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"version");
    xmlAddChild(node, child);
    write_policychecks_attribute_text_child(child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_policychecks_childs(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (xmlChar *)"policyChecks");

    xmlAddChild(node, child);
    write_policychecks_attribute_childs(child);
    write_for_each_childs(child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_mediaconch_childs(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (xmlChar *)"MediaConch");

    xmlAddChild(node, child);
    write_policychecks_childs(child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_root_template_childs(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (xmlChar *)"template");
    child->ns = node->ns;

    xmlNewNsProp(child, create_namespace_mc(NULL), (const xmlChar *)"match", (const xmlChar *)"MediaArea");

    xmlAddChild(node, child);
    write_mediaconch_childs(child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_root_output_child(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (xmlChar *)"output");
    child->ns = node->ns;

    xmlNewProp(child, (const xmlChar *)"encoding", (const xmlChar *)"UTF-8");
    xmlNewProp(child, (const xmlChar *)"version", (const xmlChar *)"1.0");
    xmlNewProp(child, (const xmlChar *)"indent", (const xmlChar *)"yes");

    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_root_default_childs(xmlNodePtr node)
{
    write_root_output_child(node);
    write_root_template_childs(node);
}

//---------------------------------------------------------------------------
xmlDocPtr XsltPolicy::create_doc()
{
    xmlDocPtr doc = xmlNewDoc((xmlChar *)"1.0");
    xmlNodePtr root_node = xmlNewNode(NULL, (xmlChar *)"stylesheet");
    xmlDocSetRootElement(doc, root_node);

    //Default Namespaces
    xmlNsPtr nsXsl = create_namespace_xsl(root_node);
    create_namespace_mc(root_node);
    create_namespace_xsi(root_node);

    root_node->ns = nsXsl;
    root_node->nsDef = nsXsl;

    xmlNewProp(root_node, (const xmlChar *)"version", (const xmlChar *)"1.0");
    xmlNewProp(root_node, (const xmlChar *)"extension-element-prefixes", (const xmlChar *)"xsi");
    write_root_default_childs(root_node);
    return doc;
}

}
