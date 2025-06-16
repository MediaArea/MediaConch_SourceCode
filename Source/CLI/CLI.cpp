/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
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
#include "Common/Reports.h"
#include <Common/MediaConchLib.h>
#include <ZenLib/ZtringList.h>
#include <ZenLib/Ztring.h>
#include <ZenLib/File.h>
#include <ZenLib/Dir.h>

#if !defined(WINDOWS)
    #include <unistd.h>
#endif //!defined(WINDOWS)

#ifdef HAVE_GLOB
    #include <glob.h>
#endif

#ifndef GLOB_TILDE
#define GLOB_TILDE 0
#endif

//****************************************************************************
// Extern
//****************************************************************************

extern ZenLib::Ztring LogFile_FileName;

//---------------------------------------------------------------------------
namespace MediaConch
{

//****************************************************************************
// Event to manage
//****************************************************************************

//--------------------------------------------------------------------------
void Log_0(struct MediaInfo_Event_Log_0* Event)
{
    MediaInfoLib::String MessageString;

    if (Event->Type >= 0xC0)
        MessageString += __T("E: ");

#if defined(UNICODE) || defined (_UNICODE)
    MessageString+=Event->MessageStringU;
#else //defined(UNICODE) || defined (_UNICODE)
    MessageString+=Event->MessageStringA;
#endif //defined(UNICODE) || defined (_UNICODE)

    //Special cases
    switch (Event->MessageCode)
    {
    case 0xF1010101:
        MessageString+=__T(" If you want to use such protocols, compile libcurl with SSL/SSH support");
        break;
    case 0xF1010102:
    case 0xF1010103:
        MessageString += __T(" If you are in a secure environment, do \"ssh %YourServerName%\" in order to add the fingerprint to the known_hosts file. If you want to ignore security issues, use --Ssh_IgnoreSecurity option");
        break;
    case 0xF1010104:
        MessageString += __T(" If you want to ignore security issues, use --Ssl_IgnoreSecurity option.");
        break;
    default:
        break;
    }

    if (Event->Type >= 0x80)
        STRINGERR(MessageString);
    else
        STRINGOUT(MessageString);
}

//--------------------------------------------------------------------------
void __stdcall Event_CallBackFunction(unsigned char* Data_Content, size_t Data_Size, void* UserHandle_Void)
{
    struct MediaInfo_Event_Generic*     Event_Generic=(struct MediaInfo_Event_Generic*)Data_Content;
    unsigned short                      EventID;
    unsigned char                       EventVersion;

    /*integrity tests*/
    if (Data_Size<4)
        return; //There is a problem

    /*Retrieving EventID*/
    EventID     =(unsigned short)((Event_Generic->EventCode&0x00FFFF00)>>8 );
    EventVersion=(unsigned char) ( Event_Generic->EventCode&0x000000FF     );

    switch (EventID)
    {
        case MediaInfo_Event_Global_BytesRead:
            if (EventVersion == 0 && Data_Size >= sizeof(struct MediaInfo_Event_Global_BytesRead_0) && UserHandle_Void != NULL)
            {
                struct MediaInfo_Event_Global_BytesRead_0* Event_Data=(struct MediaInfo_Event_Global_BytesRead_0*)Data_Content;
                std::string* UserHandle=(std::string*)UserHandle_Void;

                UserHandle->append((const char*)Event_Data->Content, Event_Data->Content_Size);
            }
            break;
        case MediaInfo_Event_Log:
            if (EventVersion == 0 && Data_Size >= sizeof(struct MediaInfo_Event_Log_0))
                Log_0((struct MediaInfo_Event_Log_0*)Data_Content);
            break;
        default: ;
    }
}

    //**************************************************************************
    // CLI
    //**************************************************************************

    //--------------------------------------------------------------------------
    CLI::CLI() : watch_folder_user(NULL), use_as_user(-1), use_daemon(false), asynchronous(false),
                 force_analyze(false), full_report(false), no_database(false), include_hidden_files(false), mil_analyze(true),
                 watch_folder_recursive(true), create_policy_mode(false), file_information(false),
                 plugins_list_mode(false), list_watch_folders_mode(false), no_needs_files_mode(false),
                 list_mode(false), fixer(false)
    {
        format = MediaConchLib::format_Max;
    }

    //--------------------------------------------------------------------------
    CLI::~CLI()
    {
    }

