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

        std::string reason;
        if (!MCL.ReportAndFormatCombination_IsValid(files, report_set, display_file,
                                                    format, reason))
        {
            STRINGOUT(ZenLib::Ztring().From_UTF8(reason));
            return -1;
        }

        MCL.load_configuration();
        use_daemon = MCL.get_use_daemon();
        return 0;
    }

    //--------------------------------------------------------------------------
    int CLI::parse_args(int argc, char **argv)
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

            int ret = Parse(this, argument);
            if (ret < 0)
                return ret; //no more tasks to do

            if (ret > 0)
                files.push_back(argv[pos]); //Append the filename to the list of filenames to parse
        }
        return 0;
    }

    //--------------------------------------------------------------------------
    int CLI::run()
    {
        MCL.analyze(files);

        // // In CLI mode, wait it is finished
        while (1)
        {
            double percent_done = 0;
            if (MCL.is_done(files, percent_done))
                break;
            usleep(50000);
        }

        //Output
        std::string report;
        if (MCL.get_report(report_set, format, files, policies, report) < 0)
            return -1;

        MediaInfoLib::String report_mi = ZenLib::Ztring().From_UTF8(report);
        STRINGOUT(report_mi);

        //Output, in a file if needed
        if (!LogFile_FileName.empty())
            LogFile_Action(report_mi);
        return 0;
    }

    //--------------------------------------------------------------------------
    int CLI::finish()
    {
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

}
