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
    std::string Daemon::version = "16.12.0";

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
    int Daemon::init(std::string& err)
    {
        if (MCL->init(err) < 0)
            return -1;

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
        if (httpd->init(err) < 0)
        {
            std::clog << err << std::endl;
            return -1;
        }

        httpd->commands.mediaconch_get_plugins_cb = on_mediaconch_get_plugins_command;
        httpd->commands.mediaconch_watch_folder_cb = on_mediaconch_watch_folder_command;
        httpd->commands.mediaconch_list_watch_folders_cb = on_mediaconch_list_watch_folders_command;
        httpd->commands.mediaconch_edit_watch_folder_cb = on_mediaconch_edit_watch_folder_command;
        httpd->commands.mediaconch_remove_watch_folder_cb = on_mediaconch_remove_watch_folder_command;

        httpd->commands.checker_analyze_cb = on_checker_analyze_command;
        httpd->commands.checker_status_cb = on_checker_status_command;
        httpd->commands.checker_report_cb = on_checker_report_command;
        httpd->commands.checker_clear_cb = on_checker_clear_command;
        httpd->commands.checker_list_cb = on_checker_list_command;
        httpd->commands.checker_validate_cb = on_checker_validate_command;
        httpd->commands.checker_file_from_id_cb = on_checker_file_from_id_command;
        httpd->commands.checker_id_from_filename_cb = on_checker_id_from_filename_command;
        httpd->commands.checker_file_information_cb = on_checker_file_information_command;
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
    int Daemon::run(std::string& err)
    {
        switch (mode)
        {
            case DAEMON_MODE_PLUGINS_LIST:
                return run_plugins_list(err);
            case DAEMON_MODE_DAEMON:
                //default, continue
                break;
        }

        if (is_daemon)
            daemonize();

        if (MCL && watch_folder.size())
        {
            long user_id = -1;
            int ret = MCL->mediaconch_watch_folder(watch_folder, watch_folder_reports, plugins, policies,
                                                   watch_folder_user, watch_folder_recursive, options, user_id, err);
            if (ret < 0)
                std::clog << "Cannot watch folder:" << watch_folder << ":" << err << std::endl;
            else
                std::clog << "Watching folder:" << watch_folder << ", user:" << user_id << std::endl;
        }

        if (!httpd)
        {
            err = "HTTP library not initialized.";
            return -1;
        }

        if (httpd->bind(err) < 0)
        {
            std::clog << httpd->get_error() << std::endl;
            return -1;
        }

        std::clog << get_date() << "Daemon is running on " << httpd->get_address() << ":" << httpd->get_port() << std::endl;
        if (httpd->start(err) < 0)
        {
            std::clog << err << std::endl;
            return -1;
        }

        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::run_plugins_list(std::string& err)
    {
        std::stringstream out;
        std::vector<std::string> list;
        if (MCL->mediaconch_get_plugins(list, err) < 0)
            return DAEMON_RETURN_ERROR;

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

#define FUN_CMD_PROTO(NAME, STRUCT)                                            \
    int Daemon::on_##NAME##_command(const RESTAPI::STRUCT##_Req* req,          \
                                    RESTAPI::STRUCT##_Res& res, void *arg)


#define FUN_CMD_START(CMD)                                              \
    Daemon *d = (Daemon*)arg;                                           \
    if (!d || !req)                                                     \
        return -1;                                                      \
                                                                        \
    std::clog << d->get_date() << "Daemon received "#CMD" command:";    \
    std::clog << req->to_str() << std::endl;

#define FUN_CMD_NOK(OBJ, ERR, ID)                                       \
    {                                                                   \
        OBJ.nok = new RESTAPI::MediaConch_Nok;                          \
        if ((long)ID != -1)                                             \
        {                                                               \
            OBJ.nok->id = new long;                                     \
            *OBJ.nok->id = ID;                                          \
        }                                                               \
        OBJ.nok->error = ERR;                                           \
    }

#define FUN_CMD_NOK_ARR(OBJ, ERR, ID)                                   \
    {                                                                   \
        RESTAPI::MediaConch_Nok *nok = new RESTAPI::MediaConch_Nok;     \
        if ((long)ID != -1)                                             \
        {                                                               \
            nok->id = new long;                                         \
            *nok->id = ID;                                              \
        }                                                               \
        nok->error = ERR;                                               \
        OBJ.nok.push_back(nok);                                         \
        continue;                                                       \
    }

#define FUN_CMD_END(CMD)                                                                      \
    std::clog << d->get_date() << "Daemon send "#CMD" result: " << res.to_str() << std::endl; \
    return 0;

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(mediaconch_get_plugins, MediaConch_Get_Plugins)
    {
        FUN_CMD_START(MediaConch_Get_Plugins)

        std::vector<std::string> vec;
        std::string err;
        if (d->MCL->mediaconch_get_plugins(vec, err) < 0)
            FUN_CMD_NOK(res, err, -1)
        else
        {
            for (size_t i = 0; i < vec.size(); ++i)
                res.plugins.push_back(vec[i]);
        }

        FUN_CMD_END(MediaConch_Get_Plugins)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(mediaconch_watch_folder, MediaConch_Watch_Folder)
    {
        FUN_CMD_START(MediaConch_Watch_Folder)

        std::string err;
        long user_id = -1;
        if (d->MCL->mediaconch_watch_folder(req->folder, req->folder_reports,
                                            req->plugins, req->policies,
                                            req->user, req->recursive, req->options, user_id, err) < 0)
            FUN_CMD_NOK(res, err, -1)
        else
            res.user = user_id;

        FUN_CMD_END(MediaConch_Watch_Folder)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(mediaconch_list_watch_folders, MediaConch_List_Watch_Folders)
    {
        FUN_CMD_START(MediaConch_List_Watch_Folders)

        std::string err;
        std::vector<std::string> list;
        if (d->MCL->mediaconch_list_watch_folders(list, err) < 0)
            FUN_CMD_NOK(res, err, -1)
        else
            for (size_t i = 0; i < list.size(); ++i)
                res.folders.push_back(list[i]);

        FUN_CMD_END(MediaConch_List_Watch_Folders)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(mediaconch_edit_watch_folder, MediaConch_Edit_Watch_Folder)
    {
        FUN_CMD_START(MediaConch_Edit_Watch_Folder)

        std::string err;
        if (d->MCL->mediaconch_edit_watch_folder(req->folder, req->folder_reports, err) < 0)
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(MediaConch_Edit_Watch_Folder)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(mediaconch_remove_watch_folder, MediaConch_Remove_Watch_Folder)
    {
        FUN_CMD_START(MediaConch_Remove_Watch_Folder)

        std::string err;
        if (d->MCL->mediaconch_remove_watch_folder(req->folder, err) < 0)
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(MediaConch_Remove_Watch_Folder)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(checker_analyze, Checker_Analyze)
    {
        FUN_CMD_START(Checker_Analyze)

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
            std::string err;
            int ret = d->MCL->checker_analyze(req->args[i].user, req->args[i].file, plugins, options, registered, out_id,
                                              err, force, req->args[i].mil_analyze);
            if (ret < 0)
                FUN_CMD_NOK_ARR(res, err, req->args[i].id)

            RESTAPI::Checker_Analyze_Ok *ok = new RESTAPI::Checker_Analyze_Ok;
            ok->inId = req->args[i].id;
            ok->outId = out_id;
            ok->create = !registered;

            res.ok.push_back(ok);
        }

        FUN_CMD_END(Checker_Analyze)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(checker_status, Checker_Status)
    {
        FUN_CMD_START(Checker_Status)

        for (size_t i = 0; i < req->ids.size(); ++i)
        {
            long id = req->ids[i];
            if (id < 0)
                FUN_CMD_NOK_ARR(res, "ID not existing", id)

            MediaConchLib::Checker_StatusRes st_res;
            std::string err;
            int ret = d->MCL->checker_status(req->user, id, st_res, err);
            if (ret < 0)
                FUN_CMD_NOK_ARR(res, err, id)

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

        FUN_CMD_END(Checker_Status)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(checker_report, Checker_Report)
    {
        FUN_CMD_START(Checker_Report)

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
        for (size_t j = 0; j < req->reports.size(); ++j)
        {
            if (req->reports[j] == RESTAPI::MEDIAINFO)
                report_set.set(MediaConchLib::report_MediaInfo);
            if (req->reports[j] == RESTAPI::MEDIATRACE)
                report_set.set(MediaConchLib::report_MediaTrace);
            if (req->reports[j] == RESTAPI::IMPLEMENTATION)
                report_set.set(MediaConchLib::report_MediaConch);
            if (req->reports[j] == RESTAPI::VERAPDF)
                report_set.set(MediaConchLib::report_MediaVeraPdf);
            if (req->reports[j] == RESTAPI::DPFMANAGER)
                report_set.set(MediaConchLib::report_MediaDpfManager);
        }

        if (!report_set.count() && !req->policies_ids.size() && !req->policies_contents.size())
            report_set.set(MediaConchLib::report_MediaConch);

        std::vector<long> files;
        for (size_t i = 0; i < req->ids.size(); ++i)
        {
            long id = req->ids[i];
            if (id < 0)
            {
                FUN_CMD_NOK(res, "ID not existing", -1)
                FUN_CMD_END(Checker_Report)
            }

            MediaConchLib::Checker_StatusRes cs_res;
            std::string err;
            if (d->MCL->checker_status(req->user, id, cs_res, err) < 0)
            {
                FUN_CMD_NOK(res, err, -1)
                FUN_CMD_END(Checker_Report)
            }

            if (!cs_res.finished)
            {
                FUN_CMD_NOK(res, "Some files are still analyzing", id)
                FUN_CMD_END(Checker_Report)
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
        std::string err;
        if (d->MCL->checker_get_report(req->user, report_set, format, files,
                                       req->policies_ids, req->policies_contents,
                                       options, &result, err, display_name, display_content) < 0)
            FUN_CMD_NOK(res, err, -1)
        else
        {
            res.ok = new RESTAPI::Checker_Report_Ok;
            res.ok->report = result.report;
            res.ok->has_valid = result.has_valid;
            res.ok->valid = result.valid;
        }

        FUN_CMD_END(Checker_Report)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(checker_clear, Checker_Clear)
    {
        FUN_CMD_START(Checker_Clear)

        for (size_t i = 0; i < req->ids.size(); ++i)
        {
            int id = req->ids[i];
            if (id < 0)
                FUN_CMD_NOK_ARR(res, "ID not existing", id)

            std::vector<long> files;
            files.push_back(id);

            std::string err;
            if (d->MCL->remove_report(req->user, files, err) < 0)
                FUN_CMD_NOK_ARR(res, err, id)

            res.ok.push_back(id);
        }

        FUN_CMD_END(Checker_Clear)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(checker_list, Checker_List)
    {
        FUN_CMD_START(Checker_List)

        std::vector<long> vec;
        std::string err;
        if (d->MCL->checker_list(req->user, vec, err) < 0)
            FUN_CMD_NOK(res, err, -1)

        for (size_t i = 0; i < vec.size(); ++i)
        {
            RESTAPI::Checker_List_File *file = new RESTAPI::Checker_List_File;
            file->id = vec[i];

            d->MCL->checker_file_from_id(req->user, vec[i], file->file, err);
            res.files.push_back(file);
        }

        FUN_CMD_END(Checker_List)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(checker_validate, Checker_Validate)
    {
        FUN_CMD_START(Checker_Validate)

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
                FUN_CMD_NOK(res, "ID not existing", id)
                FUN_CMD_END(Checker_Validate)
            }

            MediaConchLib::Checker_StatusRes st_res;
            std::string err;
            if (d->MCL->checker_status(req->user, id, st_res, err) < 0)
            {
                FUN_CMD_NOK(res, err, id)
                FUN_CMD_END(Checker_Validate)
            }
            if (!st_res.finished)
            {
                FUN_CMD_NOK(res, "File still analyzing", id)
                FUN_CMD_END(Checker_Validate)
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
        std::string err;
        if (d->MCL->checker_validate(req->user, report, files,
                                     req->policies_ids, req->policies_contents,
                                     options, result, err) < 0)
        {
            FUN_CMD_NOK(res, err, -1)
            FUN_CMD_END(Checker_Validate)
        }

        for (size_t i = 0; i < result.size(); ++i)
        {
            RESTAPI::Checker_Validate_Ok* ok = new RESTAPI::Checker_Validate_Ok;
            ok->id = result[i]->id;
            ok->valid = result[i]->valid;
            res.ok.push_back(ok);
        }

        FUN_CMD_END(Checker_Validate)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(checker_file_from_id, Checker_File_From_Id)
    {
        FUN_CMD_START(Checker_File_From_Id)

        std::string err;
        if (d->MCL->checker_file_from_id(req->user, req->id, res.file, err) < 0)
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(Checker_File_From_Id)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(checker_id_from_filename, Checker_Id_From_Filename)
    {
        FUN_CMD_START(Checker_Id_From_Filename)

        std::string err;
        if ((res.id = d->MCL->checker_id_from_filename(req->user, req->filename, req->options, err)) < 0)
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(Checker_Id_From_Filename)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(checker_file_information, Checker_File_Information)
    {
        FUN_CMD_START(Checker_File_Information)

        MediaConchLib::Checker_FileInfo info;
        std::string err;
        if (d->MCL->checker_file_information(req->user, req->id, info, err) < 0)
            FUN_CMD_NOK(res, err, -1)
        else
        {
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
        }

        FUN_CMD_END(Checker_File_Information)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(default_values_for_type, Default_Values_For_Type)
    {
        FUN_CMD_START(Default_Values_For_Type)

        std::vector<std::string> values;
        std::string err;
        if (d->MCL->policy_get_values_for_type_field(req->type, req->field, values, err) < 0)
            FUN_CMD_NOK(res, err, -1)
        else
        {
            for (size_t i = 0; i < values.size(); ++i)
                res.values.push_back(values[i]);
        }

        FUN_CMD_END(Default_Values_For_Type)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(xslt_policy_create, XSLT_Policy_Create)
    {
        FUN_CMD_START(XSLT_Policy_Create)

        std::string err;
        res.id = d->MCL->xslt_policy_create(req->user, err, req->type, req->parent_id);
        if (res.id == -1)
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(XSLT_Policy_Create)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(policy_import, Policy_Import)
    {
        FUN_CMD_START(Policy_Import)

        std::string err;
        res.id = d->MCL->policy_import(req->user, req->xml, err);
        if (res.id == -1)
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(Policy_Import)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(policy_remove, Policy_Remove)
    {
        FUN_CMD_START(Policy_Remove)

        std::string err;
        if (d->MCL->policy_remove(req->user, req->id, err) < 0)
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(Policy_Remove)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(policy_dump, Policy_Dump)
    {
        FUN_CMD_START(Policy_Dump)

        std::string err;
        if (d->MCL->policy_dump(req->user, req->id, req->must_be_public, res.xml, err) < 0)
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(Policy_Dump)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(policy_save, Policy_Save)
    {
        FUN_CMD_START(Policy_Save)

        std::string err;
        if (d->MCL->policy_save(req->user, req->id, err) < 0)
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(Policy_Save)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(policy_duplicate, Policy_Duplicate)
    {
        FUN_CMD_START(Policy_Duplicate)

        std::string err;
        res.id = d->MCL->policy_duplicate(req->user, req->id, req->dst_policy_id, req->dst_user, req->must_be_public, err);
        if (res.id == -1)
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(Policy_Duplicate)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(policy_move, Policy_Move)
    {
        FUN_CMD_START(Policy_Move)

        std::string err;
        res.id = d->MCL->policy_move(req->user, req->id, req->dst_policy_id, err);
        if (res.id == -1)
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(Policy_Move)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(policy_change_info, Policy_Change_Info)
    {
        FUN_CMD_START(Policy_Change_Info)

        std::string err;
        if (d->MCL->policy_change_info(req->user, req->id, req->name, req->description, req->license, err) < 0)
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(Policy_Change_Info)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(policy_change_type, Policy_Change_Type)
    {
        FUN_CMD_START(Policy_Change_Type)

        std::string err;
        if (d->MCL->policy_change_type(req->user, req->id, req->type, err) < 0)
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(Policy_Change_Type)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(policy_change_is_public, Policy_Change_Is_Public)
    {
        FUN_CMD_START(Policy_Change_Is_Public)

        std::string err;
        if (d->MCL->policy_change_is_public(req->user, req->id, req->is_public, err) < 0)
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(Policy_Change_Is_Public)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(policy_get, Policy_Get)
    {
        FUN_CMD_START(Policy_Get)

        std::string err;
        MediaConchLib::Get_Policy p;
        if (d->MCL->policy_get(req->user, req->id, req->format, req->must_be_public, p, err) < 0)
            FUN_CMD_NOK(res, err, -1)
        else if (p.format == "JSTREE" && p.jstree)
            res.policyTree = *p.jstree;
        else if (p.policy)
            res.policy = new MediaConchLib::Policy_Policy(p.policy);

        FUN_CMD_END(Policy_Get)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(policy_get_name, Policy_Get_Name)
    {
        FUN_CMD_START(Policy_Get_Name)

        std::string err;
        if (d->MCL->policy_get_name(req->user, req->id, res.name, err) < 0)
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(Policy_Get_Name)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(policy_get_policies_count, Policy_Get_Policies_Count)
    {
        FUN_CMD_START(Policy_Get_Policies_Count)

        std::string err;
        if ((int)(res.size = d->MCL->policy_get_policies_count(req->user, err)) < 0)
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(Policy_Get_Policies_Count)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(policy_clear_policies, Policy_Clear_Policies)
    {
        FUN_CMD_START(Policy_Clear_Policies)

        std::string err;
        if (d->MCL->policy_clear_policies(req->user, err) < 0)
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(Policy_Clear_Policies)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(policy_get_policies, Policy_Get_Policies)
    {
        FUN_CMD_START(Policy_Get_Policies)

        MediaConchLib::Get_Policies policies;
        std::string err;
        if (d->MCL->policy_get_policies(req->user, req->ids, req->format, policies, err) < 0)
            FUN_CMD_NOK(res, err, -1)
        else if (req->format == "JSTREE" && policies.jstree)
            res.policiesTree = *policies.jstree;
        else if (policies.policies)
        {
            for (size_t i = 0; i < policies.policies->size(); ++i)
            {
                MediaConchLib::Policy_Policy *ok = new MediaConchLib::Policy_Policy(policies.policies->at(i));
                res.policies.push_back(ok);
            }
        }

        FUN_CMD_END(Policy_Get_Policies)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(policy_get_public_policies, Policy_Get_Public_Policies)
    {
        FUN_CMD_START(Policy_Get_Public_Policies)

        std::vector<MediaConchLib::Policy_Public_Policy*> policies;
        std::string err;
        if (d->MCL->policy_get_public_policies(policies, err) < 0)
            FUN_CMD_NOK(res, err, -1)
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

        FUN_CMD_END(Policy_Get_Public_Policies)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(policy_get_policies_names_list, Policy_Get_Policies_Names_List)
    {
        FUN_CMD_START(Policy_Get_Policies_Names_List)

        std::vector<std::pair<int, std::string> > policies;
        std::string err;
        if (d->MCL->policy_get_policies_names_list(req->user, policies, err) < 0)
            FUN_CMD_NOK(res, err, -1)
        else
            res.policies = policies;

        FUN_CMD_END(Policy_Get_Policies_Names_List)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(xslt_policy_create_from_file, XSLT_Policy_Create_From_File)
    {
        FUN_CMD_START(XSLT_Policy_Create_From_File)

        if (req->id < 0)
            FUN_CMD_NOK(res, "ID not existing", -1)
        else
        {
            std::string err;
            res.policy_id = d->MCL->xslt_policy_create_from_file(req->user, req->id, err);

            if (res.policy_id < 0)
                FUN_CMD_NOK(res, err, req->id)
        }

        FUN_CMD_END(XSLT_Policy_Create_From_File)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(xslt_policy_rule_create, XSLT_Policy_Rule_Create)
    {
        FUN_CMD_START(XSLT_Policy_Rule_Create)

        std::string err;
        res.id = d->MCL->xslt_policy_rule_create(req->user, req->policy_id, err);
        if (res.id == -1)
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(XSLT_Policy_Rule_Create)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(xslt_policy_rule_get, XSLT_Policy_Rule_Get)
    {
        FUN_CMD_START(XSLT_Policy_Rule_Get)

        std::string err;
        XsltPolicyRule *rule = d->MCL->xslt_policy_rule_get(req->user, req->policy_id, req->id, err);

        if (!rule)
            FUN_CMD_NOK(res, err, -1)
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

        FUN_CMD_END(XSLT_Policy_Rule_Get)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(xslt_policy_rule_edit, XSLT_Policy_Rule_Edit)
    {
        FUN_CMD_START(XSLT_Policy_Rule_Edit)

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
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(XSLT_Policy_Rule_Edit)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(xslt_policy_rule_duplicate, XSLT_Policy_Rule_Duplicate)
    {
        FUN_CMD_START(XSLT_Policy_Rule_Duplicate)

        std::string err;
        if ((res.id = d->MCL->xslt_policy_rule_duplicate(req->user, req->policy_id, req->id, req->dst_policy_id, err)) < 0)
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(XSLT_Policy_Rule_Duplicate)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(xslt_policy_rule_move, XSLT_Policy_Rule_Move)
    {
        FUN_CMD_START(XSLT_Policy_Rule_Move)

        std::string err;
        if ((res.id = d->MCL->xslt_policy_rule_move(req->user, req->policy_id, req->id, req->dst_policy_id, err)) < 0)
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(XSLT_Policy_Rule_Move)
    }

    //--------------------------------------------------------------------------
    FUN_CMD_PROTO(xslt_policy_rule_delete, XSLT_Policy_Rule_Delete)
    {
        FUN_CMD_START(XSLT_Policy_Rule_Delete)

        std::string err;
        if (d->MCL->xslt_policy_rule_delete(req->user, req->policy_id, req->id, err) < 0)
            FUN_CMD_NOK(res, err, -1)

        FUN_CMD_END(XSLT_Policy_Rule_Delete)
    }

#undef FUN_CMD_PROTO
#undef FUN_CMD_START
#undef FUN_CMD_END
#undef FUN_CMD_NOK
#undef FUN_CMD_NOK_ARR
    //--------------------------------------------------------------------------
    std::string Daemon::get_date() const
    {
        std::stringstream out;
        std::string time = Core::get_date();
        out << "[" << time << "]";
        return out.str();
    }
}
