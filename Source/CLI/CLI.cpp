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
#include <ZenLib/File.h>
#include <ZenLib/Dir.h>

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
    CLI::CLI() : use_daemon(false), asynchronous(false), force_analyze(false), create_policy_mode(false), file_information(false)
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
        std::vector<long> id_to_report;
        MediaConchLib::report report_kind;

        //Return file information
        if (file_information)
            return run_file_information();

        for (size_t i = 0; i < files.size(); ++i)
        {
            bool registered = false;
            long file_id = -1;
            int ret = MCL.checker_analyze(-1, files[i], registered, file_id, force_analyze);
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

            int ready = is_ready(file_id, report_kind);
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
            id_to_report.push_back(file_id);
        }

        //Ensure to analyze before creating library
        if (create_policy_mode)
            return run_create_policy(id_to_report);

        std::map<std::string, std::string> options;
        // if compare two files
        if (policy_reference_file.size())
        {
            bool registered = false;
            long file_id;
            int ret = MCL.checker_analyze(-1, policy_reference_file, registered, file_id, force_analyze);
            if (ret < 0)
                return ret;
            if ((ret = run_policy_reference_file(file_id)) != MediaConchLib::errorHttp_TRUE)
                return ret;

            std::stringstream ss;
            ss << file_id;
            options["policy_reference_id"] = ss.str();
        }

        //Output
        MediaConchLib::Checker_ReportRes result;
        std::vector<size_t> policies_ids;
        options["verbosity"] = MCL.get_implementation_verbosity();
        MCL.checker_get_report(-1, report_set, format, id_to_report, policies_ids,
                               policies, options, &result, &display_file, NULL);
        MediaInfoLib::String report_mi = ZenLib::Ztring().From_UTF8(result.report);

        STRINGOUT(report_mi);
        //Output, in a file if needed
        if (!LogFile_FileName.empty())
            LogFile_Action(report_mi);
        return 0;
    }

    //--------------------------------------------------------------------------
    int CLI::run_create_policy(const std::vector<long>& files_ids)
    {
        if (files_ids.size() != 1)
        {
            error = "Create a policy only with one file";
            return MediaConchLib::errorHttp_INTERNAL;
        }

        size_t pos = MCL.xslt_policy_create_from_file(-1, files_ids[0], error);
        if (pos == (size_t)-1)
            return MediaConchLib::errorHttp_INTERNAL;

        std::string policy;
        if (MCL.policy_dump(-1, pos, policy, error))
            return MediaConchLib::errorHttp_INTERNAL;

        MediaInfoLib::String policy_mil = ZenLib::Ztring().From_UTF8(policy);
        STRINGOUT(policy_mil);

        return 0;
    }

    //--------------------------------------------------------------------------
    int CLI::run_file_information()
    {
        for  (size_t i = 0; i < files.size(); ++i)
        {
            long id = MCL.checker_id_from_filename(-1, files[i]);
            if (id < 0)
            {
                error = "File is not registered";
                return MediaConchLib::errorHttp_INTERNAL;
            }

            MediaConchLib::Checker_FileInfo info;
            int ret;
            if ((ret = MCL.checker_file_information(-1, id, info)) < 0)
                return ret;

            std::string report;
            file_info_report(&info, report);
            MediaInfoLib::String info_str = ZenLib::Ztring().From_UTF8(report);
            STRINGOUT(info_str);
        }
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
        if (report_kind == "MicroMediaTrace" || report_kind == "micromediatrace")
            report_set.set(MediaConchLib::report_MicroMediaTrace);
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
    int CLI::add_policy(const std::string& filename)
    {
        ZenLib::Ztring z_filename = ZenLib::Ztring().From_UTF8(filename);
        if (!ZenLib::File::Exists(z_filename))
            return -1;

        ZenLib::File file(z_filename);

        ZenLib::int64u size = file.Size_Get();
        if (size == (ZenLib::int64u)-1)
            return -1;

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
        return 0;
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
    int CLI::set_policy_reference_file(const std::string& file)
    {
        policy_reference_file = file;
        return 0;
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
    void CLI::set_file_information_mode()
    {
        file_information = true;
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
    int CLI::is_ready(long& file_id, MediaConchLib::report& report_kind)
    {
        MediaConchLib::Checker_StatusRes res;
        int ret = MCL.checker_status(-1, file_id, res);
        if (ret < 0)
            return ret;

        report_kind = MediaConchLib::report_MediaConch;

        if (use_daemon && asynchronous)
        {
            if (!res.finished)
            {
                std::string file;
                MCL.checker_file_from_id(-1, file_id, file);

                std::stringstream str;
                double percent = res.percent ? *res.percent : (double)0;
                str << "Analyzing " << file << " ; done: " << percent << "%";
                STRINGOUT(ZenLib::Ztring().From_UTF8(str.str()));
                return 1;
            }

            if (res.tool)
                report_kind = (MediaConchLib::report)*res.tool;

            return ret;
        }
        else
        {
            while (!res.finished)
            {

                #ifdef WINDOWS
                ::Sleep((DWORD)5);
                #else
                usleep(500000);
                #endif
                ret = MCL.checker_status(-1, file_id, res);
            }

            if (res.has_error)
            {
                std::stringstream ss;
                std::string file;
                MCL.checker_file_from_id(-1, file_id, file);

                ss << "File: " << file << " had a problem during analyze\n";
                ss << "\tError logs are: " << res.error_log;
                error = ss.str();
                return MediaConchLib::errorHttp_INTERNAL;
            }

            if (res.tool)
                report_kind = (MediaConchLib::report)*res.tool;

            if (res.generated_id >= 0)
            {
                file_id = res.generated_id;
                return is_ready(file_id, report_kind);
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
            case MediaConchLib::errorHttp_DAEMON_RESTART:
                TEXTOUT("Daemon has restarted, try again");
                break;
            default:
                TEXTOUT("Internal error");
                break;
        }
    }

    //--------------------------------------------------------------------------
    int CLI::get_values_for_type_field(const std::string& type, const std::string& field, std::vector<std::string>& values)
    {
        return MCL.policy_get_values_for_type_field(type, field, values);
    }

    int CLI::run_policy_reference_file(long id)
    {
        MediaConchLib::report report_kind;
        return is_ready(id, report_kind);
    }

    void CLI::file_info_report(const MediaConchLib::Checker_FileInfo* info, std::string& report)
    {
        std::stringstream ss;

        ss << "filename:               " << info->filename << "\n";
        ss << "file last modification: " << info->file_last_modification << "\n";
        ss << "analyzed:               " << std::boolalpha << info->analyzed << "\n";
        ss << "has_error:              " << std::boolalpha << info->has_error << "\n";
        if (info->has_error)
            ss << "error_log:          " << std::boolalpha << info->error_log << "\n";

        if (info->generated_id >= 0)
        {
            std::string file;
            MCL.checker_file_from_id(-1, info->generated_id, file);
            ss << "generated file:         " << file << "\n";
        }

        if (info->source_id >= 0)
        {
            std::string file;
            MCL.checker_file_from_id(-1, info->source_id, file);
            ss << "source file:            " << file << "\n";
            ss << "file generation time:   " << info->generated_time << " milliseconds\n";
            ss << "generated log:          " << info->generated_log << "\n";
            ss << "generated error log:    " << info->generated_error_log << "\n";
        }

        report = ss.str();
    }

}
