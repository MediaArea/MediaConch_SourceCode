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
#include "generated/PolicyTransformXml.h"

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// XsltPolicyNode
//***************************************************************************

//---------------------------------------------------------------------------
XsltPolicyNode::XsltPolicyNode() : parent_id((size_t)-1)
{
}

//---------------------------------------------------------------------------
XsltPolicyNode::~XsltPolicyNode()
{
}

//---------------------------------------------------------------------------
XsltPolicyNode::XsltPolicyNode(const XsltPolicyNode* n)
{
    if (n == this)
        return;

    this->kind = n->kind;
    this->parent_id = n->parent_id;
    this->node_name = n->node_name;
}

//---------------------------------------------------------------------------
XsltPolicyNode::XsltPolicyNode(const XsltPolicyNode& n)
{
    if (&n == this)
        return;

    this->kind = n.kind;
    this->parent_id = n.parent_id;
    this->node_name = n.node_name;
}

//***************************************************************************
// XsltPolicyRule
//***************************************************************************
size_t XsltPolicyRule::rule_id = 0;

//---------------------------------------------------------------------------
XsltPolicyRule::XsltPolicyRule() : XsltPolicyNode()
{
    kind = XSLT_POLICY_RULE;
    this->occurrence = -1;
}

//---------------------------------------------------------------------------
XsltPolicyRule::~XsltPolicyRule()
{
}

//---------------------------------------------------------------------------
XsltPolicyRule::XsltPolicyRule(const XsltPolicyRule* r) : XsltPolicyNode(r)
{
    if (r == this)
        return;

    this->id         = rule_id++;
    this->ope        = r->ope;
    this->track_type = r->track_type;
    this->field      = r->field;
    this->scope      = r->scope;
    this->occurrence = r->occurrence;
    this->value      = r->value;
}

//---------------------------------------------------------------------------
XsltPolicyRule::XsltPolicyRule(const XsltPolicyRule& r) : XsltPolicyNode(r)
{
    if (&r == this)
        return;

    this->id         = r.id;
    this->ope        = r.ope;
    this->track_type = r.track_type;
    this->field      = r.field;
    this->scope      = r.scope;
    this->occurrence = r.occurrence;
    this->value      = r.value;
}

//---------------------------------------------------------------------------
int XsltPolicyRule::edit_policy_rule(const XsltPolicyRule* rule, std::string&)
{
    this->node_name  = rule->node_name;
    this->ope        = rule->ope;
    this->track_type = rule->track_type;
    this->field      = rule->field;
    this->scope      = rule->scope;
    this->occurrence = rule->occurrence;
    this->value      = rule->value;

    return 0;
}

void XsltPolicyRule::create_default_name(std::string& name)
{
    if (scope.size())
        name = scope + " " + field;
    else
        name = track_type + "/" + field;
    if (ope == "=")
        name += " is ";
    else if (ope == "!=")
        name += " is not ";
    else if (ope == ">")
        name += " is greater than ";
    else if (ope == "<")
        name += " is less than ";
    else if (ope == ">=")
        name += " is greater or equal than ";
    else if (ope == "<=")
        name += " is less or equal than ";
    else if (ope == "")
        name += " exists";
    else
        name += " " + ope + " ";
    name += value;
}

//***************************************************************************
// XsltPolicy
//***************************************************************************

//---------------------------------------------------------------------------
XsltPolicy::XsltPolicy(Policies *p, bool no_https) : Policy(p, Policies::POLICY_XSLT, no_https), XsltPolicyNode()
{
    kind = XSLT_POLICY_POLICY;
}

//---------------------------------------------------------------------------
XsltPolicy::XsltPolicy(const XsltPolicy* s) : Policy(s), XsltPolicyNode(s)
{
    type = Policies::POLICY_XSLT;

    this->ope         = s->ope;
    this->name        = this->node_name;

    for (size_t i = 0; i < s->nodes.size(); ++i)
    {
        if (!s->nodes[i])
            continue;

        XsltPolicyNode *node = NULL;
        if (s->nodes[i]->kind == XSLT_POLICY_POLICY)
            node = new XsltPolicy((XsltPolicy*)s->nodes[i]);
        else
            node = new XsltPolicyRule((XsltPolicyRule*)s->nodes[i]);
        node->parent_id = this->id;
        this->nodes.push_back(node);
    }
}

