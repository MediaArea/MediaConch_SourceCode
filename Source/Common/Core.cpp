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
#include "NoDatabaseReport.h"
#include "SQLLiteReport.h"
#include "Configuration.h"
#include "Json.h"
#include "PluginLog.h"
#include "Common/Xslt.h"
#include "Common/JS_Tree.h"
#include "Common/WatchFoldersManager.h"
#include "Common/PluginsManager.h"
#include "Common/PluginsConfig.h"
#include "Common/Plugin.h"
#include "Common/generated/ImplementationReportXsl.h"
#include "Common/generated/ImplementationReportVeraPDFXsl.h"
#include "Common/generated/ImplementationReportDPFManagerXsl.h"
#if defined(_WIN32) || defined(WIN32)
#include "Common/generated/ImplementationReportDisplayTextXsl.h"
#include <Shlobj.h>
#else //defined(_WIN32) || defined(WIN32)
#include "Common/generated/ImplementationReportDisplayTextUnicodeXsl.h"
#endif //defined(_WIN32) || defined(WIN32)
#include "Common/generated/ImplementationReportDisplayHtmlXsl.h"
#include "Common/generated/ImplementationReportDisplaySimpleXsl.h"
#include "Common/generated/ImplementationReportDisplayCSVXsl.h"
#include "Common/generated/ImplementationReportMatroskaSchema.h"
#include "Common/generated/MediaTraceDisplayTextXsl.h"
#include "Common/generated/MediaTraceDisplayHtmlXsl.h"
#include "Common/generated/MicroMediaTraceToMediaTraceXsl.h"
#include "Common/generated/GeneratedCSVVideos.h"
#include "ZenLib/Ztring.h"
#include "ZenLib/File.h"
#include "ZenLib/Dir.h"
#include <zlib.h>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <ctime>
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
    MI=new MediaInfoNameSpace::MediaInfo;

    config = NULL;
    db = NULL;
    scheduler = new Scheduler(this);
    plugins_manager = new PluginsManager(this);
    watch_folders_manager = new WatchFoldersManager(this);
    policies.create_values_from_csv();
    compression_mode = MediaConchLib::compression_ZLib;
}

Core::~Core()
{
    if (watch_folders_manager)
        delete watch_folders_manager;
    if (scheduler)
        delete scheduler;
    if (plugins_manager)
        delete plugins_manager;
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
    if (config->parse() < 0)
        return;

    long scheduler_max_threads = 1;
    if (scheduler && !config->get("Scheduler_Max_Threads", scheduler_max_threads))
    {
        if (scheduler_max_threads <= 0)
            scheduler_max_threads = 1;
        scheduler->set_max_threads((size_t)scheduler_max_threads);
    }

    std::vector<Container::Value> plugins;
    if (!config->get("Plugins", plugins))
    {
        std::string error;
        PluginsConfig pc(plugins_manager);
        pc.parse_struct(plugins, error);
    }
}

//---------------------------------------------------------------------------
void Core::load_plugins_configuration()
{
    std::string error;
    PluginsConfig pc(plugins_manager);
    pc.load_file(plugins_configuration_file, error);
}

