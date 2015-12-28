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
#include <ZenLib/Ztring.h>
#include "Common/Httpd.h"
#include "Common/LibEventHttpd.h"
#include "Daemon.h"
#include "Help.h"
#include "Config.h"

#include <unistd.h>
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

    //--------------------------------------------------------------------------
    Daemon::Daemon() : is_daemon(true), httpd(NULL)
    {
        MCL = new MediaConchLib(true);
    }

    //--------------------------------------------------------------------------
    Daemon::~Daemon()
    {
        delete MCL;
    }

    //--------------------------------------------------------------------------
    int Daemon::init()
    {
        MCL->init();
        httpd = new LibEventHttpd(this);
        int port = -1;
        std::string address;
        MCL->get_daemon_address(address, port);
        if (address.length())
            httpd->set_address(address);
        if (port != -1)
            httpd->set_port(port);
        httpd->init();

        httpd->commands.analyze_cb = on_analyze_command;
        httpd->commands.status_cb = on_status_command;
        httpd->commands.report_cb = on_report_command;
        httpd->commands.retry_cb = on_retry_command;
        httpd->commands.clear_cb = on_clear_command;
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
            if (ret < 0)
                return -1;
        }
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::run()
    {
        if (is_daemon)
            daemonize();
        if (!httpd)
            return -1;

        return httpd->start();
    }

    //--------------------------------------------------------------------------
    int Daemon::finish()
    {
        if (httpd)
            httpd->finish();
        MCL->close();
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::parse(std::string& argument)
    {
#define LAUNCH(_METHOD)                             \
        {                                           \
            int ret = parse_##_METHOD(argument);    \
            if (ret < 0)                            \
                return ret;                         \
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
        if (argument=="-n")
            argument="--fork=No";

        if (argument=="-c")
        {
            last_argument="--configuration=";
            return 0;
        }

        if (0);
        OPTION("--help",          help)
        OPTION("--fork",          fork)
        OPTION("--configuration", configuration)
        OPTION("--",              other)
        else
            return Help();
        return 0;

#undef OPTION
#undef LAUNCH
    }

    //--------------------------------------------------------------------------
    int Daemon::parse_fork(const std::string& argument)
    {
        size_t equal_pos = argument.find('=');
        if (equal_pos == std::string::npos)
            return 0;
        std::string not_daemon = argument.substr(equal_pos + 1);
        if (not_daemon == "no" || not_daemon == "No")
            is_daemon = false;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::parse_help(const std::string& argument)
    {
        (void)argument;
        Help();
        return -1;
    }

    //--------------------------------------------------------------------------
    int Daemon::parse_configuration(const std::string& argument)
    {
        size_t equal_pos = argument.find('=');
        if (equal_pos == std::string::npos)
            return 0;

        std::string file = argument.substr(equal_pos + 1);
        MCL->set_configuration_file(file);
        return 0;
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
            return -1;
        }
        return 0;
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

        for (size_t i = 0; i < req->args.size(); ++i)
        {
            bool registered = false;
            int ret = d->MCL->analyze(req->args[i].file, registered);
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
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_status_command(const RESTAPI::Status_Req* req, RESTAPI::Status_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

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

            RESTAPI::Status_Ok *ok = new RESTAPI::Status_Ok;
            double percent_done = 0.0;
            bool is_done = d->MCL->is_done(*d->current_files[id], percent_done);
            ok->id = id;
            ok->finished = is_done;
            if (!is_done)
            {
                ok->has_percent = true;
                ok->done = percent_done;
            }
            res.ok.push_back(ok);
        }
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_report_command(const RESTAPI::Report_Req* req, RESTAPI::Report_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

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
        }

        if (!report_set.count() && !has_policy)
            return -1;

        bool has_valid = false;
        bool valid = true;
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

            double percent_done = 0.0;
            bool is_done = d->MCL->is_done(*d->current_files[id], percent_done);
            if (!is_done)
            {
                RESTAPI::Report_Nok *nok = new RESTAPI::Report_Nok;
                nok->id = id;
                nok->error = RESTAPI::NOT_READY;
                res.nok.push_back(nok);
                continue;
            }

            // Output
            std::vector<std::string> files;
            files.push_back(*d->current_files[id]);

            MediaConchLib::ReportRes result;
            d->MCL->get_report(report_set, format, files,
                              req->policies_names, req->policies_contents,
                              &result, display_name, display_content);
            res.ok.report = result.report;
            if (result.has_valid)
            {
                has_valid = true;
                if (!result.valid)
                    valid = false;
            }
        }
        res.ok.has_valid = has_valid;
        res.ok.valid = valid;
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_retry_command(const RESTAPI::Retry_Req* req, RESTAPI::Retry_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

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
        return 0;
    }

    //--------------------------------------------------------------------------
    int Daemon::on_clear_command(const RESTAPI::Clear_Req* req, RESTAPI::Clear_Res& res, void *arg)
    {
        Daemon *d = (Daemon*)arg;

        if (!d || !req)
            return -1;

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

    bool Daemon::id_is_existing(int id) const
    {
        if (id < 0 || id >= (int)current_files.size() || !current_files[id])
            return false;
        return true;
    }
}
