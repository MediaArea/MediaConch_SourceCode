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
#include <bitset>

//---------------------------------------------------------------------------
namespace MediaConch {

class Core;
class DaemonClient;
class Policy;
class Http;

#ifdef WINDOWS
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
        report_MediaConch,
        report_MediaInfo,
        report_MediaTrace,
        report_Max
    };

    enum format
    {
        format_Text,
        format_Xml,         // XML corresponding to only one of MediaConch, MediaInfo, MediaTrace
        format_MaXml,       // MAXML, can contain one or more of MediaConch, MediaInfo, MediaTrace
        format_JsTree,
        format_Html,
        format_Max,
    };

    struct ReportRes
    {
        std::string       report;
        bool              has_valid;
        bool              valid;
        ReportRes() :     has_valid(false), valid(true) {}
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
    int  analyze(const std::vector<std::string>& files);
    int  analyze(const std::string& file, bool& registered);
    bool is_done(const std::vector<std::string>& files, double& percent);
    bool is_done(const std::string& file, double& percent);

    // Output
    int get_report(const std::bitset<report_Max>& Report, format f,
                   const std::vector<std::string>& files,
                   const std::vector<std::string>& policies_names,
                   const std::vector<std::string>& policies_contents,
                   MediaConchLib::ReportRes* result,
                   const std::string* display_name = NULL,
                   const std::string* display_content = NULL);
    int remove_report(const std::vector<std::string>& files);

    // Policy
    void               set_implementation_schema_file(const std::string& file);
    const std::string& get_implementation_schema_file();
    void               create_default_implementation_schema();
    bool validate_policy(const std::string& file, int policy,
                         MediaConchLib::ReportRes* result,
                         const std::string* display_name, const std::string* display_content);
    bool validate_policy_memory(const std::string& file, const std::string& policy,
                                MediaConchLib::ReportRes* result,
                                const std::string* display_name, const std::string* display_content);
    int validate_policies(const std::string& file, const std::vector<std::string>& policies,
                           MediaConchLib::ReportRes* result,
                           const std::string* display_name, const std::string* display_content);

    // Xsl Transformation
    int  transform_with_xslt_file(const std::string& report, const std::string& file, std::string& result);
    int  transform_with_xslt_memory(const std::string& report, const std::string& memory, std::string& result);

    // Configuration
    void load_configuration();
    void set_configuration_file(const std::string& file);
    const std::string& get_configuration_file() const;

    bool ReportAndFormatCombination_IsValid(const std::vector<std::string>& files,
                                            const std::bitset<MediaConchLib::report_Max>& reports,
                                            const std::string& display, MediaConchLib::format& Format,
                                            std::string& reason);

    // Policies
    //   Export policy
    void                        save_policies();
    void                        save_policy(size_t pos, const std::string* filename);
    //   Policy saved?
    bool                        is_policies_saved() const;
    bool                        is_policy_saved(size_t pos) const;
    //   Import policy
    int                         import_policy_from_file(const std::string& filename, std::string& err);
    int                         import_policy_from_memory(const std::string& memory, const std::string& filename, std::string& err);
    //   Policy helper
    size_t                      get_policies_count() const;
    Policy*                     get_policy(size_t pos);
    bool                        policy_exists(const std::string& title);
    void                        add_policy(Policy* p);
    void                        remove_policy(size_t pos);
    void                        clear_policies();
    const std::vector<Policy *>& get_policies() const;

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
