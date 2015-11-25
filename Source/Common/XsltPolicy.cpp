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
bool XsltPolicy::find_xslt_header(xmlNodePtr node)
{
    if (!node || node->type != XML_ELEMENT_NODE || !node->ns)
        return false;

    std::string ns("xsl");
    if (ns.compare((const char*)node->ns->prefix))
        return false;

    std::string ref("http://www.w3.org/1999/XSL/Transform");
    if (ref.compare((const char*)node->ns->href))
        return false;
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_title_node(xmlNodePtr node, std::string& title)
{
    std::string def("attribute");
    if (!node || node->type != XML_ELEMENT_NODE ||
        !node->name || def.compare((const char*)node->name))
        return false;

    def = std::string("name");
    xmlChar *name = xmlGetNoNsProp(node, (const unsigned char*)"name");
    if (name == NULL || def.compare((const char*)name))
        return false;

    const char* descr = (const char*)xmlNodeGetContent(node);
    if (descr)
        title = std::string(descr);
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_call_template_value_node(xmlNodePtr node, std::string& value)
{
    std::string def("with-param");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    def = std::string("value");
    xmlChar *name = xmlGetNoNsProp(node, (const unsigned char*)"name");
    if (name == NULL || def.compare((const char*)name))
        return false;

    const char* select = (const char*)xmlNodeGetContent(node);
    if (select != NULL)
        value = std::string((const char*)select);

    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_call_template_xpath_node(xmlNodePtr node, std::string& xpath)
{
    std::string def("with-param");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    def = std::string("xpath");
    xmlChar *name = xmlGetNoNsProp(node, (const unsigned char*)"name");
    if (name == NULL || def.compare((const char*)name))
        return false;

    xmlChar *select = xmlGetNoNsProp(node, (const unsigned char*)"select");
    if (select != NULL)
        xpath = std::string((const char*)select);

    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_call_template_free_text_node(xmlNodePtr node, XsltRule* rule, bool& valid)
{
    std::string def("call-template");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    xmlChar *name = xmlGetNoNsProp(node, (const unsigned char*)"name");
    if (name == NULL)
        return false;
    rule->ope = std::string((const char*)name);

    xmlNodePtr child = node->children;
    while (child)
    {
        std::string xpath;
        if (find_call_template_xpath_node(child, xpath))
            if (xpath != rule->value)
                valid = false;
        child = child->next;
    }

    if (valid)
    {
        rule->text = rule->value;
        rule->value = std::string();
        rule->use_free_text = true;
    }
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_choose_call_template_node(xmlNodePtr node, XsltRule* rule, bool& valid)
{
    std::string def("call-template");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    xmlChar *name = xmlGetNoNsProp(node, (const unsigned char*)"name");
    if (name == NULL)
    {
        valid = false;
        return true;
    }
    rule->ope = std::string((const char*)name);

    xmlNodePtr child = node->children;
    while (child)
    {
        std::string tmp;
        if (find_call_template_xpath_node(child, tmp))
        {
            if (tmp != ".")
            {
                valid = false;
                return true;
            }
        }
        else if (find_call_template_value_node(child, tmp))
        {
            if (tmp != rule->value)
            {
                valid = false;
                return true;
            }
        }
        child = child->next;
    }

    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_choose_for_each_node(xmlNodePtr node, XsltRule* rule, bool& valid)
{
    std::string def("for-each");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    xmlChar *select_str = xmlGetNoNsProp(node, (const unsigned char*)"select");
    if (select_str == NULL)
        return false;
    std::string select((const char*)select_str);

    valid = parse_test_for_rule(select, rule);
    if (!valid)
        return true;

    xmlNodePtr child = node->children;
    while (child)
    {
        find_choose_call_template_node(child, rule, valid);
        if (!valid)
            return true;
        child = child->next;
    }

    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_choose_when_node(xmlNodePtr node, XsltRule* rule, bool& valid)
{
    std::string def("when");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    xmlChar *test_str = xmlGetNoNsProp(node, (const unsigned char*)"test");
    if (test_str == NULL)
        return false;
    std::string test((const char*)test_str);

    valid = parse_test_for_rule(test, rule);

    xmlNodePtr child = node->children;
    while (child)
    {
        find_choose_for_each_node(child, rule, valid);
        child = child->next;
    }

    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_choose_node(xmlNodePtr node, XsltRule* rule, bool& valid)
{
    std::string def("choose");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    xmlNodePtr child = node->children;
    while (child)
    {
        find_choose_when_node(child, rule, valid);
        if (!valid)
            return true;
        child = child->next;
    }

    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_context_attribute_value_node(xmlNodePtr node, std::string& value)
{
    std::string def("value");
    xmlChar *name = xmlGetNoNsProp(node, (const unsigned char*)"name");
    if (name == NULL || def.compare((const char*)name))
        return false;

    const char* descr = (const char*)xmlNodeGetContent(node);
    if (descr)
        value = std::string(descr);
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_context_attribute_field_node(xmlNodePtr node, std::string& field)
{
    std::string def("field");
    xmlChar *name = xmlGetNoNsProp(node, (const unsigned char*)"name");
    if (name == NULL || def.compare((const char*)name))
        return false;

    const char* descr = (const char*)xmlNodeGetContent(node);
    if (descr)
        field = std::string(descr);
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_context_attribute_node(xmlNodePtr node, XsltRule* rule)
{
    std::string def("attribute");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    if (find_context_attribute_value_node(node, rule->value))
        return true;
    if (find_context_attribute_field_node(node, rule->field))
        return true;
    return false;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_context_node(xmlNodePtr node, XsltRule* rule)
{
    std::string def("context");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    xmlNodePtr child = node->children;
    while (child)
    {
        find_context_attribute_node(child, rule);
        child = child->next;
    }
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_rule_title_node(xmlNodePtr node, std::string& t)
{
    std::string def("attribute");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    def = std::string("name");
    xmlChar *name = xmlGetNoNsProp(node, (const unsigned char*)"name");
    if (name == NULL || def.compare((const char*)name))
        return false;

    const char* descr = (const char*)xmlNodeGetContent(node);
    if (descr)
        t = std::string(descr);
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_check_node(xmlNodePtr node)
{
    std::string def("check");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    bool valid = true;
    XsltRule *r = new XsltRule;
    xmlNodePtr child = node->children;
    while (child)
    {
        if (!find_rule_title_node(child, r->title))
            if (!find_context_node(child, r))
                if (!find_choose_node(child, r, valid))
                    find_call_template_free_text_node(child, r, valid);
        if (!valid)
            break;
        child = child->next;
    }
    if (valid)
        rules.push_back(r);
    else
        delete r;
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_policychecks_description_node(xmlNodePtr node)
{
    std::string def("description");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    const char* description_str = (const char*)xmlNodeGetContent(node);
    if (description_str)
        description = std::string(description_str);
    return true;
}

//---------------------------------------------------------------------------
bool XsltPolicy::find_policychecks_name_node(xmlNodePtr node)
{
    std::string def("name");
    if (!node->name || def.compare((const char*)node->name))
        return false;

    const char* title_str = (const char*)xmlNodeGetContent(node);
    if (title_str)
        title = std::string(title_str);
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
        if (!find_policychecks_name_node(child))
            if (!find_policychecks_description_node(child))
                find_check_node(child);
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
        find_policychecks_node(child);
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
bool XsltPolicy::find_mediaconch_node(xmlNodePtr node)
{
    std::string def("MediaConch");
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
int XsltPolicy::import_schema_from_doc(const std::string& filename, xmlDocPtr doc)
{
    if (!doc)
    {
        error = "The XSL doc is not valid";
        return -1;
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (!root)
    {
        error = "No root node, leaving";
        return -1;
    }

    if (!find_xslt_header(root))
    {
        error = "Format not detected";
        return -1;
    }

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
    return 0;
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
    return xmlNewNs(node, (const xmlChar*)"https://mediaarea.net/mediaconch", NULL);
}

//---------------------------------------------------------------------------
xmlNsPtr XsltPolicy::create_namespace_ma(xmlNodePtr node)
{
    return xmlNewNs(node, (const xmlChar*)"https://mediaarea.net/mediaarea",
                    (const xmlChar *)"ma");
}

//---------------------------------------------------------------------------
xmlNsPtr XsltPolicy::create_namespace_mi(xmlNodePtr node)
{
    return xmlNewNs(node, (const xmlChar*)"https://mediaarea.net/mediainfo",
                    (const xmlChar *)"mi");
}

//---------------------------------------------------------------------------
xmlNsPtr XsltPolicy::create_namespace_xsi(xmlNodePtr node)
{
    return xmlNewNs(node, (const xmlChar*)"http://www.w3.org/2001/XMLSchema-instance",
                    (const xmlChar *)"xsi");
}

//---------------------------------------------------------------------------
void XsltPolicy::create_test_from_rule(XsltRule *rule, std::string& xpath)
{
    std::stringstream ss;

    ss << "mi:MediaInfo/mi:track[@type='";

    if (!rule->type.length())
        return;
    ss << rule->type << "']";

    ss << "[";
    if (rule->occurrence < 0)
        ss << "*";
    else
        ss << rule->occurrence;
    ss << "]";

    if (!rule->field.length())
    {
        xpath = ss.str();
        return;
    }
    ss << "/mi:" << rule->field;

    xpath = ss.str();
}

//---------------------------------------------------------------------------
bool XsltPolicy::parse_test_for_rule(const std::string& test, XsltRule *rule)
{
    std::string ss(test);
    std::string find("mi:MediaInfo/mi:track[@type='");
    size_t pos = 0;
    size_t end = 0;
    // type
    if ((pos = ss.find(find)) != 0)
        return false;

    pos += find.length();
    find = "']";
    if ((end = ss.find(find, pos)) == std::string::npos)
        return false;

    rule->type = ss.substr(pos, end - pos);
    if (!rule->type.length())
        return false;

    end += find.length();
    if (end == ss.length())
        return true;

    // occurrence
    pos = end;
    find = "[";
    if ((pos = ss.find(find, end)) == end)
    {
        pos += find.length();
        find = "]";
        if ((end = ss.find("]", pos)) == std::string::npos)
            return false;

        std::string occurrence = ss.substr(pos, end - pos);
        if (occurrence == "*")
            rule->occurrence = -1;
        else
            rule->occurrence = atoi(occurrence.c_str());
        end += find.length();
    }
    else
        rule->occurrence = -1;

    if (end == ss.length())
        return true;

    // field
    pos = end;
    find = "/mi:";
    if ((pos = ss.find(find, end)) != end)
        return false;

    pos += find.length();
    end = ss.length();

    std::string field = ss.substr(pos, end - pos);
    if (!field.length())
        return false;
    rule->field = field;

    return true;
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
void XsltPolicy::write_operator_actual(xmlNodePtr node)
{
    std::vector<std::pair<const xmlChar*, const xmlChar*> > prop;

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"actual"));
    xmlNodePtr attr = write_operator_new_node(node, (const xmlChar *)"attribute", prop);

    prop.push_back(std::make_pair((const xmlChar *)"select", (const xmlChar *)"$xpath"));
    write_operator_new_node(attr, (const xmlChar *)"value-of", prop);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_operator_choose(xmlNodePtr node, const xmlChar* test,
                                       const xmlChar* pass,
                                       const xmlChar* fail, const xmlChar* reason)
{
    std::vector<std::pair<const xmlChar*, const xmlChar*> > prop;

    xmlNodePtr choose = write_operator_new_node(node, (const xmlChar *)"choose", prop);

    prop.push_back(std::make_pair((const xmlChar *)"test", test));
    xmlNodePtr when = write_operator_new_node(choose, (const xmlChar *)"when", prop);
    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"outcome"));
    write_operator_new_node(when, (const xmlChar *)"attribute", prop, pass);

    xmlNodePtr otherwise = write_operator_new_node(choose, (const xmlChar *)"otherwise", prop);
    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"outcome"));
    write_operator_new_node(otherwise, (const xmlChar *)"attribute", prop, fail);
    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"reason"));
    write_operator_new_node(otherwise, (const xmlChar *)"attribute", prop, reason);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_operator_test_tracktypeorder(xmlNodePtr node)
{
    std::vector<std::pair<const xmlChar*, const xmlChar*> > prop;

    prop.push_back(std::make_pair((const xmlChar *)"test", (const xmlChar *)"../@typeorder"));
    xmlNodePtr ifElement = write_operator_new_node(node, (const xmlChar *)"if", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"tracktypeorder"));
    xmlNodePtr attr = write_operator_new_node(ifElement, (const xmlChar *)"attribute", prop);

    prop.push_back(std::make_pair((const xmlChar *)"select", (const xmlChar *)"../@typeorder"));
    write_operator_new_node(attr, (const xmlChar *)"value-of", prop);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_operator_test_trackid(xmlNodePtr node)
{
    std::vector<std::pair<const xmlChar*, const xmlChar*> > prop;

    prop.push_back(std::make_pair((const xmlChar *)"test", (const xmlChar *)"../mi:ID"));
    xmlNodePtr ifElement = write_operator_new_node(node, (const xmlChar *)"if", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"trackid"));
    xmlNodePtr attr = write_operator_new_node(ifElement, (const xmlChar *)"attribute", prop);

    prop.push_back(std::make_pair((const xmlChar *)"select", (const xmlChar *)"../mi:ID"));
    write_operator_new_node(attr, (const xmlChar *)"value-of", prop);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_operator_test_type(xmlNodePtr node)
{
    std::vector<std::pair<const xmlChar*, const xmlChar*> > prop;

    prop.push_back(std::make_pair((const xmlChar *)"test", (const xmlChar *)"../@type"));
    xmlNodePtr ifElement = write_operator_new_node(node, (const xmlChar *)"if", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"tracktype"));
    xmlNodePtr attr = write_operator_new_node(ifElement, (const xmlChar *)"attribute", prop);

    prop.push_back(std::make_pair((const xmlChar *)"select", (const xmlChar *)"../@type"));
    write_operator_new_node(attr, (const xmlChar *)"value-of", prop);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_operator_is_true(xmlNodePtr node)
{
    std::vector<std::pair<const xmlChar*, const xmlChar*> > prop;

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"is_true"));
    xmlNodePtr child = write_operator_new_node(node, (const xmlChar *)"template", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"xpath"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"test"));
    xmlNodePtr test = write_operator_new_node(child, (const xmlChar *)"element", prop);

    write_operator_test_type(test);
    write_operator_test_tracktypeorder(test);
    write_operator_test_trackid(test);
    write_operator_choose(test, (const xmlChar*)"$xpath", (const xmlChar*)"pass",
                          (const xmlChar*)"fail", (const xmlChar*)"is not true");
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

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"test"));
    xmlNodePtr test = write_operator_new_node(child, (const xmlChar *)"element", prop);

    write_operator_test_type(test);
    write_operator_test_tracktypeorder(test);
    write_operator_test_trackid(test);
    write_operator_actual(test);

    write_operator_choose(test, (const xmlChar*)"$xpath = $value", (const xmlChar*)"pass",
                          (const xmlChar*)"fail", (const xmlChar*)"is not equal");
}

//---------------------------------------------------------------------------
void XsltPolicy::write_operator_is_not_equal(xmlNodePtr node)
{
    std::vector<std::pair<const xmlChar*, const xmlChar*> > prop;

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"is_not_equal"));
    xmlNodePtr child = write_operator_new_node(node, (const xmlChar *)"template", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"xpath"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"value"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"test"));
    xmlNodePtr test = write_operator_new_node(child, (const xmlChar *)"element", prop);

    write_operator_test_type(test);
    write_operator_test_tracktypeorder(test);
    write_operator_test_trackid(test);
    write_operator_actual(test);

    write_operator_choose(test, (const xmlChar*)"$xpath != $value", (const xmlChar*)"pass",
                          (const xmlChar*)"fail", (const xmlChar*)"is equal");
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

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"test"));
    xmlNodePtr test = write_operator_new_node(child, (const xmlChar *)"element", prop);

    write_operator_test_type(test);
    write_operator_test_tracktypeorder(test);
    write_operator_test_trackid(test);
    write_operator_actual(test);

    write_operator_choose(test, (const xmlChar*)"$xpath > $value", (const xmlChar*)"pass",
                          (const xmlChar*)"fail", (const xmlChar*)"is less than or equal");
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

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"test"));
    xmlNodePtr test = write_operator_new_node(child, (const xmlChar *)"element", prop);

    write_operator_test_type(test);
    write_operator_test_tracktypeorder(test);
    write_operator_test_trackid(test);
    write_operator_actual(test);

    write_operator_choose(test, (const xmlChar*)"$xpath < $value", (const xmlChar*)"pass",
                          (const xmlChar*)"fail", (const xmlChar*)"is greater than or equal");
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

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"test"));
    xmlNodePtr test = write_operator_new_node(child, (const xmlChar *)"element", prop);

    write_operator_test_type(test);
    write_operator_test_tracktypeorder(test);
    write_operator_test_trackid(test);
    write_operator_actual(test);

    write_operator_choose(test, (const xmlChar*)"$xpath >= $value", (const xmlChar*)"pass",
                          (const xmlChar*)"fail", (const xmlChar*)"is less than");
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

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"test"));
    xmlNodePtr test = write_operator_new_node(child, (const xmlChar *)"element", prop);

    write_operator_test_type(test);
    write_operator_test_tracktypeorder(test);
    write_operator_test_trackid(test);
    write_operator_actual(test);

    write_operator_choose(test, (const xmlChar*)"$xpath <= $value", (const xmlChar*)"pass",
                          (const xmlChar*)"fail", (const xmlChar*)"is greater than");
}

