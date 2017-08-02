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
#include "Common/WatchFoldersManager.h"
#include "Common/PluginsManager.h"
#include "Common/PluginsConfig.h"
#include "Common/Plugin.h"
#if defined(_WIN32) || defined(WIN32)
#include <Shlobj.h>
#endif //defined(_WIN32) || defined(WIN32)
#include "Common/generated/ImplementationReportMatroskaSchema.h"
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

//***************************************************************************
// Global/static init
//***************************************************************************
//---------------------------------------------------------------------------
const MediaInfoNameSpace::Char* MEDIAINFO_TITLE=__T("MediaArea.net/MediaConch");
const std::string MediaConch::Core::database_name = "MediaConch.db";

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Core::Core() : policies(this), reports(this)
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
const std::map<std::string, std::string>& Core::get_implementation_options() const
{
    return implementation_options;
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
    {
        if (force_analyze)
        {
            std::string file_last_modification = get_last_modification_file(file);
            std::vector<long> generated_id;
            db_mutex.Enter();
            get_db()->remove_report(user, id, err);
            id = get_db()->update_file(user, id, file_last_modification, options_str, err, generated_id);
            db_mutex.Leave();
            if (id < 0)
                return -1;
        }
        registered = true;
    }

    if (!analyzed && scheduler->add_element_to_queue(user, file, id, options, plugins, mil_analyze) < 0)
        return -1;

    return id;
}

