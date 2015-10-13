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
    this->occurrence = r.occurrence;
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
    std::string def("attribute");
    if (!node || node->type != XML_ELEMENT_NODE ||
        !node->name || def.compare((const char*)node->name))
        return false;

    def = std::string("title");
    xmlChar *name = xmlGetNoNsProp(node, (const unsigned char*)"name");
    if (name == NULL || def.compare((const char*)name))
        return false;

    const char* descr = (const char*)xmlNodeGetContent(node);
    if (descr)
        title = std::string(descr);
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_rule_value_node(xmlNodePtr node, std::string& value)
{
    std::string def("with-param");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    def = std::string("value");
    xmlChar *name = xmlGetNoNsProp(node, (const unsigned char*)"name");
    if (name == NULL || def.compare((const char*)name))
        return false;

    const char* descr = (const char*)xmlNodeGetContent(node);
    if (descr)
        value = std::string(descr);
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_rule_field_node(xmlNodePtr node, std::string& field)
{
    std::string def("with-param");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    def = std::string("field");
    xmlChar *name = xmlGetNoNsProp(node, (const unsigned char*)"name");
    if (name == NULL || def.compare((const char*)name))
        return false;

    const char* descr = (const char*)xmlNodeGetContent(node);
    if (descr)
        field = std::string(descr);
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_rule_occurrence_node(xmlNodePtr node, int& occurrence)
{
    std::string def("with-param");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    def = std::string("occurrence");
    xmlChar *name = xmlGetNoNsProp(node, (const unsigned char*)"name");
    if (name == NULL || def.compare((const char*)name))
        return false;

    const char* descr = (const char*)xmlNodeGetContent(node);
    if (descr)
    {
        std::string tmp = std::string(descr);
        occurrence = atoi(tmp.c_str());
    }
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_rule_type_node(xmlNodePtr node, std::string& type)
{
    std::string def("with-param");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    def = std::string("tracktype");
    xmlChar *name = xmlGetNoNsProp(node, (const unsigned char*)"name");
    if (name == NULL || def.compare((const char*)name))
        return false;

    const char* descr = (const char*)xmlNodeGetContent(node);
    if (descr)
        type = std::string(descr);
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_rule_title_node(xmlNodePtr node, std::string& title)
{
    std::string def("with-param");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    def = std::string("title");
    xmlChar *name = xmlGetNoNsProp(node, (const unsigned char*)"name");
    if (name == NULL || def.compare((const char*)name))
        return false;

    const char* descr = (const char*)xmlNodeGetContent(node);
    if (descr)
        title = std::string(descr);
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_call_template_node(xmlNodePtr node)
{
    std::string def("call-template");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    xmlChar *name = xmlGetNoNsProp(node, (const unsigned char*)"name");
    if (name == NULL)
        return false;

    XsltRule *r = new XsltRule;

    r->ope = std::string((const char*)name);

    for (xmlNodePtr child = node->children; child; child = child->next)
    {
        if (find_rule_title_node(child, r->title))
            continue;
        if (find_rule_type_node(child, r->type))
            continue;
        if (find_rule_occurrence_node(child, r->occurrence))
            continue;
        if (find_rule_field_node(child, r->field))
            continue;
        if (find_rule_value_node(child, r->value))
            continue;
    }
    rules.push_back(r);
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_policy_node(xmlNodePtr node)
{
    std::string def("policy");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    xmlNodePtr child = node->children;
    while (child)
    {
        find_call_template_node(child);
        child = child->next;
    }
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_media_node(xmlNodePtr node)
{
    std::string def("media");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    xmlNodePtr child = node->children;
    while (child)
    {
        find_policy_node(child);
        child = child->next;
    }
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_for_each_node(xmlNodePtr node)
{
    std::string def("for-each");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    xmlNodePtr child = node->children;
    while (child)
    {
        find_media_node(child);
        child = child->next;
    }
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_policychecks_node(xmlNodePtr node)
{
    std::string def("policyChecks");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    xmlNodePtr child = node->children;
    while (child)
    {
        find_for_each_node(child);
        child = child->next;
    }
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_mediaconch_node(xmlNodePtr node)
{
    std::string def("MediaConch");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    xmlNodePtr child = node->children;
    while (child)
    {
        find_policychecks_node(child);
        child = child->next;
    }
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_template_match_node(xmlNodePtr node)
{
    xmlChar *name = xmlGetNoNsProp(node, (const unsigned char*)"match");
    if (name == NULL)
        return false;

    xmlNodePtr child = node->children;
    while (child)
    {
        find_mediaconch_node(child);
        child = child->next;
    }
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_template_name_node(xmlNodePtr node)
{
    xmlChar *name = xmlGetNoNsProp(node, (const unsigned char*)"name");
    if (name == NULL)
        return false;
    //TODO: operation
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_template_node(xmlNodePtr node)
{
    std::string def("template");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    if (find_template_match_node(node))
        return true;
    return find_template_name_node(node);
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
    if (!title.length())
    {
        title = std::string(filename);
        size_t start_index = title.find_last_of("\\/");
        if (std::string::npos != start_index)
            title = title.substr(start_index + 1);

        size_t end_index = title.find(".xsl", title.length() - 5);
        if (end_index != std::string::npos)
            title = title.substr(0, end_index);
    }
    while (child)
    {
        find_template_node(child);
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
xmlNodePtr XsltPolicy::write_operator_new_node(xmlNodePtr parent, const xmlChar* title,
                                               std::vector<std::pair<const xmlChar*, const xmlChar*> >& prop,
                                               const xmlChar* content, bool parentNs)
{
    xmlNodePtr child = xmlNewNode(NULL, title);
    for (size_t i = 0; i < prop.size(); ++i)
        xmlNewProp(child, prop[i].first, prop[i].second);
    if (parentNs)
        child->ns = parent->ns;
    xmlAddChild(parent, child);

    if (content)
        xmlNodeSetContent(child, content);

    prop.clear();
    return child;
}

//---------------------------------------------------------------------------
void XsltPolicy::write_operator_is_true(xmlNodePtr node)
{
    std::vector<std::pair<const xmlChar*, const xmlChar*> > prop;

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"is_true"));
    xmlNodePtr child = write_operator_new_node(node, (const xmlChar *)"template", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"xpath"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"value"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"expected"));
    xmlNodePtr attr1 = write_operator_new_node(child, (const xmlChar *)"attribute", prop);

    prop.push_back(std::make_pair((const xmlChar *)"select", (const xmlChar *)"$value"));
    write_operator_new_node(attr1, (const xmlChar *)"value-of", prop);

    xmlNodePtr choose = write_operator_new_node(child, (const xmlChar *)"choose", prop);

    prop.push_back(std::make_pair((const xmlChar *)"test", (const xmlChar *)"$xpath"));
    xmlNodePtr when = write_operator_new_node(choose, (const xmlChar *)"when", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"outcome"));
    write_operator_new_node(when, (const xmlChar *)"attribute", prop, (const xmlChar *)"pass");

    xmlNodePtr other = write_operator_new_node(choose, (const xmlChar *)"otherwise", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"outcome"));
    write_operator_new_node(other, (const xmlChar *)"attribute", prop, (const xmlChar *)"fail");

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"reason"));
    write_operator_new_node(other, (const xmlChar *)"attribute", prop, (const xmlChar *)"is not true");
}

//---------------------------------------------------------------------------
void XsltPolicy::write_operator_is_equal(xmlNodePtr node)
{
    std::vector<std::pair<const xmlChar*, const xmlChar*> > prop;

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"is_equal"));
    xmlNodePtr child = write_operator_new_node(node, (const xmlChar *)"template", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"xpath"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"value"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"expected"));
    xmlNodePtr attr1 = write_operator_new_node(child, (const xmlChar *)"attribute", prop);

    prop.push_back(std::make_pair((const xmlChar *)"select", (const xmlChar *)"$value"));
    write_operator_new_node(attr1, (const xmlChar *)"value-of", prop);

    xmlNodePtr choose = write_operator_new_node(child, (const xmlChar *)"choose", prop);

    prop.push_back(std::make_pair((const xmlChar *)"test", (const xmlChar *)"$xpath = $value"));
    xmlNodePtr when = write_operator_new_node(choose, (const xmlChar *)"when", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"outcome"));
    write_operator_new_node(when, (const xmlChar *)"attribute", prop, (const xmlChar *)"pass");

    xmlNodePtr other = write_operator_new_node(choose, (const xmlChar *)"otherwise", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"outcome"));
    write_operator_new_node(other, (const xmlChar *)"attribute", prop, (const xmlChar *)"fail");

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"reason"));
    write_operator_new_node(other, (const xmlChar *)"attribute", prop, (const xmlChar *)"is not equal");

    xmlNodePtr value = write_operator_new_node(other, (const xmlChar *)"value", prop, NULL, false);

    prop.push_back(std::make_pair((const xmlChar *)"select", (const xmlChar *)"$xpath"));
    write_operator_new_node(value, (const xmlChar *)"value-of", prop);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_operator_is_greater_than(xmlNodePtr node)
{
    std::vector<std::pair<const xmlChar*, const xmlChar*> > prop;

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"is_greater_than"));
    xmlNodePtr child = write_operator_new_node(node, (const xmlChar *)"template", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"xpath"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"value"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"expected"));
    xmlNodePtr attr1 = write_operator_new_node(child, (const xmlChar *)"attribute", prop);

    prop.push_back(std::make_pair((const xmlChar *)"select", (const xmlChar *)"$value"));
    write_operator_new_node(attr1, (const xmlChar *)"value-of", prop);

    xmlNodePtr choose = write_operator_new_node(child, (const xmlChar *)"choose", prop);

    prop.push_back(std::make_pair((const xmlChar *)"test", (const xmlChar *)"$xpath > $value"));
    xmlNodePtr when = write_operator_new_node(choose, (const xmlChar *)"when", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"outcome"));
    write_operator_new_node(when, (const xmlChar *)"attribute", prop, (const xmlChar *)"pass");

    xmlNodePtr other = write_operator_new_node(choose, (const xmlChar *)"otherwise", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"outcome"));
    write_operator_new_node(other, (const xmlChar *)"attribute", prop, (const xmlChar *)"fail");

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"reason"));
    write_operator_new_node(other, (const xmlChar *)"attribute", prop, (const xmlChar *)"is less or equal than");

    xmlNodePtr value = write_operator_new_node(other, (const xmlChar *)"value", prop, NULL, false);

    prop.push_back(std::make_pair((const xmlChar *)"select", (const xmlChar *)"$xpath"));
    write_operator_new_node(value, (const xmlChar *)"value-of", prop);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_operator_is_less_than(xmlNodePtr node)
{
    std::vector<std::pair<const xmlChar*, const xmlChar*> > prop;

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"is_less_than"));
    xmlNodePtr child = write_operator_new_node(node, (const xmlChar *)"template", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"xpath"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"value"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"expected"));
    xmlNodePtr attr1 = write_operator_new_node(child, (const xmlChar *)"attribute", prop);

    prop.push_back(std::make_pair((const xmlChar *)"select", (const xmlChar *)"$value"));
    write_operator_new_node(attr1, (const xmlChar *)"value-of", prop);

    xmlNodePtr choose = write_operator_new_node(child, (const xmlChar *)"choose", prop);

    prop.push_back(std::make_pair((const xmlChar *)"test", (const xmlChar *)"$xpath < $value"));
    xmlNodePtr when = write_operator_new_node(choose, (const xmlChar *)"when", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"outcome"));
    write_operator_new_node(when, (const xmlChar *)"attribute", prop, (const xmlChar *)"pass");

    xmlNodePtr other = write_operator_new_node(choose, (const xmlChar *)"otherwise", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"outcome"));
    write_operator_new_node(other, (const xmlChar *)"attribute", prop, (const xmlChar *)"fail");

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"reason"));
    write_operator_new_node(other, (const xmlChar *)"attribute", prop, (const xmlChar *)"is greater or equal than");

    xmlNodePtr value = write_operator_new_node(other, (const xmlChar *)"value", prop, NULL, false);

    prop.push_back(std::make_pair((const xmlChar *)"select", (const xmlChar *)"$xpath"));
    write_operator_new_node(value, (const xmlChar *)"value-of", prop);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_operator_is_greater_or_equal_than(xmlNodePtr node)
{
    std::vector<std::pair<const xmlChar*, const xmlChar*> > prop;

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"is_greater_or_equal_than"));
    xmlNodePtr child = write_operator_new_node(node, (const xmlChar *)"template", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"xpath"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"value"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"expected"));
    xmlNodePtr attr1 = write_operator_new_node(child, (const xmlChar *)"attribute", prop);

    prop.push_back(std::make_pair((const xmlChar *)"select", (const xmlChar *)"$value"));
    write_operator_new_node(attr1, (const xmlChar *)"value-of", prop);

    xmlNodePtr choose = write_operator_new_node(child, (const xmlChar *)"choose", prop);

    prop.push_back(std::make_pair((const xmlChar *)"test", (const xmlChar *)"$xpath >= $value"));
    xmlNodePtr when = write_operator_new_node(choose, (const xmlChar *)"when", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"outcome"));
    write_operator_new_node(when, (const xmlChar *)"attribute", prop, (const xmlChar *)"pass");

    xmlNodePtr other = write_operator_new_node(choose, (const xmlChar *)"otherwise", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"outcome"));
    write_operator_new_node(other, (const xmlChar *)"attribute", prop, (const xmlChar *)"fail");

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"reason"));
    write_operator_new_node(other, (const xmlChar *)"attribute", prop, (const xmlChar *)"is less than");

    xmlNodePtr value = write_operator_new_node(other, (const xmlChar *)"value", prop, NULL, false);

    prop.push_back(std::make_pair((const xmlChar *)"select", (const xmlChar *)"$xpath"));
    write_operator_new_node(value, (const xmlChar *)"value-of", prop);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_operator_is_less_or_equal_than(xmlNodePtr node)
{
    std::vector<std::pair<const xmlChar*, const xmlChar*> > prop;

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"is_less_or_equal_than"));
    xmlNodePtr child = write_operator_new_node(node, (const xmlChar *)"template", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"xpath"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"value"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"expected"));
    xmlNodePtr attr1 = write_operator_new_node(child, (const xmlChar *)"attribute", prop);

    prop.push_back(std::make_pair((const xmlChar *)"select", (const xmlChar *)"$value"));
    write_operator_new_node(attr1, (const xmlChar *)"value-of", prop);

    xmlNodePtr choose = write_operator_new_node(child, (const xmlChar *)"choose", prop);

    prop.push_back(std::make_pair((const xmlChar *)"test", (const xmlChar *)"$xpath <= $value"));
    xmlNodePtr when = write_operator_new_node(choose, (const xmlChar *)"when", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"outcome"));
    write_operator_new_node(when, (const xmlChar *)"attribute", prop, (const xmlChar *)"pass");

    xmlNodePtr other = write_operator_new_node(choose, (const xmlChar *)"otherwise", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"outcome"));
    write_operator_new_node(other, (const xmlChar *)"attribute", prop, (const xmlChar *)"fail");

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"reason"));
    write_operator_new_node(other, (const xmlChar *)"attribute", prop, (const xmlChar *)"is greater than");

    xmlNodePtr value = write_operator_new_node(other, (const xmlChar *)"value", prop, NULL, false);

    prop.push_back(std::make_pair((const xmlChar *)"select", (const xmlChar *)"$xpath"));
    write_operator_new_node(value, (const xmlChar *)"value-of", prop);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_operator_exists(xmlNodePtr node)
{
    std::vector<std::pair<const xmlChar*, const xmlChar*> > prop;

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"exists"));
    xmlNodePtr child = write_operator_new_node(node, (const xmlChar *)"template", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"xpath"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    xmlNodePtr choose = write_operator_new_node(child, (const xmlChar *)"choose", prop);

    prop.push_back(std::make_pair((const xmlChar *)"test", (const xmlChar *)"string-length($xpath) != 0"));
    xmlNodePtr when = write_operator_new_node(choose, (const xmlChar *)"when", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"outcome"));
    write_operator_new_node(when, (const xmlChar *)"attribute", prop, (const xmlChar *)"pass");

    xmlNodePtr other = write_operator_new_node(choose, (const xmlChar *)"otherwise", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"outcome"));
    write_operator_new_node(other, (const xmlChar *)"attribute", prop, (const xmlChar *)"fail");

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"reason"));
    write_operator_new_node(other, (const xmlChar *)"attribute", prop, (const xmlChar *)"does not exist");
}

