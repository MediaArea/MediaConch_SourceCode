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
    XsltRule() {}
    ~XsltRule() {}
    XsltRule(const XsltRule&);

    std::string description;

    bool   use_free_text;

    std::string type;
    std::string field;
    std::string validator;
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

    xmlNodePtr write_title(std::string& title);
    xmlNodePtr write_rule(XsltRule *r);
};

}

#endif
