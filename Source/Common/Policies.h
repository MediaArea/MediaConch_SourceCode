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
// Rule
//***************************************************************************

struct Rule
{
    Rule() {}
    ~Rule() {}
    Rule(const Rule&);

    string description;

    bool   use_free_text;

    string type;
    string field;
    string validator;
    string value;

    string text;

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
    void add_new_rule(string& name, Rule& r);
    void erase_policy(int index);
    xmlDocPtr  create_doc();

    //TODO: parse csv to get the types/fields/validators from file
    void dump_rules_to_stdout();

    vector<pair<string, vector<Rule *> > > rules;

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
    void find_pattern_node(xmlNodePtr node);
    void find_rule_node(xmlNodePtr node, string pattern_name);
    void find_assert_node(xmlNodePtr node, string pattern_name);
    Rule create_rule_from_data(string descr, string data);

    // HELPER for parsing
    bool try_parsing_test(string data, Rule *r);

    string parse_test_value(string& sub, const string& start, const string& after);
    string parse_test_field(string& sub, const string& end);

    bool check_test_type(const string& type);
    bool check_test_field(const string& field);
    bool check_test_validator(const string& validator);

    string     serialize_rule_for_test(Rule *r);
    xmlNodePtr write_pattern(string name, vector<Rule *>& r);
    xmlNodePtr write_rule(Rule *r);
    xmlNodePtr write_assert(Rule *r);
};

#endif
