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

#include <unistd.h>

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
    CLI::CLI() : use_daemon(false)
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
        // //If no filenames (and no options)
        if (files.empty())
            return Help_Nothing();

        // If no report selected, use Implementation by default
        if (!report_set.count())
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
            if (ret < 0)
                return ret; //no more tasks to do

            if (ret > 0)
                files.push_back(args[pos]); //Append the filename to the list of filenames to parse
        }
        return 0;
    }

    //--------------------------------------------------------------------------
    int CLI::run()
    {
        std::vector<std::string> file_to_report;
        for (size_t i = 0; i < files.size(); ++i)
        {
            bool registered = false;
            if (MCL.analyze(files[i], registered) < 0)
                continue;

            if (use_daemon && !registered)
            {
                std::stringstream str;
                str << "Registering ";
                str << files[i];
                str << " to analyze";
                STRINGOUT(ZenLib::Ztring().From_UTF8(str.str()));
            }

            int ready = is_ready(i);
            if (ready > 0)
                continue;
            else if (ready < 0)
                //TODO: PROBLEM
                ;
            file_to_report.push_back(files[i]);
        }

        //Output
        MediaConchLib::ReportRes result;
        std::vector<std::string> policies_contents;
        MCL.get_report(report_set, format, file_to_report, policies,
                       policies_contents, &result, &display_file, NULL);
        MediaInfoLib::String report_mi = ZenLib::Ztring().From_UTF8(result.report);

        STRINGOUT(report_mi);
        //Output, in a file if needed
        if (!LogFile_FileName.empty())
            LogFile_Action(report_mi);
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
    void CLI::set_implementation_schema_file(const std::string& file)
    {
        MCL.set_implementation_schema_file(file);
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
            return -1;
        }
        return 0;
    }
    
    //--------------------------------------------------------------------------
    int CLI::is_ready(size_t i)
    {
        std::vector<std::string> vec;
        vec.push_back(files[i]);
        double percent_done = 0;

        if (use_daemon)
        {
            if (!MCL.is_done(files[i], percent_done))
            {
                std::stringstream str;
                str << "Analyzing " << files[i] << " ; done: " << percent_done  << "%";
                STRINGOUT(ZenLib::Ztring().From_UTF8(str.str()));
                return 1;
            }
        }
        else
        {
            while (!MCL.is_done(files[i], percent_done))
            {
                usleep(5000);
            }
        }
        return 0;
    }

}
