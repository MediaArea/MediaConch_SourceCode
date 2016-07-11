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

/* TODO: remove */
class SchematronAssert;
class Policy;
class XsltRule;
class Core;

//***************************************************************************
// Class Policies
//***************************************************************************

class Policies
{
public:
    enum PolicyType
    {
        POLICY_SCHEMATRON,
        POLICY_XSLT,
        POLICY_UNKNOWN,
    };

    //Constructor/Destructor
    Policies(Core*);
    ~Policies();

    // Policy
    int         create_xslt_policy(const std::string& name, const std::string& description, std::string& err);
    int         import_policy(const std::string& filename);
    int         import_policy_from_memory(const char* memory, int len, bool is_system_policy);
    int         save_policy(size_t index, std::string& err);
    int         export_policy(const char* filename, size_t pos, std::string& err);
    int         duplicate_policy(int id, std::string& err);
    int         erase_policy(size_t index, std::string& err);

    // Rule
    int         create_policy_rule(int policy_id, std::string& err);
    int         edit_policy_rule(int policy_id, int rule_id, const XsltRule *rule, std::string& err);
    int         duplicate_policy_rule(int policy_id, int rule_id, std::string& err);
    int         delete_policy_rule(int policy_id, int rule_id, std::string& err);

    size_t      create_policy_from_file(const std::string& file);
    bool        policy_exists(const std::string& policy);
    xmlDocPtr   create_doc(size_t pos);

    static bool        try_parsing_test(std::string data, SchematronAssert *r);
    static std::string serialize_assert_for_test(SchematronAssert *r);

    std::vector<Policy *> policies;
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

private:
    Core        *core;
    std::string  error;

    Policies (const Policies&);
    Policies& operator=(const Policies&);

    //Helper
    void find_save_name(const char* base, std::string& save_name);
    void remove_saved_policy(const std::string& saved_name);
};

}

#endif
