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
        int init();
        int run();
        int finish();

      private:
        Daemon(const Daemon&);
        Daemon& operator=(const Daemon&);

        MediaConchLib  *MCL;
        bool            is_daemon;
        Httpd          *httpd;
        std::string     last_argument;
        std::ofstream  *logger;
        std::streambuf *clog_buffer;

        // Helper
        int daemonize();

        // Parse option helper
        int parse(std::string& argument);
        int parse_help(const std::string& argument);
        int parse_version(const std::string& argument);
        int parse_fork(const std::string& argument);
        int parse_configuration(const std::string& argument);
        int parse_plugins_configuration(const std::string& argument);
        int parse_compression(const std::string& argument);
        int parse_implementationschema(const std::string& argument);
        int parse_implementationverbosity(const std::string& argument);
        int parse_outputlog(const std::string& argument);
        int parse_other(const std::string& argument);

        // Request received callbacks
        static int on_analyze_command(const RESTAPI::Checker_Analyze_Req* req,
                                      RESTAPI::Checker_Analyze_Res& res, void *arg);
        static int on_status_command(const RESTAPI::Checker_Status_Req* req,
                                     RESTAPI::Checker_Status_Res& res, void *arg);
        static int on_report_command(const RESTAPI::Checker_Report_Req* req,
                                     RESTAPI::Checker_Report_Res& res, void *arg);
        static int on_retry_command(const RESTAPI::Checker_Retry_Req* req,
                                    RESTAPI::Checker_Retry_Res& res, void *arg);
        static int on_clear_command(const RESTAPI::Checker_Clear_Req* req,
                                    RESTAPI::Checker_Clear_Res& res, void *arg);
        static int on_list_command(const RESTAPI::Checker_List_Req* req,
                                   RESTAPI::Checker_List_Res& res, void *arg);
        static int on_validate_command(const RESTAPI::Checker_Validate_Req* req,
                                       RESTAPI::Checker_Validate_Res& res, void *arg);
        static int on_file_from_id_command(const RESTAPI::Checker_File_From_Id_Req* req,
                                           RESTAPI::Checker_File_From_Id_Res& res, void *arg);
        static int on_default_values_for_type_command(const RESTAPI::Default_Values_For_Type_Req* req,
                                                      RESTAPI::Default_Values_For_Type_Res& res, void *arg);
        static int on_xslt_policy_create_from_file_command(const RESTAPI::XSLT_Policy_Create_From_File_Req* req,
                                                           RESTAPI::XSLT_Policy_Create_From_File_Res& res, void *arg);

        size_t get_first_free_slot();
        bool id_is_existing(int id) const;
        bool file_is_registered(const std::string& file, size_t& id);
        std::string get_date() const;

        std::vector<std::string*> current_files;
    };

}

#endif
