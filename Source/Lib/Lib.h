/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef LibH
#define LibH

//---------------------------------------------------------------------------
#include "Common/MediaConchLib.h"

#include <string>
#include <vector>

//***************************************************************************
// Lib
//***************************************************************************

namespace MediaConch
{
    //--------------------------------------------------------------------------
    class Lib
    {
      public:
        Lib();
        ~Lib();

        const std::string get_last_error               ();
        long              add_file                     (const std::string& filename);
        void              add_report                   (MediaConchLib::report report);
        void              add_policy                   (const std::string& filename);
        void              set_format                   (MediaConchLib::format format);
        void              set_display                  (const std::string& filename);
        void              set_mil_option               (const std::string& name, const std::string& value);
        void              set_force_analyze            (bool force);
        void              set_policy_verbosity         (int verbosity);
        void              set_implementation_verbosity (int verbosity);
        const std::string get_report                   (long file_id);

      private:
        bool              get_file_content             (const std::string& filename, std::string& content, std::string& err);
        int               is_ready                     (long file_id, MediaConchLib::report& report_kind, std::string& err);

        MediaConchLib MCL;
        bool force_analyze;
        std::string display;
        std::string last_error;
        std::vector<long> files;
        std::string policy_verbosity;
        MediaConchLib::format format;
        std::vector<std::string> policies;
        std::bitset<MediaConchLib::report_Max> report_set;
        std::vector<std::pair<std::string,std::string> > options;
    };
}

#endif
