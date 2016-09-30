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
    #include "MediaInfo/MediaInfo.h"
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
class DatabaseReport;
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
    long        checker_analyze(int user, const std::string& filename, bool& registered,
                                const std::vector<std::string>& options, bool force_analyze = false);
    long        checker_analyze(int user, const std::string& filename, long src_id, size_t generated_time,
                                const std::string generated_log, const std::string generated_error_log,
                                const std::vector<std::string>& options, bool pre_hook=true);
    int         file_update_generated_file(int user, long src_id, long generated_id);

    bool        checker_status(int user, long file, MediaConchLib::Checker_StatusRes& res);
    int         remove_report(int user, const std::vector<long>& files);
    int         checker_get_report(int user, const std::bitset<MediaConchLib::report_Max>& Report, MediaConchLib::format f,
                                   const std::vector<long>& files,
                                   const std::vector<size_t>& policies_ids,
                                   const std::vector<std::string>& policies_contents,
                                   const std::map<std::string, std::string>& options,
                                   MediaConchLib::Checker_ReportRes* res,
                                   const std::string* display_names = NULL,
                                   const std::string* display_contents = NULL);
    int         get_reports_output_JStree(int user, const std::vector<long>& file,
                                          const std::bitset<MediaConchLib::report_Max>& report_set, std::string& report);
    int         get_reports_output_Html(int user, const std::vector<long>& file,
                                        const std::bitset<MediaConchLib::report_Max>& report_set, std::string& report);
    int         check_policies(int user, const std::vector<long>& files,
                               const std::map<std::string, std::string>& options,
                               MediaConchLib::Checker_ReportRes *result,
                               const std::vector<size_t>* policies_ids = NULL,
                               const std::vector<std::string>* policies_contents = NULL);

    void        checker_list(int user, std::vector<std::string>& vec);
    void        checker_file_from_id(int user, long id, std::string& file);
    long        checker_id_from_filename(int user, const std::string& filename);
    int         checker_file_information(int user, long id, MediaConchLib::Checker_FileInfo& info);

    int         checker_validate(int user, MediaConchLib::report report,
                                 const std::vector<long>& file,
                                 const std::vector<size_t>& policies_ids,
                                 const std::vector<std::string>& policies_contents,
                                 const std::map<std::string, std::string>& options,
                                 std::vector<MediaConchLib::Checker_ValidateRes*>& result);

    // Apply display
    int  transform_with_xslt_file(const std::string& report, const std::string& Xslt,
                                  const std::map<std::string, std::string>& opts, std::string& result);
    int  transform_with_xslt_memory(const std::string& report, const std::string& memory,
                                    const std::map<std::string, std::string>& opts, std::string& result);

    int  policy_get_values_for_type_field(const std::string& type, const std::string& field, std::vector<std::string>& values);
    int  policy_get_fields_for_type(const std::string& type, std::vector<std::string>& fields);

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

    //Plugins
    const std::map<std::string, Plugin*>& get_format_plugins() const;
    const std::vector<Plugin*>&           get_pre_hook_plugins() const;

    bool is_using_daemon() const;
    void get_daemon_address(std::string& addr, int& port) const;

    //General Report Database
    void load_database();
    bool database_is_enabled() const;
    void set_file_analyzed_to_database(int user, long id);
    void register_reports_to_database(int user, long file, MediaInfoNameSpace::MediaInfo* MI);
    void register_reports_to_database(int user, long file, const std::string& report, MediaConchLib::report report_kind,
                                      MediaInfoNameSpace::MediaInfo* curMI);
    void create_report_mi_xml(int user, const std::vector<long>& filename, std::string& report);
    void create_report_mt_xml(int user, const std::vector<long>& filename, std::string& report);
    void create_report_mmt_xml(int user, const std::vector<long>& filename, std::string& report);
    void create_report_ma_xml(int user, const std::vector<long>& files, const std::map<std::string, std::string>& options,
                              std::string& report, bitset<MediaConchLib::report_Max> reports);
    void create_report_verapdf_xml(int user, const std::vector<long>& files, std::string& report);
    void create_report_dpfmanager_xml(int user, const std::vector<long>& files, std::string& report);

    // TODO: removed and manage waiting time otherway
    void WaitRunIsFinished();

    // Get Local Paths
    static std::string get_local_data_path();
    static std::string get_local_config_path();

    bool        accepts_https();
    static void unify_no_https(std::string& str);

