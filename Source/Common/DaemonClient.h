/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// JSON functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef DaemonClientH
#define DaemonClientH

//---------------------------------------------------------------------------
#include <string>
#include <map>
#include <bitset>
#include "MediaConchLib.h"

//---------------------------------------------------------------------------
namespace MediaConch
{

class Http;

//***************************************************************************
// Class DaemonClient
//***************************************************************************

class DaemonClient
{
public:
    //Constructor/Destructor
    DaemonClient(MediaConchLib* m);
    ~DaemonClient();

    // General
    int init();
    int close();

    //***************************************************************************
    // Checker
    //***************************************************************************
    // List
    int checker_list(std::vector<std::string>& vec);

    // file_from_id
    int checker_file_from_id(int id, std::string& filename);

    // default_values_for_type
    int default_values_for_type(const std::string& type, std::vector<std::string>& values);

    // Analyze
    int checker_analyze(const std::string& file, bool& registered, bool force_analyze);

    // Status
    int checker_is_done(const std::string& file, double& done, MediaConchLib::report& report_kind);

    // Report
    int checker_get_report(int user, const std::bitset<MediaConchLib::report_Max>& report_set, MediaConchLib::format f,
                           const std::vector<std::string>& files,
                           const std::vector<size_t>& policies_names,
                           const std::vector<std::string>& policies_contents,
                           const std::map<std::string, std::string>& options,
                           MediaConchLib::Checker_ReportRes* result,
                           const std::string* display_name = NULL,
                           const std::string* display_content = NULL);
    int checker_validate(int user, MediaConchLib::report report, const std::vector<std::string>& files,
                         const std::vector<size_t>& policies_ids,
                         const std::vector<std::string>& policies_contents,
                         std::vector<MediaConchLib::Checker_ValidateRes*>& result);


    //***************************************************************************
    // Policy
    //***************************************************************************
    // create XSLT
    int xslt_policy_create(int user, const std::string& type, int parent_id, std::string& err);

    // import
    int policy_import(int user, const std::string& memory, std::string& err);

    // remove
    int policy_remove(int user, int id, std::string& err);

    // remove
    int policy_dump(int user, int id, std::string& memory, std::string& err);

    // save
    int policy_save(int user, int pos, std::string& err);

    // duplicate
    int policy_duplicate(int user, int id, std::string& err);

    // change name && description
    int policy_change_name(int user, int id, const std::string& name, const std::string& description, std::string& err);

    // get policy
    MediaConchLib::Policy_Policy* policy_get(int user, int id, const std::string& format, std::string& err);

    // get policy name
    int policy_get_name(int user, int id, std::string& name, std::string& err);

    // get the number of policies
    size_t policy_get_policies_count(int user);

    // clear policies
    int policy_clear_policies(int user, std::string& err);

    // get all policies
    void policy_get_policies(int user, const std::vector<int>&, const std::string& format, MediaConchLib::Get_Policies&);

    // get all policies with ID && name
    void policy_get_policies_names_list(int user, std::vector<std::pair<int, std::string> >&);

    // policy_create_from_file
    int xslt_policy_create_from_file(int user, const std::string& id);

    // create XSLT rule
    int xslt_policy_rule_create(int user, int policy_id, std::string& err);

    // get XSLT rule
    XsltPolicyRule *xslt_policy_rule_get(int user, int policy_id, int id, std::string& err);

    // edit XSLT rule
    int xslt_policy_rule_edit(int user, int policy_id, int rule_id, const XsltPolicyRule *rule, std::string& err);

    // duplicate XSLT rule
    int xslt_policy_rule_duplicate(int user, int policy_id, int rule_id, std::string& err);

    // delete XSLT rule
    int xslt_policy_rule_delete(int user, int policy_id, int rule_id, std::string& err);


private:
    MediaConchLib *mcl;
    Http          *http_client;

    DaemonClient(const DaemonClient&);
    DaemonClient& operator=(const DaemonClient&);

    std::map<std::string, int> file_ids;
};

}

#endif // !DaemonClientH
