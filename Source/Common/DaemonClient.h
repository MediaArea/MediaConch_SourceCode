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

    // List
    int list(std::vector<std::string>& vec);

    // file_from_id
    int file_from_id(int id, std::string& filename);

    // Analyze
    int analyze(const std::string& file, bool& registered, bool force_analyze);

    // Status
    int is_done(const std::string& file, double& done);

    // Report
    int get_report(const std::bitset<MediaConchLib::report_Max>& report_set, MediaConchLib::format f,
                   const std::vector<std::string>& files,
                   const std::vector<std::string>& policies_names,
                   const std::vector<std::string>& policies_contents,
                   const std::map<std::string, std::string>& options,
                   MediaConchLib::ReportRes* result,
                   const std::string* display_name = NULL,
                   const std::string* display_content = NULL);
    int validate(MediaConchLib::report report, const std::vector<std::string>& files,
                 const std::vector<std::string>& policies_names,
                 const std::vector<std::string>& policies_contents,
                 std::vector<MediaConchLib::ValidateRes*>& result);

private:
    MediaConchLib *mcl;
    Http          *http_client;

    DaemonClient(const DaemonClient&);
    DaemonClient& operator=(const DaemonClient&);

    std::map<std::string, int> file_ids;
};

}

#endif // !DaemonClientH