    //--------------------------------------------------------------------------
    int CLI::init(std::string& err)
    {
        if (format == MediaConchLib::format_CSV && !policies.size())
        {
            err = "CLI can use CSV format only with policies.";
            return CLI_RETURN_ERROR;
        }

        if (no_database)
            MCL.set_no_database(no_database);

        if (!no_needs_files_mode)
        {
            // If no filenames (and no options)
            if (files.empty())
                return Help_Nothing();

            // download remote policies
            if (MCL.mil_has_curl_enabled())
            {
                for (size_t i = policies.size(); i > 0; --i)
                {
                    if (policies[i-1].find("fetch://")==0)
                    {
                        std::string policy = policies[i-1].substr(8);
                        MediaInfoNameSpace::MediaInfo tmpMI;
                        tmpMI.Option(__T("ParseSpeed"), __T("1"));

                        for (size_t i = 0; i < options.size(); ++i)
                        {
                            std::string report;
                           if (!options[i].first.empty() && !options[i].second.empty() && MCL.test_mil_option(options[i].first, options[i].second, report)==0)
                               tmpMI.Option(ZenLib::Ztring().From_UTF8(options[i].first), ZenLib::Ztring().From_UTF8(options[i].second));
                        }

                        std::string data;
                        std::stringstream ss;
                        ss << "CallBack=memory://" << (ZenLib::int64u)Event_CallBackFunction << ";UserHandler=memory://" << (ZenLib::int64u)&data;
                        tmpMI.Option(__T("File_Event_CallBackFunction"), ZenLib::Ztring().From_UTF8(ss.str().c_str()));
                        tmpMI.Option(__T("Event_CallBackFunction"), ZenLib::Ztring().From_UTF8(ss.str().c_str()));
                        tmpMI.Open(ZenLib::Ztring().From_UTF8(policy));

                        if (!data.empty())
                            policies[i-1] = data;
                        else
                        {
                            err = "Unable to fetch policy file: " + policy + ".";
                            return CLI_RETURN_ERROR;
                        }

                         tmpMI.Close();
                         tmpMI.Option(__T("ParseSpeed"), __T("0.5"));
                    }
                }
            }

            // If no report selected, use Implementation by default
            if (!report_set.count() && !policies.size())
                report_set.set(MediaConchLib::report_MediaConch);

            // If no Implementation Schema registered, use one by default
            if (!MCL.get_implementation_schema_file().length())
            {
                if (MCL.create_default_implementation_schema(err) != 0)
                {
                    if (err == "Unable to write implementation schema file.")
                        err += " Check write permissions on MediaConch data directory.";
                    return CLI_RETURN_ERROR;
                }
            }

            if (!MCL.ReportAndFormatCombination_IsValid(files, report_set, display_content,
                                                        format, err))
            {
                if (err == "MicroMediaTrace requires an XML output.")
                    err += " Add -fx.";
                return CLI_RETURN_ERROR;
            }

            if (report_set[MediaConchLib::report_MediaInfo]
                && format == MediaConchLib::format_Max
                && display_content.empty())
                format = MediaConchLib::format_Text;

            if (format == MediaConchLib::format_Max && display_content.empty())
                format = MediaConchLib::format_Simple;
            else if (format == MediaConchLib::format_Max && !display_content.empty())
                format = MediaConchLib::format_Xml;

            for (size_t i = 0; i < outputs.size(); ++i)
            {
                MediaConchLib::format format = outputs[i].first;
                std::string filename = outputs[i].second;

                if (format == MediaConchLib::format_Max && display_content.empty())
                {
                    // Try to autodetect wanted output format
                    if (filename.size() >= 4 && filename.rfind(".txt") == filename.size() - 4)
                        format = MediaConchLib::format_Text;
                    else if (filename.size() >= 4 && filename.rfind(".xml") == filename.size() - 4)
                        format = MediaConchLib::format_Xml;
                    else if (filename.size() >= 5 && filename.rfind(".html") == filename.size() - 5)
                        format = MediaConchLib::format_Html;
                    else if (filename.size() >= 4 && filename.rfind(".csv") == filename.size() - 4)
                        format = MediaConchLib::format_CSV;
                    else if (filename.size() >= 5 && filename.rfind(".json") == filename.size() - 5)
                        format = MediaConchLib::format_Json;
                    else
                        format = MediaConchLib::format_Simple;

                    outputs[i].first = format;
                }

                if (format == MediaConchLib::format_CSV && !policies.size())
                {
                     err = "CLI can use CSV format only with policies.";
                     return CLI_RETURN_ERROR;
                }

                if (!MCL.ReportAndFormatCombination_IsValid(files, report_set,
                                                            format == MediaConchLib::format_Max ? display_content : std::string(),
                                                            format, err))
                {
                    if (err == "MicroMediaTrace requires an XML output.")
                        err += " Use only --output-xml=";
                    return CLI_RETURN_ERROR;
                }
            }
        }

        MCL.set_configuration_file(configuration_file);
        MCL.set_plugins_configuration_file(plugins_configuration_file);

        if (MCL.init(err) < 0)
            return CLI_RETURN_ERROR;

        use_daemon = MCL.get_use_daemon();

        MCL.register_log_callback(&Log_0);

        return CLI_RETURN_NONE;
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
        CheckerReport cr;
        MediaConchLib::report report_kind = MediaConchLib::report_Max;

        //Return plugins list
        if (plugins_list_mode)
        {
            if (run_plugins_list(err) < 0)
               return CLI_RETURN_ERROR;
            return CLI_RETURN_NONE;
        }
        //Return watch folders list
        else if (list_watch_folders_mode)
        {
            if (run_watch_folders_list(err) < 0)
               return CLI_RETURN_ERROR;
            return CLI_RETURN_NONE;
        }
        //Return list files
        else if (list_mode)
        {
            if (run_list_files(err) < 0)
               return CLI_RETURN_ERROR;
            return CLI_RETURN_NONE;
        }
        // Add a watch folder
        else if (watch_folder.size())
        {
            if (run_watch_folder_cmd(err) < 0)
               return CLI_RETURN_ERROR;
            return CLI_RETURN_NONE;
        }

        //Return file information
        if (file_information)
            return run_file_information(err);

        for (size_t i = 0; i < files.size(); ++i)
        {
            bool registered = false;
            long file_id = -1;
            std::vector<long> file_ids;
            int ret = MCL.checker_analyze(use_as_user, files[i], plugins, options, registered,
                                          file_id, err, force_analyze, mil_analyze);
            if (ret < 0)
                return CLI_RETURN_ERROR;

            if (use_daemon && asynchronous && !registered)
            {
                std::stringstream str;
                str << "Registering ";
                str << files[i];
                str << " to analyze";
                STRINGOUT(ZenLib::Ztring().From_UTF8(str.str()));
            }

            int ready = is_ready(file_id, file_ids, report_kind, err);
            if (ready == MediaConchLib::errorHttp_NONE)
                continue;
            else if (ready < 0)
                //TODO: PROBLEM
                return CLI_RETURN_ERROR;

            if (report_kind == MediaConchLib::report_Max)
                report_kind = MediaConchLib::report_MediaConch;

            if (report_set[MediaConchLib::report_MediaConch] &&
                report_kind > MediaConchLib::report_MediaTrace && report_kind < MediaConchLib::report_Max &&
                files.size() == 1)
            {
                set_report_reset();
                report_set.set(report_kind);
            }

            for (size_t j = 0; j < file_ids.size(); ++j)
                cr.files.push_back(file_ids[j]);
        }

        //Ensure to analyze before creating library
        if (create_policy_mode)
            return run_create_policy(cr.files);

        // if compare two files
        if (policy_reference_file.size())
        {
            bool registered = false;
            long file_id;
            int ret = MCL.checker_analyze(use_as_user, policy_reference_file, plugins, this->options, registered,
                                          file_id, err, force_analyze, mil_analyze);
            if (ret < 0)
                return CLI_RETURN_ERROR;

            if (run_policy_reference_file(file_id, err) < 0)
                return CLI_RETURN_ERROR;

            std::stringstream ss;
            ss << file_id;
            cr.options["policy_reference_id"] = ss.str();
        }

        if (policy_verbosity.size())
            cr.options["policy_verbosity"] = policy_verbosity;

        if (full_report)
            cr.options["full_report"] = "1";

        //Output
        cr.user = use_as_user;
        cr.report_set = report_set;
        cr.options["verbosity"] = MCL.get_implementation_verbosity();

        for (size_t i = 0; i < policies.size(); ++i)
            cr.policies_contents.push_back(policies[i]);

        if (report_set[MediaConchLib::report_MediaInfo] && mi_inform.size())
            cr.mi_inform = &mi_inform;

        if (outputs.empty()) //Print report to screen
        {
            MediaConchLib::Checker_ReportRes result;
            cr.format = format;

            cr.display_content = &display_content;

            MCL.checker_get_report(cr, &result, error);
            MediaInfoLib::String report_mi = ZenLib::Ztring().From_UTF8(result.report);

            STRINGOUT(report_mi);
            //Output, in a file if needed
            if (!LogFile_FileName.empty())
                LogFile_Action(report_mi);
        }
        else //Write report to file(s)
        {
            for (size_t i = 0; i < outputs.size(); ++i)
            {
                MediaConchLib::Checker_ReportRes result;

                MediaConchLib::format format = outputs[i].first;
                std::string filename = outputs[i].second;

                cr.display_content = NULL;
                if (format == MediaConchLib::format_Max)
                {
                    format = MediaConchLib::format_Xml; //Needed with display
                    cr.display_content = &display_content;
                }

                cr.format = format;

                MCL.checker_get_report(cr, &result, error);
                MediaInfoLib::String report_mi = ZenLib::Ztring().From_UTF8(result.report);

                ZenLib::File F;
                if (!F.Create(ZenLib::Ztring().From_UTF8(outputs[i].second.c_str())))
                {
                    STRINGOUT(__T("Unable to write to output file: ") + ZenLib::Ztring().From_UTF8(filename));
                    return CLI_RETURN_ERROR;
                }

                //Filling
                F.Write(report_mi);
            }
        }

        return CLI_RETURN_NONE;
    }

