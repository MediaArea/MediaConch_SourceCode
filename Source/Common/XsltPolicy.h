/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
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
// XsltRule
//***************************************************************************

struct XsltRule
{
    XsltRule() : occurence(1) {}
    ~XsltRule() {}
    XsltRule(const XsltRule&);

    std::string title;

    bool   use_free_text;

    std::string type;
    std::string field;
    int         occurence;
    std::string ope;
    std::string value;

    std::string text;

private:
    XsltRule& operator=(const XsltRule&);
};

//***************************************************************************
// Policy
//***************************************************************************

class XsltPolicy : public Policy
{
public:
    XsltPolicy() : Policy(Policies::POLICY_XSLT) {}
    XsltPolicy(const XsltPolicy*);
    virtual ~XsltPolicy();
    std::vector<XsltRule *> rules;
    // TODO
    /* std::vector<XsltOp *> operations; */
    xmlDocPtr  create_doc();

private:
    // HELPER
    String import_schema_from_doc(const char* filename, xmlDocPtr doc);

    bool find_title_node(xmlNodePtr node, std::string& title);
    void find_rules_node(xmlNodePtr node, std::vector<XsltRule *>& rules);

    void       write_root_default_childs(xmlNodePtr node);
    void       write_root_template_childs(xmlNodePtr node);
    void       write_root_output_child(xmlNodePtr node);
    void       write_mediaconch_childs(xmlNodePtr node);
    void       write_policychecks_childs(xmlNodePtr node);
    void       write_policychecks_attribute_childs(xmlNodePtr node);
    void       write_policychecks_attribute_text_child(xmlNodePtr node);
    void       write_for_each_childs(xmlNodePtr node);
    void       write_media_childs(xmlNodePtr node);
    void       write_media_attribute_childs(xmlNodePtr node);
    void       write_media_attribute_value_child(xmlNodePtr node);
    void       write_policy_childs(xmlNodePtr node, XsltRule *rule);
    void       write_rule_child(xmlNodePtr node, XsltRule *rule);
    void       write_rule_title_child(xmlNodePtr node, XsltRule *rule);
    void       write_rule_type_child(xmlNodePtr node, XsltRule *rule);
    void       write_rule_xpath_child(xmlNodePtr node, XsltRule *rule);
    void       write_rule_value_child(xmlNodePtr node, XsltRule *rule);
    void       write_rule_occurence_child(xmlNodePtr node, XsltRule *rule);
    void       write_rule_field_child(xmlNodePtr node, XsltRule *rule);
    void       create_xpath_from_rule(XsltRule *rule, std::string& xpath);
    xmlNsPtr   create_namespace_xsl(xmlNodePtr node);
    xmlNsPtr   create_namespace_mc(xmlNodePtr node);
    xmlNsPtr   create_namespace_xsi(xmlNodePtr node);
};

}

#endif
