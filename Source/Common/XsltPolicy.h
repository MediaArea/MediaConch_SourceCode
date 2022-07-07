/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Policies functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef XsltPolicyH
#define XsltPolicyH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifdef MEDIAINFO_DLL_RUNTIME
    #include "MediaInfoDLL/MediaInfoDLL.h"
    #define MediaInfoNameSpace MediaInfoDLL
#elif defined MEDIAINFO_DLL_STATIC
    #include "MediaInfoDLL/MediaInfoDLL_Static.h"
    #define MediaInfoNameSpace MediaInfoDLL
#else
    #include "MediaInfo/MediaInfoList.h"
    #define MediaInfoNameSpace MediaInfoLib
#endif
#include <list>
#include <vector>
#include <libxml/tree.h>
#include "Policy.h"
using namespace MediaInfoNameSpace;
//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// XsltPolicyElement
//***************************************************************************

enum XsltPolicyElement
{
    XSLT_POLICY_POLICY,
    XSLT_POLICY_RULE,
};

//***************************************************************************
// XsltPolicyNode
//***************************************************************************

class XsltPolicyNode
{
public:
    XsltPolicyNode();
    virtual ~XsltPolicyNode();

    XsltPolicyNode(const XsltPolicyNode*);
    XsltPolicyNode(const XsltPolicyNode&);

    XsltPolicyElement kind;
    std::string       node_name;
    size_t            parent_id;

private:
    XsltPolicyNode& operator=(const XsltPolicyNode&);
};

//***************************************************************************
// XsltPolicyRule
//***************************************************************************

class XsltPolicyRule : public XsltPolicyNode
{
public:
    XsltPolicyRule();
    virtual ~XsltPolicyRule();
    XsltPolicyRule(const XsltPolicyRule*);
    XsltPolicyRule(const XsltPolicyRule&);

    int edit_policy_rule(const XsltPolicyRule* rule, std::string&);
    void create_default_name(std::string& name);

    size_t        id;
    std::string   ope;
    std::string   track_type;
    std::string   field;
    std::string   scope;
    std::string   level;
    std::string   occurrence;
    std::string   value;

    static size_t rule_id;
private:
    XsltPolicyRule& operator=(const XsltPolicyRule&);
};


//***************************************************************************
// XsltPolicy
//***************************************************************************

class XsltPolicy : public Policy, public XsltPolicyNode
{
public:
    XsltPolicy(Policies *p, bool no_https);
    virtual ~XsltPolicy();

    // use by duplicate
    XsltPolicy(const XsltPolicy*);
    XsltPolicy(const XsltPolicy&, bool is_system);

    int             create_policy_from_mi(const std::string& report);
    XsltPolicyRule* get_policy_rule(int id, std::string& err);
    int             get_final_xslt(std::string& xslt, const std::map<std::string, std::string>& opts);
    int             delete_policy_rule(int rule_id, std::string& err);

    //TODO
    std::vector<XsltPolicyNode*>  nodes;
    std::string                   ope;
private:

    XsltPolicy& operator=(const XsltPolicy&);

    xmlDocPtr create_doc();
    int       import_schema_from_doc(xmlDocPtr doc, const std::string& filename);

    int       edit_policy_rule(int rule_id, const XsltPolicyRule *rule, std::string& err);

    //parse
    int run_over_siblings_nodes(xmlNodePtr node, bool is_root, XsltPolicy* current);
    int find_policy_node(xmlNodePtr node, bool is_root, XsltPolicy* current);
    int parse_policy_policy(xmlNodePtr node, bool is_root, XsltPolicy* current);
    int parse_policy_rule(xmlNodePtr node, bool is_root, XsltPolicy* current);

    //serialize
    int write_root_nodes_children(xmlDocPtr doc);
    int write_nodes_children(xmlNodePtr node, XsltPolicy *current);
    int create_node_policy_child(xmlNodePtr& node, XsltPolicy *current);
    int create_node_rule_child(xmlNodePtr& node, XsltPolicyRule *current);

    //create policy from mi
    int find_media_track_node(xmlNodePtr node);
    int find_media_track_node(xmlNodePtr node, std::string& type);
    int create_rule_from_media_track_child(xmlNodePtr node, const std::string& type, const std::string& prefix=std::string());

    // HELPER
    void replace_xlmns_in_policy(std::string& xslt);
    void replace_aliasxsl_in_policy(std::string& xslt);
    int  delete_policy_rule(int rule_id, bool& found, std::string& err);
};

}

#endif