//---------------------------------------------------------------------------
void Core::load_database()
{
#ifdef HAVE_SQLITE
    std::string db_path;
    if (!config || config->get("SQLite_Path", db_path) < 0)
        db_path = get_database_path();

    db = new SQLLiteReport;

    ((Database*)db)->set_database_directory(db_path);
    db->set_database_filename(database_name);
    if (db->init_report() < 0)
    {
        delete db;
        db = NULL;
    }
#endif
    if (!db)
    {
        db = new NoDatabaseReport;
        db->init_report();
    }
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
    implementation_options["verbosity"] = verbosity;
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

//---------------------------------------------------------------------------
const std::map<std::string, Plugin*>& Core::get_format_plugins() const
{
    return plugins_manager->get_format_plugins();
}

//---------------------------------------------------------------------------
const std::vector<Plugin*>& Core::get_pre_hook_plugins() const
{
    return plugins_manager->get_pre_hook_plugins();
}

//***************************************************************************
// Plugins
//***************************************************************************

//---------------------------------------------------------------------------
int Core::mediaconch_get_plugins(std::vector<std::string>& plugins, std::string&)
{
    const std::vector<Plugin*>& list = plugins_manager->get_plugins();

    for (size_t i = 0; i < list.size(); ++i)
    {
        if (!list[i])
            continue;

        plugins.push_back(list[i]->get_id());
    }

    return 0;
}

//---------------------------------------------------------------------------
int Core::mediaconch_watch_folder(const std::string& folder, const std::string& folder_reports,
                                  const std::vector<std::string>& plugins, const std::vector<std::string>& policies,
                                  long *in_user, bool recursive,
                                  const std::vector<std::pair<std::string,std::string> >& options,
                                  long& user_id, std::string& error)
{
    if (!watch_folders_manager)
    {
        error = "Watch folder manager not exisitng";
        return -1;
    }

    return watch_folders_manager->add_watch_folder(folder, folder_reports, plugins, policies,
                                                   in_user, recursive, options, user_id, error);
}

//---------------------------------------------------------------------------
int Core::mediaconch_list_watch_folders(std::vector<std::string>& folders, std::string& error)
{
    if (!watch_folders_manager)
    {
        error = "Watch folder manager not exisitng";
        return -1;
    }

    std::map<std::string, std::string> tmp = watch_folders_manager->get_watch_folders();
    std::map<std::string, std::string>::iterator it = tmp.begin();
    for (; it != tmp.end(); ++it)
        folders.push_back(it->first);

    return 0;
}

//---------------------------------------------------------------------------
int Core::mediaconch_edit_watch_folder(const std::string& folder, const std::string& folder_reports,
                                       std::string& error)
{
    if (!watch_folders_manager)
    {
        error = "Watch folder manager not exisitng";
        return -1;
    }

    return watch_folders_manager->edit_watch_folder(folder, folder_reports, error);
}

//---------------------------------------------------------------------------
int Core::mediaconch_remove_watch_folder(const std::string& folder, std::string& error)
{
    if (!watch_folders_manager)
    {
        error = "Watch folder manager not exisitng";
        return -1;
    }

    return watch_folders_manager->remove_watch_folder(folder, error);
}

//***************************************************************************
// Checker
//***************************************************************************

//---------------------------------------------------------------------------
void Core::get_users_ids(std::vector<long>& ids, std::string& err)
{
    db_mutex.Enter();
    get_db()->get_users_id(ids, err);
    db_mutex.Leave();
}

//***************************************************************************
// Checker
//***************************************************************************

//---------------------------------------------------------------------------
long Core::checker_analyze(int user, const std::string& file, bool& registered,
                           const std::vector<std::pair<std::string,std::string> >& options,
                           const std::vector<std::string>& plugins, std::string& err,
                           bool force_analyze, bool mil_analyze)
{
    long id = -1;
    registered = false;
    bool analyzed = false;

    std::string options_str = serialize_string_from_options_vec(options);
    id = file_is_registered_and_analyzed(user, file, analyzed, options_str, err);
    if (force_analyze)
        analyzed = false;

    if (id < 0)
    {
        std::string file_last_modification = get_last_modification_file(file);
        std::vector<long> generated_id;
        db_mutex.Enter();
        id = get_db()->add_file(user, file, file_last_modification, options_str, err, generated_id);
        db_mutex.Leave();
        if (id < 0)
            return -1;
    }
    else
        registered = true;

    if (!analyzed && scheduler->add_element_to_queue(user, file, id, options, plugins, mil_analyze) < 0)
        return -1;

    return id;
}

//---------------------------------------------------------------------------
long Core::checker_analyze(int user, const std::string& filename, long src_id, size_t generated_time,
                           const std::string generated_log, const std::string generated_error_log,
                           const std::vector<std::pair<std::string,std::string> >& options,
                           const std::vector<std::string>& plugins, std::string& err, bool mil_analyze)
{
    long id = -1;
    bool analyzed = false;
    bool force_analyze = true;
    std::string options_str = serialize_string_from_options_vec(options);

    id = file_is_registered_and_analyzed(user, filename, analyzed, options_str, err);
    if (force_analyze)
        analyzed = false;

    std::vector<long> generated_id;
    std::string file_last_modification = get_last_modification_file(filename);
    if (id < 0)
    {
        db_mutex.Enter();
        id = get_db()->add_file(user, filename, file_last_modification, options_str, err,
                                generated_id, src_id, generated_time,
                                generated_log, generated_error_log);
        db_mutex.Leave();
        if (id < 0)
            return -1;
    }
    else
    {
        db_mutex.Enter();
        id = get_db()->update_file(user, id, file_last_modification, options_str, err,
                                   generated_id, src_id, generated_time,
                                   generated_log, generated_error_log);
        db_mutex.Leave();
        if (id < 0)
            return -1;
    }

    if (!analyzed && scheduler->add_element_to_queue(user, filename, id, options, plugins, mil_analyze) < 0)
        return -1;

    return id;
}

//---------------------------------------------------------------------------
int Core::file_add_generated_file(int user, long src_id, long generated_id, std::string& err)
{
    db_mutex.Enter();
    int ret = get_db()->add_file_generated_id(user, src_id, generated_id, err);
    db_mutex.Leave();

    return ret;
}

//---------------------------------------------------------------------------
int Core::update_file_error(int user, long id, bool has_error, const std::string& error_log, std::string& err)
{
    db_mutex.Enter();
    int ret = get_db()->update_file_error(user, id, err, has_error, error_log);
    db_mutex.Leave();

    plugin_add_log(PluginLog::LOG_LEVEL_ERROR, error_log);

    return ret;
}

//---------------------------------------------------------------------------
int Core::checker_status(int user, long file_id, MediaConchLib::Checker_StatusRes& res, std::string& err)
{
    double percent_done = 0.0;
    bool is_finished = scheduler->element_is_finished(user, file_id, percent_done);
    int ret = 0;

    res.id = file_id;
    res.finished = is_finished;
    if (is_finished)
    {
        res.tool = new int;
        *res.tool = (int)MediaConchLib::report_MediaConch;;

        std::string filename;
        std::string file_time;
        size_t generated_time;
        std::string generated_log;
        std::string generated_error_log;
        std::string options;

        db_mutex.Enter();
        ret = get_db()->get_file_information_from_id(user, file_id, filename, file_time,
                                               res.generated_id, res.source_id, generated_time,
                                               generated_log, generated_error_log, options, is_finished,
                                               res.has_error, res.error_log, err);
        if (ret == 0 && is_finished)
            ret = get_db()->get_element_report_kind(user, file_id, (MediaConchLib::report&)*res.tool, err);
        db_mutex.Leave();
    }
    else
    {
        res.percent = new double;
        *res.percent = percent_done;
    }

    return ret;
}

//---------------------------------------------------------------------------
int Core::checker_file_from_id(int user, long id, std::string& file, std::string& err)
{
    db_mutex.Enter();
    int ret = get_db()->get_file_name_from_id(user, id, file, err);
    db_mutex.Leave();

    return ret;
}

//---------------------------------------------------------------------------
long Core::checker_id_from_filename(int user, const std::string& filename,
                                    const std::vector<std::pair<std::string,std::string> >& options,
                                    std::string& err)
{
    std::string time = get_last_modification_file(filename);
    std::string options_str = serialize_string_from_options_vec(options);

    db_mutex.Enter();
    long id = get_db()->get_file_id(user, filename, time, options_str, err);
    db_mutex.Leave();

    return id;
}

//---------------------------------------------------------------------------
int Core::checker_file_information(int user, long id, MediaConchLib::Checker_FileInfo& info, std::string& err)
{
    std::string options;
    int ret = 0;
    db_mutex.Enter();
    ret = get_db()->get_file_information_from_id(user, id, info.filename, info.file_last_modification,
                                                 info.generated_id, info.source_id, info.generated_time,
                                                 info.generated_log, info.generated_error_log, options,
                                                 info.analyzed, info.has_error, info.error_log, err);
    db_mutex.Leave();

    if (ret == 0)
        info.options = parse_options_vec_from_string(options);
    return ret;
}

//---------------------------------------------------------------------------
int Core::checker_list(int user, std::vector<std::string>& vec, std::string& err)
{
    int ret = 0;
    ret = scheduler->get_elements(user, vec, err);
    if (ret < 0)
        return ret;

    db_mutex.Enter();
    ret = get_db()->get_elements(user, vec, err);
    db_mutex.Leave();
    return ret;
}

//---------------------------------------------------------------------------
int Core::checker_list(int user, std::vector<long>& vec, std::string& err)
{
    int ret = scheduler->get_elements(user, vec, err);
    if (ret < 0)
        return ret;

    db_mutex.Enter();
    ret = get_db()->get_elements(user, vec, err);
    db_mutex.Leave();

    return ret;
}

//---------------------------------------------------------------------------
int Core::checker_get_report(int user, const std::bitset<MediaConchLib::report_Max>& report_set, MediaConchLib::format f,
                             const std::vector<long>& files,
                             const std::vector<size_t>& policies_ids,
                             const std::vector<std::string>& policies_contents,
                             const std::map<std::string, std::string>& options,
                             MediaConchLib::Checker_ReportRes* result, std::string& err,
                             const std::string* display_name,
                             const std::string* display_content)
{
    if (!policies_ids.empty() || !policies_contents.empty())
    {
        if (check_policies(user, files, options, result, err,
                           policies_ids.size() ? &policies_ids : NULL,
                           policies_contents.size() ? &policies_contents : NULL) < 0)
            return -1;
        if (f == MediaConchLib::format_Text)
            transform_with_xslt_text_memory(result->report, result->report);
        else if (f == MediaConchLib::format_Html)
            transform_with_xslt_html_memory(result->report, result->report);
        else if (f == MediaConchLib::format_Simple)
            transform_with_xslt_simple_memory(result->report, result->report);
        else if (f == MediaConchLib::format_CSV)
            transform_with_xslt_csv_memory(result->report, result->report);
    }
    else
    {
        // For VeraPDF and DPFManager, to get the original XML
        if ((report_set[MediaConchLib::report_MediaVeraPdf] || report_set[MediaConchLib::report_MediaDpfManager]) &&
            f == MediaConchLib::format_Xml &&
            (!display_content || !display_content->size()) &&
            (!display_name || !display_name->size()))
            f = MediaConchLib::format_OrigXml;

        switch (f)
        {
            case MediaConchLib::format_Text:
            case MediaConchLib::format_Xml:
            case MediaConchLib::format_MaXml:
            case MediaConchLib::format_Html:
            case MediaConchLib::format_OrigXml:
            case MediaConchLib::format_Simple:
            case MediaConchLib::format_CSV:
                get_reports_output(user, files, options, f, report_set, result);
                break;
            case MediaConchLib::format_JsTree:
                get_reports_output_JStree(user, files, report_set, result->report);
                break;
            default:
                return -1;
        }
    }

    if (display_name)
        transform_with_xslt_file(result->report, *display_name, options, result->report);
    else if (display_content)
        transform_with_xslt_memory(result->report, *display_content, options, result->report);

    if (result->has_valid && !result->valid)
        plugin_add_log(PluginLog::LOG_LEVEL_DEBUG, "Implementation report is not valid");
    return 0;
}

//---------------------------------------------------------------------------
int Core::checker_validate(int user, MediaConchLib::report report, const std::vector<long>& files,
                           const std::vector<size_t>& policies_ids,
                           const std::vector<std::string>& policies_contents,
                           const std::map<std::string, std::string>& options,
                           std::vector<MediaConchLib::Checker_ValidateRes*>& result, std::string& err)
{
    if (report != MediaConchLib::report_MediaConch && !policies_ids.size() && !policies_contents.size() &&
        report != MediaConchLib::report_MediaVeraPdf && report != MediaConchLib::report_MediaDpfManager)
        return -1;

    std::vector<long> file_tmp;
    for (size_t i = 0; i < files.size(); ++i)
    {
        file_tmp.clear();
        file_tmp.push_back(files[i]);

        MediaConchLib::Checker_ValidateRes* res = new MediaConchLib::Checker_ValidateRes;
        res->id = files[i];
        if (!policies_ids.empty() || !policies_contents.empty())
        {
            MediaConchLib::Checker_ReportRes tmp_res;
            if (check_policies(user, file_tmp, options, &tmp_res, err,
                               policies_ids.size() ? &policies_ids : NULL,
                               policies_contents.size() ? &policies_contents : NULL) < 0)
                continue;
            res->valid = tmp_res.has_valid ? tmp_res.valid : true;
        }
        else if (report == MediaConchLib::report_MediaConch)
        {
            //XXX
            std::string report;
            if (get_implementation_report(user, files[i], options, report, res->valid, err) < 0)
                return -1;
        }
        else if (report == MediaConchLib::report_MediaVeraPdf)
        {
            std::string report;
            res->valid = get_verapdf_report(user, files[i], report);
        }
        else if (report == MediaConchLib::report_MediaDpfManager)
        {
            std::string report;
            res->valid = get_dpfmanager_report(user, files[i], report);
        }
        result.push_back(res);
        if (!res->valid)
            plugin_add_log(PluginLog::LOG_LEVEL_DEBUG, "Implementation report is not valid");
    }

    return 0;
}

//---------------------------------------------------------------------------
int Core::remove_report(int user, const std::vector<long>& files, std::string& err)
{
    if (!get_db())
    {
        err = "Internal error. Database cannot be loaded";
        return -1;
    }

    int ret = 0;
    db_mutex.Enter();
    for (size_t i = 0; i < files.size(); ++i)
    {
        if (db->remove_report(user, files[i], err) < 0)
            ret = -1;
    }
    db_mutex.Leave();

    return ret;
}

//---------------------------------------------------------------------------
int Core::get_reports_output_JStree(int user, const std::vector<long>& files,
                                    const std::bitset<MediaConchLib::report_Max>& report_set,
                                    std::string& report)
{
    std::vector<long> vec;
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
            create_report_mi_xml(user, vec, ret);
            report += js.format_from_inform_XML(ret);
        }

        if (report_set[MediaConchLib::report_MediaTrace])
        {
            std::string ret;
            create_report_mt_xml(user, vec, ret);
            report += js.format_from_trace_XML(ret);
        }
    }

    return 0;
}

