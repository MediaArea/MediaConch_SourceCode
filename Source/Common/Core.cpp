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

const std::string Core::database_name = "MediaConch.db";

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
    db->set_database_filename(database_name);
    db->init_report();
#else
    db = new NoDatabase;
    db->init_report();
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

        f[pos]='/'; // Windows path separators are replaced by / else it is rejected by libxml2
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
void Core::set_implementation_verbosity(const std::string& verbosity)
{
    if (!verbosity.length())
        return;
    std::string v;

    if (verbosity[0] == '"')
        v = std::string("'") + verbosity + std::string("'");
    else
        v = std::string("\"") + verbosity + std::string("\"");

    implementation_options["verbosity"] = v;
}

//---------------------------------------------------------------------------
const std::string& Core::get_implementation_verbosity()
{
    return implementation_options["verbosity"];
}

//---------------------------------------------------------------------------
void Core::set_compression_mode(MediaConchLib::compression compress)
{
    compression_mode = compress;
}

//---------------------------------------------------------------------------
int Core::get_ui_poll_request() const
{
    long ui_poll_request = 5000;
    if (!config->get("UI_Poll_Request", ui_poll_request))
    {
        if (ui_poll_request < 500 || ui_poll_request > 10000)
            ui_poll_request = 1000;
    }
    return ui_poll_request;
}

//---------------------------------------------------------------------------
int Core::get_ui_database_path(std::string& path) const
{
    return config->get("UI_Database_Path", path);
}

//***************************************************************************
// Tools
//***************************************************************************

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
void Core::list(std::vector<std::string>& vec)
{
    scheduler->get_elements(vec);
    db_mutex.Enter();
    get_db()->get_elements(vec);
    db_mutex.Leave();
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
        switch (f)
        {
            case MediaConchLib::format_Text:
            case MediaConchLib::format_Xml:
            case MediaConchLib::format_MaXml:
                get_reports_output(files, f, report_set, result);
                break;
            case MediaConchLib::format_JsTree:
                get_reports_output_JStree(files, report_set, result->report);
                break;
            case MediaConchLib::format_Html:
                get_reports_output_Html(files, report_set, result->report);
                break;
            default:
                return -1;
        }
    }

    if (display_name)
        transform_with_xslt_file(result->report, *display_name, result->report);
    else if (display_content)
        transform_with_xslt_memory(result->report, *display_content, result->report);
    return 0;
}

//---------------------------------------------------------------------------
int Core::validate(MediaConchLib::report report, const std::vector<std::string>& files,
                   const std::vector<std::string>& policies_names,
                   const std::vector<std::string>& policies_contents,
                   std::vector<MediaConchLib::ValidateRes*>& result)
{
    if (report != MediaConchLib::report_MediaConch && !policies_names.size() && !policies_contents.size())
        return -1;

    std::vector<std::string> file_tmp;
    for (size_t i = 0; i < files.size(); ++i)
    {
        file_tmp.clear();
        file_tmp.push_back(files[i]);

        MediaConchLib::ValidateRes* res = new MediaConchLib::ValidateRes;
        res->file = files[i];
        if (report == MediaConchLib::report_MediaConch)
        {
            std::string report;
            res->valid = get_implementation_report(file_tmp, report);
        }
        else if (!policies_names.empty() || !policies_contents.empty())
        {
            MediaConchLib::ReportRes tmp_res;
            if (policies_check(file_tmp, &tmp_res,
                               policies_names.size() ? &policies_names : NULL,
                               policies_contents.size() ? &policies_contents : NULL) < 0)
                continue;
            res->valid = tmp_res.has_valid ? tmp_res.valid : true;
        }
        result.push_back(res);
    }
    return 0;
}

//---------------------------------------------------------------------------
int Core::remove_report(const std::vector<std::string>& files)
{
    if (!get_db())
        return false;

    db_mutex.Enter();
    for (size_t i = 0; i < files.size(); ++i)
        db->remove_report(files[i]);
    db_mutex.Leave();

    return 0;
}

