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
#ifndef MediaConchLibH
#define MediaConchLibH

#include <string>
#include <vector>
#include <map>
#include <bitset>
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
#include <MediaInfo/MediaInfo_Events.h>

//---------------------------------------------------------------------------
namespace MediaConch {

class Core;
class CheckerReport;
class DaemonClient;
class Policy;
class XsltPolicyRule;
class Http;

#ifdef _WIN32
    const std::string Path_Separator("\\");
    /* const String WPath_Separator(__T("\\")); */
#else
    const std::string Path_Separator("/");
    /* const String WPath_Separator(__T("/")); */
#endif

//***************************************************************************
// Class MediaConchLib
//***************************************************************************

class MediaConchLib
{
public:
    //Constructor/Destructor
    MediaConchLib(bool no_daemon = false);
    ~MediaConchLib();

    //Config
    enum report
    {
        report_MediaConch = 0,
        report_MediaInfo,
        report_MediaTrace,
        report_MediaVeraPdf,
        report_MediaDpfManager,
        report_MicroMediaTrace,
        report_Max,
    };

    enum format
    {
        format_Text = 0,
        format_Xml,         // XML corresponding to only one of MediaConch, MediaInfo, MediaTrace
        format_MaXml,       // MAXML, can contain one or more of MediaConch, MediaInfo, MediaTrace
        format_JsTree,
        format_Html,
        format_OrigXml,
        format_Simple,
        format_CSV,
        format_Max,
    };

    enum compression
    {
        compression_None = 0,
        compression_ZLib,
        compression_Max,
    };

    enum errorHttp
    {
        errorHttp_TRUE           = 1,
        errorHttp_NONE           = 0,
        errorHttp_INTERNAL       = -1,
        errorHttp_INVALID_DATA   = -2,
        errorHttp_INIT           = -3,
        errorHttp_CONNECT        = -4,
        errorHttp_DAEMON_RESTART = -5,
        errorHttp_MAX            = -6,
    };

    enum PluginType
    {
        PLUGIN_FORMAT = 0,
        PLUGIN_PRE_HOOK,
        PLUGIN_LOG,
        PLUGIN_MAX,
    };

    struct Checker_StatusRes
    {
        Checker_StatusRes() : id(-1), finished(false), has_error(false), percent(NULL), tool(NULL), source_id(-1) {}

        long               id;
        bool               finished;

        bool               has_error;
        std::string        error_log;

        double            *percent;

        int               *tool;
        std::vector<long>  generated_id;
        long               source_id;
    };

    struct Checker_ReportRes
    {
        std::string           report;
        bool                  has_valid;
        bool                  valid;
        Checker_ReportRes() : has_valid(false), valid(true) {}
    };

    struct Checker_ValidateRes
    {
        long                    id;
        bool                    valid;
        Checker_ValidateRes() : valid(true) {}
    };

    struct Checker_FileInfo
    {
        Checker_FileInfo() : source_id(-1), generated_time((size_t)-1), analyzed(false), has_error(false) {}

        std::string                                      filename;
        std::string                                      file_last_modification;
        std::string                                      generated_log;
        std::string                                      generated_error_log;
        std::vector<std::pair<std::string,std::string> > options;
        std::vector<long>                                generated_id;
        long                                             source_id;
        size_t                                           generated_time;
        bool                                             analyzed;
        bool                                             has_error;
        std::string                                      error_log;
    };

    union XSLT_Child;

    struct XSLT_Policy_Rule
    {
        XSLT_Policy_Rule() : id(-1), occurrence(-1) {}

        int          id;
        std::string  name;
        std::string  tracktype;
        std::string  field;
        std::string  scope;
        int          occurrence;
        std::string  ope;
        std::string  value;
        std::string  to_str() const;
    };

    struct Policy_Policy
    {
        Policy_Policy() : id(-1), parent_id(-1), is_system(false) {}
        Policy_Policy(const Policy_Policy* p) : id(p->id), parent_id(p->parent_id), is_system(p->is_system), is_public(p->is_public), kind(p->kind), type(p->type), name(p->name), description(p->description), license(p->license), children(p->children) {}
        int                                       id;
        int                                       parent_id;
        bool                                      is_system;
        bool                                      is_public;
        std::string                               kind;
        std::string                               type;
        std::string                               name;
        std::string                               description;
        std::string                               license;
        std::vector<std::pair<int, XSLT_Child> >  children;
        std::string                               to_str() const;
    };

