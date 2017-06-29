/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef DaemonH
#define DaemonH

//---------------------------------------------------------------------------
#include <string>
#include <vector>
#include <utility>
#include "MediaInfo/MediaInfo.h"
#include "Common/MediaConchLib.h"
#include "Common/REST_API.h"

//***************************************************************************
// Daemon
//***************************************************************************

namespace MediaConch
{
    class Httpd;

    //--------------------------------------------------------------------------
    class Daemon
    {
      public:
        Daemon();
        ~Daemon();
        static std::string version;

        int parse_args(int ac, char** av);
        int init(std::string& err);
        int run(std::string& err);
        int run_plugins_list(std::string& err);
        int finish();

      private:
        Daemon(const Daemon&);
        Daemon& operator=(const Daemon&);

        MediaConchLib            *MCL;
        bool                      is_daemon;
        Httpd                    *httpd;
        std::string               last_argument;
        std::ofstream            *logger;
        std::streambuf           *clog_buffer;
        std::string               watch_folder;
        std::string               watch_folder_reports;
        long                     *watch_folder_user;
        bool                      watch_folder_recursive;
        std::vector<std::string>  plugins;
        std::vector<std::string>  policies;
        std::vector<std::pair<std::string, std::string> >  options;

        //Mode
        enum daemon_mode
        {
            DAEMON_MODE_DAEMON,
            DAEMON_MODE_PLUGINS_LIST,
        };
        daemon_mode mode;

        // Helper
        int daemonize();

        // Parse option helper
        int parse(std::string& argument);
        int parse_help(const std::string& argument);
        int parse_version(const std::string& argument);
        int parse_fork(const std::string& argument);
        int parse_configuration(const std::string& argument);
        int parse_plugins_configuration(const std::string& argument);
        int parse_plugins_list(const std::string& argument);
        int parse_plugin(const std::string& argument);
        int parse_policy(const std::string& argument);
        int parse_compression(const std::string& argument);
        int parse_implementationschema(const std::string& argument);
        int parse_implementationverbosity(const std::string& argument);
        int parse_outputlog(const std::string& argument);
        int parse_watchfolder(const std::string& argument);
        int parse_watchfolder_reports(const std::string& argument);
        int parse_watchfolder_not_recursive(const std::string& argument);
        int parse_watchfolder_user(const std::string& argument);
        int parse_other(const std::string& argument);

        // Request received callbacks
        //  MediaConch
        static int on_mediaconch_get_plugins_command(const RESTAPI::MediaConch_Get_Plugins_Req* req, RESTAPI::MediaConch_Get_Plugins_Res& res, void *arg);
        static int on_mediaconch_watch_folder_command(const RESTAPI::MediaConch_Watch_Folder_Req* req,
                                                      RESTAPI::MediaConch_Watch_Folder_Res& res, void *arg);
        static int on_mediaconch_list_watch_folders_command(const RESTAPI::MediaConch_List_Watch_Folders_Req* req,
                                                            RESTAPI::MediaConch_List_Watch_Folders_Res& res, void *arg);
        static int on_mediaconch_edit_watch_folder_command(const RESTAPI::MediaConch_Edit_Watch_Folder_Req* req,
                                                           RESTAPI::MediaConch_Edit_Watch_Folder_Res& res, void *arg);
        static int on_mediaconch_remove_watch_folder_command(const RESTAPI::MediaConch_Remove_Watch_Folder_Req* req,
                                                             RESTAPI::MediaConch_Remove_Watch_Folder_Res& res, void *arg);

        //  Checker
        static int on_checker_analyze_command(const RESTAPI::Checker_Analyze_Req* req,
                                              RESTAPI::Checker_Analyze_Res& res, void *arg);
        static int on_checker_status_command(const RESTAPI::Checker_Status_Req* req,
                                             RESTAPI::Checker_Status_Res& res, void *arg);
        static int on_checker_report_command(const RESTAPI::Checker_Report_Req* req,
                                             RESTAPI::Checker_Report_Res& res, void *arg);
        static int on_checker_clear_command(const RESTAPI::Checker_Clear_Req* req,
                                            RESTAPI::Checker_Clear_Res& res, void *arg);
        static int on_checker_stop_command(const RESTAPI::Checker_Stop_Req* req,
                                           RESTAPI::Checker_Stop_Res& res, void *arg);
        static int on_checker_list_command(const RESTAPI::Checker_List_Req* req,
                                           RESTAPI::Checker_List_Res& res, void *arg);
        static int on_checker_validate_command(const RESTAPI::Checker_Validate_Req* req,
                                               RESTAPI::Checker_Validate_Res& res, void *arg);
        static int on_checker_file_from_id_command(const RESTAPI::Checker_File_From_Id_Req* req,
                                                   RESTAPI::Checker_File_From_Id_Res& res, void *arg);
        static int on_checker_id_from_filename_command(const RESTAPI::Checker_Id_From_Filename_Req* req,
                                                       RESTAPI::Checker_Id_From_Filename_Res& res, void *arg);
        static int on_checker_file_information_command(const RESTAPI::Checker_File_Information_Req* req,
                                                       RESTAPI::Checker_File_Information_Res& res, void *arg);
        static int on_default_values_for_type_command(const RESTAPI::Default_Values_For_Type_Req* req,
                                                      RESTAPI::Default_Values_For_Type_Res& res, void *arg);

