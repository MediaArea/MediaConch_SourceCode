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
#ifndef PoliciesH
#define PoliciesH
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
#include <map>
#include <vector>
#include <libxml/tree.h>
using namespace MediaInfoNameSpace;
//---------------------------------------------------------------------------

namespace MediaConch {

class Policy;
class XsltPolicy;
class XsltPolicyRule;
class Core;

//***************************************************************************
// Class Policies
//***************************************************************************

class Policies
{
public:
    enum PolicyType
    {
        POLICY_XSLT,
        POLICY_UNKNOWN,
    };

    //Constructor/Destructor
    Policies(Core*);
    ~Policies();

    // Policy
    int         create_xslt_policy(int parent_id, std::string& err);
    int         import_policy_from_memory(const std::string& memory, std::string& err, const char* filename, bool is_system_policy);
    int         duplicate_policy(int id, std::string& err);
    int         create_xslt_policy_from_file(const std::string& file, std::string& err);

    int         save_policy(int id, std::string& err);
    int         export_policy(const char* filename, int id, std::string& err);
    int         dump_policy_to_memory(int pos, std::string& memory, std::string& err);

    int         policy_change_name(int id, const std::string& name, const std::string& description, std::string& err);

    int         erase_policy(int index, std::string& err);
    void        clear_policies();

    size_t      get_policies_size() const { return policies.size(); };
    Policy*     get_policy(int pos, std::string& err);
    void        get_policies(std::vector<std::pair<size_t, std::string> >& ps);

    int         policy_get_policies(const std::vector<size_t>* policies_ids,
                                    const std::vector<std::string>* policies_contents,
                                    std::vector<std::string>& xslt_policies, std::string& err);

    // Rule
    int         create_xslt_policy_rule(int policy_id, std::string& err);
    int         edit_xslt_policy_rule(int policy_id, int rule_id, const XsltPolicyRule *rule, std::string& err);
    int         duplicate_xslt_policy_rule(int policy_id, int rule_id, std::string& err);
    int         delete_xslt_policy_rule(int policy_id, int rule_id, std::string& err);

    bool        policy_exists(const std::string& policy);
    xmlDocPtr   create_doc(int id);

    std::string get_error() const { return error; }
    //***************************************************************************
    // Type/Field/Validator
    //***************************************************************************

    void create_values_from_csv();

    struct validatorType
    {
        std::string value;
        std::string name;
        std::string pretty_name;
    };

    static std::map<std::string, std::list<std::string> > existing_type;
    static std::list<validatorType>                       existing_validator;
    static std::list<std::string>                         existing_xsltOperator;

    static std::string parse_test_value(std::string& sub, const std::string& start, const std::string& after);
    static std::string parse_test_field(std::string& sub, const std::string& end);

    static bool check_test_type(const std::string& type);
    static bool check_test_field(const std::string& field);
    static bool check_test_validator(const std::string& validator);

    size_t get_an_id() { return policy_global_id++; }

    //***************************************************************************
    // HELPER
    //***************************************************************************

    int transform_with_xslt_memory(const std::string& report, const std::string& memory,
                                   std::string& result);

private:
    Core                       *core;
    std::string                 error;
    std::map<size_t, Policy*>   policies;

    static size_t               policy_global_id;
    //mutex?

    Policies (const Policies&);
    Policies& operator=(const Policies&);

    //Helper
    void find_save_name(const char* base, std::string& save_name);
    void find_new_policy_name(std::string& title);
    void remove_saved_policy(const Policy* policy);
    XsltPolicyRule* get_xslt_policy_rule(XsltPolicy* policy, int id);
    int policy_get_policy_id(Policy* p, std::vector<std::string>& xslt_policies, std::string& err);
    int policy_get_policy_content(const std::string& policy, std::vector<std::string>& xslt_policies, std::string& err);
};

}

#endif