    union XSLT_Child
    {
        XSLT_Policy_Rule *rule;
        Policy_Policy    *policy;
    };

    struct Policy_Public_Policy
    {
        Policy_Public_Policy() : id(-1), user(-1) {}
        Policy_Public_Policy(const Policy_Public_Policy* p) : id(p->id), user(p->user), name(p->name), description(p->description), license(p->license) {}
        long                                      id;
        long                                      user;
        std::string                               name;
        std::string                               description;
        std::string                               license;
    };

    struct Get_Policies
    {
    Get_Policies() : policies(NULL) {}

        ~Get_Policies()
        {
            if (format == "JSTREE" && jstree)
                delete jstree;
            else if (policies)
            {
                for (size_t i = 0; i < policies->size(); ++i)
                    delete policies->at(i);
                policies->clear();
                delete policies;
            }
        }

        std::string                      format;
        union
        {
            std::vector<Policy_Policy*>* policies;
            std::string*                 jstree;
        };
    };

    struct Get_Policy
    {
        Get_Policy() : policy(NULL) {}

        ~Get_Policy()
        {
            if (format == "JSTREE" && jstree)
                delete jstree;
            else if (policy)
                delete policy;
        }

        std::string          format;
        union
        {
            Policy_Policy   *policy;
            std::string     *jstree;
        };
    };

    static const std::string display_xml_name;
    static const std::string display_maxml_name;
    static const std::string display_text_name;
    static const std::string display_html_name;
    static const std::string display_jstree_name;
    static const std::string display_simple_name;
    static const std::string display_csv_name;

    // General
    int  init(std::string& err);
    int  close();
    void reset_daemon_client();

    //Options
    int  test_mil_option(const std::string& key, std::string& value, std::string& report);
    bool mil_has_curl_enabled();

    // MediaConch
    int  mediaconch_get_plugins(std::vector<std::string>& plugins, std::string& error);
    int  mediaconch_list_watch_folders(std::vector<std::string>& folders, std::string& error);
    int  mediaconch_watch_folder(const std::string& folder, const std::string& folder_reports,
                                 const std::vector<std::string>& plugins, const std::vector<std::string>& policies,
                                 long* in_user, bool recursive,
                                 const std::vector<std::pair<std::string,std::string> >& options,
                                 long& user_id, std::string& error);
    int  mediaconch_edit_watch_folder(const std::string& folder, const std::string& folder_reports, std::string& error);
    int  mediaconch_remove_watch_folder(const std::string& folder, std::string& error);

    // Analyze
    int  checker_analyze(int user, const std::vector<std::string>& files,
                         const std::vector<std::string>& plugins,
                         const std::vector<std::pair<std::string,std::string> >& options,
                         std::vector<long>& files_id, std::string& error, bool force_analyze = false,
                         bool mil_analyze = true);
    int  checker_analyze(int user, const std::string& file, const std::vector<std::string>& plugins,
                         const std::vector<std::pair<std::string,std::string> >& options,
                         bool& registered, long& file_id, std::string& error, bool force_analyze = false,
                         bool mil_analyze = true);

    // Status
    int  checker_status(int user, const std::vector<long>& files_id,
                        std::vector<Checker_StatusRes>& res, std::string& error);
    int  checker_status(int user, long file_id, Checker_StatusRes& res, std::string& error);

    int  checker_list(int user, std::vector<std::string>& vec, std::string& error);
    int  checker_list(int user, std::vector<long>& vec, std::string& error);
    int  checker_file_from_id(int user, long id, std::string& filename, std::string& error);
    long checker_id_from_filename(int user, const std::string& filename,
                                  const std::vector<std::pair<std::string,std::string> >& options, std::string& error);
    int  checker_file_information(int user, long id, Checker_FileInfo& info, std::string& error);

    // Output
    int  checker_get_report(CheckerReport& c_report, Checker_ReportRes* result, std::string& error);
    int  checker_validate(int user, MediaConchLib::report report, const std::vector<long>& files,
                          const std::vector<size_t>& policies_ids,
                          const std::vector<std::string>& policies_contents,
                          const std::map<std::string, std::string>& options,
                          std::vector<Checker_ValidateRes*>& result, std::string& error);

    //Clear
    int  checker_clear(int user, const std::vector<long>& files, std::string& error);