//---------------------------------------------------------------------------
int Core::get_reports_output_JStree(const std::vector<std::string>& files,
                                    const std::bitset<MediaConchLib::report_Max>& report_set,
                                    std::string& report)
{
    std::vector<std::string> vec;
    for (size_t i = 0; i < files.size(); ++i)
    {
        vec.clear();
        vec.push_back(files[i]);
        JsTree js;
        if (i)
            report += "\n";
        if (report_set[MediaConchLib::report_MediaInfo])
        {
            std::string ret;
            get_report_saved(vec, MediaConchLib::report_MediaInfo,
                             MediaConchLib::format_Xml, ret);
            report += js.format_from_inform_XML(ret);
        }

        if (report_set[MediaConchLib::report_MediaTrace])
        {
            std::string ret;
            get_report_saved(vec, MediaConchLib::report_MediaTrace,
                         MediaConchLib::format_Xml, ret);
            report += js.format_from_trace_XML(ret);
        }
    }

    return 0;
}

//---------------------------------------------------------------------------
int Core::get_reports_output_Html(const std::vector<std::string>& files,
                                  const std::bitset<MediaConchLib::report_Max>& report_set,
                                  std::string& report)
{
    if (report_set[MediaConchLib::report_MediaConch])
    {
        std::string tmp;
        if (get_implementation_report(files, tmp))
        {
            // Apply an XSLT to have HTML
            transform_with_xslt_html_memory(tmp, tmp);
        }
        report += tmp;
    }

    if (report_set[MediaConchLib::report_MediaTrace])
    {
        std::string tmp;
        create_report_mt_xml(files, tmp);

        // Apply an XSLT to have HTML
        std::string memory(media_trace_display_html_xsl);
        transform_with_xslt_memory(tmp, memory, tmp);
        report += tmp;
    }

    return 0;
}

//---------------------------------------------------------------------------
int Core::get_reports_output_Text_Implementation(const std::vector<std::string>& files, std::string& report, bool& is_valid)
{
    std::string tmp;
    get_implementation_report(files, tmp);
    is_valid = policy_is_valid(tmp);

    if (is_valid)
    {
        // Apply an XSLT to have Text
        transform_with_xslt_text_memory(tmp, tmp);
    }
    report += tmp;
    return 0;
}

//---------------------------------------------------------------------------
int Core::get_reports_output_Xml_Implementation(const std::vector<std::string>& files, std::string& report, bool& is_valid)
{
    if (get_implementation_report(files, report))
        is_valid = policy_is_valid(report);
    else
        is_valid = false;
    return 0;
}