//---------------------------------------------------------------------------
long Core::checker_analyze(int user, const std::string& filename, long src_id, size_t generated_time,
                           const std::string generated_log, const std::string generated_error_log,
                           const std::vector<std::pair<std::string,std::string> >& options,
                           const std::vector<std::string>& plugins, std::string& err, bool mil_analyze,
                           const std::string& alias)
{
    long id = -1;
    bool analyzed = false;
    bool force_analyze = true;
    std::string options_str = serialize_string_from_options_vec(options);

    std::string tocheck = alias.size() ? alias : filename;
    id = file_is_registered_and_analyzed(user, tocheck, analyzed, options_str, err);
    if (force_analyze)
        analyzed = false;

    std::vector<long> generated_id;
    std::string file_last_modification = get_last_modification_file(filename);
    if (id < 0)
    {
        db_mutex.Enter();
        id = get_db()->add_file(user, tocheck, file_last_modification, options_str, err,
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

    if (!analyzed && scheduler->add_element_to_queue(user, filename, id, options, plugins, mil_analyze, alias) < 0)
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
        *res.tool = (int)MediaConchLib::report_MediaConch;

        std::string filename;
        std::string file_time;
        size_t generated_time;
        std::string generated_log;
        std::string generated_error_log;
        std::string options;

        db_mutex.Enter();
        ret = get_db()->get_file_information_from_id(user, file_id, filename, file_time,
                                               res.generated_id, res.source_id, generated_time,
                                               generated_log, generated_error_log, options, res.finished,
                                               res.has_error, res.error_log, err);
        if (ret == 0 && res.finished)
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
int Core::checker_clear(int user, const std::vector<long>& files, std::string& err)
{
    if (!get_db())
    {
        err = "Internal error. Database cannot be loaded";
        return -1;
    }

    int ret = 0;
    if (files.size())
    {
        db_mutex.Enter();
        for (size_t i = 0; i < files.size(); ++i)
        {
            if (get_db()->remove_report(user, files[i], err) < 0)
                ret = -1;
            if (get_db()->remove_file(user, files[i], err) < 0)
                ret = -1;
        }
        db_mutex.Leave();
    }
    else
    {
        db_mutex.Enter();
        if (get_db()->remove_all_reports(user, err) < 0)
            ret = -1;
        if (get_db()->remove_all_files(user, err) < 0)
            ret = -1;
        db_mutex.Leave();
    }

    return ret;
}

//---------------------------------------------------------------------------
int Core::checker_stop(int user, const std::vector<long>& files, std::string& err)
{
    return scheduler->stop_elements(user, files, err);
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
    ZenLib::Ztring Temp = curMI->Option(__T("File_Details_StringPointer"), ZenLib::Ztring());
    if (Temp.find_first_not_of(__T("0123456789:")) == std::string::npos) //Form is "Pointer:Size"
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
int Core::register_mediaconch_to_database(int user, long file, const std::string& options,
                                          std::string& report, std::string& err)
{
    MediaConchLib::compression mode = compression_mode;
    compress_report(report, mode);

    db_mutex.Enter();
    int ret = get_db()->save_report(user, file, MediaConchLib::report_MediaConch, MediaConchLib::format_Xml,
                                    options, report, mode, 0, err);
    db_mutex.Leave();
    return ret;
}

//---------------------------------------------------------------------------
int Core::get_report_saved(int user, const std::vector<long>& files,
                           MediaConchLib::report reportKind, MediaConchLib::format f,
                           const std::string& options, std::string& report, std::string& err)
{
    if (f == MediaConchLib::format_MaXml)
    {
        std::map<std::string, std::string> map_options;
        if (reports.create_report_ma_xml(user, files, map_options, report, reports.get_bitset_with_mi_mt(), err) < 0)
            return -1;
        return 0;
    }

    if (reportKind != MediaConchLib::report_MediaConch &&
        reportKind != MediaConchLib::report_MediaInfo && reportKind != MediaConchLib::report_MicroMediaTrace
        && reportKind != MediaConchLib::report_MediaVeraPdf && reportKind != MediaConchLib::report_MediaDpfManager)
    {
        err = "Report kind asked is not recognized.";
        return -1;
    }

    if (!get_db())
    {
        err = "The database is not correctly set.";
        return -1;
    }

    for (size_t i = 0; i < files.size(); ++i)
    {
        std::string raw;
        MediaConchLib::compression compress = MediaConchLib::compression_None;

        db_mutex.Enter();
        if (get_db()->get_report(user, files[i], reportKind, f, options, raw, compress, err) < 0)
        {
            db_mutex.Leave();
            return -1;
        }

        db_mutex.Leave();
        uncompress_report(raw, compress);
        report += raw;
    }

    return 0;
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
int Core::implem_report_is_registered(int user, long file, const std::string& options, bool& registered, std::string& err)
{
    db_mutex.Enter();
    int ret = get_db()->report_is_registered(user, file, MediaConchLib::report_MediaConch,
                                             MediaConchLib::format_Xml, options, registered, err);
    db_mutex.Leave();
    return ret;
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

#if defined(WINDOWS)

//---------------------------------------------------------------------------
char Core::get_path_separator(const std::string& path)
{
    if (path.size() < 3)
        return '\\';

    if (path[2] == '\\' || path[2] == '/')
        return path[2];

    if (path.size() > 3 && (path[3] == '\\' || path[3] == '/'))
        return path[3];

    return '\\';
}

#else

//---------------------------------------------------------------------------
char Core::get_path_separator(const std::string&)
{
    return '/';
}

#endif

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
        char separator = get_path_separator(local_path);
        if (local_path.size() > 0 && local_path[local_path.size() - 1] != separator)
            local_path.append(1, separator);
        local_path += "MediaConch";
        CoTaskMemFree(path);
    }
#elif defined(UNIX)
    const char* home = NULL;
    char separator = get_path_separator(local_path);

    if ((home = getenv("HOME")) == NULL)
    {
        struct passwd *pw = getpwuid(getuid());
        if (pw)
            home = pw->pw_dir;

        if (!home)
            home = ".";
    }

    local_path = std::string(home) + separator + ".config";
#elif defined(MACOS) || defined(MACOSX)
    const char* home = NULL;
    char separator = get_path_separator(local_path);

    if ((home = getenv("HOME")) == NULL)
    {
        struct passwd *pw = getpwuid(getuid());
        if (pw)
            home = pw->pw_dir;

        if (!home)
            home = ".";
    }
    local_path = std::string(home) + separator + "Library" + separator + "Preferences";
#endif

    char sep = get_path_separator(local_path);
    if (local_path.size() > 0 && local_path[local_path.size() - 1] != sep)
        local_path += sep;

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
	    CoTaskMemFree(path);
    }
#elif defined(UNIX)
    const char* home;
    char separator = get_path_separator(local_path);

    if ((home = getenv("HOME")) == NULL)
    {
        struct passwd *pw = getpwuid(getuid());
        if (pw)
            home = pw->pw_dir;
        if (!home)
            home = ".";
    }
    local_path = std::string(home) + separator + ".local" + separator + "share";
#elif defined(MACOS) || defined(MACOSX)
    const char* home;
    char separator = get_path_separator(local_path);

    if ((home = getenv("HOME")) == NULL)
    {
        struct passwd *pw = getpwuid(getuid());
        if (pw)
            home = pw->pw_dir;

        if (!home)
            home = ".";
    }
    local_path = std::string(home) + separator + "Library" + separator + "Application Support";
#endif

    char sep = get_path_separator(local_path);
    if (local_path.size() > 0 && local_path[local_path.size() - 1] != sep)
        local_path.append(1, sep);

    local_path += std::string("MediaConch").append(1, sep);

    Ztring z_path = ZenLib::Ztring().From_UTF8(local_path);
    if (!ZenLib::Dir::Exists(z_path))
        ZenLib::Dir::Create(z_path);

    return local_path;
}

//---------------------------------------------------------------------------
int Core::create_local_data_directory(const std::string& base_dir, std::string& report_dir)
{
    std::string local_data = Core::get_local_data_path();
    local_data += base_dir;

    char sep = get_path_separator(local_data);
    if (local_data.size() > 0 && local_data[local_data.size() - 1] != sep)
        local_data.append(1, sep);

    Ztring z_local = ZenLib::Ztring().From_UTF8(local_data);
    if (!ZenLib::Dir::Exists(z_local))
        ZenLib::Dir::Create(z_local);

    if (!ZenLib::Dir::Exists(z_local))
        return -1;

    report_dir = local_data;
    return 0;
}

//---------------------------------------------------------------------------
int Core::create_local_unique_data_directory(const std::string& base_dir, const std::string& template_dir, std::string& report_dir)
{
    std::string local_data;
    if (create_local_data_directory(base_dir, local_data) < 0)
        return -1;

    char sep = get_path_separator(local_data);
    if (local_data.size() > 0 && local_data[local_data.size() - 1] != sep)
        local_data.append(1, sep);

    std::stringstream path;
    for (size_t i = 0; ; ++i)
    {
        path.str("");
        path << local_data << template_dir;
        if (i)
            path << i;

        if (path.str().size() > 0 && path.str().at(path.str().size() - 1) != sep)
            path << sep;

        Ztring z_path = ZenLib::Ztring().From_UTF8(path.str());
        if (ZenLib::Dir::Exists(z_path))
            continue;

        report_dir = path.str();
        break;
    }

    Ztring z_path = ZenLib::Ztring().From_UTF8(report_dir);
    ZenLib::Dir::Create(z_path);
    if (!ZenLib::Dir::Exists(z_path))
        return -1;

    return 0;
}

//---------------------------------------------------------------------------
int Core::create_local_unique_data_filename(const std::string& base_dir, const std::string& template_file,
                                            const std::string& template_ext, std::string& filename)
{
    std::string local_data;
    if (create_local_data_directory(base_dir, local_data) < 0)
        return -1;

    char sep = get_path_separator(local_data);
    if (local_data.size() > 0 && local_data[local_data.size() - 1] != sep)
        local_data.append(1, sep);

    filename = local_data + template_file;
    for (size_t i = 0; ; ++i)
    {
        std::stringstream path;
        path << filename;
        if (i)
            path << i;
        if (template_ext.size())
            path << "." << template_ext;

        Ztring z_path = ZenLib::Ztring().From_UTF8(path.str());
        if (ZenLib::File::Exists(z_path))
            continue;

        filename = path.str();
        break;
    }

    return 0;
}

//---------------------------------------------------------------------------
std::string Core::get_database_path()
{
    std::string database_path = get_local_data_path();

    Ztring z_path = ZenLib::Ztring().From_UTF8(database_path);
    char sep = get_path_separator(database_path);
    if (!ZenLib::Dir::Exists(z_path))
        database_path = std::string(".").append(1, sep);
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
    char sep = get_path_separator(config_file);
    if (!ifile)
        config_file = std::string(".").append(1, sep) + configName;
    return config_file;
}

//---------------------------------------------------------------------------
void Core::set_default_scheduler_max_threads(size_t nb)
{
    if (!scheduler)
        return;
        
    scheduler->set_default_max_threads(nb);
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