    //Stop
    int  checker_stop(int user, const std::vector<long>& files, std::string& error);

    // Policies
    //   Create policy
    int                          policy_duplicate(int user, int id, int dst_policy_id, int *dst_user, bool must_be_public, std::string& err);
    int                          policy_move(int user, int id, int dst_policy_id, std::string& err);
    int                          policy_change_info(int user, int id, const std::string& name, const std::string& description, const std::string& license, std::string& err);
    int                          policy_change_type(int user, int id, const std::string& type, std::string& err);
    int                          policy_change_is_public(int user, int id, bool is_public, std::string& err);
    int                          xslt_policy_create(int user, std::string& err, const std::string& type="and", int parent_id=-1);
    int                          xslt_policy_create_from_file(int user, long file, std::string& err);
    //   Import policy
    int                          policy_import(int user, const std::string& memory, std::string& err, const char* filename=NULL, bool is_system_policy=false);

    //   Policy helper
    size_t                       policy_get_policies_count(int user, std::string& err) const;
    int                          policy_get(int user, int pos, const std::string& format, bool must_be_public,
                                            Get_Policy&, std::string& err);
    int                          policy_get_name(int user, int id, std::string& name, std::string& err);
    int                          policy_get_policies(int user, const std::vector<int>&, const std::string& format, Get_Policies&, std::string& error);
    int                          policy_get_public_policies(std::vector<Policy_Public_Policy*>& policies, std::string& err);
    int                          policy_get_policies_names_list(int user, std::vector<std::pair<int, std::string> >&, std::string& error);
    int                          policy_save(int user, int pos, std::string& err);
    int                          policy_remove(int user, int pos, std::string& err);
    int                          policy_dump(int user, int id, bool must_be_public, std::string& memory, std::string& err);
    int                          policy_clear_policies(int user, std::string& err);

    // XSLT Policy Rule
    int                         xslt_policy_rule_create(int user, int policy_id, std::string& err);
    XsltPolicyRule*             xslt_policy_rule_get(int user, int policy_id, int id, std::string& err);
    int                         xslt_policy_rule_edit(int user, int policy_id, int rule_id, const XsltPolicyRule *rule, std::string& err);
    int                         xslt_policy_rule_duplicate(int user, int policy_id, int rule_id, int dst_policy_id, std::string& err);
    int                         xslt_policy_rule_move(int user, int policy_id, int rule_id, int dst_policy_id, std::string& err);
    int                         xslt_policy_rule_delete(int user, int policy_id, int rule_id, std::string& err);

    //Generated Values
    int                         policy_get_values_for_type_field(const std::string& type, const std::string& field, std::vector<std::string>& values, std::string& error);
    int                         policy_get_fields_for_type(const std::string& type, std::vector<std::string>& fields, std::string& error);

    // Implementation checker arguments
    void               set_implementation_schema_file(const std::string& file);
    const std::string& get_implementation_schema_file();
    void               create_default_implementation_schema();
    void               set_implementation_verbosity(const std::string& verbosity);
    const std::string& get_implementation_verbosity();

    // Configuration
    void               load_configuration();
    void               set_default_scheduler_max_threads(size_t nb);
    void               set_configuration_file(const std::string& file);
    const std::string& get_configuration_file() const;
    void               load_plugins_configuration();
    void               set_plugins_configuration_file(const std::string& file);
    void               set_compression_mode(compression compress);
    int                get_ui_poll_request() const;
    int                get_ui_database_path(std::string& path) const;

    bool ReportAndFormatCombination_IsValid(const std::vector<std::string>& files,
                                            const std::bitset<MediaConchLib::report_Max>& reports,
                                            const std::string& display, MediaConchLib::format& Format,
                                            std::string& reason);

    // Daemon
    void set_use_daemon(bool use);
    bool get_use_daemon() const;
    void get_daemon_address(std::string& addr, int& port) const;

    // Helper
    int init_http_client();
    int close_http_client();
    int load_system_policy();
    int load_existing_policy();

    // Register Event callback
    void register_log_callback(void (*log)(struct MediaInfo_Event_Log_0* Event));

private:
    MediaConchLib (const MediaConchLib&);

    bool                      use_daemon;
    bool                      force_no_daemon;
    Core                     *core;
    DaemonClient             *daemon_client;
};

}

#endif
