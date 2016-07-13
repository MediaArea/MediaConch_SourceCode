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
#include "CLI.h"
#include "CommandLine_Parser.h"
#include "Help.h"
#include <ZenLib/ZtringList.h>
#include <ZenLib/Dir.h>
#include "Common/Policy.h"

#if !defined(WINDOWS)
    #include <unistd.h>
#endif //!defined(WINDOWS)

//****************************************************************************
// Extern
//****************************************************************************

extern ZenLib::Ztring LogFile_FileName;

//---------------------------------------------------------------------------
namespace MediaConch
{

    //**************************************************************************
    // CLI
    //**************************************************************************

    //--------------------------------------------------------------------------
    CLI::CLI() : use_daemon(false), asynchronous(false), force_analyze(false), create_policy_mode(false)
    {
        format = MediaConchLib::format_Text;
    }

    //--------------------------------------------------------------------------
    CLI::~CLI()
    {
    }

    //--------------------------------------------------------------------------
    int CLI::init()
    {
        // If no filenames (and no options)
        if (files.empty())
            return Help_Nothing();

        // If no report selected, use Implementation by default
        if (!report_set.count() && !policies.size())
            report_set.set(MediaConchLib::report_MediaConch);

        // If no Implementation Schema registered, use one by default
        if (!MCL.get_implementation_schema_file().length())
            MCL.create_default_implementation_schema();

        std::string reason;
        if (!MCL.ReportAndFormatCombination_IsValid(files, report_set, display_file,
                                                    format, reason))
        {
            STRINGOUT(ZenLib::Ztring().From_UTF8(reason));
            return -1;
        }

        MCL.set_configuration_file(configuration_file);
        MCL.set_plugins_configuration_file(plugins_configuration_file);
        MCL.init();
        use_daemon = MCL.get_use_daemon();
        return 0;
    }

    //--------------------------------------------------------------------------
    int CLI::parse_args(const std::vector<std::string>& args)
    {
        //Parse command line
        for (size_t pos = 1; pos < args.size(); ++pos)
        {
            //First part of argument (before "=") should be case insensitive
            std::string argument(args[pos]);
            if (!argument.compare(0, 1, "-"))
            {
                size_t egal_pos = argument.find('=');
                if (egal_pos == std::string::npos)
                    egal_pos = argument.size();
                transform(argument.begin(), argument.begin() + egal_pos,
                          argument.begin(), (int(*)(int))tolower); //(int(*)(int)) is a patch for unix
            }

            int ret = Parse(this, argument);
            if (ret == CLI_RETURN_ERROR || ret == CLI_RETURN_FINISH)
                return ret; //no more tasks to do

            if (ret == CLI_RETURN_FILE)
                add_files_recursively(args[pos]);
        }
        return CLI_RETURN_NONE;
    }

    //--------------------------------------------------------------------------
    int CLI::run()
    {
        std::vector<std::string> file_to_report;
        MediaConchLib::report report_kind;

        for (size_t i = 0; i < files.size(); ++i)
        {
            bool registered = false;
            int ret = MCL.analyze(files[i], registered, force_analyze);
            if (ret < 0)
                return ret;

            if (use_daemon && asynchronous && !registered)
            {
                std::stringstream str;
                str << "Registering ";
                str << files[i];
                str << " to analyze";
                STRINGOUT(ZenLib::Ztring().From_UTF8(str.str()));
            }

            int ready = is_ready(i, report_kind);
            if (ready == MediaConchLib::errorHttp_NONE)
                continue;
            else if (ready < 0)
                //TODO: PROBLEM
                return ready;

            if (report_set[MediaConchLib::report_MediaConch] &&
                report_kind > MediaConchLib::report_MediaTrace && report_kind != MediaConchLib::report_Max &&
                files.size() == 1)
            {
                set_report_reset();
                report_set.set(report_kind);
            }
            file_to_report.push_back(files[i]);
        }

        //Ensure to analyze before creating library
        if (create_policy_mode)
            return run_create_policy();

        //Output
        MediaConchLib::ReportRes result;
        std::vector<std::string> policies_contents;
        std::map<std::string, std::string> options;
        options["verbosity"] = MCL.get_implementation_verbosity();
        MCL.get_report(report_set, format, file_to_report, policies,
                       policies_contents, options, &result, &display_file, NULL);
        MediaInfoLib::String report_mi = ZenLib::Ztring().From_UTF8(result.report);

        STRINGOUT(report_mi);
        //Output, in a file if needed
        if (!LogFile_FileName.empty())
            LogFile_Action(report_mi);
        return 0;
    }

