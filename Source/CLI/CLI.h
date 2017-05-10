/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef CLIH
#define CLIH

//---------------------------------------------------------------------------
#include <string>
#include <vector>
#include <utility>
#include "MediaInfo/MediaInfo.h"
#include "Common/MediaConchLib.h"

//***************************************************************************
// CLI
//***************************************************************************

namespace MediaConch
{
    //--------------------------------------------------------------------------
    class CLI
    {
      public:
        CLI();
        ~CLI();

        int  parse_args(const std::vector<std::string>& args);
        int  init(std::string& err);
        int  run(std::string& err);
        int  finish();
        void set_report_set(std::string& report_kind);
        void set_report_reset();
        int  set_format(MediaConchLib::format f);
        int  set_format(const std::string& f);
        int  add_policy(const std::string& filename);
        void set_display_file(const std::string& file);
        int  register_option(const std::string& opt, std::string& value);
        void set_use_daemon(bool use) { use_daemon = use; }
        void set_configuration_file(const std::string& file);
        void set_plugins_configuration_file(const std::string& file);
        void set_implementation_schema_file(const std::string& file);
        void set_implementation_verbosity(const std::string& verbosity);
        int  set_policy_reference_file(const std::string& file);
        int  add_plugin_to_use(const std::string& plugin);
        int  set_user_to_use(const std::string& user);
        int  set_compression_mode(const std::string& mode_str);
        void set_force_analyze(bool force);
        void set_mil_analyze(bool analyze);
        void set_asynchronous(bool async);
        void set_create_policy_mode();
        void set_file_information_mode();
        void set_plugins_list_mode();
        int  get_values_for_type_field(const std::string& type, const std::string& field, std::vector<std::string>& values);
        int  set_watch_folder(const std::string& folder);
        int  set_watch_folder_reports(const std::string& folder);
        int  set_watch_folder_not_recursive();
        int  set_watch_folder_user(const std::string& user);
        void set_list_watch_folders_mode();
        void set_list_mode();

      private:
        CLI(const CLI&);
        CLI& operator=(const CLI&);

        int  run_create_policy(const std::vector<long>& files_ids);
        int  run_policy_reference_file(long file_id, std::string& err);
        int  run_file_information(std::string& err);
        int  run_plugins_list(std::string& err);
        int  run_watch_folders_list(std::string& err);
        int  run_watch_folder_cmd(std::string& err);
        int  is_ready(long file_id, std::vector<long>& file_ids, MediaConchLib::report& report_kind, std::string& err);
        void add_files_recursively(const std::string& filename);
        void file_info_report(const MediaConchLib::Checker_FileInfo* info, std::string& report);
        int  run_list_files(std::string& err);

        MediaConchLib MCL;
        std::vector<std::string> files;
        std::vector<std::string> policies;
        std::vector<std::string> plugins;
        std::vector<std::pair<std::string,std::string> > options;
        std::string              error;
        std::string              display_content;
        std::string              configuration_file;
        std::string              plugins_configuration_file;
        std::string              policy_reference_file;
        std::string              watch_folder;
        std::string              watch_folder_reports;
        long                    *watch_folder_user;
        std::bitset<MediaConchLib::report_Max> report_set;
        MediaConchLib::format   format;
        //TODO: clean
        long                    use_as_user;
        bool                    use_daemon;
        bool                    asynchronous;
        bool                    force_analyze;
        bool                    mil_analyze;
        bool                    watch_folder_recursive;
        bool                    create_policy_mode;
        bool                    file_information;
        bool                    plugins_list_mode;
        bool                    list_watch_folders_mode;
        bool                    no_needs_files_mode;
        bool                    list_mode;
    };

}

#endif
