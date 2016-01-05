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
        void add_policy(const std::string& policy);
        void set_display_file(const std::string& file);
        int  register_option(const std::string& opt);
        void set_use_daemon(bool use) { use_daemon = use; }
        void set_configuration_file(const std::string& file);
        void set_implementation_schema_file(const std::string& file);
        int  set_compression_mode(const std::string& mode_str);

      private:
        CLI(const CLI&);
        CLI& operator=(const CLI&);

        int is_ready(size_t i);

        MediaConchLib MCL;
        std::vector<std::string> files;
        std::vector<std::string> policies;
        std::string              display_file;
        std::string              configuration_file;
        std::bitset<MediaConchLib::report_Max> report_set;
        MediaConchLib::format format;
        bool use_daemon;
    };

}

#endif
