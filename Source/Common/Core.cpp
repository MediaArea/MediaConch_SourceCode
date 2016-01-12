/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "Core.h"
#include "Common/Schema.h"
#include "Database.h"
#include "NoDatabase.h"
#include "SQLLite.h"
#include "Configuration.h"
#include "Json.h"
#include "Common/Schematron.h"
#include "Common/Xslt.h"
#include "Common/JS_Tree.h"
#include "Common/ImplementationReportXsl.h"
#if defined(_WIN32) || defined(WIN32)
#include "Common/ImplementationReportDisplayTextXsl.h"
#else //defined(_WIN32) || defined(WIN32)
#include "Common/ImplementationReportDisplayTextUnicodeXsl.h"
#endif //defined(_WIN32) || defined(WIN32)
#include "Common/ImplementationReportDisplayHtmlXsl.h"
#include "Common/ImplementationReportMatroskaSchema.h"
#include "Common/MediaTraceDisplayTextXsl.h"
#include "Common/MediaTraceDisplayHtmlXsl.h"
#include "ZenLib/Ztring.h"
#include "ZenLib/File.h"
#include "ZenLib/Dir.h"
#include <zlib.h>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
//---------------------------------------------------------------------------

#if defined(UNIX)
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#elif defined(MACOS) || defined(MACOSX)
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <uuid/uuid.h>
#elif defined(WINDOWS)
#include <windows.h>
#include <Lmcons.h>
#endif

//---------------------------------------------------------------------------
const MediaInfoNameSpace::Char* MEDIAINFO_TITLE=__T("MediaArea.net/MediaConch");
ZenLib::Ztring Text_Temp;
using namespace ZenLib;
using namespace std;
//---------------------------------------------------------------------------

