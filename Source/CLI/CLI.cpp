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
    CLI::CLI() : watch_folder_user(NULL), use_as_user(-1), use_daemon(false), asynchronous(false),
                 force_analyze(false), mil_analyze(true),
                 watch_folder_recursive(true), create_policy_mode(false), file_information(false),
                 plugins_list_mode(false), list_watch_folders_mode(false), no_needs_files_mode(false)
    {
        format = MediaConchLib::format_Text;
    }

    //--------------------------------------------------------------------------
    CLI::~CLI()
    {
    }

    //--------------------------------------------------------------------------
    int CLI::init(std::string& err)
    {
        if (!no_needs_files_mode)
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

            if (!MCL.ReportAndFormatCombination_IsValid(files, report_set, display_file,
                                                        format, err))
            {
                if (err == "MicroMediaTrace requires an XML output.")
                    err += " Add -fx.";
                return -1;
            }
        }

        MCL.set_configuration_file(configuration_file);
        MCL.set_plugins_configuration_file(plugins_configuration_file);

        if (MCL.init(err) < 0)
            return -1;

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
    int CLI::run(std::string& err)
    {
        std::vector<long> id_to_report;
        MediaConchLib::report report_kind;

        //Return plugins list
        if (plugins_list_mode)
            return run_plugins_list(err);
        //Return watch folders list
        else if (list_watch_folders_mode)
            return run_watch_folders_list(err);
        // Add a watch folder
        else if (watch_folder.size())
            return run_watch_folder_cmd(err);

        //Return file information
        if (file_information)
            return run_file_information(err);

        for (size_t i = 0; i < files.size(); ++i)
        {
            bool registered = false;
            long file_id = -1;
            int ret = MCL.checker_analyze(use_as_user, files[i], plugins, options, registered,
                                          file_id, err, force_analyze, mil_analyze);
            if (ret < 0)
                return -1;

            if (use_daemon && asynchronous && !registered)
            {
                std::stringstream str;
                str << "Registering ";
                str << files[i];
                str << " to analyze";
                STRINGOUT(ZenLib::Ztring().From_UTF8(str.str()));
            }

            int ready = is_ready(file_id, report_kind, err);
            if (ready == MediaConchLib::errorHttp_NONE)
                continue;
            else if (ready < 0)
                //TODO: PROBLEM
                return -1;

            if (report_set[MediaConchLib::report_MediaConch] &&
                report_kind > MediaConchLib::report_MediaTrace && report_kind < MediaConchLib::report_Max &&
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
            int ret = MCL.checker_analyze(use_as_user, policy_reference_file, plugins, this->options, registered,
                                          file_id, err, force_analyze, mil_analyze);
            if (ret < 0)
                return -1;

            if (run_policy_reference_file(file_id, err) < 0)
                return -1;

            std::stringstream ss;
            ss << file_id;
            options["policy_reference_id"] = ss.str();
        }

        //Output
        MediaConchLib::Checker_ReportRes result;
        std::vector<size_t> policies_ids;
        options["verbosity"] = MCL.get_implementation_verbosity();
        MCL.checker_get_report(use_as_user, report_set, format, id_to_report, policies_ids,
                               policies, options, &result, error, &display_file, NULL);
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

        size_t pos = MCL.xslt_policy_create_from_file(use_as_user, files_ids[0], error);
        if (pos == (size_t)-1)
            return MediaConchLib::errorHttp_INTERNAL;

        std::string policy;
        if (MCL.policy_dump(use_as_user, pos, false, policy, error))
            return MediaConchLib::errorHttp_INTERNAL;

        MediaInfoLib::String policy_mil = ZenLib::Ztring().From_UTF8(policy);
        STRINGOUT(policy_mil);

        return 0;
    }

    //--------------------------------------------------------------------------
    int CLI::run_file_information(std::string& err)
    {
        for  (size_t i = 0; i < files.size(); ++i)
        {
            long id = MCL.checker_id_from_filename(use_as_user, files[i], options, err);
            if (id < 0)
                return -1;

            MediaConchLib::Checker_FileInfo info;
            if (MCL.checker_file_information(use_as_user, id, info, err) < 0)
                return -1;

            std::string report;
            file_info_report(&info, report);
            MediaInfoLib::String info_str = ZenLib::Ztring().From_UTF8(report);
            STRINGOUT(info_str);
        }
        return 0;
    }

    //--------------------------------------------------------------------------
    int CLI::run_plugins_list(std::string& err)
    {
        std::vector<std::string> list;
        if (MCL.mediaconch_get_plugins(list, err) < 0)
            return -1;

        std::stringstream out;
        out << "plugins:[";

        for (size_t i = 0; i < list.size(); ++i)
        {
            if (i)
                out << ", ";
            out << "\"" << list[i] << "\"";
        }

        out << "]";

        MediaInfoLib::String out_str = ZenLib::Ztring().From_UTF8(out.str());
        STRINGOUT(out_str);

        return 0;
    }

    //--------------------------------------------------------------------------
    int CLI::run_watch_folders_list(std::string& err)
    {
        std::vector<std::string> list;
        if (MCL.mediaconch_list_watch_folders(list, err) < 0)
            return -1;

        std::stringstream out;
        out << "watched folders:[";

        for (size_t i = 0; i < list.size(); ++i)
        {
            if (i)
                out << ", ";
            out << "\"" << list[i] << "\"";
        }

        out << "]";

        MediaInfoLib::String out_str = ZenLib::Ztring().From_UTF8(out.str());
        STRINGOUT(out_str);

        return 0;
    }

    //--------------------------------------------------------------------------
    int CLI::run_watch_folder_cmd(std::string& err)
    {
        long user_id = -1;
        if (MCL.mediaconch_watch_folder(watch_folder, watch_folder_reports, plugins, policies,
                                        watch_folder_user, watch_folder_recursive, options, user_id, err) < 0)
            return -1;

        std::stringstream out;
        out << "Watch folder user ID:" << user_id;

        MediaInfoLib::String out_str = ZenLib::Ztring().From_UTF8(out.str());
        STRINGOUT(out_str);

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
    int CLI::add_plugin_to_use(const std::string& plugin)
    {
        plugins.push_back(plugin);
        return 0;
    }

    //--------------------------------------------------------------------------
    int CLI::set_user_to_use(const std::string& user)
    {
        if (!user.size())
            return 0;

        if (watch_folder_user)
            delete watch_folder_user;
        watch_folder_user = new long;
        char *end = NULL;
        use_as_user = strtol(user.c_str(), &end, 10);
        return 0;
    }

    //--------------------------------------------------------------------------
    int CLI::set_watch_folder(const std::string& folder)
    {
        watch_folder = folder;
        no_needs_files_mode = true;
        return 0;
    }

    //--------------------------------------------------------------------------
    int CLI::set_watch_folder_reports(const std::string& folder)
    {
        watch_folder_reports = folder;
        return 0;
    }

    //--------------------------------------------------------------------------
    int CLI::set_watch_folder_not_recursive()
    {
        watch_folder_recursive = false;
        return 0;
    }

    //--------------------------------------------------------------------------
    int CLI::set_watch_folder_user(const std::string& user)
    {
        if (!user.size())
            return 0;

        if (watch_folder_user)
            delete watch_folder_user;
        watch_folder_user = new long;
        char *end = NULL;
        *watch_folder_user = strtol(user.c_str(), &end, 10);
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
    void CLI::set_plugins_list_mode()
    {
        plugins_list_mode = true;
        no_needs_files_mode = true;
    }

    //--------------------------------------------------------------------------
    void CLI::set_list_watch_folders_mode()
    {
        list_watch_folders_mode = true;
        no_needs_files_mode = true;
    }

    //--------------------------------------------------------------------------
    int CLI::register_option(const std::string& key, std::string& value)
    {
        std::string report;

        if (MCL.test_mil_option(key, value, report) < 0)
        {
            ZenLib::Ztring str;
            str.From_UTF8(report);
            STRINGOUT(str);
            if (report == "Option not known")
                return CLI_RETURN_ERROR;
            return CLI_RETURN_FINISH;
        }

        options.push_back(std::make_pair(key, value));
        return CLI_RETURN_NONE;
    }

    //--------------------------------------------------------------------------
    int CLI::is_ready(long& file_id, MediaConchLib::report& report_kind, std::string& err)
    {
        MediaConchLib::Checker_StatusRes res;
        int ret = MCL.checker_status(use_as_user, file_id, res, err);
        if (ret < 0)
            return -1;

        report_kind = MediaConchLib::report_MediaConch;

        if (use_daemon && asynchronous)
        {
            if (!res.finished)
            {
                std::string file;
                if (MCL.checker_file_from_id(use_as_user, file_id, file, err) < 0)
                    return -1;

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
                if (MCL.checker_status(use_as_user, file_id, res, err) < 0)
                    return -1;
            }

            if (res.has_error)
            {
                std::stringstream ss;
                std::string file;
                if (MCL.checker_file_from_id(use_as_user, file_id, file, err) < 0)
                    return -1;

                ss << "File: " << file << " had a problem during analyze\n";
                ss << "\tError logs are: " << res.error_log;
                err = ss.str();
                return -1;
            }

            if (res.tool)
                report_kind = (MediaConchLib::report)*res.tool;

            if (res.generated_id >= 0)
            {
                file_id = res.generated_id;
                return is_ready(file_id, report_kind, err);
            }
        }

        return 1;
    }

    //--------------------------------------------------------------------------
    void CLI::set_force_analyze(bool force)
    {
        force_analyze = force;
    }

    //--------------------------------------------------------------------------
    void CLI::set_mil_analyze(bool analyze)
    {
        mil_analyze = analyze;
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
    int CLI::get_values_for_type_field(const std::string& type, const std::string& field, std::vector<std::string>& values)
    {
        return MCL.policy_get_values_for_type_field(type, field, values, error);
    }

    int CLI::run_policy_reference_file(long id, std::string& err)
    {
        MediaConchLib::report report_kind;
        return is_ready(id, report_kind, err);
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
            MCL.checker_file_from_id(use_as_user, info->generated_id, file, error);
            ss << "generated file:         " << file << "\n";
        }

        if (info->source_id >= 0)
        {
            std::string file;
            MCL.checker_file_from_id(use_as_user, info->source_id, file, error);
            ss << "source file:            " << file << "\n";
            ss << "file generation time:   " << info->generated_time << " milliseconds\n";
            ss << "generated log:          " << info->generated_log << "\n";
            ss << "generated error log:    " << info->generated_error_log << "\n";
        }

        ss << "options:                ";
        for (size_t i = 0; i < info->options.size(); ++i)
        {
            if (i)
                ss << ",";
            ss << info->options[i].first << "=" << info->options[i].second;
        }
        ss << "\n";

        report = ss.str();
    }

}