        //  Policy
        static int on_xslt_policy_create_command(const RESTAPI::XSLT_Policy_Create_Req* req,
                                                 RESTAPI::XSLT_Policy_Create_Res& res, void *arg);
        static int on_policy_import_command(const RESTAPI::Policy_Import_Req* req,
                                            RESTAPI::Policy_Import_Res& res, void *arg);
        static int on_policy_remove_command(const RESTAPI::Policy_Remove_Req* req,
                                            RESTAPI::Policy_Remove_Res& res, void *arg);
        static int on_policy_dump_command(const RESTAPI::Policy_Dump_Req* req,
                                          RESTAPI::Policy_Dump_Res& res, void *arg);
        static int on_policy_save_command(const RESTAPI::Policy_Save_Req* req,
                                          RESTAPI::Policy_Save_Res& res, void *arg);
        static int on_policy_duplicate_command(const RESTAPI::Policy_Duplicate_Req* req,
                                               RESTAPI::Policy_Duplicate_Res& res, void *arg);
        static int on_policy_move_command(const RESTAPI::Policy_Move_Req* req,
                                          RESTAPI::Policy_Move_Res& res, void *arg);
        static int on_policy_change_info_command(const RESTAPI::Policy_Change_Info_Req* req,
                                                 RESTAPI::Policy_Change_Info_Res& res, void *arg);
        static int on_policy_change_type_command(const RESTAPI::Policy_Change_Type_Req* req,
                                                 RESTAPI::Policy_Change_Type_Res& res, void *arg);
        static int on_policy_change_is_public_command(const RESTAPI::Policy_Change_Is_Public_Req* req,
                                                      RESTAPI::Policy_Change_Is_Public_Res& res, void *arg);
        static int on_policy_get_command(const RESTAPI::Policy_Get_Req* req,
                                         RESTAPI::Policy_Get_Res& res, void *arg);
        static int on_policy_get_name_command(const RESTAPI::Policy_Get_Name_Req* req,
                                              RESTAPI::Policy_Get_Name_Res& res, void *arg);
        static int on_policy_get_policies_count_command(const RESTAPI::Policy_Get_Policies_Count_Req* req,
                                                        RESTAPI::Policy_Get_Policies_Count_Res& res, void *arg);
        static int on_policy_clear_policies_command(const RESTAPI::Policy_Clear_Policies_Req* req,
                                                    RESTAPI::Policy_Clear_Policies_Res& res, void *arg);
        static int on_policy_get_policies_command(const RESTAPI::Policy_Get_Policies_Req* req,
                                                  RESTAPI::Policy_Get_Policies_Res& res, void *arg);
        static int on_policy_get_public_policies_command(const RESTAPI::Policy_Get_Public_Policies_Req* req,
                                                         RESTAPI::Policy_Get_Public_Policies_Res& res, void *arg);
        static int on_policy_get_policies_names_list_command(const RESTAPI::Policy_Get_Policies_Names_List_Req* req,
                                                             RESTAPI::Policy_Get_Policies_Names_List_Res& res, void *arg);
        static int on_xslt_policy_create_from_file_command(const RESTAPI::XSLT_Policy_Create_From_File_Req* req,
                                                           RESTAPI::XSLT_Policy_Create_From_File_Res& res, void *arg);
        static int on_xslt_policy_rule_create_command(const RESTAPI::XSLT_Policy_Rule_Create_Req* req,
                                                      RESTAPI::XSLT_Policy_Rule_Create_Res& res, void *arg);
        static int on_xslt_policy_rule_get_command(const RESTAPI::XSLT_Policy_Rule_Get_Req* req,
                                                   RESTAPI::XSLT_Policy_Rule_Get_Res& res, void *arg);
        static int on_xslt_policy_rule_edit_command(const RESTAPI::XSLT_Policy_Rule_Edit_Req* req,
                                                    RESTAPI::XSLT_Policy_Rule_Edit_Res& res, void *arg);
        static int on_xslt_policy_rule_duplicate_command(const RESTAPI::XSLT_Policy_Rule_Duplicate_Req* req,
                                                         RESTAPI::XSLT_Policy_Rule_Duplicate_Res& res, void *arg);
        static int on_xslt_policy_rule_move_command(const RESTAPI::XSLT_Policy_Rule_Move_Req* req,
                                                    RESTAPI::XSLT_Policy_Rule_Move_Res& res, void *arg);
        static int on_xslt_policy_rule_delete_command(const RESTAPI::XSLT_Policy_Rule_Delete_Req* req,
                                                      RESTAPI::XSLT_Policy_Rule_Delete_Res& res, void *arg);

        std::string get_date() const;
    };

}

#endif