//---------------------------------------------------------------------------
void XsltPolicy::write_operator_exists(xmlNodePtr node)
{
    std::vector<std::pair<const xmlChar*, const xmlChar*> > prop;

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"exists"));
    xmlNodePtr child = write_operator_new_node(node, (const xmlChar *)"template", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"xpath"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"test"));
    xmlNodePtr test = write_operator_new_node(child, (const xmlChar *)"element", prop);

    write_operator_test_type(test);
    write_operator_test_tracktypeorder(test);
    write_operator_test_trackid(test);
    write_operator_actual(test);

    write_operator_choose(test, (const xmlChar*)"string-length($xpath) != 0", (const xmlChar*)"pass",
                          (const xmlChar*)"fail", (const xmlChar*)"does not exist");
}

//---------------------------------------------------------------------------
void XsltPolicy::write_operator_does_not_exist(xmlNodePtr node)
{
    std::vector<std::pair<const xmlChar*, const xmlChar*> > prop;

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"does_not_exist"));
    xmlNodePtr child = write_operator_new_node(node, (const xmlChar *)"template", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"xpath"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"test"));
    xmlNodePtr test = write_operator_new_node(child, (const xmlChar *)"element", prop);

    write_operator_test_type(test);
    write_operator_test_tracktypeorder(test);
    write_operator_test_trackid(test);
    write_operator_actual(test);

    write_operator_choose(test, (const xmlChar*)"string-length($xpath) = '0'", (const xmlChar*)"pass",
                          (const xmlChar*)"fail", (const xmlChar*)"exists");
}