//---------------------------------------------------------------------------
bool Core::check_policies_xslts(int user, const std::vector<long>& files,
                                const std::map<std::string, std::string>& options,
                                const std::vector<std::string>& policies,
                                std::stringstream& Out)
{
    bool valid = true;
    for (size_t i = 0; i < policies.size(); ++i)
    {
        std::string tmp;
        if (!validate_xslt_from_memory(user, files, options, policies[i], tmp))
        {
            valid = false;
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
int Core::check_policies(int user, const std::vector<long>& files,
                         const std::map<std::string, std::string>& opts,
                         MediaConchLib::Checker_ReportRes* result, std::string& err,
                         const std::vector<size_t>* policies_ids,
                         const std::vector<std::string>* policies_contents)
{
    if (!files.size())
    {
        err = "No file to validate";
        return -1;
    }

    std::map<std::string, std::string> options;
    std::map<std::string, std::string>::const_iterator it = opts.begin();
    for (; it != opts.end(); ++it)
        options[it->first] = it->second;

    unify_policy_options(user, options);

    std::vector<std::string> policies;
    if (this->policies.policy_get_policies(user, policies_ids, policies_contents, options, policies, err) < 0)
        return -1;

    std::stringstream Out;
    result->has_valid = true;
    result->valid = true;
    if (!check_policies_xslts(user, files, options, policies, Out))
        result->valid = false;

    result->report = Out.str();
    return 0;
}

//***************************************************************************
// XSL transform
//***************************************************************************

//---------------------------------------------------------------------------
int Core::transform_with_xslt_file(const std::string& report, const std::string& xslt,
                                   const std::map<std::string, std::string>& opts, std::string& result)
{
    Schema *S = new Xslt(!accepts_https());

    if (!S->register_schema_from_file(xslt.c_str()))
    {
        result = report;
        delete S;
        return -1;
    }

    S->set_options(opts);
    int valid = S->validate_xml(report);
    if (valid < 0)
    {
        result = report;
        delete S;
        return -1;
    }

    result = S->get_report();
    delete S;
    return 0;
}

//---------------------------------------------------------------------------
int Core::transform_with_xslt_memory(const std::string& report, const std::string& memory,
                                     const std::map<std::string, std::string>& opts,
                                     std::string& result)
{
    Schema *S = new Xslt(!accepts_https());

    if (!S->register_schema_from_memory(memory))
    {
        result = report;
        return -1;
    }

    S->set_options(opts);
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
    std::map<std::string, std::string> opts;
    std::string memory(implementation_report_display_html_xsl);
    return transform_with_xslt_memory(report, memory, opts, result);
}

//---------------------------------------------------------------------------
int Core::transform_with_xslt_text_memory(const std::string& report, std::string& result)
{
    const std::map<std::string, std::string> opts;
#if defined(_WIN32) || defined(WIN32)
    std::string memory(implementation_report_display_text_xsl);
#else //defined(_WIN32) || defined(WIN32)
    std::string memory(implementation_report_display_textunicode_xsl);
#endif //defined(_WIN32) || defined(WIN32)
    return transform_with_xslt_memory(report, memory, opts, result);
}

//---------------------------------------------------------------------------
int Core::transform_with_xslt_simple_memory(const std::string& report, std::string& result)
{
    std::map<std::string, std::string> opts;
    std::string memory(implementation_report_display_simple_xsl);
    return transform_with_xslt_memory(report, memory, opts, result);
}

//---------------------------------------------------------------------------
int Core::transform_with_xslt_csv_memory(const std::string& report, std::string& result)
{
    std::map<std::string, std::string> opts;
    std::string memory(implementation_report_display_csv_xsl);
    return transform_with_xslt_memory(report, memory, opts, result);
}

//***************************************************************************
// Policy validation
//***************************************************************************

//---------------------------------------------------------------------------
bool Core::validate_xslt_policy(int user, const std::vector<long>& files,
                                const std::map<std::string, std::string>&,
                                int pos, std::string& report)
{
    bool valid = true;
    xmlDocPtr doc = NULL;
    Schema *S = new Xslt(!accepts_https());

    if (pos >= 0)
        doc = policies.create_doc(user, pos);

    if (doc && S->register_schema_from_doc(doc))
        valid = validation(user, files, S, report);
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
bool Core::validate_xslt_policy_from_file(int user, const std::vector<long>& files,
                                          const std::map<std::string, std::string>&,
                                          const std::string& policy, std::string& report)
{
    bool valid = true;
    Schema *S = new Xslt(!accepts_https());

    if (S->register_schema_from_file(policy.c_str()))
        valid = validation(user, files, S, report);
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
bool Core::validate_xslt_from_memory(int user, const std::vector<long>& files,
                                     const std::map<std::string, std::string>& opts,
                                     const std::string& memory, std::string& report,
                                     bool is_implem)
{
    bool valid = true;
    Schema *S = new Xslt(!accepts_https());

    if (is_implem)
    {
        std::map<std::string, std::string> options = implementation_options;

        // XXX: replace or add new XSL option for implementation
        std::map<std::string, std::string>::const_iterator it = opts.begin();
        for (; it != opts.end(); ++it)
            options[it->first] = it->second;

        unify_implementation_options(options);
        S->set_options(options);
    }

    if (S->register_schema_from_memory(memory))
        valid = validation(user, files, S, report);
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
bool Core::validation(int user, const std::vector<long>& files, Schema* S, std::string& report)
{
    std::string xml;
    create_report_ma_xml(user, files, S->get_options(), xml, get_bitset_with_mi_mmt());
    bool valid = true;

    int ret = S->validate_xml(xml);
    if (ret < 0)
    {
        report = "Validation generated an internal error";
        std::vector<std::string> errors = S->get_errors();
        for (size_t i = 0; i < errors.size(); ++i)
            report += "\t" + errors[i];
        valid = false;
    }
    else
        report = S->get_report();
    return valid;
}

//---------------------------------------------------------------------------
void Core::unify_implementation_options(std::map<std::string, std::string>& opts)
{
    if (opts.find("verbosity") != opts.end() && opts["verbosity"].length() && opts["verbosity"] != "-1")
    {
        std::string& verbosity = opts["verbosity"];
        if (verbosity[0] == '"')
            verbosity = std::string("'") + verbosity + std::string("'");
        else
            verbosity = std::string("\"") + verbosity + std::string("\"");
    }
    else
        opts["verbosity"] = "\"5\"";
}

//---------------------------------------------------------------------------
void Core::unify_policy_options(int user, std::map<std::string, std::string>& opts)
{
    std::map<std::string, std::string>::iterator it;
    if ((it = opts.find("policy_reference_id")) != opts.end())
    {
        std::string file = it->second;
        opts.erase(it);

        if (!file.length())
            return;

        std::vector<long> files;
        std::string report;
        char *end = NULL;

        long file_id = strtol(file.c_str(), &end, 10);

        files.push_back(file_id);
        create_report_ma_xml(user, files, opts, report, get_bitset_with_mi_mmt());

        std::string path = get_local_data_path();
        path += "policies_references_files/";
        if (!ZenLib::Dir::Exists(ZenLib::Ztring().From_UTF8(path)))
            if (!ZenLib::Dir::Create(ZenLib::Ztring().From_UTF8(path)))
                path = "./";

        for (size_t i = 0; ; ++i)
        {
            std::stringstream ss;
            ss << path << "reference";
            if (i)
                ss << i;
            ss << ".xml";
            if (ZenLib::File::Exists(ZenLib::Ztring().From_UTF8(ss.str())))
                continue;

            path = ss.str();
            break;
        }
        ZenLib::File fd;

        fd.Create(ZenLib::Ztring().From_UTF8(path));

        fd.Open(ZenLib::Ztring().From_UTF8(path), ZenLib::File::Access_Write);
        fd.Write(ZenLib::Ztring().From_UTF8(report));
        fd.Close();

        opts["compare"] = "\"" + path + "\"";
    }
}

//---------------------------------------------------------------------------
void Core::unify_no_https(std::string& str)
{
    size_t pos;
    while ((pos = str.find("https://")) != std::string::npos)
        str.replace(pos + 4, 1, "");
}

//---------------------------------------------------------------------------
bool Core::accepts_https()
{
    return MI->Option(__T("Https_Get")) != __T("0"); //With test on "0", we handle old version of MediaInfoLib which do not have this option
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
void Core::compress_report_copy(std::string& report, const char* src, size_t src_len, MediaConchLib::compression& compress)
{
    switch (compress)
    {
        case MediaConchLib::compression_ZLib:
        {
            uLongf dst_len;

            dst_len = (uLongf)src_len;
            Bytef* dst = new Bytef[src_len + 1];

            if (compress2((Bytef*)dst, &dst_len, (const Bytef*)src, src_len, Z_BEST_COMPRESSION) != Z_OK || src_len <= dst_len)
            {
                //Fallback to no compression
                report = std::string((const char *)src, src_len);
                compress = MediaConchLib::compression_None;
            }
            else
                report = std::string((const char *)dst, dst_len);
            delete[] dst;
        }
        break;
        default:
            report = std::string((const char *)src, src_len);
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
                        dst_len <<= 1;
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
bool Core::file_is_existing(const std::string& filename)
{
    return ZenLib::File::Exists(Ztring().From_UTF8(filename));
}

//---------------------------------------------------------------------------
void Core::register_report_xml_to_database(int user, long file, const std::string& report,
                                           MediaConchLib::report report_kind, const std::string& options)
{
    MediaConchLib::compression mode = compression_mode;
    std::string new_report(report);
    compress_report(new_report, mode);

    std::string err;
    db_mutex.Enter();
    get_db()->save_report(user, file, report_kind, MediaConchLib::format_Xml,
                          options, new_report, mode, true, err);
    db_mutex.Leave();
}

//---------------------------------------------------------------------------
void Core::register_report_mediainfo_text_to_database(int user, long file, MediaInfoNameSpace::MediaInfo* curMI)
{
    curMI->Option(__T("Details"), __T("0"));
    curMI->Option(__T("Inform"), String());
    std::string report = Ztring(curMI->Inform()).To_UTF8();

    MediaConchLib::compression mode = compression_mode;
    compress_report(report, mode);

    std::string err;
    db_mutex.Enter();
    get_db()->save_report(user, file, MediaConchLib::report_MediaInfo, MediaConchLib::format_Text,
                          "", report, mode, true, err);
    db_mutex.Leave();
}

//---------------------------------------------------------------------------
void Core::register_report_mediainfo_xml_to_database(int user, long file, MediaInfoNameSpace::MediaInfo* curMI)
{
    curMI->Option(__T("Details"), __T("0"));
    curMI->Option(__T("Inform"), __T("MIXML"));
    std::string report = Ztring(curMI->Inform()).To_UTF8();
    MediaConchLib::compression mode = compression_mode;
    compress_report(report, mode);

    std::string err;
    db_mutex.Enter();
    get_db()->save_report(user, file, MediaConchLib::report_MediaInfo, MediaConchLib::format_Xml,
                          "", report, mode, true, err);
    db_mutex.Leave();
}

//---------------------------------------------------------------------------
void Core::register_report_micromediatrace_xml_to_database(int user, long file, MediaInfoNameSpace::MediaInfo* curMI)
{
    curMI->Option(__T("Details"), __T("1"));
    curMI->Option(__T("Inform"), __T("MICRO_XML"));

    //Trying with direct access to the string from MediaInfoLib, then use the classic method if it failed 
    std::string report;
    MediaConchLib::compression mode = compression_mode;
    Ztring Temp = curMI->Option(__T("File_Details_StringPointer"), Ztring());
    if (Temp.find_first_not_of(__T("0123456789:")) == string::npos) //Form is "Pointer:Size"
    {
        ZtringList TempZL;
        TempZL.Separator_Set(0, __T(":"));
        TempZL.Write(Temp);
        if (TempZL.size() == 2)
        {
            const char* report_buffer = (const char*)TempZL[0].To_int64u();
            size_t report_size = (size_t)TempZL[1].To_int64u();
            compress_report_copy(report, report_buffer, report_size, mode);
        }
    }
    if (report.empty())
    {
        std::string report = Ztring(curMI->Inform()).To_UTF8();
        compress_report(report, mode);
    }

    std::string err;
    db_mutex.Enter();
    get_db()->save_report(user, file, MediaConchLib::report_MicroMediaTrace, MediaConchLib::format_Xml,
                          "", report, mode, true, err);
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
void Core::set_file_analyzed_to_database(int user, long id)
{
    std::string err;
    db_mutex.Enter();
    get_db()->update_file_analyzed(user, id, err, true);
    db_mutex.Leave();
}

//---------------------------------------------------------------------------
void Core::register_reports_to_database(int user, long file, const std::string& report,
                                        MediaConchLib::report report_kind, const std::string& options,
                                        MediaInfoNameSpace::MediaInfo* curMI)
{
    // Implementation
    register_report_xml_to_database(user, file, report, report_kind, options);

    //MI and MT
    register_reports_to_database(user, file, curMI);
}

//---------------------------------------------------------------------------
void Core::register_reports_to_database(int user, long file, MediaInfoNameSpace::MediaInfo* curMI)
{
    // MediaInfo
    register_report_mediainfo_text_to_database(user, file, curMI);
    register_report_mediainfo_xml_to_database(user, file, curMI);

    // MicroMediaTrace
    register_report_micromediatrace_xml_to_database(user, file, curMI);
}

//---------------------------------------------------------------------------
void Core::register_reports_to_database(int user, long file)
{
    // MediaInfo
    register_report_mediainfo_text_to_database(user, file, MI);
    register_report_mediainfo_xml_to_database(user, file, MI);

    // MicroMediaTrace
    register_report_micromediatrace_xml_to_database(user, file, MI);
}

//---------------------------------------------------------------------------
void Core::create_report_mi_xml(int user, const std::vector<long>& files, std::string& report)
{
    bool AcceptsHttps = accepts_https();

    std::string version = ZenLib::Ztring(Menu_Option_Preferences_Option (__T("Info_Version"), __T(""))).To_UTF8();
    std::string search(" - v");
    size_t pos = version.find(search);
    if (pos != std::string::npos)
        version = version.substr(pos + search.length());

    std::stringstream start;
    start << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    start << "<MediaInfo\n";
    start << "xmlns=\"http" << (AcceptsHttps ? "s" : string()) << "://mediaarea.net/mediainfo\"\n";
    start << "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
    start << "xsi:schemaLocation=\"http" << (AcceptsHttps ? "s" : string()) << "://mediaarea.net/mediaarea http" << (AcceptsHttps ? "s" : string()) << "://mediaarea.net/mediainfo/mediainfo_2_0.xsd\"\n";
    start << "version=\"2.0beta1\">\n";
    start << "<!-- Work in progress, not for production -->\n";
    start << "<creatingLibrary version=\"";
    start << version;
    start << "\" url=\"http" << (AcceptsHttps ? "s" : string()) << "://mediaarea.net/MediaInfo\">MediaInfoLib</creatingLibrary>\n";
    report += start.str();
    std::vector<long> vec;
    for (size_t i = 0; i < files.size(); ++i)
    {
        vec.clear();
        vec.push_back(files[i]);

        std::string file, err;
        checker_file_from_id(user, files[i], file, err);
        xml_escape_attributes(file);
        report += "<media ref=\"" + file + "\">\n";

        std::string info;
        get_report_saved(user, vec, MediaConchLib::report_MediaInfo, MediaConchLib::format_Xml, "", info);
        get_content_of_media_in_xml(info);
        if (info.length())
            report += info;
        report += std::string("</media>\n");
    }
    report += std::string("</MediaInfo>");
}

//---------------------------------------------------------------------------
void Core::create_report_mt_xml(int user, const std::vector<long>& files, std::string& report)
{
    bool AcceptsHttps = accepts_https();
    std::string version = ZenLib::Ztring(Menu_Option_Preferences_Option (__T("Info_Version"), __T(""))).To_UTF8();
    std::string search(" - v");
    size_t pos = version.find(search);
    if (pos != std::string::npos)
        version = version.substr(pos + search.length());

    std::stringstream start;
    start << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    start << "<MediaTrace\n";
    start << "xmlns=\"http" << (AcceptsHttps ? "s" : string()) << "://mediaarea.net/mediatrace\"\n";
    start << "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
    start << "xsi:schemaLocation=\"http" << (AcceptsHttps ? "s" : string()) << "://mediaarea.net/mediaarea http" << (AcceptsHttps ? "s" : string()) << "://mediaarea.net/mediatrace/mediatrace_0_1.xsd\"\n";
    start << "version=\"0.1\">\n";
    start << "<creatingLibrary version=\"";
    start << version;
    start << "\" url=\"http" << (AcceptsHttps ? "s" : string()) << "://mediaarea.net/MediaInfo\">MediaInfoLib</creatingLibrary>\n";

    report += start.str();

    std::vector<long> vec;
    for (size_t i = 0; i < files.size(); ++i)
    {
        vec.clear();
        vec.push_back(files[i]);

        std::string file, err;
        checker_file_from_id(user, files[i], file, err);
        xml_escape_attributes(file);
        report += "<media ref=\"" + file + "\">\n";

        std::string tmp;
        create_report_mmt_xml(user, vec, tmp);
        if (tmp.length())
        {
            std::map<std::string, std::string> opts;
            std::string memory(micromediatrace_to_mediatrace_xsl);
            transform_with_xslt_memory(tmp, memory, opts, tmp);
            get_content_of_media_in_xml(tmp);
            report += tmp;
        }
        report += std::string("</media>\n");
    }
    report += std::string("</MediaTrace>");
}

//---------------------------------------------------------------------------
void Core::create_report_mmt_xml(int user, const std::vector<long>& files, std::string& report)
{
    bool AcceptsHttps = accepts_https();
    std::string version = ZenLib::Ztring(Menu_Option_Preferences_Option (__T("Info_Version"), __T(""))).To_UTF8();
    std::string search(" - v");
    size_t pos = version.find(search);
    if (pos != std::string::npos)
        version = version.substr(pos + search.length());

    std::stringstream start;
    start << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    start << "<MicroMediaTrace";
    start << " xmlns=\"http" << (AcceptsHttps ? "s" : string()) << "://mediaarea.net/micromediatrace\"";
    start << " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"";
    start << " mtsl=\"http" << (AcceptsHttps ? "s" : string()) << "://mediaarea.net/micromediatrace https://mediaarea.net/micromediatrace/micromediatrace.xsd\"";
    start << " version=\"0.1\">";
    start << "<creatingLibrary version=\"";
    start << version;
    start << "\" url=\"http" << (AcceptsHttps ? "s" : string()) << "://mediaarea.net/MediaInfo\">MediaInfoLib</creatingLibrary>";

    report += start.str();

    std::vector<long> vec;
    for (size_t i = 0; i < files.size(); ++i)
    {
        vec.clear();
        vec.push_back(files[i]);

        std::string file, err;
        checker_file_from_id(user, files[i], file, err);
        xml_escape_attributes(file);
        report += "<media ref=\"" + file + "\">";

        std::string trace;
        get_report_saved(user, vec, MediaConchLib::report_MicroMediaTrace, MediaConchLib::format_Xml, "", trace);
        if (trace.length())
            report += trace;
        report += std::string("</media>");
    }
    report += std::string("</MicroMediaTrace>");
}

//---------------------------------------------------------------------------
void Core::create_report_ma_xml(int user, const std::vector<long>& files,
                                const std::map<std::string, std::string>& options,
                                std::string& report,
                                std::bitset<MediaConchLib::report_Max> reports)
{
    bool AcceptsHttps = accepts_https();

    std::string version = ZenLib::Ztring(Menu_Option_Preferences_Option (__T("Info_Version"), __T(""))).To_UTF8();
    std::string search(" - v");
    size_t pos = version.find(search);
    if (pos != std::string::npos)
        version = version.substr(pos + search.length());

    std::stringstream start;
    start << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    start << "<MediaArea\n";
    start << "xmlns=\"http" << (AcceptsHttps ? "s" : string()) << "://mediaarea.net/mediaarea\"\n";
    start << "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
    start << "xsi:schemaLocation=\"http" << (AcceptsHttps ? "s" : string()) << "://mediaarea.net/mediaarea http" << (AcceptsHttps ? "s" : string()) << "://mediaarea.net/mediaarea/mediaarea_0_1.xsd\"\n";
    start << "version=\"0.1\">\n";
    start << "<!-- Work in progress, not for production -->\n";
    start << "<creatingLibrary version=\"";
    start << version;
    start << "\" url=\"http" << (AcceptsHttps ? "s" : string()) << "://mediaarea.net/MediaInfo\">MediaInfoLib</creatingLibrary>\n";
    report += start.str();

    std::vector<long> vec;
    for (size_t i = 0; i < files.size(); ++i)
    {
        vec.clear();
        vec.push_back(files[i]);

        std::string file, err;
        checker_file_from_id(user, files[i], file, err);
        xml_escape_attributes(file);
        report += "<media ref=\"" + file + "\">\n";

        if (reports[MediaConchLib::report_MediaInfo])
        {
            std::string info;
            get_report_saved(user, vec, MediaConchLib::report_MediaInfo, MediaConchLib::format_Xml, "", info);
            get_content_of_media_in_xml(info);
            if (info.length())
                report += "<MediaInfo xmlns=\"http" + (AcceptsHttps ? "s" : string()) + "://mediaarea.net/mediainfo\" version=\"2.0beta1\">" + info + "</MediaInfo>\n";
        }

        if (reports[MediaConchLib::report_MediaTrace])
        {
            std::string trace;
            create_report_mt_xml(user, vec, trace);
            get_content_of_media_in_xml(trace);
            report += "<MediaTrace xmlns=\"http" + (AcceptsHttps ? "s" : string()) + "://mediaarea.net/mediatrace\" version=\"0.1\">\n";
            if (trace.length())
                report += trace;
            report += "</MediaTrace>\n";
        }

        if (reports[MediaConchLib::report_MicroMediaTrace])
        {
            std::string trace;
            get_report_saved(user, vec, MediaConchLib::report_MicroMediaTrace,
                             MediaConchLib::format_Xml, "", trace);
            report += "<MicroMediaTrace xmlns=\"http" + (AcceptsHttps ? "s" : string()) + "://mediaarea.net/micromediatrace\" version=\"0.1\">\n";
            if (trace.length())
                report += trace;

            report += "</MicroMediaTrace>\n";
        }

        if (reports[MediaConchLib::report_MediaConch])
        {
            std::string implem;
            bool valid;
            if (get_implementation_report(user, files[i], options, implem, valid, err) < 0)
                implem = std::string();
            else
                get_content_of_media_in_xml(implem);

            report += "<MediaConch xmlns=\"http" + (AcceptsHttps ? "s" : string()) +
                "://mediaarea.net/mediaconch\" version=\"0.2\">\n" + implem + "</MediaConch>\n";
        }
        report += std::string("</media>\n");
    }
    report += std::string("</MediaArea>");
}

//---------------------------------------------------------------------------
void Core::create_report_verapdf_xml(int user, const std::vector<long>& files, std::string& report)
{
    if (files.size() == 1)
    {
        get_report_saved(user, files, MediaConchLib::report_MediaVeraPdf, MediaConchLib::format_Xml, "", report);
        return;
    }
    report = "Not implemented";
}

//---------------------------------------------------------------------------
void Core::create_report_dpfmanager_xml(int user, const std::vector<long>& files, std::string& report)
{
    if (files.size() == 1)
    {
        get_report_saved(user, files, MediaConchLib::report_MediaDpfManager, MediaConchLib::format_Xml, "", report);
        return;
    }
    report = "Not implemented";
}

//---------------------------------------------------------------------------
void Core::get_report_saved(int user, const std::vector<long>& files,
                            MediaConchLib::report reportKind, MediaConchLib::format f,
                            const std::string& options, std::string& report)
{
    if (f == MediaConchLib::format_MaXml)
    {
        std::map<std::string, std::string> map_options;
        create_report_ma_xml(user, files, map_options, report, get_bitset_with_mi_mt());
        return;
    }

    if (reportKind != MediaConchLib::report_MediaConch &&
        reportKind != MediaConchLib::report_MediaInfo && reportKind != MediaConchLib::report_MicroMediaTrace
        && reportKind != MediaConchLib::report_MediaVeraPdf && reportKind != MediaConchLib::report_MediaDpfManager)
        return;

    if (!get_db())
        return;

    for (size_t i = 0; i < files.size(); ++i)
    {
        std::string raw;
        MediaConchLib::compression compress = MediaConchLib::compression_None;
        std::string err;
        db_mutex.Enter();
        get_db()->get_report(user, files[i], reportKind, f, options, raw, compress, err);
        db_mutex.Leave();
        uncompress_report(raw, compress);
        report += raw;
    }
}

//---------------------------------------------------------------------------
void Core::get_reports_output(int user, const std::vector<long>& files,
                              const std::map<std::string, std::string>& options,
                              MediaConchLib::format f,
                              std::bitset<MediaConchLib::report_Max> report_set,
                              MediaConchLib::Checker_ReportRes* result)
{
    if (f == MediaConchLib::format_MaXml)
    {
        create_report_ma_xml(user, files, options, result->report, report_set);
        result->report += "\r\n";
    }
    else
    {
        if (report_set[MediaConchLib::report_MediaInfo])
        {
            if (f == MediaConchLib::format_Xml)
                create_report_mi_xml(user, files, result->report);
            else
                get_report_saved(user, files, MediaConchLib::report_MediaInfo, f, "", result->report);
            result->report += "\r\n";
        }

        if (report_set[MediaConchLib::report_MediaTrace])
        {
            std::string tmp;
            create_report_mt_xml(user, files, tmp);
            if (f == MediaConchLib::format_Xml)
                result->report += tmp;
            else if (f == MediaConchLib::format_Html)
            {
                // Apply an XSLT to have HTML
                std::string memory(media_trace_display_html_xsl);
                std::map<std::string, std::string> opts;
                transform_with_xslt_memory(tmp, memory, opts, tmp);
                result->report += tmp;
            }
            else if (f == MediaConchLib::format_Text)
            {
                // Apply an XSLT to have Text
                std::string memory(media_trace_display_text_xsl);
                std::map<std::string, std::string> opts;
                transform_with_xslt_memory(tmp, memory, opts, tmp);
                result->report += tmp;
            }
            result->report += "\r\n";
        }

        if (report_set[MediaConchLib::report_MicroMediaTrace])
        {
            std::string tmp;
            create_report_mmt_xml(user, files, tmp);
            if (f == MediaConchLib::format_Xml)
                result->report += tmp;
        }

        if (report_set[MediaConchLib::report_MediaConch])
        {
            std::string tmp;
            std::string err;
            bool is_valid = false;
            if (get_implementation_reports(user, files, options, tmp, is_valid, err) < 0)
                tmp = std::string();

            if (f == MediaConchLib::format_Html)
                transform_with_xslt_html_memory(tmp, tmp);
            else if (f == MediaConchLib::format_Xml)
            {
                // No transformation for XML
            }
            else if (f == MediaConchLib::format_Simple)
                transform_with_xslt_simple_memory(tmp, tmp);
            else if (f == MediaConchLib::format_CSV)
                transform_with_xslt_csv_memory(tmp, tmp);
            else
                transform_with_xslt_text_memory(tmp, tmp);

            result->report += tmp;

            if (!result->has_valid)
                result->has_valid = true;
            if (!is_valid)
                result->valid = false;
            result->report += "\r\n";
        }

        if (report_set[MediaConchLib::report_MediaVeraPdf])
        {
            std::string tmp, transformed;
            create_report_verapdf_xml(user, files, tmp);

            std::string memory(implementation_report_vera_pdf_xsl);
            const std::map<std::string, std::string> opts;
            transform_with_xslt_memory(tmp, memory, opts, transformed);

            if (!policy_is_valid(transformed))
                result->valid = false;
            else
                result->valid = true;

            if (f == MediaConchLib::format_OrigXml)
            {
                // No transformation, keep the original XML
                result->report += tmp;
            }
            else if (f == MediaConchLib::format_Xml)
            {
                // Get the transformed XML for applying display
                result->report += transformed;
            }
            else
            {
                if (f == MediaConchLib::format_Html)
                    transform_with_xslt_html_memory(transformed, transformed);
                else if (f == MediaConchLib::format_Simple)
                    transform_with_xslt_simple_memory(transformed, transformed);
                else if (f == MediaConchLib::format_CSV)
                    transform_with_xslt_csv_memory(transformed, transformed);
                else
                    transform_with_xslt_text_memory(transformed, transformed);

                result->report += transformed;
            }
            result->report += "\r\n";
        }

        if (report_set[MediaConchLib::report_MediaDpfManager])
        {
            std::string tmp, transformed;
            create_report_dpfmanager_xml(user, files, tmp);

            std::string memory(implementation_report_dpf_manager_xsl);
            const std::map<std::string, std::string> opts;
            transform_with_xslt_memory(tmp, memory, opts, transformed);

            if (!policy_is_valid(transformed))
                result->valid = false;
            else
                result->valid = true;

            if (f == MediaConchLib::format_OrigXml)
            {
                // No transformation, keep the original XML
                result->report += tmp;
            }
            else if (f == MediaConchLib::format_Xml)
            {
                // Get the transformed XML for applying display
                result->report += transformed;
            }
            else
            {
                if (f == MediaConchLib::format_Html)
                    transform_with_xslt_html_memory(transformed, transformed);
                if (f == MediaConchLib::format_Simple)
                    transform_with_xslt_simple_memory(transformed, transformed);
                else if (f == MediaConchLib::format_CSV)
                    transform_with_xslt_csv_memory(transformed, transformed);
                else
                    transform_with_xslt_text_memory(transformed, transformed);

                result->report += transformed;
            }
            result->report += "\r\n";
        }
    }
}

//---------------------------------------------------------------------------
int Core::get_implementation_reports(int user, const std::vector<long>& files,
                                     const std::map<std::string, std::string>& options,
                                     std::string& report, bool& valid, std::string& err)
{
    bool AcceptsHttps = accepts_https();
    valid = true;

    std::stringstream verbo;
    std::map<std::string, std::string>::const_iterator it = options.find("verbosity");
    if (it != options.end() && it->second.size())
        verbo << it->second;
    else
        verbo << "5";

    report = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    report += "<MediaConch xmlns=\"http" + (AcceptsHttps ? "s" : string()) +
        "://mediaarea.net/mediaconch\" version=\"0.2\" verbosity=\"" + verbo.str() + "\">\n";
    for (size_t i = 0; i < files.size(); ++i)
    {
        std::string file;
        checker_file_from_id(user, files[i], file, err);
        xml_escape_attributes(file);

        report += "  <media ref=\"" + file + "\">";
        std::string implem;
        bool v = false;
        if (get_implementation_report(user, files[i], options, implem, v, err) < 0)
            implem = std::string();
        else
            get_content_of_media_in_xml(implem);

        report += implem + "</media>\n";

        if (!v)
            valid = false;
    }

    report += "</MediaConch>\n";

    return 0;
}

//---------------------------------------------------------------------------
int Core::get_implementation_report(int user, long file, const std::map<std::string, std::string>& options,
                                    std::string& report, bool& valid, std::string& err)
{
    std::string options_str = serialize_string_from_options_map(options);
    valid = true;
    std::string tmp_report;

    db_mutex.Enter();
    bool registered = get_db()->report_is_registered(user, file, MediaConchLib::report_MediaConch, MediaConchLib::format_Xml, options_str, err);
    db_mutex.Leave();
    if (!registered)
    {
        std::string memory(implementation_report_xsl);
        std::vector<long> tocheck;
        tocheck.push_back(file);
        valid = validate_xslt_from_memory(user, tocheck, options, memory, tmp_report, true);

        MediaConchLib::compression mode = compression_mode;
        compress_report(tmp_report, mode);

        db_mutex.Enter();
        get_db()->save_report(user, file, MediaConchLib::report_MediaConch, MediaConchLib::format_Xml,
                              options_str, tmp_report, mode, 0, err);
        db_mutex.Leave();
        tmp_report = std::string();
    }

    MediaConchLib::compression compress = MediaConchLib::compression_None;
    db_mutex.Enter();
    get_db()->get_report(user, file, MediaConchLib::report_MediaConch, MediaConchLib::format_Xml,
                         options_str, tmp_report, compress, err);
    db_mutex.Leave();

    uncompress_report(tmp_report, compress);
    if (valid)
        valid = implementation_is_valid(tmp_report);
    report += tmp_report;

    return 0;
}

//---------------------------------------------------------------------------
bool Core::get_verapdf_report(int user, long file, std::string& report)
{
    std::vector<long> files;
    files.push_back(file);

    get_report_saved(user, files, MediaConchLib::report_MediaVeraPdf, MediaConchLib::format_Xml,
                     "", report);

    return verapdf_report_is_valid(report);
}

//---------------------------------------------------------------------------
bool Core::get_dpfmanager_report(int user, long file, std::string& report)
{
    std::vector<long> files;
    files.push_back(file);

    get_report_saved(user, files, MediaConchLib::report_MediaDpfManager, MediaConchLib::format_Xml,
                     "", report);

    return dpfmanager_report_is_valid(report);
}

//---------------------------------------------------------------------------
long Core::file_is_registered_and_analyzed_in_db(int user, const std::string& filename, bool& analyzed,
                                                 const std::string& options, std::string& err)
{
    err.clear();
    bool is_existing = file_is_existing(filename);

    std::string time;
    if (is_existing)
        time = get_last_modification_file(filename);

    db_mutex.Enter();

    long id = get_db()->get_file_id(user, filename, time, options, err);
    if (id < 0)
    {
        analyzed = false;
        db_mutex.Leave();
        return id;
    }

    analyzed = get_db()->file_is_analyzed(user, id, err);
    db_mutex.Leave();

    if (!is_existing && !analyzed)
        return -1;

    return id;
}

//---------------------------------------------------------------------------
long Core::file_is_registered_in_queue(int user, const std::string& filename,
                                       const std::string& options, std::string& err)
{
    if (!scheduler)
    {
        err = "Internal Error: Scheduler is not loaded.";
        return -1;
    }

    return scheduler->element_exists(user, filename, options, err);
}

//---------------------------------------------------------------------------
long Core::file_is_registered_and_analyzed(int user, const std::string& filename, bool& analyzed,
                                           const std::string& options, std::string& err)
{
    long id;
    analyzed = false;
    if ((id = file_is_registered_in_queue(user, filename, options, err)) >= 0)
        return id;

    return file_is_registered_and_analyzed_in_db(user, filename, analyzed, options, err);
}

//---------------------------------------------------------------------------
bool Core::sort_pair_options(const std::pair<std::string,std::string>& a, const std::pair<std::string,std::string>& b)
{
    return a.first.compare(b.first) <= 0;
}

//---------------------------------------------------------------------------
std::string Core::serialize_string_from_options_vec(const std::vector<std::pair<std::string,std::string> >& options)
{
    std::stringstream toreturn;

    std::vector<std::pair<std::string,std::string> > opts;
    for (size_t i = 0; i < options.size(); ++i)
    {
        std::string key_option = options[i].first;
        std::string value_option = options[i].second;

        if (!key_option.size())
            continue;

        transform(key_option.begin(), key_option.end(), key_option.begin(), (int(*)(int))tolower);

        if (key_option == "event_callbackfunction")
            continue;

        // transform(value_option.begin(), value_option.end(), value_option.begin(), (int(*)(int))tolower);

        opts.push_back(std::make_pair(key_option, value_option));
    }

    std::sort(opts.begin(), opts.end(), Core::sort_pair_options);

    for (size_t i = 0; i < opts.size(); ++i)
        toreturn << opts[i].first.size() << "," << opts[i].first << opts[i].second.size() << "," << opts[i].second;

    return toreturn.str();
}

//---------------------------------------------------------------------------
std::vector<std::pair<std::string,std::string> > Core::parse_options_vec_from_string(const std::string& options)
{
    std::vector<std::pair<std::string,std::string> > toreturn;

    size_t pos = 0;
    size_t start;

    std::vector<std::string> opt;
    while (pos != std::string::npos)
    {
        start = pos;
        pos = options.find(",", start);
        if (pos == std::string::npos)
            continue;

        std::string size = options.substr(start, pos - start);
        pos += 1;
        char *end = NULL;
        long len = strtol(size.c_str(), &end, 10);
        if (len < 0 || pos + len > options.size())
        {
            pos = std::string::npos;
            continue;
        }

        opt.push_back(options.substr(pos, len));
        pos += len;
    }

    for (size_t i = 0; i + 1 < opt.size();)
    {
        toreturn.push_back(std::make_pair(opt[i], opt[i + 1]));
        i += 2;
    }

    return toreturn;
}

//---------------------------------------------------------------------------
std::string Core::serialize_string_from_options_map(const std::map<std::string,std::string>& options)
{
    std::stringstream toreturn;

    std::map<std::string,std::string>::const_iterator it = options.begin();
    for (; it != options.end(); ++it)
        toreturn << it->first.size() << "," << it->first << it->second.size() << "," << it->second;

    return toreturn.str();
}

//---------------------------------------------------------------------------
std::map<std::string, std::string> Core::parse_options_map_from_string(const std::string& options)
{
    std::map<std::string, std::string> toreturn;

    size_t pos = 0;
    size_t start;

    std::vector<std::string> opt;
    while (pos != std::string::npos)
    {
        start = pos;
        pos = options.find(",", start);
        if (pos == std::string::npos)
            continue;

        std::string size = options.substr(start, pos - start);
        pos += 1;
        char *end = NULL;
        long len = strtol(size.c_str(), &end, 10);
        if (len < 0 || pos + len > options.size())
        {
            pos = std::string::npos;
            continue;
        }

        opt.push_back(options.substr(pos, len));
        pos += len;
    }

    for (size_t i = 0; i + 1 < opt.size(); i += 2)
        toreturn[opt[i]] = opt[i + 1];

    return toreturn;
}

//---------------------------------------------------------------------------
std::string Core::get_local_config_path()
{
    std::string local_path(".");
#if defined(WINDOWS)
    PWSTR path = NULL;

    if (SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_CREATE, NULL, &path) == S_OK)
    {
        local_path = Ztring(path).To_UTF8();
        local_path += "/MediaConch/";
        CoTaskMemFree(path);

        for (;;)
        {
            size_t pos = 0;
            pos = local_path.find('\\');
            if (pos == std::string::npos)
                break;

            local_path[pos] = '/';
        }
    }
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
    local_path = std::string(home) + std::string("/.config/");
#elif defined(MACOS) || defined(MACOSX)
    const char* home = NULL;

    if ((home = getenv("HOME")) == NULL)
    {
        struct passwd *pw = getpwuid(getuid());
        if (pw)
            home = pw->pw_dir;
        else
            home = ".";
    }
    local_path = std::string(home) + std::string("/Library/Preferences/");
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
    PWSTR path = NULL;

    if (SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_CREATE, NULL, &path) == S_OK)
    {
        local_path = Ztring(path).To_UTF8();
        local_path += "/MediaConch/";
	CoTaskMemFree(path);

	for (;;)
	{
	    size_t pos = 0;
	    pos = local_path.find('\\');
	    if (pos == std::string::npos)
	      break;

	    local_path[pos] = '/';
	}
    }
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
    local_path = std::string(home) + std::string("/.local/share/MediaConch/");
#elif defined(MACOS) || defined(MACOSX)
    const char* home;

    if ((home = getenv("HOME")) == NULL)
    {
        struct passwd *pw = getpwuid(getuid());
        if (pw)
            home = pw->pw_dir;
        else
            home = ".";
    }
    local_path = std::string(home) + std::string("/Library/Application Support/MediaConch/");
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
void Core::set_plugins_configuration_file(const std::string& file)
{
    plugins_configuration_file = file;
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
DatabaseReport *Core::get_db()
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
void Core::xml_escape_attributes(std::string& xml)
{
    size_t len = xml.length();
    for (size_t i = 0; i < len; ++i)
    {
        switch(xml[i])
        {
            case '&':
                xml.replace(i, 1, "&amp;");
                len += 4;
                i += 4;
                break;
            case '<':
                xml.replace(i, 1, "&lt;");
                len += 3;
                i += 3;
                break;
            case '>':
                xml.replace(i, 1, "&gt;");
                len += 3;
                i += 3;
                break;
            case '\'':
                xml.replace(i, 1, "&apos;");
                len += 5;
                i += 5;
                break;
            case '"':
                xml.replace(i, 1, "&quot;");
                len += 5;
                i += 5;
                break;
            default:
                break;
        }
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
std::bitset<MediaConchLib::report_Max> Core::get_bitset_with_mi_mmt()
{
    std::bitset<MediaConchLib::report_Max> bits;

    bits.set(MediaConchLib::report_MediaInfo);
    bits.set(MediaConchLib::report_MicroMediaTrace);
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
bool Core::has_outcome_fail(const std::string& report)
{
    size_t pos = report.find(" outcome=\"fail\"");
    if (pos != std::string::npos)
        return false;
    return true;
}

//---------------------------------------------------------------------------
bool Core::implementation_is_valid(const std::string& report)
{
    size_t pos = 0;
    std::string search("<implementationChecks ");
    while (1)
    {
        pos = report.find(search.c_str(), pos);
        if (pos == std::string::npos)
            return true;

        size_t end = report.find(">", pos);
        if (end == std::string::npos)
            return true;

        std::string search("fail_count=\"");
        size_t f = report.find(search, pos);

        if (f > end || f + search.size() + 2 > report.size())
            return true;

        if (report[f + search.size()] != '0')
            return false;

        pos += search.size();
    }

    return true;
}

//---------------------------------------------------------------------------
bool Core::policy_is_valid(const std::string& report)
{
    size_t pos = report.find("<policy ");
    if (pos == std::string::npos)
        return true;

    size_t end = report.find(">", pos);
    if (end == std::string::npos)
        return true;

    std::string search("outcome=\"");
    size_t f = report.find(search, pos);

    if (f > end || f + search.size() + 4 > report.size())
        return true;

    if (report[f + search.size()] == 'f' && report[f + search.size() + 1] == 'a' && report[f + search.size() + 2] == 'i' && report[f + search.size() + 3] == 'l')
        return false;

    return true;
}

//---------------------------------------------------------------------------
bool Core::verapdf_report_is_valid(const std::string& report)
{
    size_t pos = report.find("<summary");
    if (pos == std::string::npos)
        return true;

    std::string invalid("inValid=\"");
    pos = report.find(invalid, pos);
    if (pos == std::string::npos)
        return true;

    if (report.size() > pos + invalid.size() && report[pos + invalid.size()] != '0')
        return false;

    return true;
}

//---------------------------------------------------------------------------
bool Core::dpfmanager_report_is_valid(const std::string& report)
{
    size_t pos = report.find("<level>error</level>");
    if (pos != std::string::npos)
        return false;

    return true;
}

//---------------------------------------------------------------------------
int Core::policy_get_fields_for_type(const std::string& type, std::vector<std::string>& fields, std::string& err)
{
    const std::map<std::string, std::list<std::string> >& types = Policies::existing_type;

    if (types.find(type) == types.end())
    {
        err = "Type is not existing";
        return -1;
    }

    const std::list<std::string>& list = types.at(type);
    std::list<std::string>::const_iterator it = list.begin();
    for (; it != list.end(); ++it)
        fields.push_back(*it);
    return 0;
}

//---------------------------------------------------------------------------
int Core::policy_get_values_for_type_field(const std::string& type, const std::string& field, std::vector<std::string>& values,
                                           std::string& err)
{
    std::map<std::string, std::map<std::string, std::vector<std::string> > > vs;
    if (get_generated_values_from_csv(vs) < 0)
    {
        err = "Internal error, cannot generate values";
        return -1;
    }

    if (vs.find(type) != vs.end())
    {
        if (vs[type].find(field) != vs[type].end())
        {
            for (size_t i = 0; i < vs[type][field].size(); ++i)
                values.push_back(vs[type][field][i]);
        }
    }

    return 0;
}

//--------------------------------------------------------------------------
std::string Core::get_date()
{
    time_t t;
    time(&t);

    std::string str;

    const char* t_str = ctime(&t);
    if (t_str)
        str = std::string(t_str);

    if (str.length())
        str[str.length() - 1] = '\0';

    return str;
}

//--------------------------------------------------------------------------
void Core::plugin_add_log(int level, const std::string& log)
{
    if (plugins_manager)
        plugins_manager->write_log(level, log);
}

//--------------------------------------------------------------------------
void Core::plugin_add_log_timestamp(int level, const std::string& log)
{
    if (plugins_manager)
        plugins_manager->write_log_timestamp(level, log);
}

}