namespace MediaConch {


//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Core::Core() : policies(this)
{
    MI=new MediaInfoNameSpace::MediaInfoList;

    config = NULL;
    db = NULL;
    scheduler = new Scheduler(this);
    policies.create_values_from_csv();
    compression_mode = MediaConchLib::compression_ZLib;
}

Core::~Core()
{
    if (scheduler)
        delete scheduler;
    if (db)
        delete db;
    delete MI;
    if (config)
        delete config;
}

//***************************************************************************
// Menu
//***************************************************************************

//---------------------------------------------------------------------------
void Core::Menu_Option_Preferences_Inform (const String& Inform)
{
    MI->Option(__T("Inform"), Inform);
}

//---------------------------------------------------------------------------
String Core::Menu_Option_Preferences_Option (const String& Param, const String& Value)
{
    return MI->Option(Param, Value);
}

//***************************************************************************
// General
//***************************************************************************

//---------------------------------------------------------------------------
void Core::load_configuration()
{
    config = new Configuration;
    std::string config_file = get_config_file();
    config->set_file(config_file);
    config->parse();

    long scheduler_max_threads = 1;
    if (scheduler && !config->get("Scheduler_Max_Threads", scheduler_max_threads))
    {
        if (scheduler_max_threads <= 0)
            scheduler_max_threads = 1;
        scheduler->set_max_threads((size_t)scheduler_max_threads);
    }
}

//---------------------------------------------------------------------------
void Core::load_database()
{
#ifdef HAVE_SQLITE
    std::string db_path;
    if (!config || config->get("SQLite_Path", db_path) < 0)
        db_path = get_database_path();

    db = new SQLLite;

    db->set_database_directory(db_path);
    db->init();
#else
    db = new NoDatabase;
    db->init();
#endif
}

//---------------------------------------------------------------------------
void Core::set_implementation_schema_file(const std::string& file)
{
    if (!file.length())
        return;
    std::string f;

    if (file[0] == '"')
        f = std::string("'") + file + std::string("'");
    else
        f = std::string("\"") + file + std::string("\"");

    //TODO: real uri encoding
    for (;;)
    {
        size_t pos = 0;
        pos = f.find(" ");
        if (pos == std::string::npos)
          break;

        f.replace(pos, 1, "%20");
    }
    #ifdef WIN32
    for (;;)
    {
        size_t pos = 0;
        pos = f.find('\\');
        if (pos == std::string::npos)
          break;

        f[pos]='/'; // Windows path separators are replaced by / else it is reected by libxml2
    }
    #endif //WIN32
    implementation_options["schema"] = f;
}

//---------------------------------------------------------------------------
const std::string& Core::get_implementation_schema_file()
{
    return implementation_options["schema"];
}

//---------------------------------------------------------------------------
void Core::create_default_implementation_schema()
{
    std::string path = get_local_data_path();
    std::string file = path + "MatroskaSchema.xml";

    std::ofstream ofs;
    ofs.open(file.c_str(), std::ofstream::out | std::ofstream::binary);

    if (!ofs.is_open())
        return;

    ofs << xsl_schema_matroska_schema;
    ofs.close();

    set_implementation_schema_file(file);
}

//---------------------------------------------------------------------------
void Core::set_compression_mode(MediaConchLib::compression compress)
{
    compression_mode = compress;
}

//***************************************************************************
// Tools
//***************************************************************************

//---------------------------------------------------------------------------
void Core::Close ()
{
    MI->Close();
}

//---------------------------------------------------------------------------
void Core::Run (std::string file)
{
    // Parsing
    bool registered = false;
    if (file.length())
        open_file(file, registered);
}

//---------------------------------------------------------------------------
int Core::open_file(const std::string& file, bool& registered, bool force_analyze)
{
    //TODO: When ZenLib will manage network files
    // if (!ZenLib::File::Exists(Ztring().From_UTF8(file)))
    //     return -1;

    registered = false;
    if (!force_analyze)
        registered = file_is_registered(file);

    if (!registered)
    {
        if (get_db())
            scheduler->add_element_to_queue(file);
        else
            MI->Open(Ztring().From_UTF8(file));
    }
    return 0;
}

//---------------------------------------------------------------------------
bool Core::is_done(const std::string& file, double& percent_done)
{
    return scheduler->element_is_finished(file, percent_done);
}

//---------------------------------------------------------------------------
int Core::get_report(const std::bitset<MediaConchLib::report_Max>& report_set, MediaConchLib::format f,
                     const std::vector<std::string>& files,
                     const std::vector<std::string>& policies_names,
                     const std::vector<std::string>& policies_contents,
                     MediaConchLib::ReportRes* result,
                     const std::string* display_name,
                     const std::string* display_content)
{
    if (!policies_names.empty() || !policies_contents.empty())
    {
        if (policies_check(files, result,
                           policies_names.size() ? &policies_names : NULL,
                           policies_contents.size() ? &policies_contents : NULL) < 0)
            return -1;
        if (f == MediaConchLib::format_Text)
            transform_with_xslt_text_memory(result->report, result->report);
        else if (f == MediaConchLib::format_Html)
            transform_with_xslt_html_memory(result->report, result->report);
    }
    else
    {
        for (size_t i = 0; i < files.size(); ++i)
        {
            switch (f)
            {
                case MediaConchLib::format_Text:
                case MediaConchLib::format_Xml:
                case MediaConchLib::format_MaXml:
                    get_reports_output(files[i], f, report_set, result);
                    break;
                case MediaConchLib::format_JsTree:
                    get_reports_output_JStree(files[i], report_set, result->report);
                    break;
                case MediaConchLib::format_Html:
                    get_reports_output_Html(files[i], report_set, result->report);
                    break;
                default:
                    return -1;
            }
        }
    }

    if (display_name)
        transform_with_xslt_file(result->report, *display_name, result->report);
    else if (display_content)
        transform_with_xslt_memory(result->report, *display_content, result->report);
    return 0;
}

//---------------------------------------------------------------------------
int Core::remove_report(const std::vector<std::string>& files)
{
    if (!get_db())
        return false;

    for (size_t i = 0; i < files.size(); ++i)
        db->remove_report(files[i]);
    return 0;
}

//---------------------------------------------------------------------------
int Core::get_reports_output_JStree(const std::string& file,
                                    const std::bitset<MediaConchLib::report_Max>& report_set,
                                    std::string& report)
{
    JsTree js;
    if (report_set[MediaConchLib::report_MediaInfo])
    {
        std::string ret;
        get_report_saved(file, MediaConchLib::report_MediaInfo,
                         MediaConchLib::format_Xml, ret);
        report += js.format_from_inform_XML(ret);
    }

    if (report_set[MediaConchLib::report_MediaTrace])
    {
        std::string ret;
        get_report_saved(file, MediaConchLib::report_MediaTrace,
                         MediaConchLib::format_Xml, ret);
        report += js.format_from_trace_XML(ret);
    }

    return 0;
}

//---------------------------------------------------------------------------
int Core::get_reports_output_Html(const std::string& file,
                                  const std::bitset<MediaConchLib::report_Max>& report_set,
                                  std::string& report)
{
    if (report_set[MediaConchLib::report_MediaConch])
    {
        std::string tmp;
        get_implementation_report(file, tmp);

        // Apply an XSLT to have HTML
        transform_with_xslt_html_memory(tmp, tmp);
        report += tmp;
    }
    if (report_set[MediaConchLib::report_MediaTrace])
    {
        std::string tmp;
        get_report_saved(file, MediaConchLib::report_MediaTrace,
                         MediaConchLib::format_Xml, tmp);

        // Apply an XSLT to have HTML
        std::string memory(media_trace_display_html_xsl);
        transform_with_xslt_memory(tmp, memory, tmp);
        report += tmp;
    }
    return 0;
}

//---------------------------------------------------------------------------
int Core::get_reports_output_Text_Implementation(const std::string& file, std::string& report, bool& is_valid)
{
    std::string tmp;
    get_implementation_report(file, tmp);
    is_valid = policy_is_valid(tmp);

    // Apply an XSLT to have Text
    transform_with_xslt_text_memory(tmp, tmp);
    report += tmp;
    return 0;
}

//---------------------------------------------------------------------------
int Core::get_reports_output_Xml_Implementation(const std::string& file, std::string& report, bool& is_valid)
{
    get_implementation_report(file, report);
    is_valid = policy_is_valid(report);

    return 0;
}

//---------------------------------------------------------------------------
bool Core::policies_check_contents(const std::string& file,
                                   const std::vector<std::string>& policies_contents,
                                   std::stringstream& Out)
{
    bool valid = true;
    for (size_t i = 0; i < policies_contents.size(); ++i)
    {
        std::string tmp;
        if (!validate_xslt_policy_from_memory(file, policies_contents[i], tmp))
        {
            std::string retain = tmp;
            tmp = std::string();
            if (!validate_schematron_policy_from_memory(file, policies_contents[i], tmp))
            {
                Out << retain;
                valid = false;
            }
            else
                Out << tmp;
        }
        else
        {
            Out << tmp;
            if (!policy_is_valid(tmp))
                valid = false;
        }
    }
    return valid;
}

//---------------------------------------------------------------------------
bool Core::policies_check_files(const std::string& file,
                                const std::vector<std::string>& policies_names,
                                std::stringstream& Out)
{
    bool valid = true;
    for (size_t i = 0; i < policies_names.size(); ++i)
    {
        std::string tmp;
        if (is_schematron_file(policies_names[i]))
        {
            if (!validate_schematron_policy_from_file(file, policies_names[i], tmp))
            {
                std::string retain = tmp;
                tmp = std::string();
                if (!validate_xslt_policy_from_file(file, policies_names[i], tmp))
                {
                    Out << file << retain;
                    valid = false;
                }
                else
                {
                    Out << tmp;
                    if (!policy_is_valid(tmp))
                        valid = false;
                }
            }
            else
                Out << file << ": "  << tmp;
        }
        else
        {
            if (!validate_xslt_policy_from_file(file, policies_names[i], tmp))
            {
                std::string retain = tmp;
                tmp = std::string();
                if (!validate_schematron_policy_from_file(file, policies_names[i], tmp))
                {
                    Out << retain;
                    valid = false;
                }
                else
                    Out << file << ": "  << tmp;
            }
            else
            {
                if (!policy_is_valid(tmp))
                    valid = false;
                Out << tmp;
            }
        }
    }
    return valid;
}

//---------------------------------------------------------------------------
int Core::policies_check(const std::vector<std::string>& files,
                         MediaConchLib::ReportRes* result,
                         const std::vector<std::string>* policies_names,
                         const std::vector<std::string>* policies_contents)
{
    if (!files.size())
    {
        result->report = "No file to validate";
        return -1;
    }

    if (!policies_names && !policies_contents)
    {
        result->report = "No policy to apply";
        return -1;
    }

    std::stringstream Out;
    bool valid = true;
    for (size_t i = 0; i < files.size(); ++i)
    {
        if (policies_names)
            if (!policies_check_files(files[i], *policies_names, Out))
                valid = false;
        if (policies_contents)
            if (!policies_check_contents(files[i], *policies_contents, Out))
                valid = false;
    }
    result->report = Out.str();
    result->has_valid = true;
    result->valid = valid;
    return 0;
}

// //---------------------------------------------------------------------------
// bool Core::policy_schematron(const std::string& file, const std::string& policy,
//                              MediaConchLib::format, std::stringstream& Out)
// {
//     Schema* S = new Schematron;
//     bool valid = false;

//     if (S->register_schema_from_file(policy.c_str()))
//     {
//         std::string report;
//         valid = validation(file, S, report);

//         if (!valid)
//         {
//             Out << "NOT VALID\n";
//             Out << report;
//         }
//         else
//             Out << "VALID";
//     }
//     else
//     {
//         std::vector<std::string> errors = S->get_errors();
//         Out << "internal error for parsing file" << std::endl;
//         for (size_t pos = 0; pos < errors.size(); pos++)
//             Out << "\t" << errors[pos].c_str();
//     }
//     delete S;
//     return valid;
// }

// //---------------------------------------------------------------------------
// bool Core::policy_xslt(const std::string& file, const std::string& policy, MediaConchLib::format f, std::stringstream& Out)
// {
//     Schema *S = new Xslt;
//     bool valid = false;

//     if (S->register_schema_from_file(policy.c_str()))
//     {
//         std::string report;
//         valid = validation(file, S, report);
//         if (xslt_display.length())
//             transform_with_xslt_file(report, xslt_display, report);
//         else 
//             switch (f)
//             {
//                 case MediaConchLib::format_Text:
//                                         // Apply an XSLT to have Text
//                                         transform_with_xslt_text_memory(report, report);
//                                     break;
//                 case MediaConchLib::format_Html:
//                                         // Apply an XSLT to have HTML
//                                         transform_with_xslt_html_memory(report, report);
//                                     break;
//                 default:            ;
//             }

//         Out << report;
//     }
//     else
//     {
//         std::vector<std::string> errors = S->get_errors();
//         Out << file << ": ";
//         Out << "internal error for parsing file" << endl;
//         for (size_t pos = 0; pos < errors.size(); pos++)
//             Out << "\t" << errors[pos].c_str();
//     }
//     delete S;
//     return valid;
// }

//***************************************************************************
// API
//***************************************************************************

//---------------------------------------------------------------------------
bool Core::validate_policy(const std::string& file, int policy,
                           MediaConchLib::ReportRes* result,
                           const std::string* display_name,
                           const std::string* display_content)
{
    bool valid = false;
    if (policy == -1)
    {
        result->report = "Policy not found";
        return valid;
    }

    if (policies.policies[policy]->type == Policies::POLICY_SCHEMATRON)
        valid = validate_schematron_policy(file, policy, result->report);
    else
        valid = validate_xslt_policy(file, policy, result->report);

    if (display_name)
        transform_with_xslt_file(result->report, *display_name, result->report);
    else if (display_content)
        transform_with_xslt_memory(result->report, *display_content, result->report);

    return valid;
}

//---------------------------------------------------------------------------
bool Core::validate_policy_file(const std::string& file, const std::string& policy,
                                MediaConchLib::ReportRes* result,
                                const std::string* display_name,
                                const std::string* display_content)
{
    if (!policy.length())
    {
        result->report = "Policy invalid";
        return false;
    }

    bool valid = validate_schematron_policy_from_file(file, policy, result->report);
    if (!valid)
    {
        std::string tmp = result->report;
        valid = validate_xslt_policy_from_file(file, policy, result->report);
        if (!valid)
            result->report = tmp;
    }

    if (display_name)
        transform_with_xslt_file(result->report, *display_name, result->report);
    else if (display_content)
        transform_with_xslt_memory(result->report, *display_content, result->report);

    return valid;
}

//---------------------------------------------------------------------------
bool Core::validate_policy_memory(const std::string& file, const std::string& policy,
                                  MediaConchLib::ReportRes* result,
                                  const std::string* display_name,
                                  const std::string* display_content)
{
    if (!policy.length())
    {
        result->report = "Policy invalid";
        return false;
    }

    bool valid = validate_schematron_policy_from_memory(file, policy, result->report);
    if (!valid)
    {
        std::string tmp = result->report;
        valid = validate_xslt_policy_from_memory(file, policy, result->report);
        if (!valid)
            result->report = tmp;
    }

    if (display_name)
        transform_with_xslt_file(result->report, *display_name, result->report);
    else if (display_content)
        transform_with_xslt_memory(result->report, *display_content, result->report);

    return valid;
}

//---------------------------------------------------------------------------
int Core::transform_with_xslt_file(const std::string& report, const std::string& xslt, std::string& result)
{
    Schema *S = new Xslt;

    if (!S->register_schema_from_file(xslt.c_str()))
    {
        result = report;
        return -1;
    }

    int valid = S->validate_xml(report);
    if (valid < 0)
    {
        result = report;
        return -1;
    }

    result = S->get_report();
    delete S;
    return 0;
}

//---------------------------------------------------------------------------
int Core::transform_with_xslt_memory(const std::string& report, const std::string& memory,
                                     std::string& result)
{
    Schema *S = new Xslt;

    if (!S->register_schema_from_memory(memory))
    {
        result = report;
        return -1;
    }

    int valid = S->validate_xml(report);
    if (valid < 0)
    {
        result = report;
        return -1;
    }

    result = S->get_report();
    delete S;
    return 0;
}

//---------------------------------------------------------------------------
int Core::transform_with_xslt_html_memory(const std::string& report, std::string& result)
{
    std::string memory(implementation_report_display_html_xsl);
    return transform_with_xslt_memory(report, memory, result);
}

//---------------------------------------------------------------------------
int Core::transform_with_xslt_text_memory(const std::string& report, std::string& result)
{
#if defined(_WIN32) || defined(WIN32)
    std::string memory(implementation_report_display_text_xsl);
#else //defined(_WIN32) || defined(WIN32)
    std::string memory(implementation_report_display_textunicode_xsl);
#endif //defined(_WIN32) || defined(WIN32)
    return transform_with_xslt_memory(report, memory, result);
}

//***************************************************************************
// HELPER
//***************************************************************************

//---------------------------------------------------------------------------
bool Core::validate_schematron_policy(const std::string& file, int pos, std::string& report)
{
    std::string policyFile;
    xmlDocPtr doc = NULL;
    Schema *S = new Schematron;
    bool valid = true;

    if (pos >= 0)
        doc = policies.create_doc(pos);

    if (doc && S->register_schema_from_doc(doc))
        valid = validation(file, S, report);
    else
    {
        valid = false;

        std::stringstream Out;
        std::vector<std::string> errors = S->get_errors();

        Out << "internal error for parsing Policy" << endl;
        for (size_t i = 0; i < errors.size(); i++)
            Out << "\t" << errors[i].c_str();
        report = Out.str();
    }
    xmlFreeDoc(doc);
    delete S;
    return valid;
}

//---------------------------------------------------------------------------
bool Core::validate_schematron_policy_from_file(const std::string& file, const std::string& policy, std::string& report)
{
    bool valid = true;
    Schema *S = new Schematron;

    if (S->register_schema_from_file(policy.c_str()))
        valid = validation(file, S, report);
    else
    {
        valid = false;

        std::stringstream Out;
        std::vector<std::string> errors = S->get_errors();

        Out << "internal error for parsing Policy" << endl;
        for (size_t i = 0; i < errors.size(); i++)
            Out << "\t" << errors[i].c_str();
        report = Out.str();
    }
    delete S;
    return valid;
}

//---------------------------------------------------------------------------
bool Core::validate_schematron_policy_from_memory(const std::string& file, const std::string& memory, std::string& report)
{
    bool valid = true;
    Schema *S = new Schematron;

    if (S->register_schema_from_memory(memory))
        valid = validation(file, S, report);
    else
    {
        valid = false;

        std::stringstream Out;
        std::vector<std::string> errors = S->get_errors();

        Out << "internal error for parsing Policy" << endl;
        for (size_t i = 0; i < errors.size(); i++)
            Out << "\t" << errors[i].c_str();
        report = Out.str();
    }
    delete S;
    return valid;
}

//---------------------------------------------------------------------------
bool Core::validate_xslt_policy(const std::string& file, int pos, std::string& report)
{
    bool valid = true;
    std::string policyFile;
    xmlDocPtr doc = NULL;
    Schema *S = new Xslt;

    if (pos >= 0)
        doc = policies.create_doc(pos);

    if (doc && S->register_schema_from_doc(doc))
        valid = validation(file, S, report);
    else
    {
        valid = false;

        std::stringstream Out;
        std::vector<std::string> errors = S->get_errors();

        Out << "internal error for parsing Policy" << endl;
        for (size_t i = 0; i < errors.size(); i++)
            Out << "\t" << errors[i].c_str();
        report = Out.str();
    }
    delete S;
    return valid;
}

//---------------------------------------------------------------------------
bool Core::validate_xslt_policy_from_file(const std::string& file, const std::string& policy, std::string& report)
{
    bool valid = true;
    Schema *S = new Xslt;

    if (S->register_schema_from_file(policy.c_str()))
        valid = validation(file, S, report);
    else
    {
        valid = false;

        std::stringstream Out;
        std::vector<std::string> errors = S->get_errors();

        Out << "internal error for parsing Policy" << endl;
        for (size_t i = 0; i < errors.size(); i++)
            Out << "\t" << errors[i].c_str();
        report = Out.str();
    }
    delete S;
    return valid;
}

//---------------------------------------------------------------------------
bool Core::validate_xslt_policy_from_memory(const std::string& file, const std::string& memory,
                                            std::string& report, bool is_implem)
{
    bool valid = true;
    Schema *S = new Xslt;

    if (is_implem)
        S->set_options(implementation_options);

    if (S->register_schema_from_memory(memory))
        valid = validation(file, S, report);
    else
    {
        valid = false;

        std::stringstream Out;
        std::vector<std::string> errors = S->get_errors();

        Out << "internal error for parsing Policy" << endl;
        for (size_t i = 0; i < errors.size(); i++)
            Out << "\t" << errors[i].c_str();
        report = Out.str();
    }
    delete S;
    return valid;
}

//---------------------------------------------------------------------------
bool Core::validation(const std::string& file, Schema* S, std::string& report)
{
    std::string xml;
    get_report_saved(file, MediaConchLib::report_MediaConch,
                     MediaConchLib::format_MaXml, xml);
    bool valid = true;

    int ret = S->validate_xml(xml);
    if (ret < 0)
    {
        report = "Validation generated an internal error";
        valid = false;
    }
    else
    {
        report = S->get_report();
        valid = ret == 0 ? true : false;
    }
    return valid;
}

bool Core::is_schematron_file(const std::string& file)
{
    if (file.compare(file.length() - 4, 4, ".sch"))
        return false;

    return true;
}

//---------------------------------------------------------------------------
void Core::compress_report(std::string& report, MediaConchLib::compression& compress)
{
    if (compress == MediaConchLib::compression_None || compress >= MediaConchLib::compression_Max)
        return;

    if (compress == MediaConchLib::compression_ZLib)
    {
        uLongf dst_len, src_len;

        src_len = report.length();
        dst_len = src_len;
        Bytef* dst = new Bytef [src_len + 1];

        if (compress2((Bytef*)dst, &dst_len, (const Bytef*)report.c_str(), src_len, Z_BEST_COMPRESSION) != Z_OK || src_len <= dst_len)
            //Fallback to no compression
            compress = MediaConchLib::compression_None;
        else
            report = std::string((const char *)dst, dst_len);
        delete [] dst;
    }
}

//---------------------------------------------------------------------------
int Core::uncompress_report(std::string& report, MediaConchLib::compression compress)
{
    switch (compress)
    {
        case MediaConchLib::compression_None:
            break;
        case MediaConchLib::compression_ZLib:
            uLongf dst_len, src_len;

            src_len = report.length();
            dst_len = src_len;

            do
            {
                Bytef* dst = new Bytef [dst_len + 1];

                int ret;
                if ((ret = uncompress(dst, &dst_len, (const Bytef*)report.c_str(), src_len)) != Z_OK)
                {
                    delete [] dst;
                    if (ret == Z_BUF_ERROR)
                    {
                        dst_len += 4096;
                        continue;
                    }
                    return -1;
                }
                report = std::string((const char*)dst, dst_len);
                delete [] dst;
                break;
            } while (1);
            break;
        default:
            break;
    }
    return 0;
}

//---------------------------------------------------------------------------
std::string Core::get_last_modification_file(const std::string& filename)
{
    Ztring time = ZenLib::File::Modified_Get(Ztring().From_UTF8(filename));
    return time.To_UTF8();
}

//---------------------------------------------------------------------------
void Core::register_report_mediainfo_text_to_database(std::string& file, const std::string& time,
                                                      MediaInfoNameSpace::MediaInfoList* curMI)
{
    curMI->Option(__T("Details"), __T("0"));
    curMI->Option(__T("Inform"), String());
    std::string report = Ztring(curMI->Inform()).To_UTF8();

    MediaConchLib::compression mode = compression_mode;
    compress_report(report, mode);
    db->save_report(MediaConchLib::report_MediaInfo, MediaConchLib::format_Text,
                    file, time,
                    report, mode);
}

//---------------------------------------------------------------------------
void Core::register_report_mediainfo_xml_to_database(std::string& file, const std::string& time,
                                                     MediaInfoNameSpace::MediaInfoList* curMI)
{
    curMI->Option(__T("Details"), __T("0"));
    curMI->Option(__T("Inform"), __T("MIXML"));
    std::string report = Ztring(curMI->Inform()).To_UTF8();
    MediaConchLib::compression mode = compression_mode;
    compress_report(report, mode);
    db->save_report(MediaConchLib::report_MediaInfo, MediaConchLib::format_Xml,
                    file, time,
                    report, mode);
}

//---------------------------------------------------------------------------
void Core::register_report_mediatrace_xml_to_database(std::string& file, const std::string& time,
                                                      MediaInfoNameSpace::MediaInfoList* curMI)
{
    curMI->Option(__T("Details"), __T("1"));
    curMI->Option(__T("Inform"), __T("XML"));
    std::string report = Ztring(curMI->Inform()).To_UTF8();
    MediaConchLib::compression mode = compression_mode;
    compress_report(report, mode);
    db->save_report(MediaConchLib::report_MediaTrace, MediaConchLib::format_Xml,
                    file, time,
                    report, mode);
}

//---------------------------------------------------------------------------
void Core::register_report_implementation_xml_to_database(const std::string& file, const std::string& time,
                                                          std::string& report)
{
    MediaConchLib::compression mode = compression_mode;
    compress_report(report, mode);
    db->save_report(MediaConchLib::report_MediaConch, MediaConchLib::format_Xml,
                    file, time,
                    report, mode);
}

//---------------------------------------------------------------------------
void Core::get_content_of_media_in_xml(std::string& report)
{
    std::string media_start("<media ref");
    size_t start = report.find(media_start);

    if (start == std::string::npos)
        return;

    start += media_start.length();
    media_start = std::string(">");
    start = report.find(media_start, start);
    size_t end = report.rfind("</media>");
    if (start == std::string::npos || end == std::string::npos)
        return;

    report = report.substr(start + media_start.length(), end - start - media_start.length());
}

//---------------------------------------------------------------------------
void Core::register_file_to_database(std::string& filename, MediaInfoNameSpace::MediaInfoList* curMI)
{
    const std::string& time = get_last_modification_file(filename);

    // MediaInfo
    register_report_mediainfo_text_to_database(filename, time, curMI);
    register_report_mediainfo_xml_to_database(filename, time, curMI);

    // MediaTrace
    register_report_mediatrace_xml_to_database(filename, time, curMI);
}

//---------------------------------------------------------------------------
void Core::register_file_to_database(std::string& filename)
{
    const std::string& time = get_last_modification_file(filename);

    // MediaInfo
    register_report_mediainfo_text_to_database(filename, time, MI);
    register_report_mediainfo_xml_to_database(filename, time, MI);

    // MediaTrace
    register_report_mediatrace_xml_to_database(filename, time, MI);
}

//---------------------------------------------------------------------------
void Core::create_report_mi_xml(const std::string& filename, std::string& report)
{
    get_report_saved(filename, MediaConchLib::report_MediaInfo, MediaConchLib::format_Xml, report);
}

//---------------------------------------------------------------------------
void Core::create_report_mt_xml(const std::string& filename, std::string& report)
{
    get_report_saved(filename, MediaConchLib::report_MediaTrace, MediaConchLib::format_Xml, report);
}

//---------------------------------------------------------------------------
void Core::create_report_ma_xml(const std::string& filename, std::string& report,
                                std::bitset<MediaConchLib::report_Max> reports)
{
    std::string version = ZenLib::Ztring(Menu_Option_Preferences_Option (__T("Info_Version"), __T(""))).To_UTF8();
    std::string search(" - v");
    size_t pos = version.find(search);
    if (pos != std::string::npos)
        version = version.substr(pos + search.length());

    std::stringstream start;
    start << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    start << "<MediaArea\n";
    start << "xmlns=\"https://mediaarea.net/mediaarea\"\n";
    start << "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
    start << "xsi:schemaLocation=\"https://mediaarea.net/mediaarea https://mediaarea.net/mediaarea/mediaarea_0_1.xsd\"\n";
    start << "version=\"0.1\">\n";
    start << "<!-- Work in progress, not for production -->\n";
    start << "<creatingLibrary version=\"";
    start << version;
    start << "\" url=\"https://mediaarea.net/MediaInfo\">MediaInfoLib</creatingLibrary>\n";
    start << "<media ref=\"";
    start << filename << "\">\n";
    report += start.str();

    if (reports[MediaConchLib::report_MediaInfo])
    {
        std::string info;
        get_report_saved(filename, MediaConchLib::report_MediaInfo, MediaConchLib::format_Xml, info);
        get_content_of_media_in_xml(info);
        if (info.length())
            report += "<MediaInfo xmlns=\"https://mediaarea.net/mediainfo\" version=\"2.0beta1\">" + info + "</MediaInfo>\n";
    }

    if (reports[MediaConchLib::report_MediaTrace])
    {
        std::string trace;
        get_report_saved(filename, MediaConchLib::report_MediaTrace,
                         MediaConchLib::format_Xml, trace);
        get_content_of_media_in_xml(trace);
        if (trace.length())
            report += "<MediaTrace xmlns=\"https://mediaarea.net/mediatrace\" version=\"0.1\">"
                + trace + "</MediaTrace>\n";
    }

    if (reports[MediaConchLib::report_MediaConch])
    {
        std::string implem;
        get_implementation_report(filename, implem);
        get_content_of_media_in_xml(implem);
        if (implem.length())
        {
            report += "<MediaConch xmlns=\"https://mediaarea.net/mediaconch\" version=\"0.1\">"
            + implem + "</MediaConch>\n";
        }
    }
    report += std::string("</media>\n"
                          "</MediaArea>");
}

//---------------------------------------------------------------------------
void Core::get_report_saved(const std::string& filename,
                            MediaConchLib::report reportKind, MediaConchLib::format f,
                            std::string& report)
{
    if (f == MediaConchLib::format_MaXml)
    {
        create_report_ma_xml(filename, report, get_bitset_with_mi_mt());
        return;
    }

    if (reportKind >= MediaConchLib::report_Max || f >= MediaConchLib::format_Max)
        return;

    if (!get_db())
        return;
    MediaConchLib::compression compress;
    std::string time = get_last_modification_file(filename);
    std::string raw;
    db->get_report(reportKind, f, filename, time, raw, compress);
    uncompress_report(raw, compress);
    report += raw;
}

//---------------------------------------------------------------------------
void Core::get_reports_output(const std::string& file, MediaConchLib::format f,
                              std::bitset<MediaConchLib::report_Max> report_set,
                              MediaConchLib::ReportRes* result)
{
    if (f == MediaConchLib::format_MaXml)
    {
        create_report_ma_xml(file, result->report, report_set);
        result->report += "\r\n";
    }
    else
    {
        if (report_set[MediaConchLib::report_MediaInfo])
        {
            if (f == MediaConchLib::format_Xml)
                create_report_mi_xml(file, result->report);
            else
                get_report_saved(file, MediaConchLib::report_MediaInfo, f, result->report);
            result->report += "\r\n";
        }
        if (report_set[MediaConchLib::report_MediaTrace])
        {
            std::string tmp;
            create_report_mt_xml(file, tmp);
            if (f == MediaConchLib::format_Xml)
                result->report += tmp;
            else if (f == MediaConchLib::format_Html)
            {
                // Apply an XSLT to have HTML
                std::string memory(media_trace_display_html_xsl);
                transform_with_xslt_memory(tmp, memory, tmp);
                result->report += tmp;
            }
            else if (f == MediaConchLib::format_Text)
            {
                // Apply an XSLT to have Text
                std::string memory(media_trace_display_text_xsl);
                transform_with_xslt_memory(tmp, memory, tmp);
                result->report += tmp;
            }
            result->report += "\r\n";
        }

        if (report_set[MediaConchLib::report_MediaConch])
        {
            std::string tmp;
            bool is_valid = true;
            if (f == MediaConchLib::format_Xml)
                get_reports_output_Xml_Implementation(file, tmp, is_valid);
            else
                get_reports_output_Text_Implementation(file, tmp, is_valid);
            result->report += tmp;

            if (!result->has_valid)
                result->has_valid = true;
            if (!is_valid)
                result->valid = false;
            result->report += "\r\n";
        }
    }
}

//---------------------------------------------------------------------------
void Core::get_implementation_report(const std::string& file, std::string& report)
{
    std::string r;
    get_report_saved(file, MediaConchLib::report_MediaConch,
                     MediaConchLib::format_Xml, r);

    if (!r.length())
    {
        bool valid;
        std::string memory(implementation_report_xsl);
        valid = validate_xslt_policy_from_memory(file, memory, r, true);
        if (valid)
        {
            std::string tmp = r;
            std::string time = get_last_modification_file(file);
            register_report_implementation_xml_to_database(file, time, tmp);
        }
    }
    report += r;
}

//---------------------------------------------------------------------------
bool Core::file_is_registered_in_db(const std::string& filename)
{
    if (!get_db())
        return false;

    std::string time = get_last_modification_file(filename);

    return db->file_is_registered(MediaConchLib::report_MediaInfo, MediaConchLib::format_Xml, filename, time);
}

//---------------------------------------------------------------------------
bool Core::file_is_registered_in_queue(const std::string& filename)
{
    if (!scheduler)
        return false;

    return scheduler->element_exists(filename);;
}

//---------------------------------------------------------------------------
bool Core::file_is_registered(const std::string& filename)
{
    if (file_is_registered_in_queue(filename))
        return true;
    return file_is_registered_in_db(filename);
}

//---------------------------------------------------------------------------
std::string Core::get_local_config_path()
{
    std::string local_path(".");
#if defined(WINDOWS)
    char username[UNLEN+1];
    DWORD username_len = UNLEN+1;
    GetUserNameA(username, &username_len);

    std::string wuser(username, username_len-1);
    std::string user_name(wuser.begin(), wuser.end());
    std::stringstream path;

    path << "C:\\Users\\" << user_name << "\\AppData\\Roaming\\MediaConch\\";
    local_path = path.str();
#elif defined(UNIX)
    const char* home = NULL;

    if ((home = getenv("HOME")) == NULL)
    {
        struct passwd *pw = getpwuid(getuid());
        if (pw)
            home = pw->pw_dir;
        else
            home = ".";
    }
    local_path = std::string(home) + Path_Separator + std::string(".config/");
#elif defined(MACOS) || defined(MACOSX)
    const char* user = NULL;

    if ((user = getenv("USER")) == NULL)
    {
        struct passwd *pw = getpwuid(getuid());
        if (pw)
            user = pw->pw_name;
    }
    local_path = std::string("/Users/") + user + std::string("/Library/Preferences/");
#endif

    Ztring z_path = ZenLib::Ztring().From_UTF8(local_path);
    if (!ZenLib::Dir::Exists(z_path))
        ZenLib::Dir::Create(z_path);

    return local_path;
}

//---------------------------------------------------------------------------
std::string Core::get_local_data_path()
{
    std::string local_path(".");
#if defined(WINDOWS)
    char username[UNLEN+1];
    DWORD username_len = UNLEN+1;
    GetUserNameA(username, &username_len);

    std::string wuser(username, username_len-1);
    std::string user_name(wuser.begin(), wuser.end());
    std::stringstream path;

    path << "C:\\Users\\" << user_name << "\\AppData\\Roaming\\MediaConch\\";
    local_path = path.str();
#elif defined(UNIX)
    const char* home;

    if ((home = getenv("HOME")) == NULL)
    {
        struct passwd *pw = getpwuid(getuid());
        if (pw)
            home = pw->pw_dir;
        else
            home = ".";
    }
    local_path = std::string(home) + Path_Separator + std::string(".local/share/MediaConch/");
#elif defined(MACOS) || defined(MACOSX)
    const char* user = NULL;

    if ((user = getenv("USER")) == NULL)
    {
        struct passwd *pw = getpwuid(getuid());
        if (pw)
            user = pw->pw_name;
    }
    local_path = std::string("/Users/") + user + std::string("/Library/Application Support/MediaConch/");
#endif

    Ztring z_path = ZenLib::Ztring().From_UTF8(local_path);
    if (!ZenLib::Dir::Exists(z_path))
        ZenLib::Dir::Create(z_path);

    return local_path;
}

//---------------------------------------------------------------------------
std::string Core::get_database_path()
{
    std::string database_path = get_local_data_path();

    Ztring z_path = ZenLib::Ztring().From_UTF8(database_path);
    if (!ZenLib::Dir::Exists(z_path))
        database_path = std::string(".") + Path_Separator;
    return database_path;
}

//---------------------------------------------------------------------------
std::string Core::get_config_file()
{
    if (configuration_file.length())
        return configuration_file;

    std::string config_file = get_local_config_path();

    config_file += configName;
    std::ifstream ifile(config_file.c_str());
    if (!ifile)
        config_file = std::string(".") + Path_Separator + configName;
    return config_file;
}

//---------------------------------------------------------------------------
void Core::set_configuration_file(const std::string& file)
{
    configuration_file = file;
}

//---------------------------------------------------------------------------
const std::string& Core::get_configuration_file() const
{
    return configuration_file;
}

//---------------------------------------------------------------------------
bool Core::database_is_enabled() const
{
    if (!config)
        return false;
    bool enabled = false;
    if (config->get("Database_Enabled", enabled))
        return false;
    return enabled;
}

//---------------------------------------------------------------------------
Database *Core::get_db()
{
    if (!db)
        load_database();
    return db;
}

//---------------------------------------------------------------------------
void Core::WaitRunIsFinished()
{
    while (1)
    {
        if (scheduler->is_finished())
            break;
        #ifdef WINDOWS
        ::Sleep((DWORD)50);
        #else
        usleep(50000);
        #endif
    }
}

//---------------------------------------------------------------------------
std::bitset<MediaConchLib::report_Max> Core::get_bitset_with_mi_mt()
{
    std::bitset<MediaConchLib::report_Max> bits;

    bits.set(MediaConchLib::report_MediaInfo);
    bits.set(MediaConchLib::report_MediaTrace);
    return bits;
}

//---------------------------------------------------------------------------
bool Core::is_using_daemon() const
{
    bool use_daemon = false;
    if (config->get("Use_Daemon", use_daemon) < 0)
        return false;
    return use_daemon;
}

//---------------------------------------------------------------------------
void Core::get_daemon_address(std::string& addr, int& port) const
{
    config->get("Daemon_Address", addr);
    long int port_l;
    if (config->get("Daemon_Port", port_l) >= 0)
        port = (int)port_l;
}

//---------------------------------------------------------------------------
bool Core::policy_is_valid(const std::string& report)
{
    size_t pos = report.find(" outcome=\"fail\"");
    if (pos != std::string::npos)
        return false;
    return true;
}

}
