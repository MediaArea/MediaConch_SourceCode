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
    };

    //Constructor/Destructor
    Policies(Core*);
    ~Policies();

    std::string import_schema(const std::string& filename);
    std::string import_schema_from_memory(const std::string& filename, const char* memory, int len);
    void        export_schema(const char* filename, size_t pos);
    xmlDocPtr   create_doc(size_t pos);
    void        erase_policy(size_t index);
    bool        policy_exists(std::string policy);

    static bool        try_parsing_test(std::string data, SchematronAssert *r);
    static std::string serialize_assert_for_test(SchematronAssert *r);

    std::vector<Policy *> policies;
    //***************************************************************************
    // Type/Field/Validator
    //***************************************************************************

    void create_values_from_csv();

    struct validatorType
    {
        std::string value;
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
    Core *core;

    Policies (const Policies&);
    Policies& operator=(const Policies&);
};

}

#endif