//---------------------------------------------------------------------------
bool Core::policies_check_contents(const std::vector<std::string>& files,
                                   const std::vector<std::string>& policies_contents,
                                   std::stringstream& Out)
{
    bool valid = true;
    for (size_t i = 0; i < policies_contents.size(); ++i)
    {
        std::string tmp;
        if (!validate_xslt_policy_from_memory(files, policies_contents[i], tmp))
        {
            std::string retain = tmp;
            tmp = std::string();
            if (!validate_schematron_policy_from_memory(files, policies_contents[i], tmp))
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
bool Core::policies_check_files(const std::vector<std::string>& files,
                                const std::vector<std::string>& policies_names,
                                std::stringstream& Out)
{
    bool valid = true;
    for (size_t i = 0; i < policies_names.size(); ++i)
    {
        std::string tmp;
        if (is_schematron_file(policies_names[i]))
        {
            if (!validate_schematron_policy_from_file(files, policies_names[i], tmp))
            {
                std::string retain = tmp;
                tmp = std::string();
                if (!validate_xslt_policy_from_file(files, policies_names[i], tmp))
                {
                    Out << policies_names[i] << retain;
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
                Out << policies_names[i] << ": "  << tmp;
        }
        else
        {
            if (!validate_xslt_policy_from_file(files, policies_names[i], tmp))
            {
                std::string retain = tmp;
                tmp = std::string();
                if (!validate_schematron_policy_from_file(files, policies_names[i], tmp))
                {
                    Out << retain;
                    valid = false;
                }
                else
                    Out << policies_names[i] << ": "  << tmp;
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
    if (policies_names)
        if (!policies_check_files(files, *policies_names, Out))
            valid = false;
    if (policies_contents)
        if (!policies_check_contents(files, *policies_contents, Out))
            valid = false;
    result->report = Out.str();
    result->has_valid = true;
    result->valid = valid;
    return 0;
}

//***************************************************************************
// XSL transform
//***************************************************************************

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
// Policy validation
//***************************************************************************

//---------------------------------------------------------------------------
bool Core::validate_schematron_policy(const std::vector<std::string>& files, int pos, std::string& report)
{
    std::string policyFile;
    xmlDocPtr doc = NULL;
    Schema *S = new Schematron;
    bool valid = true;

    if (pos >= 0)
        doc = policies.create_doc(pos);

    if (doc && S->register_schema_from_doc(doc))
        valid = validation(files, S, report);
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
bool Core::validate_schematron_policy_from_file(const std::vector<std::string>& files, const std::string& policy, std::string& report)
{
    bool valid = true;
    Schema *S = new Schematron;

    if (S->register_schema_from_file(policy.c_str()))
        valid = validation(files, S, report);
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
bool Core::validate_schematron_policy_from_memory(const std::vector<std::string>& files, const std::string& memory, std::string& report)
{
    bool valid = true;
    Schema *S = new Schematron;

    if (S->register_schema_from_memory(memory))
        valid = validation(files, S, report);
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
bool Core::validate_xslt_policy(const std::vector<std::string>& files, int pos, std::string& report)
{
    bool valid = true;
    std::string policyFile;
    xmlDocPtr doc = NULL;
    Schema *S = new Xslt;

    if (pos >= 0)
        doc = policies.create_doc(pos);

    if (doc && S->register_schema_from_doc(doc))
        valid = validation(files, S, report);
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
bool Core::validate_xslt_policy_from_file(const std::vector<std::string>& files, const std::string& policy, std::string& report)
{
    bool valid = true;
    Schema *S = new Xslt;

    if (S->register_schema_from_file(policy.c_str()))
        valid = validation(files, S, report);
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
bool Core::validate_xslt_policy_from_memory(const std::vector<std::string>& files, const std::string& memory,
                                            std::string& report, bool is_implem)
{
    bool valid = true;
    Schema *S = new Xslt;

    if (is_implem)
        S->set_options(implementation_options);

    if (S->register_schema_from_memory(memory))
        valid = validation(files, S, report);
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
bool Core::validation(const std::vector<std::string>& files, Schema* S, std::string& report)
{
    std::string xml;
    create_report_ma_xml(files, xml, get_bitset_with_mi_mt());
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

//***************************************************************************
// Compression
//***************************************************************************

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

//***************************************************************************
// HELPER
//***************************************************************************

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

    db_mutex.Enter();
    db->save_report(MediaConchLib::report_MediaInfo, MediaConchLib::format_Text,
                    file, time,
                    report, mode);
    db_mutex.Leave();
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
    db_mutex.Enter();
    db->save_report(MediaConchLib::report_MediaInfo, MediaConchLib::format_Xml,
                    file, time,
                    report, mode);
    db_mutex.Leave();
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
    db_mutex.Enter();
    db->save_report(MediaConchLib::report_MediaTrace, MediaConchLib::format_Xml,
                    file, time,
                    report, mode);
    db_mutex.Leave();
}

//---------------------------------------------------------------------------
void Core::get_content_of_media_in_xml(std::string& report)
{
    std::string media_start("<media ref");
    size_t start = report.find(media_start);

    if (start == std::string::npos)
    {
        report = std::string();
        return;
    }

    start += media_start.length();
    media_start = std::string(">");
    start = report.find(media_start, start);
    size_t end = report.rfind("</media>");
    if (start == std::string::npos || end == std::string::npos)
    {
        report = std::string();
        return;
    }

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
void Core::create_report_mi_xml(const std::vector<std::string>& files, std::string& report)
{
    if (files.size() == 1)
    {
        get_report_saved(files, MediaConchLib::report_MediaInfo, MediaConchLib::format_Xml, report);
        return;
    }

    std::string version = ZenLib::Ztring(Menu_Option_Preferences_Option (__T("Info_Version"), __T(""))).To_UTF8();
    std::string search(" - v");
    size_t pos = version.find(search);
    if (pos != std::string::npos)
        version = version.substr(pos + search.length());

    std::stringstream start;
    start << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    start << "<MediaInfo\n";
    start << "xmlns=\"https://mediaarea.net/mediainfo\"\n";
    start << "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
    start << "xsi:schemaLocation=\"https://mediaarea.net/mediaarea https://mediaarea.net/mediainfo/mediainfo_2_0.xsd\"\n";
    start << "version=\"2.0beta1\">\n";
    start << "<!-- Work in progress, not for production -->\n";
    start << "<creatingLibrary version=\"";
    start << version;
    start << "\" url=\"https://mediaarea.net/MediaInfo\">MediaInfoLib</creatingLibrary>\n";
    report += start.str();
    std::vector<std::string> vec;
    for (size_t i = 0; i < files.size(); ++i)
    {
        vec.clear();
        vec.push_back(files[i]);
        report += "<media ref=\"" + files[i] + "\">\n";

        std::string info;
        get_report_saved(vec, MediaConchLib::report_MediaInfo, MediaConchLib::format_Xml, info);
        get_content_of_media_in_xml(info);
        if (info.length())
            report += info;
        report += std::string("</media>\n");
    }
    report += std::string("</MediaInfo>");
}

//---------------------------------------------------------------------------
void Core::create_report_mt_xml(const std::vector<std::string>& files, std::string& report)
{
    if (files.size() == 1)
    {
        get_report_saved(files, MediaConchLib::report_MediaTrace, MediaConchLib::format_Xml, report);
        return;
    }

    std::string version = ZenLib::Ztring(Menu_Option_Preferences_Option (__T("Info_Version"), __T(""))).To_UTF8();
    std::string search(" - v");
    size_t pos = version.find(search);
    if (pos != std::string::npos)
        version = version.substr(pos + search.length());

    std::stringstream start;
    start << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    start << "<MediaTrace\n";
    start << "xmlns=\"https://mediaarea.net/mediatrace\"\n";
    start << "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
    start << "xsi:schemaLocation=\"https://mediaarea.net/mediaarea https://mediaarea.net/mediatrace/mediatrace_0_1.xsd\"\n";
    start << "version=\"0.1\">\n";
    start << "<creatingLibrary version=\"";
    start << version;
    start << "\" url=\"https://mediaarea.net/MediaInfo\">MediaInfoLib</creatingLibrary>\n";
    report += start.str();

    std::vector<std::string> vec;
    for (size_t i = 0; i < files.size(); ++i)
    {
        vec.clear();
        vec.push_back(files[i]);
        report += "<media ref=\"" + files[i] + "\">\n";

        std::string info;
        get_report_saved(vec, MediaConchLib::report_MediaTrace, MediaConchLib::format_Xml, info);
        get_content_of_media_in_xml(info);
        if (info.length())
            report += info;
        report += std::string("</media>\n");
    }
    report += std::string("</MediaTrace>");
}

//---------------------------------------------------------------------------
void Core::create_report_ma_xml(const std::vector<std::string>& files, std::string& report,
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
    report += start.str();

    std::vector<std::string> vec;
    for (size_t i = 0; i < files.size(); ++i)
    {
        vec.clear();
        vec.push_back(files[i]);
        report += "<media ref=\"" + files[i] + "\">\n";

        if (reports[MediaConchLib::report_MediaInfo])
        {
            std::string info;
            get_report_saved(vec, MediaConchLib::report_MediaInfo, MediaConchLib::format_Xml, info);
            get_content_of_media_in_xml(info);
            if (info.length())
                report += "<MediaInfo xmlns=\"https://mediaarea.net/mediainfo\" version=\"2.0beta1\">" + info + "</MediaInfo>\n";
        }

        if (reports[MediaConchLib::report_MediaTrace])
        {
        std::string trace;
        get_report_saved(vec, MediaConchLib::report_MediaTrace,
                         MediaConchLib::format_Xml, trace);
        get_content_of_media_in_xml(trace);
        if (trace.length())
            report += "<MediaTrace xmlns=\"https://mediaarea.net/mediatrace\" version=\"0.1\">"
                + trace + "</MediaTrace>\n";
        }

        if (reports[MediaConchLib::report_MediaConch])
        {
            std::string implem;
            if (get_implementation_report(vec, implem))
                get_content_of_media_in_xml(implem);
            else
                implem = std::string();

            report += "<MediaConch xmlns=\"https://mediaarea.net/mediaconch\" version=\"0.1\">"
                + implem + "</MediaConch>\n";
        }
        report += std::string("</media>\n");
    }
    report += std::string("</MediaArea>");
}

//---------------------------------------------------------------------------
void Core::get_report_saved(const std::vector<std::string>& files,
                            MediaConchLib::report reportKind, MediaConchLib::format f,
                            std::string& report)
{
    if (f == MediaConchLib::format_MaXml)
    {
        create_report_ma_xml(files, report, get_bitset_with_mi_mt());
        return;
    }

    if (reportKind >= MediaConchLib::report_Max || f >= MediaConchLib::format_Max)
        return;

    if (!get_db())
        return;

    if (reportKind == MediaConchLib::report_MediaConch)
    {
        std::string raw;
        get_implementation_report(files, raw);
        report += raw;
        return;
    }

    for (size_t i = 0; i < files.size(); ++i)
    {
        std::string raw;
        MediaConchLib::compression compress;
        std::string time = get_last_modification_file(files[i]);
        db_mutex.Enter();
        db->get_report(reportKind, f, files[i], time, raw, compress);
        db_mutex.Leave();
        uncompress_report(raw, compress);
        report += raw;
    }
}

//---------------------------------------------------------------------------
void Core::get_reports_output(const std::vector<std::string>& files, MediaConchLib::format f,
                              std::bitset<MediaConchLib::report_Max> report_set,
                              MediaConchLib::ReportRes* result)
{
    if (f == MediaConchLib::format_MaXml)
    {
        create_report_ma_xml(files, result->report, report_set);
        result->report += "\r\n";
    }
    else
    {
        if (report_set[MediaConchLib::report_MediaInfo])
        {
            if (f == MediaConchLib::format_Xml)
                create_report_mi_xml(files, result->report);
            else
                get_report_saved(files, MediaConchLib::report_MediaInfo, f, result->report);
            result->report += "\r\n";
        }
        if (report_set[MediaConchLib::report_MediaTrace])
        {
            std::string tmp;
            create_report_mt_xml(files, tmp);
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
                get_reports_output_Xml_Implementation(files, tmp, is_valid);
            else
                get_reports_output_Text_Implementation(files, tmp, is_valid);
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
bool Core::get_implementation_report(const std::vector<std::string>& files, std::string& report)
{
    std::string memory(implementation_report_xsl);
    std::string r;
    bool valid = validate_xslt_policy_from_memory(files, memory, r, true);
    report += r;
    return valid;
}

//---------------------------------------------------------------------------
bool Core::file_is_registered_in_db(const std::string& filename)
{
    if (!get_db())
        return false;

    std::string time = get_last_modification_file(filename);

    db_mutex.Enter();
    bool res = db->file_is_registered(MediaConchLib::report_MediaInfo,
                                      MediaConchLib::format_Xml, filename, time);
    db_mutex.Leave();
    return res;
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

    path << "C:/Users/" << user_name << "/AppData/Roaming/MediaConch/";
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

    path << "C:/Users/" << user_name << "/AppData/Roaming/MediaConch/";
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
