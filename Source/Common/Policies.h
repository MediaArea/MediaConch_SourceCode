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
#include "MediaConchLib.h"
using namespace MediaInfoNameSpace;
//---------------------------------------------------------------------------

namespace MediaConch {

class Policy;
class XsltPolicy;
class XsltPolicyRule;
class XsltPolicyNode;
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
    int         create_xslt_policy(int user, const std::string& type, int parent_id, std::string& err);
    int         import_policy_from_memory(int user, const std::string& memory, std::string& err, const char* filename, bool is_system_policy);
    int         import_policy_from_file(int user, const std::string& file, std::string& err);
    int         duplicate_policy(int user, int id, int dst_policy_id, int *dst_user, bool must_be_public, std::string& err, bool copy_name=true);
    int         move_policy(int user, int id, int dst_policy_id, std::string& err);
    int         create_xslt_policy_from_file(int user, long file, std::string& err);

    int         save_policy(int user, int id, std::string& err);
    int         export_policy(int user, const char* filename, int id, std::string& err, bool is_save=false);
    int         dump_policy_to_memory(int user, int pos, bool must_be_public, std::string& memory, std::string& err);

    int         policy_change_info(int user, int id, const std::string& name, const std::string& description,
                                   const std::string& license, std::string& err);
    int         policy_change_type(int user, int id, const std::string& type, std::string& err);
    int         policy_change_is_public(int user, int id, bool is_public, std::string& err);

    int         erase_policy(int user, int index, std::string& err);
    int         clear_policies(int user, std::string& err);

    size_t      get_policies_size(int user, std::string& err);
    Policy*     get_policy(int user, int pos, std::string& err);
    int         policy_get_name(int user, int id, std::string& name, std::string& err);
    int         get_policies(int user, const std::vector<int>&, const std::string& format,
                             MediaConchLib::Get_Policies& ps, std::string& err);
    int         get_public_policies(std::vector<MediaConchLib::Policy_Public_Policy*>& policies, std::string& err);
    int         get_policies_names_list(int user, std::vector<std::pair<int, std::string> >& ps, std::string& err);
    int         policy_get(int user, int pos, const std::string& format, bool must_be_public,
                           MediaConchLib::Get_Policy& policy, std::string& err);

    int         policy_get_policies(int user, const std::vector<size_t>* policies_ids,
                                    const std::vector<std::string>* policies_contents,
                                    const std::map<std::string, std::string>& opts,
                                    std::vector<std::string>& xslt_policies, std::string& err);

    // Rule
    int         create_xslt_policy_rule(int user, int policy_id, std::string& err);
    XsltPolicyRule *get_xslt_policy_rule(int user, int policy_id, int id, std::string& err);
    int         edit_xslt_policy_rule(int user, int policy_id, int rule_id, const XsltPolicyRule *rule, std::string& err);
    int         duplicate_xslt_policy_rule(int user, int policy_id, int rule_id, int dst_policy_id, std::string& err, bool copy_name=true);
    int         move_xslt_policy_rule(int user, int policy_id, int rule_id, int dst_policy_id, std::string& err);
    int         delete_xslt_policy_rule(int user, int policy_id, int rule_id, std::string& err);

    bool        policy_exists(int user, const std::string& policy);
    xmlDocPtr   create_doc(int user, int id);

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
                                   const std::map<std::string, std::string>& opts, std::string& result);
    void add_system_policies_to_user_policies(int user);
    void add_recursively_policy_to_user_policies(int user, Policy* p);

private:
    Core                                      *core;
    std::string                                error;
    std::map<int, std::map<size_t, Policy*> >  policies;
    std::vector<Policy*>                       system_policies;

    static size_t                              policy_global_id;
    //mutex?

    Policies (const Policies&);
    Policies& operator=(const Policies&);

    //Helper
    void unified_file_name(std::string& filename);
    void delete_xslt_sub_policy(std::map<size_t, Policy*>& policies, XsltPolicy* p);
    void find_save_name(int user, const char* base, std::string& save_name, const char* filename = NULL);
    void find_new_policy_name(int user, std::string& title);
    int remove_policy(int user, int id, std::string& err);
    void remove_saved_policy(const Policy* policy);
    XsltPolicyRule* get_xslt_policy_rule(XsltPolicy* policy, int id);
    int policy_get_policy_id(Policy* p, const std::map<std::string, std::string>& opts,
                             std::vector<std::string>& xslt_policies, std::string& err);
    int policy_get_policy_content(const std::string& policy, const std::map<std::string, std::string>& opts,
                                  std::vector<std::string>& xslt_policies, std::string& err);
    int erase_xslt_policy_node(std::map<size_t, Policy *>& user_policies, int id, std::string& err);
    MediaConchLib::Policy_Policy *policy_to_mcl_policy(Policy *p, std::string& err);
    MediaConchLib::Policy_Policy* xslt_policy_to_mcl_policy(XsltPolicy *policy, std::string&);
    int xslt_policy_child_to_mcl_policy(XsltPolicyNode *node, MediaConchLib::Policy_Policy *, std::string&);
    MediaConchLib::XSLT_Policy_Rule* xslt_policy_rule_to_mcl_policy(XsltPolicyRule *rule, std::string& error);
};

}

#endif
