/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#include <string>
#include <vector>
#include <algorithm>
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "CommandLine_Parser.h"
#include "Help.h"
//---------------------------------------------------------------------------

//Parse Command Line
#define LAUNCH(_METHOD) \
    { \
        int Return=_METHOD(cli, argument); \
        if (Return < 0) \
            return Return; \
    } \

#define OPTION(_TEXT, _TOLAUNCH) \
    else if (argument.find(_TEXT)==0)        LAUNCH(_TOLAUNCH) \

#define OPTION2(_TEXT, _TOLAUNCH) \
    else if (argument.find(_TEXT)==0)        _TOLAUNCH(); \


//***************************************************************************
// Defaults
//***************************************************************************

ZenLib::Ztring LogFile_FileName;
std::string Last_Argument;

//***************************************************************************
// Main
//***************************************************************************

int Parse(MediaConch::CLI* cli, std::string& argument)
{
    // With 1 other argument
    if (Last_Argument.length())
    {
        argument = Last_Argument.append(argument);
        Last_Argument = "";
    }
    if (argument=="-p")
    {
        Last_Argument = "--policy=";
        return 0;
    }
    if (argument=="-d")
    {
        Last_Argument = "--display=";
        return 0;
    }
    if (argument=="-c")
    {
        Last_Argument = "--configuration=";
        return 0;
    }
    if (argument=="-i")
    {
        Last_Argument = "--implementationschema=";
        return 0;
    }

    // Help short options
    if (argument=="-ha")
        argument = "--help=Advanced";
    if (argument=="-v")
        argument = "--version";

    // Backward compatibility
    if (argument=="-tc")
        argument = "--report=MediaConch";
    if (argument=="-ti")
        argument = "--report=MediaInfo";
    if (argument=="-tt")
        argument = "--report=MediaTrace";

    // Report other options
    if (argument=="--mediaconch")
        argument = "-mc";
    if (argument=="--mediainfo")
        argument = "-mi";
    if (argument=="--mediatrace")
        argument = "-mt";
    
    // Report short options
    if (argument=="-mc")
        argument = "--report=MediaConch";
    if (argument=="-mi")
        argument = "--report=MediaInfo";
    if (argument=="-mt")
        argument = "--report=MediaTrace";

    // Format short options
    if (argument=="-ft")
        argument = "--format=Text";
    if (argument=="-fx")
        argument = "--format=XML";
    if (argument=="-fa")
        argument = "--format=MAXML";
    if (argument=="-fj")
        argument = "--format=JSTREE";
    if (argument=="-fh")
        argument = "--format=HTML";
    
    // Listing
    if (0);
    OPTION("--help",                                        Help)
    OPTION("-h",                                            Help)
    OPTION("--version",                                     Version)
    OPTION("--report",                                      Report)
    OPTION("--format",                                      Format)
    OPTION("--policy",                                      PolicyOption)
    OPTION("--display",                                     Display)
    OPTION("--logfile",                                     LogFile)
    OPTION("--configuration",                               Configuration)
    OPTION("--implementationschema",                        ImplementationSchema)
    //Default
    OPTION("--",                                            Default)
    else
        return 1;

    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(Help)
{
    (void)cli;
    //Form : --Help=Advanced
    size_t egal_pos = argument.find('=');
    if (egal_pos != std::string::npos)
    {
        std::string level = argument.substr(egal_pos + 1);
        transform(level.begin(), level.end(), level.begin(), (int(*)(int))tolower); //(int(*)(int)) is a patch for unix

        if (level == "advanced")
            return Help_Advanced();
        else if (level == "ssl")
            return Help_Ssl();
        else if (level == "ssh")
            return Help_Ssh();
    }
    return Help();
}

//---------------------------------------------------------------------------
CL_OPTION(Version)
{
    (void)cli;
    (void)argument;
    return Version();
}

//---------------------------------------------------------------------------
CL_OPTION(Report)
{
    //Form : --Inform=Text
    size_t egal_pos = argument.find('=');
    if (egal_pos == std::string::npos)
        return Help();
        
    // New requested reports
    std::string report_kind = argument.substr(egal_pos + 1);
    cli->set_report_set(report_kind);

    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(Format)
{
    //Form : --Inform=Text
    size_t egal_pos = argument.find('=');
    if (egal_pos == std::string::npos)
        return Help();

    std::string format = argument.substr(egal_pos + 1);
    return cli->set_format(format);
}

//---------------------------------------------------------------------------
CL_OPTION(PolicyOption)
{
    //Form : --Inform=Text
    size_t egal_pos = argument.find('=');
    if (egal_pos == std::string::npos)
        return Help_Policy();

    std::string file;
    file.assign(argument, egal_pos + 1, std::string::npos);
    cli->add_policy(file);

    std::string report = "MediaConch";
    cli->set_report_set(report);
    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(Display)
{
    //Form : --Inform=Text
    size_t egal_pos = argument.find('=');
    if (egal_pos == std::string::npos)
        return Help();

    std::string file;
    file.assign(argument, egal_pos +1 , std::string::npos);
    cli->set_display_file(file);
    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(LogFile)
{
    //Form : --LogFile=Text
    (void)cli;
    LogFile_FileName.assign(ZenLib::Ztring().From_UTF8(argument), 10, std::string::npos);

    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(Configuration)
{
    //Form : --Configuration=File
    size_t egal_pos = argument.find('=');
    if (egal_pos == std::string::npos)
        return Help();

    std::string file;
    file.assign(argument, egal_pos + 1 , std::string::npos);
    cli->set_configuration_file(file);
    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(ImplementationSchema)
{
    //Form : --ImplemnetationSchema=File
    size_t egal_pos = argument.find('=');
    if (egal_pos == std::string::npos)
        return Help();

    std::string file;
    file.assign(argument, egal_pos + 1 , std::string::npos);
    cli->set_implementation_schema_file(file);
    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(Default)
{
    return cli->register_option(argument);
}

//---------------------------------------------------------------------------
void LogFile_Action(ZenLib::Ztring Inform)
{
    if (LogFile_FileName.empty())
        return;

    std::string Inform_Ansi=Inform.To_UTF8();
    std::fstream File(LogFile_FileName.To_Local().c_str(), std::ios_base::out|std::ios_base::trunc);
    File.write(Inform_Ansi.c_str(), Inform_Ansi.size());
}

//---------------------------------------------------------------------------
void CallBack_Set(MediaConch::CLI* cli, void* Event_CallBackFunction)
{
    //CallBack configuration
    std::stringstream callback_mem;
    callback_mem << "--Event_CallBackFunction=";
    callback_mem << "CallBack=memory://";
    callback_mem << (size_t)Event_CallBackFunction;
    cli->register_option(callback_mem.str());
}