//---------------------------------------------------------------------------
XsltPolicy::XsltPolicy(const XsltPolicy& s, bool is_system) : Policy(s), XsltPolicyNode(s)
{
    if (&s == this)
        return;

    this->type        = Policies::POLICY_XSLT;

    this->ope         = s.ope;
    this->name        = this->node_name;
    this->is_system   = is_system;

    for (size_t i = 0; i < s.nodes.size(); ++i)
    {
        if (!s.nodes[i])
            continue;

        XsltPolicyNode *node = NULL;
        if (s.nodes[i]->kind == XSLT_POLICY_POLICY)
            node = new XsltPolicy(*(XsltPolicy*)s.nodes[i], is_system);
        else
            node = new XsltPolicyRule(*(XsltPolicyRule*)s.nodes[i]);
        node->parent_id = this->id;
        this->nodes.push_back(node);
    }
}

//---------------------------------------------------------------------------
XsltPolicy::~XsltPolicy()
{
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        delete nodes[i];
        nodes[i] = NULL;
    }
    nodes.clear();
}

//---------------------------------------------------------------------------
int XsltPolicy::get_final_xslt(std::string& xslt, const std::map<std::string, std::string>& opts)
{
    if (dump_schema(xslt) < 0)
        return -1;

    if (policies->transform_with_xslt_memory(xslt, policy_transform_xml, opts, xslt) < 0)
        return -1;

    replace_aliasxsl_in_policy(xslt);
    replace_xlmns_in_policy(xslt);
    return 0;
}

//***************************************************************************
// XsltPolicy Parsing
//***************************************************************************

