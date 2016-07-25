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

//---------------------------------------------------------------------------
namespace MediaConch {

class Core;
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
        errorHttp_TRUE         = 1,
        errorHttp_NONE         = 0,
        errorHttp_INVALID_DATA = -1,
        errorHttp_INIT         = -2,
        errorHttp_CONNECT      = -3,
        errorHttp_INTERNAL     = -4,
        errorHttp_MAX          = -5,
    };

    enum PluginType
    {
        PLUGIN_FORMAT = 0,
        PLUGIN_MAX,
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
        std::string             file;
        bool                    valid;
        Checker_ValidateRes() : valid(true) {}
    };

    static const std::string display_xml_name;
    static const std::string display_maxml_name;
    static const std::string display_text_name;
    static const std::string display_html_name;
    static const std::string display_jstree_name;

    // General
    int init();
    int close();

    //Options
    int add_option(const std::string& option, std::string& report);

    // Analyze
    int  checker_analyze(const std::vector<std::string>& files, bool force_analyze = false);
    int  checker_analyze(const std::string& file, bool& registered, bool force_analyze = false);
    int  checker_is_done(const std::vector<std::string>& files, double& percent);
    int  checker_is_done(const std::string& file, double& percent, report& report_kind);

    void checker_list(std::vector<std::string>& vec);
    void checker_file_from_id(int id, std::string& filename);

    // Output
    int  checker_get_report(const std::bitset<report_Max>& Report, format f,
                            const std::vector<std::string>& files,
                            const std::vector<size_t>& policies_ids,
                            const std::vector<std::string>& policies_contents,
                            const std::map<std::string, std::string>& options,
                            Checker_ReportRes* result,
                            const std::string* display_name = NULL,
                            const std::string* display_content = NULL);
    int checker_validate(MediaConchLib::report report, const std::vector<std::string>& files,
                         const std::vector<size_t>& policies_ids,
                         const std::vector<std::string>& policies_contents,
                         std::vector<Checker_ValidateRes*>& result);

    //Clear
    int remove_report(const std::vector<std::string>& files);

    // Implementation checker arguments
    void               set_implementation_schema_file(const std::string& file);
    const std::string& get_implementation_schema_file();
    void               create_default_implementation_schema();
    void               set_implementation_verbosity(const std::string& verbosity);
    const std::string& get_implementation_verbosity();

    // Xsl Transformation
    int  transform_with_xslt_file(const std::string& report, const std::string& file,
                                  std::string& result);
    int  transform_with_xslt_memory(const std::string& report, const std::string& memory,
                                    std::string& result);

    // Configuration
    void               load_configuration();
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

    // Policies
    //   Create policy
    int                          policy_duplicate(int id, std::string& err);
    int                          policy_change_name(int id, const std::string& name, const std::string& description, std::string& err);
    int                          xslt_policy_create(std::string& err, int parent_id=-1);
    int                          xslt_policy_create_from_file(const std::string& file, std::string& err);
    //   Import policy
    int                          policy_import_from_file(const std::string& filename, std::string& err);
    int                          policy_import_from_memory(const char* filename, const std::string& memory, std::string& err, bool is_system_policy=false);

    //   Policy helper
    size_t                       policy_get_policies_count() const;
    Policy*                      policy_get(int pos);
    void                         policy_get_policies(std::vector<std::pair<size_t, std::string> >&);
    bool                         policy_exists(const std::string& title);
    int                          policy_save(size_t pos, std::string& err);
    int                          policy_remove(size_t pos, std::string& err);
    int                          policy_export(const char* filename, size_t pos, std::string& err);
    int                          policy_dump_to_memory(int id, std::string& memory, std::string& err);
    void                         policy_clear_policies();

    // XSLT Policy Rule
    int                         xslt_policy_rule_create(int policy_id, std::string& err);
    int                         xslt_policy_rule_edit(int policy_id, int rule_id, const XsltPolicyRule *rule, std::string& err);
    int                         xslt_policy_rule_duplicate(int policy_id, int rule_id, std::string& err);
    int                         xslt_policy_rule_delete(int policy_id, int rule_id, std::string& err);

    //Generated Values
    int                         policy_get_values_for_type_field(const std::string& type, const std::string& field, std::vector<std::string>& values);
    int                         policy_get_fields_for_type(const std::string& type, std::vector<std::string>& fields);

    // Daemon
    void set_use_daemon(bool use);
    bool get_use_daemon() const;
    void get_daemon_address(std::string& addr, int& port) const;

    // Helper
    int init_http_client();
    int close_http_client();

private:
    MediaConchLib (const MediaConchLib&);

    std::vector<std::string>  Options;
    bool                      use_daemon;
    bool                      force_no_daemon;
    Core                     *core;
    DaemonClient             *daemon_client;
};

}

#endif