//---------------------------------------------------------------------------
void XsltPolicy::write_operator_contains_string(xmlNodePtr node)
{
    std::vector<std::pair<const xmlChar*, const xmlChar*> > prop;

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"contains_string"));
    xmlNodePtr child = write_operator_new_node(node, (const xmlChar *)"template", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"xpath"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"value"));
    write_operator_new_node(child, (const xmlChar *)"param", prop);

    prop.push_back(std::make_pair((const xmlChar *)"name", (const xmlChar *)"test"));
    xmlNodePtr test = write_operator_new_node(child, (const xmlChar *)"element", prop);

    write_operator_test_type(test);
    write_operator_test_tracktypeorder(test);
    write_operator_test_trackid(test);
    write_operator_actual(test);

    write_operator_choose(test, (const xmlChar*)"contains($xpath, $value)", (const xmlChar*)"pass",
                          (const xmlChar*)"fail", (const xmlChar*)"does not contain");
}

//---------------------------------------------------------------------------
void XsltPolicy::write_operators(xmlNodePtr node)
{
    write_operator_is_true(node);
    write_operator_is_equal(node);
    write_operator_is_not_equal(node);
    write_operator_is_greater_than(node);
    write_operator_is_less_than(node);
    write_operator_is_greater_or_equal_than(node);
    write_operator_is_less_or_equal_than(node);
    write_operator_exists(node);
    write_operator_does_not_exist(node);
    write_operator_contains_string(node);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_check_call_template_field_child(xmlNodePtr node, XsltRule *rule)
{
    if (rule->use_free_text || !rule->field.length() ||
        (rule->ope != "is_greater_than" && rule->ope != "is_less_than" &&
         rule->ope != "is_greater_or_equal_than" && rule->ope != "is_less_or_equal_than" &&
         rule->ope != "exists" && rule->ope != "does_not_exist"))
        return;

    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"with-param");
    child->ns = node->ns;
    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"field");
    xmlNodeSetContent(child, (const xmlChar *)rule->field.c_str());

    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_check_call_template_value_child(xmlNodePtr node, XsltRule *rule)
{
    if (rule->use_free_text || !rule->value.length())
        return;

    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"with-param");
    child->ns = node->ns;
    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"value");
    xmlNodeSetContent(child, (const xmlChar *)rule->value.c_str());

    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_check_call_template_xpath_child(xmlNodePtr node, XsltRule *rule)
{
    if (rule->use_free_text && !rule->text.length())
        return;

    const char *xpath_str = ".";
    if (rule->use_free_text)
        xpath_str = rule->text.c_str();

    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"with-param");
    child->ns = node->ns;
    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"xpath");
    xmlNewProp(child, (const xmlChar *)"select", (const xmlChar *)xpath_str);

    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_check_call_template_child(xmlNodePtr node, XsltRule *rule)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"call-template");
    child->ns = create_namespace_xsl(NULL);
    xmlAddChild(node, child);

    const char *operat = NULL;

    if (!rule->use_free_text)
        operat = rule->ope.c_str();
    else
        operat = "is_true";

    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)operat);
    write_check_call_template_xpath_child(child, rule);
    write_check_call_template_value_child(child, rule);
    write_check_call_template_field_child(child, rule);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_check_for_each_child(xmlNodePtr node, XsltRule *rule)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"for-each");
    child->ns = node->ns;
    xmlAddChild(node, child);

    std::string select;
    create_test_from_rule(rule, select);
    xmlNewProp(child, (const xmlChar *)"select", (const xmlChar *)select.c_str());
    write_check_call_template_child(child, rule);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_check_when_child(xmlNodePtr node, XsltRule *rule)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"when");
    child->ns = node->ns;
    xmlAddChild(node, child);

    std::string test;
    create_test_from_rule(rule, test);
    xmlNewProp(child, (const xmlChar *)"test", (const xmlChar *)test.c_str());
    write_check_for_each_child(child, rule);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_check_otherwise_outcome_child(xmlNodePtr node, XsltRule *)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"test");
    xmlAddChild(node, child);

    xmlNewProp(child, (const xmlChar *)"outcome", (const xmlChar *)"N/A");
}

