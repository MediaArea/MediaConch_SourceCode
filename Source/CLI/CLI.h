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
        int  init();
        int  run();
        int  finish();
        void set_report_set(std::string& report_kind);
        void set_report_reset();
        int  set_format(MediaConchLib::format f);
        int  set_format(const std::string& f);
        int  add_policy(const std::string& filename);
        void set_display_file(const std::string& file);
        int  register_option(const std::string& opt);
        void set_use_daemon(bool use) { use_daemon = use; }
        void set_configuration_file(const std::string& file);
        void set_plugins_configuration_file(const std::string& file);
        void set_implementation_schema_file(const std::string& file);
        void set_implementation_verbosity(const std::string& verbosity);
        int  set_policy_reference_file(const std::string& file);
        int  set_compression_mode(const std::string& mode_str);
        void set_force_analyze(bool force);
        void set_asynchronous(bool async);
        void set_create_policy_mode();
        int  get_values_for_type_field(const std::string& type, const std::string& field, std::vector<std::string>& values);

        void print_error(MediaConchLib::errorHttp code);

      private:
        CLI(const CLI&);
        CLI& operator=(const CLI&);

        int  run_create_policy(const std::vector<long>& files_ids);
        int  run_policy_reference_file(long file_id);
        int  is_ready(long file_id, MediaConchLib::report& report_kind);
        void add_files_recursively(const std::string& filename);

        MediaConchLib MCL;
        std::vector<std::string> files;
        std::vector<std::string> policies;
        std::string              error;
        std::string              display_file;
        std::string              configuration_file;
        std::string              plugins_configuration_file;
        std::string              policy_reference_file;
        std::bitset<MediaConchLib::report_Max> report_set;
        MediaConchLib::format   format;
        bool                    use_daemon;
        bool                    asynchronous;
        bool                    force_analyze;
        bool                    create_policy_mode;
    };

}

#endif
