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
#include <ZenLib/CriticalSection.h>
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
class PluginsManager;
class Plugin;

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


//---------------------------------------------------------------------------
    bool        is_done(const std::string& file, double& percent_done, MediaConchLib::report& report_kind);
    int         open_file(const std::string& filename, bool& registered, bool force_analyze = false);
    int         remove_report(const std::vector<std::string>& files);
    int         get_report(const std::bitset<MediaConchLib::report_Max>& Report, MediaConchLib::format f,
                           const std::vector<std::string>& files,
                           const std::vector<std::string>& policies_names,
                           const std::vector<std::string>& policies_contents,
                           const std::map<std::string, std::string>& options,
                           MediaConchLib::ReportRes* res,
                           const std::string* display_names = NULL,
                           const std::string* display_contents = NULL);
    int         get_reports_output_JStree(const std::vector<std::string>& file, const std::bitset<MediaConchLib::report_Max>& report_set, std::string& report);
    int         get_reports_output_Html(const std::vector<std::string>& file, const std::bitset<MediaConchLib::report_Max>& report_set, std::string& report);
    int         policies_check(const std::vector<std::string>& files,
                               const std::map<std::string, std::string>& options,
                               MediaConchLib::ReportRes *result,
                               const std::vector<std::string>* policies_names = NULL,
                               const std::vector<std::string>* policies_contents = NULL);

    void        list(std::vector<std::string>& vec);

    int         validate(MediaConchLib::report report, const std::vector<std::string>& file,
                         const std::vector<std::string>& policies_names,
                         const std::vector<std::string>& policies_contents,
                         std::vector<MediaConchLib::ValidateRes*>& result);

    // Apply display
    int  transform_with_xslt_file(const std::string& report, const std::string& Xslt, std::string& result);
    int  transform_with_xslt_memory(const std::string& report, const std::string& memory, std::string& result);

    Policies policies;
    std::string xslt_display;

    //General Configuration
    void load_configuration();
    void set_configuration_file(const std::string& file);
    const std::string& get_configuration_file() const;
    void load_plugins_configuration();
    void set_plugins_configuration_file(const std::string& file);
    void set_implementation_schema_file(const std::string& file);
    const std::string& get_implementation_schema_file();
    void create_default_implementation_schema();
    void set_implementation_verbosity(const std::string& verbosity);
    const std::string& get_implementation_verbosity();
    void set_compression_mode(MediaConchLib::compression compress);
    int get_ui_poll_request() const;
    int get_ui_database_path(std::string& path) const;
    const std::map<std::string, Plugin*>& get_format_plugins() const;

    bool is_using_daemon() const;
    void get_daemon_address(std::string& addr, int& port) const;

    //General Database
    void load_database();
    bool database_is_enabled() const;
    void register_file_to_database(std::string& file, MediaInfoNameSpace::MediaInfoList* MI);
    void register_file_to_database(std::string& file, const std::string& report, MediaConchLib::report report_kind);
    void create_report_mi_xml(const std::vector<std::string>& filename, std::string& report);
    void create_report_mt_xml(const std::vector<std::string>& filename, std::string& report);
    void create_report_ma_xml(const std::vector<std::string>& files, const std::map<std::string, std::string>& options, std::string& report, bitset<MediaConchLib::report_Max> reports);
    void create_report_verapdf_xml(const std::vector<std::string>& files, std::string& report);
    void create_report_dpfmanager_xml(const std::vector<std::string>& files, std::string& report);

    // TODO: removed and manage waiting time otherway
    void WaitRunIsFinished();

    // Get Local Paths
    static std::string get_local_data_path();
    static std::string get_local_config_path();

private:
    Core (const Core&);

    MediaInfoNameSpace::MediaInfoList* MI;
    Database*                          db;
    CriticalSection                    db_mutex;
    static const std::string           database_name;
    Configuration*                     config;
    std::string                        configuration_file;
    std::string                        plugins_configuration_file;
    std::map<std::string, std::string> implementation_options;
    //TODO: remove with the daemon
    Scheduler                         *scheduler;
    PluginsManager                    *pluginsManager;
    MediaConchLib::compression         compression_mode;

    bool policies_check_contents(const std::vector<std::string>& files,
                                 const std::map<std::string, std::string>& options,
                                 const std::vector<std::string>& policies_contents,
                                 std::stringstream& Out);
    bool policies_check_files(const std::vector<std::string>& files,
                              const std::map<std::string, std::string>& options,
                              const std::vector<std::string>& policies_names,
                              std::stringstream& Out);
    bool policy_is_valid(const std::string& report);
    bool verapdf_report_is_valid(const std::string& report);
    bool dpfmanager_report_is_valid(const std::string& report);

    //Helper
    bool validation(const std::vector<std::string>& files, Schema* S, std::string& report);
    bool validate_schematron_policy(const std::vector<std::string>& files, int pos, std::string& report);
    bool validate_schematron_policy_from_memory(const std::vector<std::string>& files, const std::string& memory, std::string& report);
    bool validate_schematron_policy_from_file(const std::vector<std::string>& files, const std::string& policy, std::string& report);
    bool validate_xslt_policy(const std::vector<std::string>& files, const std::map<std::string, std::string>& opts, int pos, std::string& report);
    bool validate_xslt_policy_from_memory(const std::vector<std::string>& files, const std::map<std::string, std::string>& opts, const std::string& memory, std::string& report, bool is_implem=false);
    bool validate_xslt_policy_from_file(const std::vector<std::string>& files, const std::map<std::string, std::string>& opts, const std::string& policy, std::string& report);
    bool is_schematron_file(const std::string& file);
    void unify_implementation_options(std::map<std::string, std::string>& opts);

    int transform_with_xslt_text_memory(const std::string& report, std::string& result);
    int transform_with_xslt_html_memory(const std::string& report, std::string& result);

    bool   file_is_registered(const std::string& file);
    bool   file_is_registered_in_db(const std::string& file);
    bool   file_is_registered_in_queue(const std::string& file);
    std::string get_last_modification_file(const std::string& file);
    void compress_report(std::string& report, MediaConchLib::compression& compress);
    int  uncompress_report(std::string& report, MediaConchLib::compression compress);
    void get_report_saved(const std::vector<std::string>& file, MediaConchLib::report reportKind, MediaConchLib::format f, std::string& report);
    void get_reports_output(const std::vector<std::string>& files,
                            const std::map<std::string, std::string>& options,
                            MediaConchLib::format f,
                            std::bitset<MediaConchLib::report_Max> report_set,
                            MediaConchLib::ReportRes* result);
    bool get_implementation_report(const std::vector<std::string>& file, const std::map<std::string, std::string>& options, std::string& report);
    bool get_verapdf_report(const std::string& file, std::string& report);
    bool get_dpfmanager_report(const std::string& file, std::string& report);

    void register_file_to_database(std::string& file);
    void register_report_xml_to_database(std::string& file, const std::string& time,
                                         const std::string& report,
                                         MediaConchLib::report report_kind);
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
};

}

#endif