//---------------------------------------------------------------------------
void XsltPolicy::write_check_otherwise_child(xmlNodePtr node, XsltRule *rule)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"otherwise");
    child->ns = node->ns;
    xmlAddChild(node, child);

    write_check_otherwise_outcome_child(child, rule);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_check_choose_child(xmlNodePtr node, XsltRule *rule)
{
    if (rule->use_free_text)
    {
        write_check_call_template_child(node, rule);
        return;
    }

    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"choose");
    child->ns = create_namespace_xsl(NULL);
    xmlAddChild(node, child);
    write_check_when_child(child, rule);
    write_check_otherwise_child(child, rule);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_check_context_value_child(xmlNodePtr node, XsltRule *rule)
{
    const char *value = NULL;
    if (!rule->use_free_text && rule->value.length())
        value = rule->value.c_str();
    else if (rule->use_free_text && rule->text.length())
        value = rule->text.c_str();
    else
        return;

    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"attribute");
    child->ns = create_namespace_xsl(NULL);
    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"value");
    xmlNodeSetContent(child, (const xmlChar *)value);

    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_check_context_field_child(xmlNodePtr node, XsltRule *rule)
{
    if (!rule->field.length())
        return;

    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"attribute");
    child->ns = create_namespace_xsl(NULL);
    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"field");
    xmlNodeSetContent(child, (const xmlChar *)rule->field.c_str());

    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_check_context_child(xmlNodePtr node, XsltRule *rule)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"context");
    child->ns = node->ns;
    xmlAddChild(node, child);
    
    write_check_context_field_child(child, rule);
    write_check_context_value_child(child, rule);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_check_name_child(xmlNodePtr node, XsltRule *rule)
{
    if (!rule->title.length())
        return;

    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"attribute");
    child->ns = create_namespace_xsl(NULL);
    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"name");
    xmlNodeSetContent(child, (const xmlChar *)rule->title.c_str());

    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_check_childs(xmlNodePtr node, XsltRule *rule)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"check");
    xmlAddChild(node, child);

    write_check_name_child(child, rule);
    write_check_context_child(child, rule);
    write_check_choose_child(child, rule);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_policychecks_description_child(xmlNodePtr node)
{
    if (!description.length())
        return;

    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"description");
    xmlNodeSetContent(child, (const xmlChar *)description.c_str());

    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_policychecks_name_child(xmlNodePtr node)
{
    if (!title.length())
        return;

    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"name");
    xmlNodeSetContent(child, (const xmlChar *)title.c_str());

    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_policychecks_childs(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"policyChecks");

    xmlAddChild(node, child);
    write_policychecks_name_child(child);
    write_policychecks_description_child(child);
    for (size_t i = 0; i < rules.size(); ++i)
        write_check_childs(child, rules[i]);
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
    write_policychecks_childs(child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_for_each_childs(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"for-each");
    child->ns = create_namespace_xsl(NULL);

    xmlNewProp(child, (const xmlChar *)"select", (const xmlChar *)"ma:media");
    xmlAddChild(node, child);
    write_media_childs(child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_mediaconch_attribute_text_child(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"text");
    child->ns = node->ns;

    xmlNodeSetContent(child, (const xmlChar *)"0.1");
    xmlAddChild(node, child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_mediaconch_attribute_childs(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"attribute");
    child->ns = create_namespace_xsl(NULL);

    xmlNewProp(child, (const xmlChar *)"name", (const xmlChar *)"version");
    xmlAddChild(node, child);
    write_mediaconch_attribute_text_child(child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_mediaconch_childs(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"MediaConch");

    xmlAddChild(node, child);
    write_mediaconch_attribute_childs(child);
    write_for_each_childs(child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_root_template_childs(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"template");
    child->ns = node->ns;

    xmlNewProp(child, (const xmlChar *)"match", (const xmlChar *)"ma:MediaArea");

    xmlAddChild(node, child);
    write_mediaconch_childs(child);
}

//---------------------------------------------------------------------------
void XsltPolicy::write_root_output_child(xmlNodePtr node)
{
    xmlNodePtr child = xmlNewNode(NULL, (const xmlChar *)"output");
    child->ns = node->ns;

    xmlNewProp(child, (const xmlChar *)"encoding", (const xmlChar *)"UTF-8");
    xmlNewProp(child, (const xmlChar *)"method", (const xmlChar *)"xml");
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
    create_namespace_ma(root_node);
    create_namespace_mi(root_node);
    create_namespace_xsi(root_node);

    root_node->ns = nsXsl;
    root_node->nsDef = nsXsl;

    xmlNewProp(root_node, (const xmlChar *)"version", (const xmlChar *)"1.0");
    xmlNewProp(root_node, (const xmlChar *)"extension-element-prefixes", (const xmlChar *)"xsi ma");
    write_root_default_childs(root_node);
    return doc;
}

}
