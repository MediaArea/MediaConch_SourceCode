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
class CheckerReport;

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
    int  init(std::string& err);
    int  close();
    void reset();
    bool is_init(std::string& err);

    //***************************************************************************
    // MediaConch General
    //***************************************************************************
    // Plugins
    int  mediaconch_get_plugins(std::vector<std::string>& plugins, std::string& error);
    int  mediaconch_list_watch_folders(std::vector<std::string>& folders, std::string& error);
    int  mediaconch_watch_folder(const std::string& folder, const std::string& folder_reports,
                                 const std::vector<std::string>& plugins, const std::vector<std::string>& policies,
                                 long *in_user, bool recursive,
                                 const std::vector<std::pair<std::string,std::string> >& options,
                                 long& user_id, std::string& error);
    int  mediaconch_edit_watch_folder(const std::string& folder, const std::string& folder_reports,
                                      std::string& error);
    int  mediaconch_remove_watch_folder(const std::string& folder, std::string& error);

    //***************************************************************************
    // Checker
    //***************************************************************************
    // List
    int checker_list(int user, std::vector<std::string>& vec, std::string& error);

    // filename_from_id
    int checker_file_from_id(int user, long id, std::string& filename, std::string& error);
    long checker_id_from_filename(int user, const std::string& filename,
                                  const std::vector<std::pair<std::string,std::string> >& options, std::string& error);
    int checker_file_information(int user, long id, MediaConchLib::Checker_FileInfo& info, std::string& error);

    // default_values_for_type
    int default_values_for_type(const std::string& type, std::vector<std::string>& values, std::string& error);

    // Analyze
    int checker_analyze(int user, const std::string& file, const std::vector<std::string>& plugins,
                        const std::vector<std::pair<std::string,std::string> >& options,
                        bool& registered, bool force_analyze, bool mil_analyze, long& file_id, std::string& error);

    // Status
    int checker_status(int user, long file_id, MediaConchLib::Checker_StatusRes& res, std::string& error);

    // Clear
    int checker_clear(int user, const std::vector<long>& files, std::string& error);

    // Stop
    int checker_stop(int user, const std::vector<long>& files, std::string& error);

    // Report
    int checker_get_report(CheckerReport& c_report, MediaConchLib::Checker_ReportRes* result, std::string& error);
    int checker_validate(int user, MediaConchLib::report report, const std::vector<long>& files,
                         const std::vector<size_t>& policies_ids,
                         const std::vector<std::string>& policies_contents,
                         const std::map<std::string, std::string>& options,
                         std::vector<MediaConchLib::Checker_ValidateRes*>& result, std::string& error);


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
    int policy_dump(int user, int id, bool must_be_public, std::string& memory, std::string& err);

    // save
    int policy_save(int user, int pos, std::string& err);

    // duplicate
    int policy_duplicate(int user, int id, int dst_policy_id, int *dst_user, bool must_be_public, std::string& err);

    // policy move
    int policy_move(int user, int id, int dst_policy_id, std::string& err);

    // change name && description
    int policy_change_info(int user, int id, const std::string& name, const std::string& description,
                           const std::string& license, std::string& err);

    // change type
    int policy_change_type(int user, int id, const std::string& type, std::string& err);

    // change is_public
    int policy_change_is_public(int user, int id, bool is_public, std::string& err);

    // get policy
    int policy_get(int user, int id, const std::string& format, bool must_be_public,
                   MediaConchLib::Get_Policy& policy, std::string& err);

    // get policy name
    int policy_get_name(int user, int id, std::string& name, std::string& err);

    // get the number of policies
    size_t policy_get_policies_count(int user, std::string& error);

    // clear policies
    int policy_clear_policies(int user, std::string& err);

    // get all policies
    int  policy_get_policies(int user, const std::vector<int>&, const std::string& format, MediaConchLib::Get_Policies&,
                             std::string& error);

    // get all public policies
    int policy_get_public_policies(std::vector<MediaConchLib::Policy_Public_Policy*>& policies, std::string& err);

    // get all policies with ID && name
    int  policy_get_policies_names_list(int user, std::vector<std::pair<int, std::string> >&, std::string& err);

    // policy_create_from_file
    int xslt_policy_create_from_file(int user, long id, std::string& err);

    // create XSLT rule
    int xslt_policy_rule_create(int user, int policy_id, std::string& err);

    // get XSLT rule
    XsltPolicyRule *xslt_policy_rule_get(int user, int policy_id, int id, std::string& err);

    // edit XSLT rule
    int xslt_policy_rule_edit(int user, int policy_id, int rule_id, const XsltPolicyRule *rule, std::string& err);

    // duplicate XSLT rule
    int xslt_policy_rule_duplicate(int user, int policy_id, int rule_id, int dst_policy_id, std::string& err);

    // move XSLT rule
    int xslt_policy_rule_move(int user, int policy_id, int rule_id, int dst_policy_id, std::string& err);

    // delete XSLT rule
    int xslt_policy_rule_delete(int user, int policy_id, int rule_id, std::string& err);


private:
    MediaConchLib *mcl;
    Http          *http_client;

    DaemonClient(const DaemonClient&);
    DaemonClient& operator=(const DaemonClient&);
};

}

#endif // !DaemonClientH