    //--------------------------------------------------------------------------
    int CLI::run_create_policy()
    {
        if (files.size() != 1)
        {
            error = "Create a policy only with one file";
            return MediaConchLib::errorHttp_INTERNAL;
        }

        size_t pos = MCL.create_policy_from_file(files[0]);
        Policy *p = NULL;
        std::string policy;
        if (pos == (size_t)-1 || (p = MCL.get_policy(pos)) == NULL || p->dump_schema(policy) < 0)
        {
            error = std::string("Cannot create policy from: ") + files[0];
            return MediaConchLib::errorHttp_INTERNAL;
        }

        MediaInfoLib::String policy_mil = ZenLib::Ztring().From_UTF8(policy);
        STRINGOUT(policy_mil);

        return 0;
    }

    //--------------------------------------------------------------------------
    int CLI::finish()
    {
        MCL.close();
        return 0;
    }

    //--------------------------------------------------------------------------
    void CLI::set_report_set(std::string& report_kind)
    {
        if (report_kind == "MediaConch" || report_kind == "mediaconch")
            report_set.set(MediaConchLib::report_MediaConch);
        if (report_kind == "MediaInfo" || report_kind == "mediainfo")
            report_set.set(MediaConchLib::report_MediaInfo);
        if (report_kind == "MediaTrace" || report_kind == "mediatrace")
            report_set.set(MediaConchLib::report_MediaTrace);
    }

    //--------------------------------------------------------------------------
    void CLI::set_report_reset()
    {
        report_set.reset();
    }

    //--------------------------------------------------------------------------
    int CLI::set_format(MediaConchLib::format f)
    {
        format = f;
        return 0;
    }

    //--------------------------------------------------------------------------
    int CLI::set_format(const std::string& f)
    {
        if (f == "Text" || f == "text")
            return set_format(MediaConchLib::format_Text);
        if (f == "XML" || f == "xml")
            return set_format(MediaConchLib::format_Xml);
        if (f == "MAXML" || f == "maxml")
            return set_format(MediaConchLib::format_MaXml);
        if (f == "JSTREE" || f == "jstree")
            return set_format(MediaConchLib::format_JsTree);
        if (f == "HTML" || f == "html")
            return set_format(MediaConchLib::format_Html);

        return -1;
    }

    //--------------------------------------------------------------------------
    void CLI::add_policy(const std::string& policy)
    {
        policies.push_back(policy);
    }

    //--------------------------------------------------------------------------
    void CLI::set_display_file(const std::string& file)
    {
        display_file = file;
    }

    //--------------------------------------------------------------------------
    void CLI::set_configuration_file(const std::string& file)
    {
        configuration_file = file;
    }

    //--------------------------------------------------------------------------
    void CLI::set_plugins_configuration_file(const std::string& file)
    {
        plugins_configuration_file = file;
    }

    //--------------------------------------------------------------------------
    void CLI::set_implementation_schema_file(const std::string& file)
    {
        MCL.set_implementation_schema_file(file);
    }

    //--------------------------------------------------------------------------
    void CLI::set_implementation_verbosity(const std::string& verbosity)
    {
        MCL.set_implementation_verbosity(verbosity);
    }