//---------------------------------------------------------------------------
void XsltPolicy::write_operator_does_not_exist(xmlNodePtr node)
{
    std::vector<std::pair<const xmlChar*, const xmlChar*> > prop;

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"exists"));
    xmlNodePtr child = write_operator_new_node(node, (const xmlChar *)"template", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"xpath"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    xmlNodePtr choose = write_operator_new_node(child, (const xmlChar *)"choose", prop);

    prop.push_back(std::make_pair((const xmlChar *)"test", (const xmlChar *)"string-length($xpath) = 0"));
    xmlNodePtr when = write_operator_new_node(choose, (const xmlChar *)"when", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"outcome"));
    write_operator_new_node(when, (const xmlChar *)"attribute", prop, (const xmlChar *)"pass");

    xmlNodePtr other = write_operator_new_node(choose, (const xmlChar *)"otherwise", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"outcome"));
    write_operator_new_node(other, (const xmlChar *)"attribute", prop, (const xmlChar *)"fail");

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"reason"));
    write_operator_new_node(other, (const xmlChar *)"attribute", prop, (const xmlChar *)"exists");
}

//---------------------------------------------------------------------------
void XsltPolicy::write_operator_matches_regex(xmlNodePtr node)
{
    std::vector<std::pair<const xmlChar*, const xmlChar*> > prop;

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"maches_regex"));
    xmlNodePtr child = write_operator_new_node(node, (const xmlChar *)"template", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"xpath"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"value"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"expected"));
    xmlNodePtr attr1 = write_operator_new_node(child, (const xmlChar *)"attribute", prop);

    prop.push_back(std::make_pair((const xmlChar *)"select", (const xmlChar *)"$value"));
    write_operator_new_node(attr1, (const xmlChar *)"value-of", prop);

    xmlNodePtr choose = write_operator_new_node(child, (const xmlChar *)"choose", prop);

    prop.push_back(std::make_pair((const xmlChar *)"test", (const xmlChar *)"contains($xpath, $value)"));
    xmlNodePtr when = write_operator_new_node(choose, (const xmlChar *)"when", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"outcome"));
    write_operator_new_node(when, (const xmlChar *)"attribute", prop, (const xmlChar *)"pass");

    xmlNodePtr other = write_operator_new_node(choose, (const xmlChar *)"otherwise", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"outcome"));
    write_operator_new_node(other, (const xmlChar *)"attribute", prop, (const xmlChar *)"fail");

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"reason"));
    write_operator_new_node(other, (const xmlChar *)"attribute", prop, (const xmlChar *)"does not match regex");

    xmlNodePtr value = write_operator_new_node(other, (const xmlChar *)"value", prop, NULL, false);

    prop.push_back(std::make_pair((const xmlChar *)"select", (const xmlChar *)"$xpath"));
    write_operator_new_node(value, (const xmlChar *)"value-of", prop);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_operators(xmlNodePtr node)
{
    write_operator_is_true(node);
    write_operator_is_equal(node);
    write_operator_is_greater_than(node);
    write_operator_is_less_than(node);
    write_operator_is_greater_or_equal_than(node);
    write_operator_is_less_or_equal_than(node);
    write_operator_exists(node);
    write_operator_does_not_exist(node);
    write_operator_matches_regex(node);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_rule_title_child(xmlNodePtr node, XsltRule *rule)
{
    if (!rule->title.length())
        return;

    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"with-param");
    child->ns = node->ns;
    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"title");
    xmlNodeSetContent(child, (const xmlChar *)rule->title.c_str());

    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_rule_xpath_child(xmlNodePtr node, XsltRule *rule)
{
    std::string xpath;
    create_xpath_from_rule(rule, xpath);
    if (!xpath.length())
        return;

    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"with-param");
    child->ns = node->ns;
    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"xpath");
    xmlNewProp(child, (const xmlChar *)"select", (const xmlChar *)xpath.c_str());

    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_rule_value_child(xmlNodePtr node, XsltRule *rule)
{
    if (!rule->value.length())
        return;

    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"with-param");
    child->ns = node->ns;
    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"value");
    xmlNodeSetContent(child, (const xmlChar *)rule->value.c_str());

    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_rule_type_child(xmlNodePtr node, XsltRule *rule)
{
    if (!rule->type.length())
        return;

    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"with-param");
    child->ns = node->ns;
    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"tracktype");
    xmlNodeSetContent(child, (const xmlChar *)rule->type.c_str());

    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_rule_occurrence_child(xmlNodePtr node, XsltRule *rule)
{
    std::stringstream ss;
    ss << rule->occurrence;
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"with-param");
    child->ns = node->ns;
    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"occurrence");
    xmlNodeSetContent(child, (const xmlChar *)ss.str().c_str());

    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_rule_field_child(xmlNodePtr node, XsltRule *rule)
{
    if (!rule->field.length())
        return;

    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"with-param");
    child->ns = node->ns;
    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"field");
    xmlNodeSetContent(child, (const xmlChar *)rule->field.c_str());

    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_rule_child(xmlNodePtr node, XsltRule *rule)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"call-template");
    child->ns = create_namespace_xsl(NULL);
    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)rule->ope.c_str());

    xmlAddChild(node, child);
    write_rule_title_child(child, rule);
    write_rule_xpath_child(child, rule);
    write_rule_value_child(child, rule);
    write_rule_type_child(child, rule);
    write_rule_occurrence_child(child, rule);
    write_rule_field_child(child, rule);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_policy_childs(xmlNodePtr node, XsltRule *rule)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"policy");
    xmlAddChild(node, child);

    write_rule_child(child, rule);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_media_attribute_value_child(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"value-of");
    child->ns = node->ns;

    xmlNewProp(child, (const xmlChar *)"select", (const xmlChar *)"./@ref");
    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_media_attribute_childs(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"attribute");
    child->ns = create_namespace_xsl(NULL);

    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"ref");
    xmlAddChild(node, child);
    write_media_attribute_value_child(child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_media_childs(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"media");
    xmlAddChild(node, child);

    write_media_attribute_childs(child);
    for (size_t i = 0; i < rules.size(); ++i)
        write_policy_childs(child, rules[i]);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_for_each_childs(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"for-each");
    child->ns = create_namespace_xsl(NULL);

    xmlNewNsProp(child, create_namespace_mc(NULL), (const xmlChar *)"select", (const xmlChar *)"media");
    xmlAddChild(node, child);
    write_media_childs(child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_policychecks_attribute_text_child(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"text");
    child->ns = node->ns;

    xmlNodeSetContent(child, (const xmlChar *)"0.1");
    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_policychecks_attribute_childs(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"attribute");
    child->ns = create_namespace_xsl(NULL);

    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"version");
    xmlAddChild(node, child);
    write_policychecks_attribute_text_child(child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_policychecks_childs(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"policyChecks");

    xmlAddChild(node, child);
    write_policychecks_attribute_childs(child);
    write_for_each_childs(child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_mediaconch_childs(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"MediaConch");

    xmlAddChild(node, child);
    write_policychecks_childs(child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_root_template_childs(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"template");
    child->ns = node->ns;

    xmlNewProp(child, (const xmlChar *)"match", (const xmlChar *)"mc:MediaArea");

    xmlAddChild(node, child);
    write_mediaconch_childs(child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_root_output_child(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"output");
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
    write_operators(node);
}

//---------------------------------------------------------------------------
xmlDocPtr XsltPolicy::create_doc()
{
    xmlDocPtr doc = xmlNewDoc((const xmlChar *)"1.0");
    xmlNodePtr root_node = xmlNewNode(NULL, (const xmlChar *)"stylesheet");
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