private:
    Core (const Core&);

    MediaInfoNameSpace::MediaInfo     *MI;
    DatabaseReport*                    db;
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

    bool check_policies_xslts(int user, const std::vector<long>& files,
                              const std::map<std::string, std::string>& options,
                              const std::vector<std::string>& policies,
                              std::stringstream& Out);
    bool has_outcome_fail(const std::string& report);
    bool implementation_is_valid(const std::string& report);
    bool policy_is_valid(const std::string& report);
    bool verapdf_report_is_valid(const std::string& report);
    bool dpfmanager_report_is_valid(const std::string& report);

    //Helper
    bool validation(int user, const std::vector<long>& files, Schema* S, std::string& report);
    bool validate_xslt_policy(int user, const std::vector<long>& files,
                              const std::map<std::string, std::string>& opts,
                              int pos, std::string& report);
    bool validate_xslt_from_memory(int user, const std::vector<long>& files, const std::map<std::string, std::string>& opts,
                                   const std::string& memory, std::string& report, bool is_implem=false);
    bool validate_xslt_policy_from_file(int user, const std::vector<long>& files, const std::map<std::string, std::string>& opts,
                                        const std::string& policy, std::string& report);
    void unify_implementation_options(std::map<std::string, std::string>& opts);
    void unify_policy_options(int user, std::map<std::string, std::string>& opts);

    int transform_with_xslt_text_memory(const std::string& report, std::string& result);
    int transform_with_xslt_html_memory(const std::string& report, std::string& result);

    long   file_is_registered_and_analyzed(int user, const std::string& file, bool& analyzed);
    long   file_is_registered_and_analyzed_in_db(int user, const std::string& file, bool& analyzed);
    long   file_is_registered_in_queue(int user, const std::string& file);
    std::string get_last_modification_file(const std::string& file);
    bool   file_is_existing(const std::string& filename);
    void compress_report(std::string& report, MediaConchLib::compression& compress);
    void compress_report_copy(std::string& report, const char* src, size_t src_len, MediaConchLib::compression& compress);
    int  uncompress_report(std::string& report, MediaConchLib::compression compress);
    void get_report_saved(int user, const std::vector<long>& file, MediaConchLib::report reportKind,
                          MediaConchLib::format f, std::string& report);
    void get_reports_output(int user, const std::vector<long>& files,
                            const std::map<std::string, std::string>& options,
                            MediaConchLib::format f,
                            std::bitset<MediaConchLib::report_Max> report_set,
                            MediaConchLib::Checker_ReportRes* result);
    bool get_implementation_report(int user, const std::vector<long>& file, const std::map<std::string, std::string>& options,
                                   std::string& report);
    bool get_verapdf_report(int user, long file, std::string& report);
    bool get_dpfmanager_report(int user, long file, std::string& report);

    void add_file_to_db(int user, std::string& filename, const std::string& time);
    void register_reports_to_database(int user, long file);
    void register_report_xml_to_database(int user, long file, const std::string& report,
                                         MediaConchLib::report report_kind);
    void register_report_mediainfo_text_to_database(int user, long file, MediaInfoNameSpace::MediaInfo* MI);
    void register_report_mediainfo_xml_to_database(int user, long file, MediaInfoNameSpace::MediaInfo* MI);
    void register_report_micromediatrace_xml_to_database(int user, long file, MediaInfoNameSpace::MediaInfo* MI);
    void get_content_of_media_in_xml(std::string& report);
    //No idea how to do it better way
    bitset<MediaConchLib::report_Max> get_bitset_with_mi_mt();
    bitset<MediaConchLib::report_Max> get_bitset_with_mi_mmt();

    std::string get_config_file();
    std::string get_database_path();
    DatabaseReport *get_db();
    void xml_escape_attributes(std::string& xml);
};

}

#endif
