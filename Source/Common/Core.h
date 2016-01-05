/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Core functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef CoreH
#define CoreH
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
#include <map>
#include <bitset>
#include <vector>
#include "MediaConchLib.h"
#include "Policies.h"
#include "Policy.h"
#include "Configuration.h"
#include "Scheduler.h"

using namespace MediaInfoNameSpace;
using namespace std;
//---------------------------------------------------------------------------

namespace MediaConch {

class Schema;
class Database;

//***************************************************************************
// Class Core
//***************************************************************************

class Core
{
public:
    //Constructor/Destructor
    Core();
    ~Core();

    //Menu
    void    Menu_Option_Preferences_Inform  (const MediaInfoNameSpace::String &Inform);
    String  Menu_Option_Preferences_Option  (const MediaInfoNameSpace::String &Param, const MediaInfoNameSpace::String &Value);

    void        Close();
    void        Run(std::string file = std::string());
    bool        is_done(const std::string& file, double& percent_done);
    int         open_file(const std::string& filename, bool& registered);
    int         remove_report(const std::vector<std::string>& files);
    int         get_report(const std::bitset<MediaConchLib::report_Max>& Report, MediaConchLib::format f,
                           const std::vector<std::string>& files,
                           const std::vector<std::string>& policies_names,
                           const std::vector<std::string>& policies_contents,
                           MediaConchLib::ReportRes* res,
                           const std::string* display_names = NULL,
                           const std::string* display_contents = NULL);
    int         get_reports_output_Text_Implementation(const std::string& file, std::string& report, bool& is_valid);
    int         get_reports_output_Xml_Implementation(const std::string& file, std::string& report, bool& is_valid);
    int         get_reports_output_JStree(const std::string& file, const std::bitset<MediaConchLib::report_Max>& report_set, std::string& report);
    int         get_reports_output_Html(const std::string& file, const std::bitset<MediaConchLib::report_Max>& report_set, std::string& report);
    int         policies_check(const std::vector<std::string>& files,
                               MediaConchLib::ReportRes *result,
                               const std::vector<std::string>* policies_names = NULL,
                               const std::vector<std::string>* policies_contents = NULL);

    bool        validate_policy(const std::string& file, int policy,
                                MediaConchLib::ReportRes* result,
                                const std::string* display_name = NULL, const std::string* display_content = NULL);
    bool        validate_policy_file(const std::string& file, const std::string& policy,
                                     MediaConchLib::ReportRes* result,
                                     const std::string* display_name = NULL, const std::string* display_content = NULL);
    bool        validate_policy_memory(const std::string& file, const std::string& policy,
                                       MediaConchLib::ReportRes* result,
                                       const std::string* display_name = NULL, const std::string* display_content = NULL);

    // Apply display
    int  transform_with_xslt_file(const std::string& report, const std::string& Xslt, std::string& result);
    int  transform_with_xslt_memory(const std::string& report, const std::string& memory, std::string& result);

    Policies policies;
    std::string xslt_display;

    //General Configuration
    void load_configuration();
    void set_configuration_file(const std::string& file);
    const std::string& get_configuration_file() const;
    void set_implementation_schema_file(const std::string& file);
    const std::string& get_implementation_schema_file();
    void create_default_implementation_schema();

    bool is_using_daemon() const;
    void get_daemon_address(std::string& addr, int& port) const;

    //General Database
    void load_database();
    bool database_is_enabled() const;
    void register_file_to_database(std::string& file, MediaInfoNameSpace::MediaInfoList* MI);
    void create_report_mi_xml(const std::string& filename, std::string& report);
    void create_report_mt_xml(const std::string& filename, std::string& report);
    void create_report_ma_xml(const std::string& filename, std::string& report, bitset<MediaConchLib::report_Max> reports);

    // TODO: removed and manage waiting time otherway
    void WaitRunIsFinished();

private:
    Core (const Core&);

    MediaInfoNameSpace::MediaInfoList* MI;
    Database*                          db;
    Configuration*                     config;
    std::string                        configuration_file;
    std::map<std::string, std::string> implementation_options;
    //TODO: remove with the daemon
    Scheduler                         *scheduler;

    bool policies_check_contents(const std::string& files,
                                 const std::vector<std::string>& policies_contents,
                                 std::stringstream& Out);
    bool policies_check_files(const std::string& files,
                              const std::vector<std::string>& policies_names,
                              std::stringstream& Out);
    bool policy_is_valid(const std::string& report);
    /* bool policy_schematron(const std::string& file, const std::string& policy, MediaConchLib::format f, std::stringstream& Out); */
    /* bool policy_xslt(const std::string& file, const std::string& policy, MediaConchLib::format f, std::stringstream& Out); */

    //Helper
    bool validation(const std::string& file, Schema* S, std::string& report);
    bool validate_schematron_policy(const std::string& file, int pos, std::string& report);
    bool validate_schematron_policy_from_memory(const std::string& file, const std::string& memory, std::string& report);
    bool validate_schematron_policy_from_file(const std::string& file, const std::string& policy, std::string& report);
    bool validate_xslt_policy(const std::string& file, int pos, std::string& report);
    bool validate_xslt_policy_from_memory(const std::string& file, const std::string& memory, std::string& report, bool is_implem=false);
    bool validate_xslt_policy_from_file(const std::string& file, const std::string& policy, std::string& report);
    bool is_schematron_file(const std::string& file);

    int transform_with_xslt_text_memory(const std::string& report, std::string& result);
    int transform_with_xslt_html_memory(const std::string& report, std::string& result);

    bool   file_is_registered(const std::string& file);
    bool   file_is_registered_in_db(const std::string& file);
    bool   file_is_registered_in_queue(const std::string& file);
    std::string get_last_modification_file(const std::string& file);
    void get_report_saved(const std::string& file, MediaConchLib::report reportKind, MediaConchLib::format f, std::string& report);
    void get_reports_output(const std::string& file, MediaConchLib::format f,
                            std::bitset<MediaConchLib::report_Max> report_set,
                            MediaConchLib::ReportRes* result);
    void get_implementation_report(const std::string& file, std::string& report);

    void register_file_to_database(std::string& file);
    void register_report_mediainfo_text_to_database(std::string& file, const std::string& time,
                                                    MediaInfoNameSpace::MediaInfoList* MI);
    void register_report_mediainfo_xml_to_database(std::string& file, const std::string& time,
                                                   MediaInfoNameSpace::MediaInfoList* MI);
    void register_report_mediatrace_text_to_database(std::string& file, const std::string& time,
                                                     MediaInfoNameSpace::MediaInfoList* MI);
    void register_report_mediatrace_xml_to_database(std::string& file, const std::string& time,
                                                    MediaInfoNameSpace::MediaInfoList* MI);
    void register_report_implementation_xml_to_database(const std::string& file, const std::string& time,
                                                        std::string& report);
    void get_content_of_media_in_xml(std::string& report);
    //No idea how to do it better way
    bitset<MediaConchLib::report_Max> get_bitset_with_mi_mt();

    std::string get_config_file();
    std::string get_database_path();
    Database *get_db();

    std::string get_local_data_path();
    std::string get_local_config_path();
};

}

#endif
