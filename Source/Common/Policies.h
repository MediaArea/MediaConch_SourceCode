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
#include <vector>
#include <libxml/tree.h>
using namespace MediaInfoNameSpace;
using namespace std;
//---------------------------------------------------------------------------

//***************************************************************************
// Assert
//***************************************************************************

struct Assert
{
    Assert() {}
    ~Assert() {}
    Assert(const Assert&);

    string description;

    bool   use_free_text;

    string type;
    string field;
    string validator;
    string value;

    string text;

private:
    Assert& operator=(const Assert&);
};

//***************************************************************************
// Rule
//***************************************************************************

struct Rule
{
    Rule() {}
    ~Rule();
    Rule(const Rule&);

    vector<Assert *> asserts;
private:
    Rule& operator=(const Rule&);
};

//***************************************************************************
// Class Policies
//***************************************************************************

class Policies
{
public:
    //Constructor/Destructor
    Policies();
    ~Policies();

    String import_schematron(const char* filename);
    void export_schematron(const char* filename);
    void erase_policy(int index);
    xmlDocPtr  create_doc();
    string     serialize_assert_for_test(Assert *r);
    bool       try_parsing_test(string data, Assert *r);

    //TODO: parse csv to get the types/fields/validators from file
    void dump_pattern_to_stdout();

    vector<pair<string, vector<Rule *> > > patterns;

    //***************************************************************************
    // Type/Field/Validator
    //***************************************************************************

    //TODO: parse csv file
    void create_values_from_csv();

    struct validatorType
    {
        string value;
        string pretty_name;
    };

    list<string> existing_type;
    list<string> existing_field;
    list<validatorType> existing_validator;

private:
    Policies (const Policies&);
    Policies& operator=(const Policies&);

    // HELPER
    void find_patterns_node(xmlNodePtr node);
    void find_rules_node(xmlNodePtr node, vector<Rule *>& rules);
    void find_asserts_node(xmlNodePtr node, vector<Assert *>& asserts);
    Assert* create_assert_from_data(string descr, string data);

    string parse_test_value(string& sub, const string& start, const string& after);
    string parse_test_field(string& sub, const string& end);

    bool check_test_type(const string& type);
    bool check_test_field(const string& field);
    bool check_test_validator(const string& validator);

    xmlNodePtr write_pattern(string name, vector<Rule *>& rules);
    xmlNodePtr write_rule(Rule *r);
    xmlNodePtr write_assert(Assert *r);
};

#endif