//---------------------------------------------------------------------------
int XsltPolicy::import_schema_from_doc(xmlDocPtr doc, const std::string& filename)
{
    if (!doc)
    {
        error = "The XML doc is not valid";
        return -1;
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (!root)
    {
        error = "No root node, leaving";
        return -1;
    }

    this->filename = filename;

    //try the root if the XML is badly formated
    if (run_over_siblings_nodes(root, true, NULL) < 0)
    {
        for (size_t i = 0; i < nodes.size(); ++i)
            delete nodes[i];
        nodes.clear();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int XsltPolicy::run_over_siblings_nodes(xmlNodePtr node, bool is_root, XsltPolicy* current)
{
    int ret = 0;
    for (; node; node = node->next)
    {
        switch (node->type)
        {
            case XML_TEXT_NODE: 
            case XML_COMMENT_NODE:
                continue;
            default:;
        }

        int tmp = find_policy_node(node, is_root, current);
        if (tmp < 0)
        {
            ret = -1;
            break;
        }
    }

    return ret;
}

//---------------------------------------------------------------------------
int XsltPolicy::find_policy_node(xmlNodePtr node, bool is_root, XsltPolicy* current)
{
    if (!node->name)
        return 0;

    std::string name((const char *)node->name);
    if (name == "policy")
        return parse_policy_policy(node, is_root, current);
    else if (name == "rule")
        return parse_policy_rule(node, is_root, current);
    else if (name == "description")
    {
        // Get description
        xmlChar *value = xmlNodeGetContent(node);
        if (value)
        {
            current->description = std::string((const char*)value);
            xmlFree(value);
        }
        return 0;
    }


    error = "The XML policy tag accepted are 'policy' or 'rule'";
    return -1;
}

//---------------------------------------------------------------------------
int XsltPolicy::parse_policy_policy(xmlNodePtr node, bool is_root, XsltPolicy* current)
{
    XsltPolicyNode *new_node = NULL;

    if (is_root)
        new_node = this;
    else
    {
        new_node = new XsltPolicy(policies, no_https);;
        new_node->parent_id = current->id;
        current->nodes.push_back(new_node);
    }

    new_node->kind = XSLT_POLICY_POLICY;

    XsltPolicy *p = (XsltPolicy*)new_node;
    //Get name
    xmlChar *name = xmlGetNoNsProp(node, (const unsigned char*)"name");
    if (name)
    {
        p->name = std::string((const char*)name);
        p->node_name = p->name;
        xmlFree(name);
    }

    // Get type
    xmlChar *type = xmlGetNoNsProp(node, (const unsigned char*)"type");
    if (type)
    {
        p->ope = std::string((const char*)type);
        xmlFree(type);
    }
    else
        p->ope = "and";

    if (p->ope != "and" && p->ope != "or")
    {
        error = "The XML policy type accepted are 'and' or 'or'";
        return -1;
    }

    xmlChar *is_public = xmlGetNoNsProp(node, (const unsigned char*)"isPublic");
    if (is_public && is_root)
    {
        std::string is_public_str = std::string((const char*)is_public);
        if (is_public_str == "true")
            p->is_public = true;
        xmlFree(is_public);
    }

    xmlChar *license = xmlGetNoNsProp(node, (const unsigned char*)"license");
    if (license && is_root)
    {
        p->license = std::string((const char*)license);
        xmlFree(license);
    }

    if (!node->children)
        return 0;

    return run_over_siblings_nodes(node->children, false, p);
}

//---------------------------------------------------------------------------
int XsltPolicy::parse_policy_rule(xmlNodePtr node, bool is_root, XsltPolicy* current)
{
    XsltPolicyNode *new_node = NULL;

    if (is_root)
    {
        error = "Policy has to start with a policy node";
        return -1;
    }
    else
    {
        new_node = new XsltPolicyRule;
        ((XsltPolicyRule*)new_node)->id = ((XsltPolicyRule*)new_node)->rule_id++;
        new_node->parent_id = current->id;
        current->nodes.push_back(new_node);
    }

    new_node->kind = XSLT_POLICY_RULE;

    XsltPolicyRule *r = (XsltPolicyRule*)new_node;
    //Get name
    xmlChar *name = xmlGetNoNsProp(node, (const unsigned char*)"name");
    if (name)
    {
        r->node_name = std::string((const char*)name);
        xmlFree(name);
    }

    //Get ope
    xmlChar *ope = xmlGetNoNsProp(node, (const unsigned char*)"operator");
    if (ope)
    {
        r->ope = std::string((const char*)ope);
        xmlFree(ope);
    }

    //Get trackType
    xmlChar *track_type = xmlGetNoNsProp(node, (const unsigned char*)"tracktype");
    if (track_type)
    {
        r->track_type = std::string((const char*)track_type);
        xmlFree(track_type);
    }

    //Get field
    xmlChar *field = xmlGetNoNsProp(node, (const unsigned char*)"value");
    if (field)
    {
        r->field = std::string((const char*)field);
        xmlFree(field);
    }

    //Get scope
    xmlChar *scope = xmlGetNoNsProp(node, (const unsigned char*)"scope");
    if (scope)
    {
        r->scope = std::string((const char*)scope);
        xmlFree(scope);
    }

    //Get occurrence
    xmlChar *occurrence = xmlGetNoNsProp(node, (const unsigned char*)"occurrence");
    if (occurrence)
    {
        std::string occ((const char*)occurrence);

        if (occ == "*")
            r->occurrence = -1;
        else
        {
            char *end;
            r->occurrence = strtol(occ.c_str(), &end, 10);
        }
        xmlFree(occurrence);
    }

    //Get value
    xmlChar *value = xmlNodeGetContent(node);
    if (value)
    {
        r->value = std::string((const char*)value);
        xmlFree(value);
    }

    return 0;
}

//***************************************************************************
// XsltPolicy Serializing
//***************************************************************************

//---------------------------------------------------------------------------
int XsltPolicy::create_node_policy_child(xmlNodePtr& node, XsltPolicy *current)
{
    node = xmlNewNode(NULL, (const xmlChar*)"policy");
    if (!node)
    {
        error = "Cannot create the policy children";
        return -1;
    }

    //type ("and", "or")
    if (current->ope.size())
        xmlNewProp(node, (const xmlChar *)"type", (const xmlChar *)current->ope.c_str());
    else
        xmlNewProp(node, (const xmlChar *)"type", (const xmlChar *)"and");

    //name
    if (current->node_name.size())
        xmlNewProp(node, (const xmlChar *)"name", (const xmlChar *)current->node_name.c_str());

    //description
    if (current->description.size())
    {
        xmlNodePtr new_node = xmlNewNode(NULL, (const xmlChar*)"description");
        if (!new_node)
        {
            error = "Cannot create the policy children";
            return -1;
        }

        xmlNodeSetContent(new_node, (const xmlChar*)current->description.c_str());
        if (!xmlAddChild(node, new_node))
        {
            error = "Cannot add child to policy children";
            return -1;
        }
    }

    //isPublic
    if (current->keep_public && current->is_public)
        xmlNewProp(node, (const xmlChar *)"isPublic", (const xmlChar *)"true");

    //license
    if (current->license.size())
        xmlNewProp(node, (const xmlChar *)"license", (const xmlChar *)current->license.c_str());

    if (write_nodes_children(node, current) < 0)
    {
        error = "Cannot create the policy children";
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int XsltPolicy::create_node_rule_child(xmlNodePtr& node, XsltPolicyRule *current)
{
    node = xmlNewNode(NULL, (const xmlChar *)"rule");
    if (!node)
    {
        error = "Cannot create the rule children";
        return -1;
    }

    //name
    if (current->node_name.size())
        xmlNewProp(node, (const xmlChar *)"name", (const xmlChar *)current->node_name.c_str());

    //field
    if (current->field.size())
        xmlNewProp(node, (const xmlChar *)"value", (const xmlChar *)current->field.c_str());

    //track type
    if (current->track_type.size())
        xmlNewProp(node, (const xmlChar *)"tracktype", (const xmlChar *)current->track_type.c_str());

    //occurrence
    std::stringstream ss;
    std::string occurrence;
    if (current->occurrence >= 0)
    {
        ss << current->occurrence;
        occurrence = ss.str();
    }
    else
        occurrence = "*";
    xmlNewProp(node, (const xmlChar *)"occurrence", (const xmlChar *)occurrence.c_str());

    //operator
    if (current->ope.size())
        xmlNewProp(node, (const xmlChar *)"operator", (const xmlChar *)current->ope.c_str());

    //operator
    if (current->scope.size())
        xmlNewProp(node, (const xmlChar *)"scope", (const xmlChar *)current->scope.c_str());

    //value
    if (current->value.size())
        xmlNodeSetContent(node, (const xmlChar*)current->value.c_str());

    return 0;
}

//---------------------------------------------------------------------------
int XsltPolicy::write_nodes_children(xmlNodePtr node, XsltPolicy *current)
{
    for (size_t i = 0; i < current->nodes.size(); ++i)
    {
        if (!current->nodes[i])
            continue;

        xmlNodePtr new_node = NULL;
        if (current->nodes[i]->kind == XSLT_POLICY_POLICY)
        {
            if (create_node_policy_child(new_node, (XsltPolicy*)current->nodes[i]))
                return -1;
        }
        else
        {
            if (create_node_rule_child(new_node, (XsltPolicyRule*)current->nodes[i]))
                return -1;
        }

        if (!xmlAddChild(node, new_node))
        {
            error = "Cannot add child to policy children";
            return -1;
        }
    }

    return 0;
}

//---------------------------------------------------------------------------
int XsltPolicy::write_root_nodes_children(xmlDocPtr doc)
{
    xmlNodePtr node = NULL;
    if (kind == XSLT_POLICY_POLICY)
    {
        if (create_node_policy_child(node, this) < 0)
            return -1;
    }
    else
    {
        if (create_node_rule_child(node, (XsltPolicyRule*)this) < 0)
            return -1;
    }

    if (!node)
    {
        error = "Cannot create the policy children";
        return -1;
    }

    xmlDocSetRootElement(doc, node);

    return 0;
}

//---------------------------------------------------------------------------
xmlDocPtr XsltPolicy::create_doc()
{
    xmlDocPtr doc = xmlNewDoc((const xmlChar *)"1.0");

    if (write_root_nodes_children(doc) < 0)
    {
        xmlFreeDoc(doc);
        return NULL;
    }

    return doc;
}

//***************************************************************************
// XsltPolicy creation from file
//***************************************************************************

//---------------------------------------------------------------------------
int XsltPolicy::create_rule_from_media_track_child(xmlNodePtr node, const std::string& type)
{
    for (xmlNodePtr child = node->children; child; child = child->next)
    {
        if (child->type != XML_ELEMENT_NODE || !child->name)
            continue;

        std::string name((const char*)child->name);
        if (name == "FileSize"
         || name == "Duration"
         || name == "DURATION"
         || name == "OverallBitRate"
         || name == "FrameCount"
         || name == "StreamSize"
         || name == "BitRate"
         || name == "Delay"
         || name == "extra")
            continue;

        XsltPolicyRule *rule = new XsltPolicyRule;

        rule->id = rule->rule_id++;
        rule->track_type = type;
        rule->field = (const char*)child->name;
        rule->ope = "=";
        rule->occurrence = -1;
        xmlChar *content = xmlNodeGetContent(child);
        if (content)
            rule->value = (const char*)content;
        rule->create_default_name(rule->node_name);
        nodes.push_back(rule);
    }
    return 0;
}

//---------------------------------------------------------------------------
int XsltPolicy::find_media_track_node(xmlNodePtr node, std::string& type)
{
    xmlChar *property = xmlGetNoNsProp(node, (const unsigned char*)"type");

    if (!property)
        return -1;

    type = std::string((const char*)property);
    return 0;
}

//---------------------------------------------------------------------------
int XsltPolicy::find_media_track_node(xmlNodePtr node)
{
    for (xmlNodePtr child = node->children; child; child = child->next)
    {
        std::string def("track");
        if (!child->name || def.compare((const char*)child->name))
            continue;

        std::string type;
        if (find_media_track_node(child, type) < 0)
            return -1;

        if (create_rule_from_media_track_child(child, type) < 0)
            return -1;
    }
    return 0;
}

//---------------------------------------------------------------------------
int XsltPolicy::create_policy_from_mi(const std::string& report)
{
    Xslt s(no_https);
    xmlSetGenericErrorFunc(&s, &s.manage_generic_error);

    xmlDocPtr doc = xmlParseMemory(report.c_str(), report.length());
    xmlSetGenericErrorFunc(NULL, NULL);

    if (!doc)
    {
        // maybe put the errors from s.errors
        error = "The MediaInfo report given cannot be parsed";
        return -1;
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (!root)
    {
        error = "No root node, leaving";
        xmlFreeDoc(doc);
        return 0;
    }

    for (xmlNodePtr child = root->children; child; child = child->next)
    {
        std::string def("media");
        if (child->type == XML_TEXT_NODE || !child->name || def.compare((const char*)child->name))
            continue;

        if (find_media_track_node(child) < 0)
        {
            xmlFreeDoc(doc);
            return -1;
        }
        break;
    }

    xmlFreeDoc(doc);
    return 0;
}

XsltPolicyRule* XsltPolicy::get_policy_rule(int id, std::string& err)
{
    if (id < 0)
    {
        err = "Policy Rule does not exist";
        return NULL;
    }

    for (size_t i = 0; i < nodes.size(); ++i)
    {
        if (!nodes[i])
            continue;

        if (nodes[i]->kind == XSLT_POLICY_RULE && ((XsltPolicyRule*)nodes[i])->id == (size_t)id)
            return (XsltPolicyRule*)nodes[i];

        if (nodes[i]->kind == XSLT_POLICY_POLICY)
        {
            XsltPolicyRule* r = ((XsltPolicy*)nodes[i])->get_policy_rule(id, err);
            if (r)
                return r;
        }
    }

    err = "Policy Rule does not exist";
    return NULL;
}

//---------------------------------------------------------------------------
int XsltPolicy::delete_policy_rule(int rule_id, bool& found, std::string& err)
{
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        if (!nodes[i])
            continue;

        if (nodes[i]->kind == XSLT_POLICY_RULE && ((XsltPolicyRule*)nodes[i])->id == (size_t)rule_id)
        {
            found = true;
            nodes.erase(nodes.begin() + i);
            return 0;
        }
        else if (nodes[i]->kind == XSLT_POLICY_POLICY)
        {
            int ret = ((XsltPolicy*)nodes[i])->delete_policy_rule(rule_id, found, err);
            if (found)
                return ret;
        }
    }

    return -1;
}

//---------------------------------------------------------------------------
int XsltPolicy::delete_policy_rule(int rule_id, std::string& err)
{
    if (rule_id < 0)
    {
        err = "Policy rule does not exist";
        return -1;
    }

    bool found = false;
    int ret = delete_policy_rule(rule_id, found, err);

    if (found)
        return ret;

    err = "Policy rule does not exist";
    return -1;
}

// HELPER
void XsltPolicy::replace_xlmns_in_policy(std::string& xslt)
{
    //replace xmlns:xsl=\"my:namespace\" xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\"
    size_t pos;
    while ((pos = xslt.find("xmlns:xsl=\"my:namespace\"")) != std::string::npos)
        xslt.replace(pos, 24, "xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\"");
}

void XsltPolicy::replace_aliasxsl_in_policy(std::string& xslt)
{
    //replace aliasxsl by xsl
    size_t pos;
    while ((pos = xslt.find("aliasxsl")) != std::string::npos)
        xslt.replace(pos, 8, "xsl");
}

}
