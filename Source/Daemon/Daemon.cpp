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
#include <algorithm>
#include <map>
#include <ZenLib/Ztring.h>
#include "Common/Httpd.h"
#include "Common/LibEventHttpd.h"
#include "Common/GeneratedCSVVideos.h"
#include "Common/Policy.h"
#include "Daemon.h"
#include "Help.h"
#include "Config.h"

#ifndef _WIN32
#include <unistd.h>
#endif //_WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <ctime>

//****************************************************************************
// Extern
//****************************************************************************

extern ZenLib::Ztring LogFile_FileName;

//---------------------------------------------------------------------------
namespace MediaConch
{
    //**************************************************************************
    // Daemon
    //**************************************************************************
    std::string Daemon::version = "16.06.0";

    //--------------------------------------------------------------------------
    Daemon::Daemon() : is_daemon(true), httpd(NULL), logger(NULL)
    {
        MCL = new MediaConchLib(true);
        clog_buffer = std::clog.rdbuf();
    }

    //--------------------------------------------------------------------------
    Daemon::~Daemon()
    {
        delete MCL;
        if (logger)
        {
            std::clog.rdbuf(clog_buffer);
            delete logger;
        }
    }

    //--------------------------------------------------------------------------
    int Daemon::init()
    {
        MCL->init();

        // If no Implementation Schema registered, use one by default
        if (!MCL->get_implementation_schema_file().length())
            MCL->create_default_implementation_schema();

        httpd = new LibEventHttpd(this);
        int port = -1;
        std::string address;
        MCL->get_daemon_address(address, port);
        if (address.length())
            httpd->set_address(address);
        if (port != -1)
            httpd->set_port(port);
        if (httpd->init() < 0)
        {
            std::clog << httpd->get_error() << std::endl;
            return -1;
        }

        httpd->commands.analyze_cb = on_analyze_command;
        httpd->commands.status_cb = on_status_command;
        httpd->commands.report_cb = on_report_command;
        httpd->commands.retry_cb = on_retry_command;
        httpd->commands.clear_cb = on_clear_command;
        httpd->commands.list_cb = on_list_command;
        httpd->commands.validate_cb = on_validate_command;
        httpd->commands.file_from_id_cb = on_file_from_id_command;
        httpd->commands.default_values_for_type_cb = on_default_values_for_type_command;
        httpd->commands.create_policy_from_file_cb = on_create_policy_from_file_command;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::parse_args(int argc, char **argv)
    {
        //Parse command line
        for (int pos = 1; pos < argc; ++pos)
        {
            //First part of argument (before "=") should be case insensitive
            std::string argument(argv[pos]);
            if (!argument.compare(0, 1, "-"))
            {
                size_t egal_pos = argument.find('=');
                if (egal_pos == std::string::npos)
                    egal_pos = argument.size();
                transform(argument.begin(), argument.begin() + egal_pos,
                          argument.begin(), (int(*)(int))tolower); //(int(*)(int)) is a patch for unix
            }
            int ret = parse(argument);
            if (ret == DAEMON_RETURN_ERROR || ret == DAEMON_RETURN_FINISH)
                return ret;
        }
        return DAEMON_RETURN_NONE;
    }

    //--------------------------------------------------------------------------
    int Daemon::run()
    {
        if (is_daemon)
            daemonize();
        if (!httpd)
            return -1;

        if (httpd->bind() < 0)
        {
            std::clog << httpd->get_error() << std::endl;
            return -1;
        }

        std::clog << get_date() << "Daemon is running on " << httpd->get_address() << ":" << httpd->get_port() << std::endl;
        if (httpd->start() < 0)
        {
            std::clog << httpd->get_error() << std::endl;
            return -1;
        }
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::finish()
    {
        if (httpd)
            httpd->finish();
        MCL->close();
        std::clog << get_date() << "Daemon is stopped" << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::parse(std::string& argument)
    {
#define LAUNCH(_METHOD)                                                    \
        {                                                                  \
            int ret = parse_##_METHOD(argument);                           \
            if (ret == DAEMON_RETURN_ERROR || ret == DAEMON_RETURN_FINISH) \
                return ret;                                                \
        }

#define OPTION(_TEXT, _TOLAUNCH)                                    \
        else if (argument.find(_TEXT)==0)        LAUNCH(_TOLAUNCH)

        if (last_argument.length())
        {
            argument = last_argument + argument;
            last_argument = std::string();
        }

        if (argument=="-h")
            argument="--help";
        if (argument=="-v")
            argument="--version";
        if (argument=="-n")
            argument="--fork=No";

        if (argument=="-c")
        {
            last_argument="--configuration=";
            return DAEMON_RETURN_NONE;
        }
        if (argument=="-pc")
        {
            last_argument="--pluginsconfiguration=";
            return DAEMON_RETURN_NONE;
        }
        if (argument=="-i")
        {
            last_argument = "--implementationschema=";
            return DAEMON_RETURN_NONE;
        }
        if (argument=="-iv")
        {
            last_argument = "--implementationverbosity=";
            return DAEMON_RETURN_NONE;
        }
        if (argument=="-o")
        {
            last_argument = "--outputlog=";
            return DAEMON_RETURN_NONE;
        }

        // Compression short option
        if (argument=="-cz")
            argument = "--compression=zlib";

        if (0);
        OPTION("--help",                    help)
        OPTION("--version",                 version)
        OPTION("--fork",                    fork)
        OPTION("--configuration",           configuration)
        OPTION("--pluginsconfiguration",    plugins_configuration)
        OPTION("--compression",             compression)
        OPTION("--implementationschema",    implementationschema)
        OPTION("--implementationverbosity", implementationverbosity)
        OPTION("--outputlog",               outputlog)
        OPTION("--",                        other)
        else
        {
            Help();
            return DAEMON_RETURN_ERROR;
        }
        return DAEMON_RETURN_NONE;

#undef OPTION
#undef LAUNCH
    }

    //--------------------------------------------------------------------------
    int Daemon::parse_fork(const std::string& argument)
    {
        size_t equal_pos = argument.find('=');
        if (equal_pos == std::string::npos)
            return DAEMON_RETURN_NONE;

        std::string not_daemon = argument.substr(equal_pos + 1);
        if (not_daemon == "no" || not_daemon == "No")
            is_daemon = false;
        return DAEMON_RETURN_NONE;
    }

    //--------------------------------------------------------------------------
    int Daemon::parse_help(const std::string& argument)
    {
        (void)argument;
        return Help();
    }

    //--------------------------------------------------------------------------
    int Daemon::parse_version(const std::string& argument)
    {
        (void)argument;
        return Version();
    }

    //--------------------------------------------------------------------------
    int Daemon::parse_configuration(const std::string& argument)
    {
        size_t equal_pos = argument.find('=');
        if (equal_pos == std::string::npos)
            return DAEMON_RETURN_NONE;

        std::string file = argument.substr(equal_pos + 1);
        MCL->set_configuration_file(file);
        return DAEMON_RETURN_NONE;
    }

    //--------------------------------------------------------------------------
    int Daemon::parse_plugins_configuration(const std::string& argument)
    {
        size_t equal_pos = argument.find('=');
        if (equal_pos == std::string::npos)
        {
            Help();
            return DAEMON_RETURN_ERROR;
        }

        std::string file = argument.substr(equal_pos + 1);
        MCL->set_plugins_configuration_file(file);
        return DAEMON_RETURN_NONE;
    }

    //--------------------------------------------------------------------------
    int Daemon::parse_compression(const std::string& argument)
    {
        size_t equal_pos = argument.find('=');
        if (equal_pos == std::string::npos)
        {
            Help();
            return DAEMON_RETURN_ERROR;
        }

        std::string mode_str;
        mode_str.assign(argument, equal_pos + 1 , std::string::npos);
        transform(mode_str.begin(), mode_str.end(), mode_str.begin(), (int(*)(int))tolower); //(int(*)(int)) is a patch for unix
        MediaConchLib::compression mode;

        if (mode_str == "none")
            mode = MediaConchLib::compression_None;
        else if (mode_str == "zlib")
            mode = MediaConchLib::compression_ZLib;
        else
        {
            Help();
            return DAEMON_RETURN_ERROR;
        }

        MCL->set_compression_mode(mode);
        return DAEMON_RETURN_NONE;
    }

    //--------------------------------------------------------------------------
    int Daemon::parse_implementationschema(const std::string& argument)
    {
        //Form : --ImplemnetationSchema=File
        size_t egal_pos = argument.find('=');
        if (egal_pos == std::string::npos)
        {
            Help();
            return DAEMON_RETURN_ERROR;
        }

        std::string schema;
        schema.assign(argument, egal_pos + 1 , std::string::npos);
        MCL->set_implementation_schema_file(schema);
        return DAEMON_RETURN_NONE;
    }

    //--------------------------------------------------------------------------
    int Daemon::parse_implementationverbosity(const std::string& argument)
    {
        //Form : --ImplemnetationSchema=File
        size_t egal_pos = argument.find('=');
        if (egal_pos == std::string::npos)
        {
            Help();
            return DAEMON_RETURN_ERROR;
        }

        std::string verbosity;
        verbosity.assign(argument, egal_pos + 1 , std::string::npos);
        MCL->set_implementation_verbosity(verbosity);
        return DAEMON_RETURN_NONE;
    }

    //--------------------------------------------------------------------------
    int Daemon::parse_outputlog(const std::string& argument)
    {
        size_t egal_pos = argument.find('=');
        if (egal_pos == std::string::npos)
        {
            Help();
            return DAEMON_RETURN_ERROR;
        }
        std::string log_file;
        log_file.assign(argument, egal_pos + 1 , std::string::npos);
        logger = new std::ofstream(log_file.c_str());
        std::clog.rdbuf(logger->rdbuf());
        return DAEMON_RETURN_NONE;
    }

    //--------------------------------------------------------------------------
    int Daemon::parse_other(const std::string& argument)
    {
        std::string report;
        if (MCL->add_option(argument, report) < 0)
        {
            ZenLib::Ztring str;
            str.From_UTF8(report);
            STRINGOUT(str);
            if (report == "Option not known")
                return DAEMON_RETURN_ERROR;
            return DAEMON_RETURN_FINISH;
        }
        return DAEMON_RETURN_NONE;
    }

#if defined(_WIN32) || defined(WIN32)
    //---------------------------------------------------------------------------
    int Daemon::daemonize()
    {
        return 0;
    }

#else
    //---------------------------------------------------------------------------
    int Daemon::daemonize()
    {
        // Fork
        pid_t pid = fork();
        if (pid < 0)
            return -1;

        // close the parent
        if (pid > 0)
            exit(EXIT_SUCCESS);

        umask(0);

        pid_t sid = setsid();
        if (sid < 0)
            return -1;

        if ((chdir("/")) < 0)
            return -1;

        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        return 0;
    }
#endif

    //--------------------------------------------------------------------------
    int Daemon::on_analyze_command(const RESTAPI::Analyze_Req* req, RESTAPI::Analyze_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received an analyze command: ";
        std::clog << req->to_str() << std::endl;
        for (size_t i = 0; i < req->args.size(); ++i)
        {
            bool force = false;
            if (req->args[i].has_force_analyze)
                force = req->args[i].force_analyze;
            bool registered = false;
            int ret = d->MCL->analyze(req->args[i].file, registered, force);
            if (ret < 0)
            {
                RESTAPI::Analyze_Nok *nok = new RESTAPI::Analyze_Nok;
                nok->id = req->args[i].id;
                nok->error = RESTAPI::FILE_NOT_EXISTING;
                res.nok.push_back(nok);
                continue;
            }

            RESTAPI::Analyze_Ok *ok = new RESTAPI::Analyze_Ok;
            ok->inId = req->args[i].id;
            if (registered)
            {
                size_t id = 0;
                for (; id < d->current_files.size(); ++id)
                    if (d->id_is_existing(id) && *d->current_files[id] == req->args[i].file)
                    {
                        ok->outId = id;
                        break;
                    }
                if (id < d->current_files.size())
                {
                    ok->create = false;
                    res.ok.push_back(ok);
                    continue;
                }
            }

            size_t new_id = d->get_first_free_slot();
            d->current_files[new_id] = new std::string(req->args[i].file);
            ok->outId = new_id;
            ok->create = !registered;
            res.ok.push_back(ok);
        }
        std::clog << d->get_date() << "Daemon send analyze result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_status_command(const RESTAPI::Status_Req* req, RESTAPI::Status_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a status command: ";
        std::clog << req->to_str() << std::endl;
        for (size_t i = 0; i < req->ids.size(); ++i)
        {
            int id = req->ids[i];
            if (!d->id_is_existing(id))
            {
                RESTAPI::Status_Nok *nok = new RESTAPI::Status_Nok;
                nok->id = id;
                nok->error = RESTAPI::ID_NOT_EXISTING;
                res.nok.push_back(nok);
                continue;
            }

            MediaConchLib::report report_kind;
            double percent_done = 0.0;
            int is_done = d->MCL->is_done(*d->current_files[id], percent_done, report_kind);

            if (is_done < 0)
            {
                RESTAPI::Status_Nok *nok = new RESTAPI::Status_Nok;
                nok->id = id;
                nok->error = RESTAPI::NO_REASON;
                res.nok.push_back(nok);
                continue;
            }
            RESTAPI::Status_Ok *ok = new RESTAPI::Status_Ok;

            ok->id = id;
            if (is_done == MediaConchLib::errorHttp_TRUE)
            {
                ok->finished = true;
                ok->has_tool = false;
                if (report_kind == MediaConchLib::report_MediaVeraPdf)
                {
                    ok->has_tool = true;
                    ok->tool = RESTAPI::VERAPDF;
                }
                else if (report_kind == MediaConchLib::report_MediaDpfManager)
                {
                    ok->has_tool = true;
                    ok->tool = RESTAPI::DPFMANAGER;
                }
            }
            else
            {
                ok->finished = false;
                ok->has_percent = true;
                ok->done = percent_done;
            }
            res.ok.push_back(ok);
        }
        std::clog << d->get_date() << "Daemon send status result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_report_command(const RESTAPI::Report_Req* req, RESTAPI::Report_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a report command: ";
        std::clog << req->to_str() << std::endl;
        MediaConchLib::format format = MediaConchLib::format_Xml;
        if (req->display_name == MediaConchLib::display_text_name)
            format = MediaConchLib::format_Text;
        else if (req->display_name == MediaConchLib::display_maxml_name)
            format = MediaConchLib::format_MaXml;
        else if (req->display_name == MediaConchLib::display_html_name)
            format = MediaConchLib::format_Html;
        else if (req->display_name == MediaConchLib::display_jstree_name)
            format = MediaConchLib::format_JsTree;

        const std::string* display_name = req->display_name.length() ? &req->display_name : NULL;
        const std::string* display_content = req->display_content.length() ? &req->display_content : NULL;
        if (format != MediaConchLib::format_Xml)
            display_name = NULL;

        std::bitset<MediaConchLib::report_Max> report_set;
        bool has_policy = false;
        for (size_t j = 0; j < req->reports.size(); ++j)
        {
            if (req->reports[j] == RESTAPI::MEDIAINFO)
                report_set.set(MediaConchLib::report_MediaInfo);
            if (req->reports[j] == RESTAPI::MEDIATRACE)
                report_set.set(MediaConchLib::report_MediaTrace);
            if (req->reports[j] == RESTAPI::IMPLEMENTATION)
                report_set.set(MediaConchLib::report_MediaConch);
            if (req->reports[j] == RESTAPI::POLICY)
                has_policy = true;
            if (req->reports[j] == RESTAPI::VERAPDF)
                report_set.set(MediaConchLib::report_MediaVeraPdf);
            if (req->reports[j] == RESTAPI::DPFMANAGER)
                report_set.set(MediaConchLib::report_MediaDpfManager);
        }

        if (!report_set.count() && !has_policy)
            return -1;

        bool has_valid = false;
        bool valid = true;
        std::vector<std::string> files;
        for (size_t i = 0; i < req->ids.size(); ++i)
        {
            int id = req->ids[i];
            if (!d->id_is_existing(id))
            {
                RESTAPI::Report_Nok *nok = new RESTAPI::Report_Nok;
                nok->id = id;
                nok->error = RESTAPI::ID_NOT_EXISTING;
                res.nok.push_back(nok);
                continue;
            }

            MediaConchLib::report report_kind;
            double percent_done = 0.0;
            int is_done = d->MCL->is_done(*d->current_files[id], percent_done, report_kind);
            if (is_done != MediaConchLib::errorHttp_TRUE)
            {
                RESTAPI::Report_Nok *nok = new RESTAPI::Report_Nok;
                nok->id = id;
                nok->error = RESTAPI::NOT_READY;
                res.nok.push_back(nok);
                continue;
            }

            files.push_back(*d->current_files[id]);
        }

        std::stringstream verbosity;
        if (req->has_verbosity && req->verbosity != -1)
            verbosity << req->verbosity;
        else
            verbosity << d->MCL->get_implementation_verbosity();

        std::map<std::string, std::string> options;
        options["verbosity"] = verbosity.str();

        // Output
        MediaConchLib::ReportRes result;
        d->MCL->get_report(report_set, format, files,
                           req->policies_names, req->policies_contents,
                           options, &result, display_name, display_content);
        res.ok.report = result.report;
        if (result.has_valid)
        {
            has_valid = true;
            if (!result.valid)
                valid = false;
        }
        res.ok.has_valid = has_valid;
        res.ok.valid = valid;
        std::clog << d->get_date() << "Daemon send report result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_retry_command(const RESTAPI::Retry_Req* req, RESTAPI::Retry_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a retry command: ";
        std::clog << req->to_str() << std::endl;
        for (size_t i = 0; i < req->ids.size(); ++i)
        {
            int id = req->ids[i];
            if (!d->id_is_existing(id))
            {
                RESTAPI::Retry_Nok *nok = new RESTAPI::Retry_Nok;
                nok->id = id;
                nok->error = RESTAPI::ID_NOT_EXISTING;
                res.nok.push_back(nok);
                continue;
            }

            std::vector<std::string> files;
            files.push_back(*d->current_files[id]);
            d->MCL->remove_report(files);
            bool registered = false;
            int ret = d->MCL->analyze(*d->current_files[id], registered);
            if (ret < 0)
            {
                RESTAPI::Retry_Nok *nok = new RESTAPI::Retry_Nok;
                nok->id = id;
                nok->error = RESTAPI::FILE_NOT_EXISTING;
                res.nok.push_back(nok);
                continue;
            }
            res.ok.push_back(id);
        }
        std::clog << d->get_date() << "Daemon send retry result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_clear_command(const RESTAPI::Clear_Req* req, RESTAPI::Clear_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a clear command: ";
        std::clog << req->to_str() << std::endl;
        for (size_t i = 0; i < req->ids.size(); ++i)
        {
            int id = req->ids[i];
            if (!d->id_is_existing(id))
            {
                RESTAPI::Clear_Nok *nok = new RESTAPI::Clear_Nok;
                nok->id = id;
                nok->error = RESTAPI::ID_NOT_EXISTING;
                res.nok.push_back(nok);
                continue;
            }

            std::vector<std::string> files;
            files.push_back(*d->current_files[id]);
            d->MCL->remove_report(files);
            delete d->current_files[id];
            d->current_files[id] = NULL;
            res.ok.push_back(id);
        }
        std::clog << d->get_date() << "Daemon send clear result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_list_command(const RESTAPI::List_Req* req, RESTAPI::List_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a list command" << std::endl;
        std::vector<std::string> vec;
        d->MCL->list(vec);
        for (size_t i = 0; i < vec.size(); ++i)
        {
            RESTAPI::List_File *file = new RESTAPI::List_File;
            file->file = vec[i];

            size_t id = 0;
            if (!d->file_is_registered(vec[i], id))
            {
                id = d->get_first_free_slot();
                d->current_files[id] = new std::string(vec[i]);
            }

            file->id = id;
            res.files.push_back(file);
        }
        std::clog << d->get_date() << "Daemon send list result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_validate_command(const RESTAPI::Validate_Req* req, RESTAPI::Validate_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a validate command: ";
        std::clog << req->to_str() << std::endl;

        MediaConchLib::report report;
        if (req->report == RESTAPI::IMPLEMENTATION)
            report = MediaConchLib::report_MediaConch;
        else if (req->report == RESTAPI::VERAPDF)
            report = MediaConchLib::report_MediaVeraPdf;
        else if (req->report == RESTAPI::DPFMANAGER)
            report = MediaConchLib::report_MediaDpfManager;
        else if (req->report != RESTAPI::POLICY)
            return -1;
        else
            report = MediaConchLib::report_Max;

        std::map<std::string, int> saved_ids;
        std::vector<std::string> files;
        for (size_t i = 0; i < req->ids.size(); ++i)
        {
            int id = req->ids[i];
            if (!d->id_is_existing(id))
            {
                RESTAPI::Validate_Nok *nok = new RESTAPI::Validate_Nok;
                nok->id = id;
                nok->error = RESTAPI::ID_NOT_EXISTING;
                res.nok.push_back(nok);
                continue;
            }

            MediaConchLib::report report_kind;
            double percent_done = 0.0;
            int is_done = d->MCL->is_done(*d->current_files[id], percent_done, report_kind);
            if (is_done != MediaConchLib::errorHttp_TRUE)
            {
                RESTAPI::Validate_Nok *nok = new RESTAPI::Validate_Nok;
                nok->id = id;
                nok->error = RESTAPI::NOT_READY;
                res.nok.push_back(nok);
                continue;
            }

            // Output
            files.push_back(*d->current_files[id]);
            saved_ids[*d->current_files[id]] = id;
        }

        std::vector<MediaConchLib::ValidateRes*> result;
        if (d->MCL->validate(report, files, req->policies_names, req->policies_contents,
                             result))
            return -1;

        for (size_t i = 0; i < result.size(); ++i)
        {
            RESTAPI::Validate_Ok* ok = new RESTAPI::Validate_Ok;
            ok->id = saved_ids[result[i]->file];
            ok->valid = result[i]->valid;
            res.ok.push_back(ok);
        }
        std::clog << d->get_date() << "Daemon send validate result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_file_from_id_command(const RESTAPI::File_From_Id_Req* req, RESTAPI::File_From_Id_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a file_from_id command" << std::endl;
        std::clog << req->to_str() << std::endl;

        if (d->id_is_existing(req->id))
            res.file = *d->current_files[req->id];

        std::clog << d->get_date() << "Daemon send file_from_id result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_default_values_for_type_command(const RESTAPI::Default_Values_For_Type_Req* req, RESTAPI::Default_Values_For_Type_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a default_values_for_type command: ";
        std::clog << req->to_str() << std::endl;

        std::map<std::string, std::map<std::string, std::vector<std::string> > > values;
        if (get_generated_values_from_csv(values) < 0)
            return 0;

        if (values.find(req->type) != values.end())
        {
            if (values[req->type].find(req->field) != values[req->type].end())
            {
                for (size_t i = 0; i < values[req->type][req->field].size(); ++i)
                    res.values.push_back(values[req->type][req->field][i]);
            }
        }

        std::clog << d->get_date() << "Daemon send default_values_for_type result: " << res.to_str() << std::endl;

        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_create_policy_from_file_command(const RESTAPI::Create_Policy_From_File_Req* req, RESTAPI::Create_Policy_From_File_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a create_policy_from_file command: ";
        std::clog << req->to_str() << std::endl;

        if (!d->id_is_existing(req->id))
        {
            RESTAPI::Create_Policy_From_File_Nok *nok = new RESTAPI::Create_Policy_From_File_Nok;
            nok->id = req->id;
            nok->error = RESTAPI::ID_NOT_EXISTING;
            res.nok = nok;
        }
        else
        {
            size_t pos = d->MCL->create_policy_from_file(*d->current_files[req->id]);
            Policy *p = NULL;
            std::string policy;
            if (pos == (size_t)-1 || (p = d->MCL->get_policy(pos)) == NULL || p->dump_schema(policy) < 0)
            {
                RESTAPI::Create_Policy_From_File_Nok *nok = new RESTAPI::Create_Policy_From_File_Nok;
                nok->id = req->id;
                nok->error = RESTAPI::NO_REASON;
                res.nok = nok;
            }
            else
                res.policy = policy;
        }

        std::clog << d->get_date() << "Daemon send create_policy_from_file result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    size_t Daemon::get_first_free_slot()
    {
        size_t i = 0;
        for (; i < current_files.size();  ++i)
            if (!current_files[i])
                return i;
        current_files.push_back(NULL);
        return current_files.size() - 1;
    }

    //--------------------------------------------------------------------------
    bool Daemon::id_is_existing(int id) const
    {
        if (id < 0 || id >= (int)current_files.size() || !current_files[id])
            return false;
        return true;
    }

    //--------------------------------------------------------------------------
    std::string Daemon::get_date() const
    {
        std::stringstream out;
        time_t            t;

        time(&t);
        std::string str(ctime(&t));
        str = str.substr(0, str.length() - 1);
        out << "[" << str << "]";
        return out.str();
    }

    //--------------------------------------------------------------------------
    bool Daemon::file_is_registered(const std::string& file, size_t& id)
    {
        for (size_t i = 0; i < current_files.size(); ++i)
            if (current_files[i] && !current_files[i]->compare(file))
            {
                id = i;
                return true;
            }
        return false;
    }
}
