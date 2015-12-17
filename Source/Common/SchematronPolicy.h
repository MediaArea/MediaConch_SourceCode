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
#ifndef SchematronPolicyH
#define SchematronPolicyH
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
// SchematronAssert
//***************************************************************************

class SchematronAssert
{
public:
    SchematronAssert() {}
    ~SchematronAssert() {}
    SchematronAssert(const SchematronAssert&);

    std::string description;

    bool   use_free_text;

    std::string type;
    std::string field;
    std::string validator;
    std::string value;

    std::string text;

private:
    SchematronAssert& operator=(const SchematronAssert&);
};

//***************************************************************************
// SchematronRule
//***************************************************************************

struct SchematronRule
{
    SchematronRule() {}
    ~SchematronRule();
    SchematronRule(const SchematronRule&);

    std::vector<SchematronAssert *> asserts;
private:
    SchematronRule& operator=(const SchematronRule&);
};

//***************************************************************************
// SchematronPattern
//***************************************************************************

struct SchematronPattern
{
    SchematronPattern() {}
    ~SchematronPattern();
    SchematronPattern(const SchematronPattern&);

    std::string name;
    std::vector<SchematronRule *> rules;
private:
    SchematronPattern& operator=(const SchematronPattern&);
};

//***************************************************************************
// Policy
//***************************************************************************

class SchematronPolicy : public Policy
{
public:
    SchematronPolicy() : Policy(Policies::POLICY_SCHEMATRON) {}
    SchematronPolicy(const SchematronPolicy*);
    virtual ~SchematronPolicy();
    std::vector<SchematronPattern *> patterns;
    xmlDocPtr  create_doc();

private:
    // HELPER
    int import_schema_from_doc(const std::string& filename, xmlDocPtr doc);

    bool find_schematron_header(xmlNodePtr node);
    bool find_title_node(xmlNodePtr node, std::string& title);
    void find_patterns_node(xmlNodePtr node, std::vector<SchematronPattern *>& patterns);
    void find_rules_node(xmlNodePtr node, std::vector<SchematronRule *>& rules);
    void find_asserts_node(xmlNodePtr node, std::vector<SchematronAssert *>& asserts);
    SchematronAssert* create_assert_from_data(std::string descr, std::string data);

    xmlNodePtr write_ns();
    xmlNodePtr write_title(std::string& title);
    xmlNodePtr write_pattern(SchematronPattern *p);
    xmlNodePtr write_rule(SchematronRule *r);
    xmlNodePtr write_assert(SchematronAssert *r);
};

}

#endif
