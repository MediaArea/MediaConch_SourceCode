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

class XsltRule
{
public:
    XsltRule() : use_free_text(false), occurrence(1) {}
    ~XsltRule() {}
    XsltRule(const XsltRule&);

    std::string title;

    bool   use_free_text;

    std::string type;
    std::string field;
    int         occurrence;
    std::string ope;
    std::string value;

    std::string text; // Free text

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
    String     import_schema_from_doc(const std::string& filename, xmlDocPtr doc);

    bool       find_xslt_header(xmlNodePtr node);
    bool       find_title_node(xmlNodePtr node, std::string& title);
    bool       find_template_node(xmlNodePtr node);
    bool       find_template_match_node(xmlNodePtr node);
    bool       find_template_name_node(xmlNodePtr node);
    bool       find_mediaconch_node(xmlNodePtr node);
    bool       find_policychecks_node(xmlNodePtr node);
    bool       find_policychecks_title_node(xmlNodePtr node);
    bool       find_policychecks_description_node(xmlNodePtr node);
    bool       find_for_each_node(xmlNodePtr node);
    bool       find_media_node(xmlNodePtr node);
    bool       find_policy_node(xmlNodePtr node);

    bool       find_rule_title_node(xmlNodePtr node, std::string& title);
    bool       find_context_node(xmlNodePtr node, XsltRule* rule);
    bool       find_context_attribute_node(xmlNodePtr node, XsltRule* rule);
    bool       find_context_attribute_value_node(xmlNodePtr node, std::string& value);
    bool       find_context_attribute_field_node(xmlNodePtr node, std::string& value);
    bool       find_choose_node(xmlNodePtr node, XsltRule* rule, bool& valid);
    bool       find_choose_when_node(xmlNodePtr node, XsltRule* rule, bool& valid);
    bool       find_choose_for_each_node(xmlNodePtr node, XsltRule* rule, bool& valid);
    bool       find_choose_call_template_node(xmlNodePtr node, XsltRule* rule, bool& valid);
    bool       find_call_template_free_text_node(xmlNodePtr node, XsltRule* rule, bool& valid);
    bool       find_call_template_xpath_node(xmlNodePtr node, std::string& type);
    bool       find_call_template_value_node(xmlNodePtr node, std::string& field);
    bool       parse_test_for_rule(const std::string& test, XsltRule *rule);

    void       write_root_default_childs(xmlNodePtr node);
    void       write_root_template_childs(xmlNodePtr node);
    void       write_root_output_child(xmlNodePtr node);
    void       write_mediaconch_childs(xmlNodePtr node);
    void       write_mediaconch_attribute_childs(xmlNodePtr node);
    void       write_mediaconch_attribute_text_child(xmlNodePtr node);
    void       write_policychecks_childs(xmlNodePtr node);
    void       write_policychecks_title_child(xmlNodePtr node);
    void       write_policychecks_description_child(xmlNodePtr node);
    void       write_for_each_childs(xmlNodePtr node);
    void       write_media_childs(xmlNodePtr node);
    void       write_media_attribute_childs(xmlNodePtr node);
    void       write_media_attribute_value_child(xmlNodePtr node);
    void       write_policy_childs(xmlNodePtr node, XsltRule *rule);
    void       write_policy_title_child(xmlNodePtr node, XsltRule *rule);
    void       write_policy_context_child(xmlNodePtr node, XsltRule *rule);
    void       write_policy_context_value_child(xmlNodePtr node, XsltRule *rule);
    void       write_policy_context_field_child(xmlNodePtr node, XsltRule *rule);
    void       write_policy_choose_child(xmlNodePtr node, XsltRule *rule);
    void       write_policy_when_child(xmlNodePtr node, XsltRule *rule);
    void       write_policy_otherwise_child(xmlNodePtr node, XsltRule *rule);
    void       write_policy_otherwise_outcome_child(xmlNodePtr node, XsltRule *rule);
    void       write_policy_for_each_child(xmlNodePtr node, XsltRule *rule);
    void       write_policy_call_template_child(xmlNodePtr node, XsltRule *rule);
    void       write_policy_call_template_xpath_child(xmlNodePtr node, XsltRule *rule);
    void       write_policy_call_template_value_child(xmlNodePtr node, XsltRule *rule);
    void       write_policy_call_template_field_child(xmlNodePtr node, XsltRule *rule);
    void       create_test_from_rule(XsltRule *rule, std::string& xpath);
    xmlNsPtr   create_namespace_xsl(xmlNodePtr node);
    xmlNsPtr   create_namespace_mc(xmlNodePtr node);
    xmlNsPtr   create_namespace_ma(xmlNodePtr node);
    xmlNsPtr   create_namespace_mi(xmlNodePtr node);
    xmlNsPtr   create_namespace_xsi(xmlNodePtr node);

    void       write_operators(xmlNodePtr node);
    void       write_operator_is_true(xmlNodePtr node);
    void       write_operator_is_equal(xmlNodePtr node);
    void       write_operator_is_not_equal(xmlNodePtr node);
    void       write_operator_is_greater_than(xmlNodePtr node);
    void       write_operator_is_less_than(xmlNodePtr node);
    void       write_operator_is_greater_or_equal_than(xmlNodePtr node);
    void       write_operator_is_less_or_equal_than(xmlNodePtr node);
    void       write_operator_exists(xmlNodePtr node);
    void       write_operator_does_not_exist(xmlNodePtr node);
    void       write_operator_contains_string(xmlNodePtr node);
    void       write_operator_test_type(xmlNodePtr node);
    void       write_operator_test_streamid(xmlNodePtr node);
    void       write_operator_actual(xmlNodePtr node);
    void       write_operator_choose(xmlNodePtr node, const xmlChar* test,
                                     const xmlChar* pass,
                                     const xmlChar* fail, const xmlChar* reason);
    xmlNodePtr write_operator_new_node(xmlNodePtr node, const xmlChar* title,
                                       std::vector<std::pair<const xmlChar*, const xmlChar*> >& prop,
                                       const xmlChar* content = NULL, bool parentNs=true);
};

}

#endif