    //--------------------------------------------------------------------------
    int CLI::set_compression_mode(const std::string& mode_str)
    {
        MediaConchLib::compression mode;
        if (mode_str == "none")
            mode = MediaConchLib::compression_None;
        else if (mode_str == "zlib")
            mode = MediaConchLib::compression_ZLib;
        else
            return Help();

        MCL.set_compression_mode(mode);
        return 0;
    }

    //--------------------------------------------------------------------------
    int CLI::register_option(const std::string& opt)
    {
        std::string report;
        if (MCL.add_option(opt, report) < 0)
        {
            ZenLib::Ztring str;
            str.From_UTF8(report);
            STRINGOUT(str);
            if (report == "Option not known")
                return CLI_RETURN_ERROR;
            return CLI_RETURN_FINISH;
        }
        return CLI_RETURN_NONE;
    }

    //--------------------------------------------------------------------------
    int CLI::is_ready(size_t i, MediaConchLib::report& report_kind)
    {
        double percent_done = 0;

        int ret = MCL.is_done(files[i], percent_done, report_kind);
        if (use_daemon && asynchronous)
        {
            if (ret == MediaConchLib::errorHttp_NONE)
            {
                std::stringstream str;
                str << "Analyzing " << files[i] << " ; done: " << percent_done  << "%";
                STRINGOUT(ZenLib::Ztring().From_UTF8(str.str()));
                return 1;
            }
            return ret;
        }
        else
        {
            while (ret != MediaConchLib::errorHttp_TRUE)
            {
                if (ret < 0)
                    return ret;
                #ifdef WINDOWS
                ::Sleep((DWORD)5);
                #else
                usleep(500000);
                #endif
                ret = MCL.is_done(files[i], percent_done, report_kind);
            }
        }
        return MediaConchLib::errorHttp_TRUE;
    }

    //--------------------------------------------------------------------------
    void CLI::set_force_analyze(bool force)
    {
        force_analyze = force;
    }

    //--------------------------------------------------------------------------
    void CLI::set_asynchronous(bool async)
    {
        asynchronous = async;
    }

    //--------------------------------------------------------------------------
    void CLI::set_create_policy_mode()
    {
        create_policy_mode = true;
    }

    //--------------------------------------------------------------------------
    void CLI::add_files_recursively(const std::string& filename)
    {
        ZenLib::Ztring dirname = ZenLib::Ztring().From_UTF8(filename);
        if (!ZenLib::Dir::Exists(dirname))
        {
            files.push_back(filename);
            return;
        }

        ZenLib::ZtringList list = ZenLib::Dir::GetAllFileNames(dirname,
                                                               (ZenLib::Dir::dirlist_t)(ZenLib::Dir::Include_Files |
                                                                                        ZenLib::Dir::Include_Hidden |
                                                                                        ZenLib::Dir::Parse_SubDirs));

        for (size_t i =0; i < list.size(); ++i)
            files.push_back(ZenLib::Ztring(list[i]).To_UTF8()); //Append the filename to the list of filenames to parse
    }

    //--------------------------------------------------------------------------
    void CLI::print_error(MediaConchLib::errorHttp code)
    {
        switch (code)
        {
            case MediaConchLib::errorHttp_INVALID_DATA:
                TEXTOUT("Data sent to the daemon is not correct");
                break;
            case MediaConchLib::errorHttp_INIT:
                TEXTOUT("Cannot initialize the HTTP connection");
                break;
            case MediaConchLib::errorHttp_CONNECT:
                TEXTOUT("Cannot connect to the daemon");
                break;
            case MediaConchLib::errorHttp_INTERNAL:
            {
                MediaInfoLib::String error_mil = ZenLib::Ztring().From_UTF8(error);
                STRINGOUT(error_mil);
                break;
            }
            default:
                TEXTOUT("Internal error");
                break;
        }
    }

    //--------------------------------------------------------------------------
    int CLI::get_values_for_type_field(const std::string& type, const std::string& field, std::vector<std::string>& values)
    {
        return MCL.get_values_for_type_field(type, field, values);
    }

}