    //--------------------------------------------------------------------------
    int CLI::run_create_policy(const std::vector<long>& files_ids)
    {
        if (files_ids.size() != 1)
        {
            error = "Create a policy only with one file";
            return -1;
        }

        size_t pos = MCL.xslt_policy_create_from_file(use_as_user, files_ids[0], error);
        if (pos == (size_t)-1)
            return -1;

        std::string policy;
        if (MCL.policy_dump(use_as_user, pos, false, policy, error) < 0)
            return -1;

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
    int CLI::run_list_files(std::string& err)
    {
        std::vector<std::string> list;
        if (MCL.checker_list(use_as_user, list, err) < 0)
            return -1;

        std::stringstream out;
        out << "{\"files_analyzed\":[";

        for (size_t i = 0; i < list.size(); ++i)
        {
            if (i)
                out << ", ";
            out << "\"" << list[i] << "\"";
        }

        out << "]}";

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
        else if (f == "XML" || f == "xml")
            return set_format(MediaConchLib::format_Xml);
        else if (f == "MAXML" || f == "maxml")
            return set_format(MediaConchLib::format_MaXml);
        else if (f == "JSTREE" || f == "jstree")
            return set_format(MediaConchLib::format_JsTree);
        else if (f == "HTML" || f == "html")
            return set_format(MediaConchLib::format_Html);
        else if (f == "SIMPLE" || f == "simple")
            return set_format(MediaConchLib::format_Simple);
        else if (f == "CSV" || f == "csv")
            return set_format(MediaConchLib::format_CSV);
        else if (f == "JSON" || f == "json")
            return set_format(MediaConchLib::format_Json);
        else
            mi_inform = f;

        return 0;
    }

    //--------------------------------------------------------------------------
    int CLI::add_policy(const std::string& pattern)
    {
        if (pattern.find("://")!=std::string::npos && MCL.mil_has_curl_enabled())
        {
            policies.push_back("fetch://" + pattern);
            return 0;
        }

        std::vector<std::string> filenames;
#ifdef HAVE_GLOB
        // Policy filenames pattern matching
        glob_t gl;
        if (glob(pattern.c_str(), GLOB_TILDE, NULL, &gl) == 0)
        {
            for (size_t j = 0; j < gl.gl_pathc; ++j)
                filenames.push_back(gl.gl_pathv[j]);
            globfree(&gl);
        }
        else
            filenames.push_back(pattern);
#else
        filenames.push_back(pattern);
#endif

        for (size_t i = 0; i < filenames.size() ; ++i)
        {
            std::string& filename = filenames[i];
            ZenLib::Ztring z_filename = ZenLib::Ztring().From_UTF8(filename);
            if (!ZenLib::File::Exists(z_filename))
            {
               STRINGERR(__T("Policy file: ") + z_filename + __T(" not found."));
               return CLI_RETURN_ERROR;
            }

            ZenLib::File file(z_filename);

            ZenLib::int64u size = file.Size_Get();
            if (size == (ZenLib::int64u)-1)
            {
               STRINGERR(__T("Unable to open policy file: ") + z_filename + __T("."));
               return CLI_RETURN_ERROR;
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
        }
        return 0;
    }

    //--------------------------------------------------------------------------
    void CLI::set_display_file(const std::string& file)
    {
        std::ifstream file_handler(file.c_str(), std::ios_base::ate);
        if (!file_handler.is_open())
        {
            display_content = std::string();
            return;
        }

        int size = file_handler.tellg();
        if (size < 0)
        {
            display_content = std::string();
            return;
        }

        file_handler.seekg(0, file_handler.beg);
        display_content.reserve(size);
        display_content.assign(std::istreambuf_iterator<char>(file_handler),
                               std::istreambuf_iterator<char>());
        file_handler.close();
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
    void CLI::set_policy_verbosity(const std::string& verbosity)
    {
        policy_verbosity = verbosity;
    }

    //--------------------------------------------------------------------------
    void CLI::set_full_report(bool full)
    {
        full_report = full;
    }

    //--------------------------------------------------------------------------
    void CLI::set_include_hidden(bool hidden)
    {
        include_hidden_files = hidden;
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
    void CLI::set_list_mode()
    {
        list_mode = true;
        no_needs_files_mode = true;
    }

    //--------------------------------------------------------------------------
    int  CLI::add_output(const std::string& format, const std::string& filename)
    {
        MediaConchLib::format f;
        if (format == "text")
            f = MediaConchLib::format_Text;
        else if (format == "xml")
            f = MediaConchLib::format_Xml;
        else if (format == "maxml")
            f = MediaConchLib::format_MaXml;
        else if (format == "jstree")
            f = MediaConchLib::format_JsTree;
        else if (format == "html")
            f = MediaConchLib::format_Html;
        else if (format == "csv")
            f = MediaConchLib::format_CSV;
        else if (format == "json")
            f = MediaConchLib::format_Json;
        else if (format == "simple")
            f = MediaConchLib::format_Simple;
        else
            f = MediaConchLib::format_Max;

        outputs.push_back(std::make_pair(f, filename));
        return 0;
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

        ZenLib::Ztring str = ZenLib::Ztring().From_UTF8(key.c_str());
        if (str.Compare(__T("File_TryToFix")))
        {
            fixer = true;
            options.push_back(std::make_pair(key, value));
            options.push_back(std::make_pair("file_parsespeed", "1"));
        }
        else if (fixer && (str.Compare(__T("File_ParseSpeed")) || str.Compare(__T("ParseSpeed"))))
        {
            ZenLib::Ztring str = ZenLib::Ztring().From_UTF8(value.c_str());
            if (str != __T("1"))
            {
                STRINGOUT(ZenLib::Ztring().From_UTF8("ParseSpeed option must be set to 1 with File_TryToFix option."));
                return CLI_RETURN_ERROR;
            }
        }

        options.push_back(std::make_pair(key, value));
        return CLI_RETURN_NONE;
    }

    //--------------------------------------------------------------------------
    int CLI::is_ready(long file_id, std::vector<long>& file_ids, MediaConchLib::report& report_kind, std::string& err)
    {
        MediaConchLib::Checker_StatusRes res;
        int ret = MCL.checker_status(use_as_user, file_id, res, err);
        if (ret < 0)
            return -1;

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

            if (res.tool && report_kind == MediaConchLib::report_Max)
                report_kind = (MediaConchLib::report)*res.tool;

            file_ids.push_back(file_id);
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

            file_ids.push_back(file_id);

            if (res.tool && report_kind == MediaConchLib::report_Max)
                report_kind = (MediaConchLib::report)*res.tool;

            for (size_t i = 0; i < res.generated_id.size(); ++i)
            {
                if (res.generated_id[i] == -1)
                    continue;

                file_id = res.generated_id[i];
                if ((ret = is_ready(file_id, file_ids, report_kind, err)) <= 0)
                    return ret;
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
    void CLI::set_no_database(bool no_db)
    {
        no_database = no_db;
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

        int flags = ZenLib::Dir::Include_Files | ZenLib::Dir::Parse_SubDirs;
        if (include_hidden_files)
            flags |= ZenLib::Dir::Include_Hidden;

        ZenLib::ZtringList list = ZenLib::Dir::GetAllFileNames(dirname, (ZenLib::Dir::dirlist_t)flags);

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
        MediaConchLib::report report_kind = MediaConchLib::report_Max;
        std::vector<long> file_ids;
        return is_ready(id, file_ids, report_kind, err);
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

        if (info->generated_id.size())
        {
            ss << "generated file:         [";
            for (size_t i = 0; i < info->generated_id.size(); ++i)
            {
                if (i)
                    ss << ",";

                std::string file;
                if (MCL.checker_file_from_id(use_as_user, info->generated_id[i], file, error) >= 0)
                    ss << file;
                else
                    ss << i;
            }
            ss << "]\n";
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
