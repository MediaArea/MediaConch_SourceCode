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
#include <ZenLib/File.h>
#include "Common/Httpd.h"
#include "Common/Core.h"
#include "Common/LibEventHttpd.h"
#include "Common/Policy.h"
#include "Common/XsltPolicy.h"
#include "Daemon.h"
#include "Help.h"
#include "Config.h"

#ifndef _WIN32
#include <unistd.h>
#endif //_WIN32
#include <sys/types.h>
#include <sys/stat.h>

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
    std::string Daemon::version = "16.11.0";

    //--------------------------------------------------------------------------
    Daemon::Daemon() : is_daemon(true), httpd(NULL), logger(NULL), watch_folder_user(NULL),
                       watch_folder_recursive(true), mode(DAEMON_MODE_DAEMON)
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

        if (watch_folder_user)
            delete watch_folder_user;
    }

    //--------------------------------------------------------------------------
    int Daemon::init()
    {
        MCL->init();

        // If no Implementation Schema registered, use one by default
        if (!MCL->get_implementation_schema_file().length())
            MCL->create_default_implementation_schema();

        // Load policy
        MCL->load_system_policy();
        MCL->load_existing_policy();

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

        httpd->commands.mediaconch_get_plugins_cb = on_mediaconch_get_plugins_command;
        httpd->commands.mediaconch_watch_folder_cb = on_mediaconch_watch_folder_command;
        httpd->commands.mediaconch_list_watch_folders_cb = on_mediaconch_list_watch_folders_command;
        httpd->commands.mediaconch_edit_watch_folder_cb = on_mediaconch_edit_watch_folder_command;
        httpd->commands.mediaconch_remove_watch_folder_cb = on_mediaconch_remove_watch_folder_command;

        httpd->commands.analyze_cb = on_analyze_command;
        httpd->commands.status_cb = on_status_command;
        httpd->commands.report_cb = on_report_command;
        httpd->commands.retry_cb = on_retry_command;
        httpd->commands.clear_cb = on_clear_command;
        httpd->commands.list_cb = on_list_command;
        httpd->commands.validate_cb = on_validate_command;
        httpd->commands.file_from_id_cb = on_file_from_id_command;
        httpd->commands.id_from_filename_cb = on_id_from_filename_command;
        httpd->commands.file_information_cb = on_file_information_command;
        httpd->commands.default_values_for_type_cb = on_default_values_for_type_command;

        httpd->commands.xslt_policy_create_cb = on_xslt_policy_create_command;
        httpd->commands.policy_import_cb = on_policy_import_command;
        httpd->commands.policy_save_cb = on_policy_save_command;
        httpd->commands.policy_dump_cb = on_policy_dump_command;
        httpd->commands.policy_remove_cb = on_policy_remove_command;
        httpd->commands.policy_duplicate_cb = on_policy_duplicate_command;
        httpd->commands.policy_move_cb = on_policy_move_command;
        httpd->commands.policy_change_info_cb = on_policy_change_info_command;
        httpd->commands.policy_change_type_cb = on_policy_change_type_command;
        httpd->commands.policy_change_is_public_cb = on_policy_change_is_public_command;
        httpd->commands.policy_get_cb = on_policy_get_command;
        httpd->commands.policy_get_name_cb = on_policy_get_name_command;
        httpd->commands.policy_get_policies_count_cb = on_policy_get_policies_count_command;
        httpd->commands.policy_clear_policies_cb = on_policy_clear_policies_command;
        httpd->commands.policy_get_policies_cb = on_policy_get_policies_command;
        httpd->commands.policy_get_public_policies_cb = on_policy_get_public_policies_command;
        httpd->commands.policy_get_policies_names_list_cb = on_policy_get_policies_names_list_command;
        httpd->commands.xslt_policy_create_from_file_cb = on_xslt_policy_create_from_file_command;
        httpd->commands.xslt_policy_rule_create_cb = on_xslt_policy_rule_create_command;
        httpd->commands.xslt_policy_rule_get_cb = on_xslt_policy_rule_get_command;
        httpd->commands.xslt_policy_rule_edit_cb = on_xslt_policy_rule_edit_command;
        httpd->commands.xslt_policy_rule_duplicate_cb = on_xslt_policy_rule_duplicate_command;
        httpd->commands.xslt_policy_rule_move_cb = on_xslt_policy_rule_move_command;
        httpd->commands.xslt_policy_rule_delete_cb = on_xslt_policy_rule_delete_command;
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
        switch (mode)
        {
            case DAEMON_MODE_PLUGINS_LIST:
                return run_plugins_list();
            case DAEMON_MODE_DAEMON:
                //default, continue
                break;
        }

        if (is_daemon)
            daemonize();

        if (MCL && watch_folder.size())
        {
            std::string err;
            long user_id = -1;
            int ret = MCL->mediaconch_watch_folder(watch_folder, watch_folder_reports, plugins, policies,
                                                   watch_folder_user, watch_folder_recursive, options, user_id, err);
            if (ret < 0)
                std::clog << "Cannot watch folder:" << watch_folder << ":" << err << std::endl;
            else
                std::clog << "Watching folder:" << watch_folder << ", user:" << user_id << std::endl;
        }
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
    int Daemon::run_plugins_list()
    {
        std::stringstream out;
        std::vector<std::string> list;
        std::string error;
        if (MCL->mediaconch_get_plugins(list, error) < 0)
            return MediaConchLib::errorHttp_INTERNAL;

        out << "plugins:[";

        for (size_t i = 0; i < list.size(); ++i)
        {
            if (i)
                out << ", ";
            out << "\"" << list[i] << "\"";
        }

        out << "]" << std::endl;
        TEXTOUT(out.str().c_str());
        return DAEMON_RETURN_NONE;
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
        OPTION("--help",                      help)
        OPTION("--version",                   version)
        OPTION("--fork",                      fork)
        OPTION("--configuration",             configuration)
        OPTION("--pluginsconfiguration",      plugins_configuration)
        OPTION("--pluginslist",               plugins_list)
        OPTION("--plugin",                    plugin)
        OPTION("--policy",                    policy)
        OPTION("--compression",               compression)
        OPTION("--implementationschema",      implementationschema)
        OPTION("--implementationverbosity",   implementationverbosity)
        OPTION("--outputlog",                 outputlog)
        OPTION("--watchfolder-reports",       watchfolder_reports)
        OPTION("--watchfolder-not-recursive", watchfolder_not_recursive)
        OPTION("--watchfolder-user",          watchfolder_user)
        OPTION("--watchfolder",               watchfolder)
        OPTION("--",                          other)
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
    int Daemon::parse_plugin(const std::string& argument)
    {
        size_t equal_pos = argument.find('=');
        if (equal_pos == std::string::npos)
        {
            Help();
            return DAEMON_RETURN_ERROR;
        }

        std::string plugin = argument.substr(equal_pos + 1);
        plugins.push_back(plugin);
        return DAEMON_RETURN_NONE;
    }

    //--------------------------------------------------------------------------
    int Daemon::parse_plugins_list(const std::string& argument)
    {
        (void)argument;

        mode = DAEMON_MODE_PLUGINS_LIST;
        return DAEMON_RETURN_NONE;
    }

    //--------------------------------------------------------------------------
    int Daemon::parse_policy(const std::string& argument)
    {
        size_t equal_pos = argument.find('=');
        if (equal_pos == std::string::npos)
        {
            Help();
            return DAEMON_RETURN_ERROR;
        }

        std::string filename = argument.substr(equal_pos + 1);
        ZenLib::Ztring z_filename = ZenLib::Ztring().From_UTF8(filename);
        if (!ZenLib::File::Exists(z_filename))
        {
            TEXTOUT("Policiy file does not exists");
            return DAEMON_RETURN_ERROR;
        }

        ZenLib::File file(z_filename);

        ZenLib::int64u size = file.Size_Get();
        if (size == (ZenLib::int64u)-1)
        {
            TEXTOUT("Cannot read the policiy file");
            return DAEMON_RETURN_ERROR;
        }

        ZenLib::int8u* Buffer = new ZenLib::int8u[size + 1];
        size_t len = file.Read(Buffer, size);
        Buffer[len] = '\0';

        ZenLib::Ztring FromFile;
        FromFile.From_UTF8((char*)Buffer);
        if (FromFile.empty())
            FromFile.From_Local((char*)Buffer);

        file.Close();
        policies.push_back(FromFile.To_UTF8());
        delete [] Buffer;
        return DAEMON_RETURN_NONE;
    }

    //--------------------------------------------------------------------------
    int Daemon::parse_watchfolder(const std::string& argument)
    {
        size_t egal_pos = argument.find('=');
        if (egal_pos == std::string::npos)
        {
            Help();
            return DAEMON_RETURN_ERROR;
        }
        std::string folder;
        folder.assign(argument, egal_pos + 1 , std::string::npos);
        watch_folder = folder;
        return DAEMON_RETURN_NONE;
    }

    //--------------------------------------------------------------------------
    int Daemon::parse_watchfolder_reports(const std::string& argument)
    {
        size_t egal_pos = argument.find('=');
        if (egal_pos == std::string::npos)
        {
            Help();
            return DAEMON_RETURN_ERROR;
        }
        std::string dir;
        dir.assign(argument, egal_pos + 1 , std::string::npos);
        watch_folder_reports = dir;
        return DAEMON_RETURN_NONE;
    }

    //--------------------------------------------------------------------------
    int Daemon::parse_watchfolder_user(const std::string& argument)
    {
        size_t egal_pos = argument.find('=');
        if (egal_pos == std::string::npos)
        {
            Help();
            return DAEMON_RETURN_ERROR;
        }

        std::string user;
        user.assign(argument, egal_pos + 1 , std::string::npos);

        if (user.size())
        {
            char *end = NULL;
            watch_folder_user = new long;
            *watch_folder_user = strtol(user.c_str(), &end, 10);
        }
        return DAEMON_RETURN_NONE;
    }

    //--------------------------------------------------------------------------
    int Daemon::parse_watchfolder_not_recursive(const std::string& argument)
    {
        (void)argument;
        watch_folder_recursive = false;
        return DAEMON_RETURN_NONE;
    }

    //--------------------------------------------------------------------------
    int Daemon::parse_other(const std::string& argument)
    {
        if (argument.size() < 2)
            return DAEMON_RETURN_NONE;

        std::string key;
        std::string value;
        size_t egal_pos = argument.find('=');
        if (egal_pos != std::string::npos)
        {
            key.assign(argument, 2 , egal_pos - 2);
            value.assign(argument, egal_pos + 1, std::string::npos);
        }
        else
            key.assign(argument, 2, std::string::npos);

        std::string report;
        if (MCL->test_mil_option(key, value, report) < 0)
        {
            ZenLib::Ztring str;
            str.From_UTF8(report);
            STRINGOUT(str);
            if (report == "Option not known")
                return DAEMON_RETURN_ERROR;
            return DAEMON_RETURN_FINISH;
        }

        options.push_back(std::make_pair(key, value));
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
    int Daemon::on_mediaconch_get_plugins_command(const RESTAPI::MediaConch_Get_Plugins_Req* req, RESTAPI::MediaConch_Get_Plugins_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a mediaconch get plugins command" << std::endl;
        std::vector<std::string> vec;
        std::string error;
        if (d->MCL->mediaconch_get_plugins(vec, error) < 0)
        {
            res.nok = new RESTAPI::MediaConch_Nok;
            res.nok->error = error;
        }
        else
        {
            for (size_t i = 0; i < vec.size(); ++i)
                res.plugins.push_back(vec[i]);
        }

        std::clog << d->get_date() << "Daemon send get mediaconch plugins result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_mediaconch_watch_folder_command(const RESTAPI::MediaConch_Watch_Folder_Req* req,
                                                   RESTAPI::MediaConch_Watch_Folder_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a mediaconch watch folder command: ";
        std::clog << req->to_str() << std::endl;
        std::string error;
        long user_id = -1;
        if (d->MCL->mediaconch_watch_folder(req->folder, req->folder_reports,
                                            req->plugins, req->policies,
                                            req->user, req->recursive, req->options, user_id, error) < 0)
        {
            res.nok = new RESTAPI::MediaConch_Nok;
            res.nok->error = error;
        }
        else
            res.user = user_id;

        std::clog << d->get_date() << "Daemon send mediaconch watch folder result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_mediaconch_list_watch_folders_command(const RESTAPI::MediaConch_List_Watch_Folders_Req* req,
                                                         RESTAPI::MediaConch_List_Watch_Folders_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a mediaconch list watch folders command" << std::endl;
        std::string error;
        std::vector<std::string> list;
        if (d->MCL->mediaconch_list_watch_folders(list, error) < 0)
        {
            res.nok = new RESTAPI::MediaConch_Nok;
            res.nok->error = error;
        }
        else
            for (size_t i = 0; i < list.size(); ++i)
                res.folders.push_back(list[i]);

        std::clog << d->get_date() << "Daemon send mediaconch list watch folders result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_mediaconch_edit_watch_folder_command(const RESTAPI::MediaConch_Edit_Watch_Folder_Req* req,
                                                        RESTAPI::MediaConch_Edit_Watch_Folder_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a mediaconch edit watch folder command: ";
        std::clog << req->to_str() << std::endl;
        std::string error;
        if (d->MCL->mediaconch_edit_watch_folder(req->folder, req->folder_reports, error) < 0)
        {
            res.nok = new RESTAPI::MediaConch_Nok;
            res.nok->error = error;
        }

        std::clog << d->get_date() << "Daemon send mediaconch edit watch folder result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_mediaconch_remove_watch_folder_command(const RESTAPI::MediaConch_Remove_Watch_Folder_Req* req,
                                                          RESTAPI::MediaConch_Remove_Watch_Folder_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a mediaconch remove watch folder command: ";
        std::clog << req->to_str() << std::endl;
        std::string error;
        if (d->MCL->mediaconch_remove_watch_folder(req->folder, error) < 0)
        {
            res.nok = new RESTAPI::MediaConch_Nok;
            res.nok->error = error;
        }

        std::clog << d->get_date() << "Daemon send mediaconch remove watch folder result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_analyze_command(const RESTAPI::Checker_Analyze_Req* req, RESTAPI::Checker_Analyze_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a checker_analyze command: ";
        std::clog << req->to_str() << std::endl;
        for (size_t i = 0; i < req->args.size(); ++i)
        {
            bool force = false;
            if (req->args[i].has_force_analyze)
                force = req->args[i].force_analyze;
            std::vector<std::string> plugins;
            for (size_t j = 0; j < req->args[i].plugins.size(); ++j)
                plugins.push_back(req->args[i].plugins[j]);

            std::vector<std::pair<std::string, std::string> > options;
            for (size_t j = 0; j < req->args[i].options.size(); ++j)
                options.push_back(std::make_pair(req->args[i].options[j].first, req->args[i].options[j].second));

            bool registered = false;
            long out_id = -1;
            int ret = d->MCL->checker_analyze(req->args[i].user, req->args[i].file, plugins, options, registered, out_id, force, req->args[i].mil_analyze);
            if (ret < 0)
            {
                RESTAPI::Checker_Analyze_Nok *nok = new RESTAPI::Checker_Analyze_Nok;
                nok->id = req->args[i].id;
                nok->error = RESTAPI::FILE_NOT_EXISTING;
                res.nok.push_back(nok);
                continue;
            }

            RESTAPI::Checker_Analyze_Ok *ok = new RESTAPI::Checker_Analyze_Ok;
            ok->inId = req->args[i].id;
            ok->outId = out_id;
            ok->create = !registered;

            res.ok.push_back(ok);
        }

        std::clog << d->get_date() << "Daemon send checker analyze result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_status_command(const RESTAPI::Checker_Status_Req* req, RESTAPI::Checker_Status_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a checker_status command: ";
        std::clog << req->to_str() << std::endl;
        for (size_t i = 0; i < req->ids.size(); ++i)
        {
            long id = req->ids[i];
            if (id < 0)
            {
                RESTAPI::Checker_Status_Nok *nok = new RESTAPI::Checker_Status_Nok;
                nok->id = id;
                nok->error = RESTAPI::ID_NOT_EXISTING;
                res.nok.push_back(nok);
                continue;
            }

            MediaConchLib::Checker_StatusRes st_res;
            int ret = d->MCL->checker_status(req->user, id, st_res);

            if (ret < 0)
            {
                RESTAPI::Checker_Status_Nok *nok = new RESTAPI::Checker_Status_Nok;
                nok->id = id;
                nok->error = RESTAPI::NO_REASON;
                res.nok.push_back(nok);
                continue;
            }
            RESTAPI::Checker_Status_Ok *ok = new RESTAPI::Checker_Status_Ok;

            ok->id = id;
            ok->finished = st_res.finished;
            ok->has_error = st_res.has_error;
            if (ok->has_error)
                ok->error_log = st_res.error_log;

            if (st_res.tool)
            {
                if (*st_res.tool == (int)MediaConchLib::report_MediaVeraPdf)
                {
                    ok->tool = new RESTAPI::Report;
                    *ok->tool = RESTAPI::VERAPDF;
                }
                else if (*st_res.tool == (int)MediaConchLib::report_MediaDpfManager)
                {
                    ok->tool = new RESTAPI::Report;
                    *ok->tool = RESTAPI::DPFMANAGER;
                }
            }

            if (st_res.percent)
            {
                ok->percent = new double;
                *ok->percent = *st_res.percent;
            }

            if (st_res.generated_id >= 0)
                ok->generated_id = st_res.generated_id;

            if (st_res.source_id >= 0)
                ok->source_id = st_res.source_id;

            res.ok.push_back(ok);
        }
        std::clog << d->get_date() << "Daemon send checker status result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_report_command(const RESTAPI::Checker_Report_Req* req, RESTAPI::Checker_Report_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a checker_report command: ";
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
        std::vector<long> files;
        for (size_t i = 0; i < req->ids.size(); ++i)
        {
            long id = req->ids[i];
            if (id < 0)
            {
                RESTAPI::Checker_Report_Nok *nok = new RESTAPI::Checker_Report_Nok;
                nok->id = id;
                nok->error = RESTAPI::ID_NOT_EXISTING;
                res.nok.push_back(nok);
                continue;
            }

            MediaConchLib::Checker_StatusRes cs_res;
            int is_done = d->MCL->checker_status(req->user, id, cs_res);
            if (is_done < 0 || !cs_res.finished)
            {
                RESTAPI::Checker_Report_Nok *nok = new RESTAPI::Checker_Report_Nok;
                nok->id = id;
                nok->error = RESTAPI::NOT_READY;
                res.nok.push_back(nok);
                continue;
            }

            files.push_back(id);
        }

        std::map<std::string, std::string> options;
        std::map<std::string, std::string>::const_iterator it = req->options.begin();
        for (; it != req->options.end(); ++it)
        {
            if (!it->first.size())
                continue;

            options[it->first.c_str()] = it->second;
        }

        // Output
        MediaConchLib::Checker_ReportRes result;
        d->MCL->checker_get_report(req->user, report_set, format, files,
                                   req->policies_ids, req->policies_contents,
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
        std::clog << d->get_date() << "Daemon send checker report result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_retry_command(const RESTAPI::Checker_Retry_Req* req, RESTAPI::Checker_Retry_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a checker retry command: ";
        std::clog << req->to_str() << std::endl;
        for (size_t i = 0; i < req->ids.size(); ++i)
        {
            long id = req->ids[i];
            if (id < 0)
            {
                RESTAPI::Checker_Retry_Nok *nok = new RESTAPI::Checker_Retry_Nok;
                nok->id = id;
                nok->error = RESTAPI::ID_NOT_EXISTING;
                res.nok.push_back(nok);
                continue;
            }

            std::string filename;
            d->MCL->checker_file_from_id(req->user, id, filename);
            if (!filename.size())
            {
                RESTAPI::Checker_Retry_Nok *nok = new RESTAPI::Checker_Retry_Nok;
                nok->id = id;
                nok->error = RESTAPI::ID_NOT_EXISTING;
                res.nok.push_back(nok);
                continue;
            }

            std::vector<long> files;
            files.push_back(id);
            d->MCL->remove_report(req->user, files);

            bool registered = false;
            long new_id = -1;
            std::vector<std::string> plugins;
            int ret = d->MCL->checker_analyze(req->user, filename, plugins, d->options, registered, new_id);
            if (ret < 0)
            {
                RESTAPI::Checker_Retry_Nok *nok = new RESTAPI::Checker_Retry_Nok;
                nok->id = id;
                nok->error = RESTAPI::FILE_NOT_EXISTING;
                res.nok.push_back(nok);
                continue;
            }
            res.ok.push_back(new_id);
        }
        std::clog << d->get_date() << "Daemon send checker retry result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_clear_command(const RESTAPI::Checker_Clear_Req* req, RESTAPI::Checker_Clear_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a checker clear command: ";
        std::clog << req->to_str() << std::endl;
        for (size_t i = 0; i < req->ids.size(); ++i)
        {
            int id = req->ids[i];
            if (id < 0)
            {
                RESTAPI::Checker_Clear_Nok *nok = new RESTAPI::Checker_Clear_Nok;
                nok->id = id;
                nok->error = RESTAPI::ID_NOT_EXISTING;
                res.nok.push_back(nok);
                continue;
            }

            std::vector<long> files;
            files.push_back(id);
            d->MCL->remove_report(req->user, files);
            res.ok.push_back(id);
        }

        std::clog << d->get_date() << "Daemon send checker clear result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_list_command(const RESTAPI::Checker_List_Req* req, RESTAPI::Checker_List_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a checker list command: ";
        std::clog << req->to_str() << std::endl;
        std::vector<long> vec;
        d->MCL->checker_list(req->user, vec);
        for (size_t i = 0; i < vec.size(); ++i)
        {
            RESTAPI::Checker_List_File *file = new RESTAPI::Checker_List_File;
            file->id = vec[i];

            d->MCL->checker_file_from_id(req->user, vec[i], file->file);
            res.files.push_back(file);
        }
        std::clog << d->get_date() << "Daemon send checker list result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_validate_command(const RESTAPI::Checker_Validate_Req* req, RESTAPI::Checker_Validate_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a checker validate command: ";
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

        std::vector<long> files;
        for (size_t i = 0; i < req->ids.size(); ++i)
        {
            int id = req->ids[i];
            if (id < 0)
            {
                RESTAPI::Checker_Validate_Nok *nok = new RESTAPI::Checker_Validate_Nok;
                nok->id = id;
                nok->error = RESTAPI::ID_NOT_EXISTING;
                res.nok.push_back(nok);
                continue;
            }

            MediaConchLib::Checker_StatusRes st_res;
            int is_done = d->MCL->checker_status(req->user, id, st_res);
            if (is_done < 0 || !st_res.finished)
            {
                RESTAPI::Checker_Validate_Nok *nok = new RESTAPI::Checker_Validate_Nok;
                nok->id = id;
                nok->error = RESTAPI::NOT_READY;
                res.nok.push_back(nok);
                continue;
            }

            // Output
            files.push_back(id);
        }

        std::map<std::string, std::string> options;
        std::map<std::string, std::string>::const_iterator it = req->options.begin();
        for (; it != req->options.end(); ++it)
        {
            if (!it->first.size())
                continue;

            options[it->first.c_str()] = it->second;
        }

        std::vector<MediaConchLib::Checker_ValidateRes*> result;
        if (d->MCL->checker_validate(req->user, report, files,
                                     req->policies_ids, req->policies_contents,
                                     options, result))
            return -1;

        for (size_t i = 0; i < result.size(); ++i)
        {
            RESTAPI::Checker_Validate_Ok* ok = new RESTAPI::Checker_Validate_Ok;
            ok->id = result[i]->id;
            ok->valid = result[i]->valid;
            res.ok.push_back(ok);
        }
        std::clog << d->get_date() << "Daemon send checker validate result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_file_from_id_command(const RESTAPI::Checker_File_From_Id_Req* req, RESTAPI::Checker_File_From_Id_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a checker_file_from_id command:";
        std::clog << req->to_str() << std::endl;

        d->MCL->checker_file_from_id(req->user, req->id, res.file);

        std::clog << d->get_date() << "Daemon send checker_file_from_id result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_id_from_filename_command(const RESTAPI::Checker_Id_From_Filename_Req* req, RESTAPI::Checker_Id_From_Filename_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a checker_id_from_filename command:";
        std::clog << req->to_str() << std::endl;

        res.id = d->MCL->checker_id_from_filename(req->user, req->filename, req->options);

        std::clog << d->get_date() << "Daemon send checker_id_from_filename result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_file_information_command(const RESTAPI::Checker_File_Information_Req* req, RESTAPI::Checker_File_Information_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a checker_file_information command" << std::endl;
        std::clog << req->to_str() << std::endl;

        MediaConchLib::Checker_FileInfo info;
        d->MCL->checker_file_information(req->user, req->id, info);

        res.filename = info.filename;
        res.file_last_modification = info.file_last_modification;
        res.analyzed = info.analyzed;
        res.generated_id = info.generated_id;
        res.source_id = info.source_id;
        res.generated_time = info.generated_time;
        res.generated_log = info.generated_log;
        res.generated_error_log = info.generated_error_log;
        for (size_t i = 0; i < info.options.size(); ++i)
            res.options.push_back(std::make_pair(info.options[i].first, info.options[i].second));
        res.has_error = info.has_error;
        res.error_log = info.error_log;

        std::clog << d->get_date() << "Daemon send checker_file_information result: " << res.to_str() << std::endl;
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

        std::vector<std::string> values;
        if (d->MCL->policy_get_values_for_type_field(req->type, req->field, values) >= 0)
            for (size_t i = 0; i < values.size(); ++i)
                res.values.push_back(values[i]);

        std::clog << d->get_date() << "Daemon send default_values_for_type result: " << res.to_str() << std::endl;

        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_xslt_policy_create_command(const RESTAPI::XSLT_Policy_Create_Req* req,
                                              RESTAPI::XSLT_Policy_Create_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a xslt_policy_create command: ";
        std::clog << req->to_str() << std::endl;

        std::string err;
        res.id = d->MCL->xslt_policy_create(req->user, err, req->type, req->parent_id);
        if (res.id == -1)
        {
            res.nok = new RESTAPI::Policy_Nok;
            res.nok->error = err;
        }

        std::clog << d->get_date() << "Daemon send xslt_policy_create result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_policy_import_command(const RESTAPI::Policy_Import_Req* req,
                                         RESTAPI::Policy_Import_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a policy_import command: ";
        std::clog << req->to_str() << std::endl;

        std::string err;
        res.id = d->MCL->policy_import(req->user, req->xml, err);
        if (res.id == -1)
        {
            res.nok = new RESTAPI::Policy_Nok;
            res.nok->error = err;
        }

        std::clog << d->get_date() << "Daemon send policy_import result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_policy_remove_command(const RESTAPI::Policy_Remove_Req* req,
                                         RESTAPI::Policy_Remove_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a policy_remove command: ";
        std::clog << req->to_str() << std::endl;

        std::string err;
        if (d->MCL->policy_remove(req->user, req->id, err) == -1)
        {
            res.nok = new RESTAPI::Policy_Nok;
            res.nok->error = err;
        }

        std::clog << d->get_date() << "Daemon send policy_remove result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_policy_dump_command(const RESTAPI::Policy_Dump_Req* req,
                                       RESTAPI::Policy_Dump_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a policy_dump command: ";
        std::clog << req->to_str() << std::endl;

        std::string err;
        if (d->MCL->policy_dump(req->user, req->id, req->must_be_public, res.xml, err) == -1)
        {
            res.nok = new RESTAPI::Policy_Nok;
            res.nok->error = err;
        }

        std::clog << d->get_date() << "Daemon send policy_dump result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_policy_save_command(const RESTAPI::Policy_Save_Req* req,
                                       RESTAPI::Policy_Save_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a policy_save command: ";
        std::clog << req->to_str() << std::endl;

        std::string err;
        if (d->MCL->policy_save(req->user, req->id, err) == -1)
        {
            res.nok = new RESTAPI::Policy_Nok;
            res.nok->error = err;
        }

        std::clog << d->get_date() << "Daemon send policy_save result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_policy_duplicate_command(const RESTAPI::Policy_Duplicate_Req* req,
                                            RESTAPI::Policy_Duplicate_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a policy_duplicate command: ";
        std::clog << req->to_str() << std::endl;

        std::string err;
        res.id = d->MCL->policy_duplicate(req->user, req->id, req->dst_policy_id, req->dst_user, req->must_be_public, err);
        if (res.id == -1)
        {
            res.nok = new RESTAPI::Policy_Nok;
            res.nok->error = err;
        }

        std::clog << d->get_date() << "Daemon send policy_duplicate result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_policy_move_command(const RESTAPI::Policy_Move_Req* req,
                                       RESTAPI::Policy_Move_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a policy_move command: ";
        std::clog << req->to_str() << std::endl;

        std::string err;
        res.id = d->MCL->policy_move(req->user, req->id, req->dst_policy_id, err);
        if (res.id == -1)
        {
            res.nok = new RESTAPI::Policy_Nok;
            res.nok->error = err;
        }

        std::clog << d->get_date() << "Daemon send policy_move result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_policy_change_info_command(const RESTAPI::Policy_Change_Info_Req* req,
                                              RESTAPI::Policy_Change_Info_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a policy_change_info command: ";
        std::clog << req->to_str() << std::endl;

        std::string err;
        if (d->MCL->policy_change_info(req->user, req->id, req->name, req->description, req->license, err) < 0)
        {
            res.nok = new RESTAPI::Policy_Nok;
            res.nok->error = err;
        }

        std::clog << d->get_date() << "Daemon send policy_change_info result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_policy_change_type_command(const RESTAPI::Policy_Change_Type_Req* req,
                                              RESTAPI::Policy_Change_Type_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a policy_change_type command: ";
        std::clog << req->to_str() << std::endl;

        std::string err;
        if (d->MCL->policy_change_type(req->user, req->id, req->type, err) < 0)
        {
            res.nok = new RESTAPI::Policy_Nok;
            res.nok->error = err;
        }

        std::clog << d->get_date() << "Daemon send policy_change_type result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_policy_change_is_public_command(const RESTAPI::Policy_Change_Is_Public_Req* req,
                                                   RESTAPI::Policy_Change_Is_Public_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a policy_change_is_public command: ";
        std::clog << req->to_str() << std::endl;

        std::string err;
        if (d->MCL->policy_change_is_public(req->user, req->id, req->is_public, err) < 0)
        {
            res.nok = new RESTAPI::Policy_Nok;
            res.nok->error = err;
        }

        std::clog << d->get_date() << "Daemon send policy_change_is_public result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_policy_get_command(const RESTAPI::Policy_Get_Req* req,
                                      RESTAPI::Policy_Get_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a policy_get command: ";
        std::clog << req->to_str() << std::endl;

        std::string err;
        MediaConchLib::Get_Policy p;
        if (d->MCL->policy_get(req->user, req->id, req->format, req->must_be_public, p, err) < 0)
        {
            res.nok = new RESTAPI::Policy_Nok;
            res.nok->error = err;
        }
        else if (p.format == "JSTREE" && p.jstree)
            res.policyTree = *p.jstree;
        else if (p.policy)
            res.policy = new MediaConchLib::Policy_Policy(p.policy);

        std::clog << d->get_date() << "Daemon send policy_get result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_policy_get_name_command(const RESTAPI::Policy_Get_Name_Req* req,
                                           RESTAPI::Policy_Get_Name_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a policy_get_name command: ";
        std::clog << req->to_str() << std::endl;

        std::string err;
        if (d->MCL->policy_get_name(req->user, req->id, res.name, err) < 0)
        {
            res.nok = new RESTAPI::Policy_Nok;
            res.nok->error = err;
        }

        std::clog << d->get_date() << "Daemon send policy_get_name result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_policy_get_policies_count_command(const RESTAPI::Policy_Get_Policies_Count_Req* req,
                                                     RESTAPI::Policy_Get_Policies_Count_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a policy_get_policies_count command" << std::endl;

        std::string err;
        res.size = d->MCL->policy_get_policies_count(req->user);

        std::clog << d->get_date() << "Daemon send policy_get_policies_count result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_policy_clear_policies_command(const RESTAPI::Policy_Clear_Policies_Req* req,
                                                 RESTAPI::Policy_Clear_Policies_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a policy_clear_policies command" << std::endl;

        std::string err;
        if (d->MCL->policy_clear_policies(req->user, err) < 0)
        {
            res.nok = new RESTAPI::Policy_Nok;
            res.nok->error = err;
        }

        std::clog << d->get_date() << "Daemon send policy_clear_policies result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_policy_get_policies_command(const RESTAPI::Policy_Get_Policies_Req* req,
                                               RESTAPI::Policy_Get_Policies_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a policy_get_policies command: ";
        std::clog << req->to_str() << std::endl;

        MediaConchLib::Get_Policies policies;
        d->MCL->policy_get_policies(req->user, req->ids, req->format, policies);
        if (req->format == "JSTREE" && policies.jstree)
            res.policiesTree = *policies.jstree;
        else if (policies.policies)
        {
            for (size_t i = 0; i < policies.policies->size(); ++i)
            {
                MediaConchLib::Policy_Policy *ok = new MediaConchLib::Policy_Policy(policies.policies->at(i));
                res.policies.push_back(ok);
            }
        }

        std::clog << d->get_date() << "Daemon send policy_get_policies result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_policy_get_public_policies_command(const RESTAPI::Policy_Get_Public_Policies_Req* req,
                                                      RESTAPI::Policy_Get_Public_Policies_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a policy_get_public_policies command" << std::endl;

        std::vector<MediaConchLib::Policy_Public_Policy*> policies;
        std::string err;
        if (d->MCL->policy_get_public_policies(policies, err) < 0)
        {
            res.nok = new RESTAPI::Policy_Nok;
            res.nok->error = err;
        }
        else
        {
            for (size_t i = 0; i < policies.size(); ++i)
            {
                if (!policies[i])
                    continue;

                RESTAPI::Policy_Public_Policy *policy = new RESTAPI::Policy_Public_Policy;
                policy->id = policies[i]->id;
                policy->user = policies[i]->user;
                policy->name = policies[i]->name;
                policy->description = policies[i]->description;
                policy->license = policies[i]->license;
                res.policies.push_back(policy);
            }
        }

        std::clog << d->get_date() << "Daemon send policy_get_public_policies result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_policy_get_policies_names_list_command(const RESTAPI::Policy_Get_Policies_Names_List_Req* req,
                                                          RESTAPI::Policy_Get_Policies_Names_List_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a policy_get_policies_names_list command: ";
        std::clog << req->to_str() << std::endl;

        std::vector<std::pair<int, std::string> > policies;
        d->MCL->policy_get_policies_names_list(req->user, policies);
        res.policies = policies;

        std::clog << d->get_date() << "Daemon send policy_get_policies_names_list result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_xslt_policy_create_from_file_command(const RESTAPI::XSLT_Policy_Create_From_File_Req* req, RESTAPI::XSLT_Policy_Create_From_File_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a xslt_policy_create_from_file command: ";
        std::clog << req->to_str() << std::endl;

        if (req->id < 0)
        {
            RESTAPI::XSLT_Policy_Create_From_File_Nok *nok = new RESTAPI::XSLT_Policy_Create_From_File_Nok;
            nok->id = req->id;
            nok->error = RESTAPI::ID_NOT_EXISTING;
            res.nok = nok;
        }
        else
        {
            std::string err;
            int pos = d->MCL->xslt_policy_create_from_file(req->user, req->id, err);

            if (pos == -1)
            {
                RESTAPI::XSLT_Policy_Create_From_File_Nok *nok = new RESTAPI::XSLT_Policy_Create_From_File_Nok;
                nok->id = req->id;
                nok->error = RESTAPI::NO_REASON;
                res.nok = nok;
            }
            else
                res.policy_id = pos;
        }

        std::clog << d->get_date() << "Daemon send xslt_policy_create_from_file result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_xslt_policy_rule_create_command(const RESTAPI::XSLT_Policy_Rule_Create_Req* req,
                                                   RESTAPI::XSLT_Policy_Rule_Create_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a xslt_policy_rule_create command: ";
        std::clog << req->to_str() << std::endl;

        std::string err;
        res.id = d->MCL->xslt_policy_rule_create(req->user, req->policy_id, err);
        if (res.id == -1)
        {
            res.nok = new RESTAPI::Policy_Nok;
            res.nok->error = err;
        }

        std::clog << d->get_date() << "Daemon send xslt_policy_rule_create result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_xslt_policy_rule_get_command(const RESTAPI::XSLT_Policy_Rule_Get_Req* req,
                                                RESTAPI::XSLT_Policy_Rule_Get_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a xslt_policy_rule_get command: ";
        std::clog << req->to_str() << std::endl;

        std::string err;
        XsltPolicyRule *rule = d->MCL->xslt_policy_rule_get(req->user, req->policy_id, req->id, err);

        if (!rule)
        {
            res.nok = new RESTAPI::Policy_Nok;
            res.nok->error = err;
        }
        else
        {
            res.rule.id = rule->id;
            res.rule.name = rule->node_name;
            res.rule.tracktype = rule->track_type;
            res.rule.field = rule->field;
            res.rule.scope = rule->scope;
            res.rule.occurrence = rule->occurrence;
            res.rule.ope = rule->ope;
            res.rule.value = rule->value;
        }

        std::clog << d->get_date() << "Daemon send xslt_policy_rule_get result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_xslt_policy_rule_edit_command(const RESTAPI::XSLT_Policy_Rule_Edit_Req* req,
                                                 RESTAPI::XSLT_Policy_Rule_Edit_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a xslt_policy_rule_edit command: ";
        std::clog << req->to_str() << std::endl;

        std::string err;
        XsltPolicyRule rule;

        rule.id = req->rule.id;
        rule.node_name = req->rule.name;
        rule.track_type = req->rule.tracktype;
        rule.field = req->rule.field;
        rule.scope = req->rule.scope;
        rule.occurrence = req->rule.occurrence;
        rule.ope = req->rule.ope;
        rule.value = req->rule.value;

        if (d->MCL->xslt_policy_rule_edit(req->user, req->policy_id, req->rule.id, &rule, err) < 0)
        {
            res.nok = new RESTAPI::Policy_Nok;
            res.nok->error = err;
        }

        std::clog << d->get_date() << "Daemon send xslt_policy_rule_edit result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_xslt_policy_rule_duplicate_command(const RESTAPI::XSLT_Policy_Rule_Duplicate_Req* req,
                                                      RESTAPI::XSLT_Policy_Rule_Duplicate_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a xslt_policy_rule_duplicate command: ";
        std::clog << req->to_str() << std::endl;

        std::string err;
        if ((res.id = d->MCL->xslt_policy_rule_duplicate(req->user, req->policy_id, req->id, req->dst_policy_id, err)) < 0)
        {
            res.nok = new RESTAPI::Policy_Nok;
            res.nok->error = err;
        }

        std::clog << d->get_date() << "Daemon send xslt_policy_rule_duplicate result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_xslt_policy_rule_move_command(const RESTAPI::XSLT_Policy_Rule_Move_Req* req,
                                                 RESTAPI::XSLT_Policy_Rule_Move_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a xslt_policy_rule_move command: ";
        std::clog << req->to_str() << std::endl;

        std::string err;
        if ((res.id = d->MCL->xslt_policy_rule_move(req->user, req->policy_id, req->id, req->dst_policy_id, err)) < 0)
        {
            res.nok = new RESTAPI::Policy_Nok;
            res.nok->error = err;
        }

        std::clog << d->get_date() << "Daemon send xslt_policy_rule_move result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_xslt_policy_rule_delete_command(const RESTAPI::XSLT_Policy_Rule_Delete_Req* req,
                                                   RESTAPI::XSLT_Policy_Rule_Delete_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

        std::clog << d->get_date() << "Daemon received a xslt_policy_rule_delete command: ";
        std::clog << req->to_str() << std::endl;

        std::string err;
        if (d->MCL->xslt_policy_rule_delete(req->user, req->policy_id, req->id, err) < 0)
        {
            res.nok = new RESTAPI::Policy_Nok;
            res.nok->error = err;
        }

        std::clog << d->get_date() << "Daemon send xslt_policy_rule_delete result: " << res.to_str() << std::endl;
        return 0;
    }

    //--------------------------------------------------------------------------
    std::string Daemon::get_date() const
    {
        std::stringstream out;
        std::string time = Core::get_date();
        out << "[" << time << "]";
        return out.str();
    }
}
