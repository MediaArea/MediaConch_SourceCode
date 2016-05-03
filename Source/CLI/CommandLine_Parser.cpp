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
#include "Common/GeneratedCSVVideos.hpp"
#include "CommandLine_Parser.h"
#include "Help.h"
//---------------------------------------------------------------------------

//Parse Command Line
#define LAUNCH(_METHOD) \
    { \
        int Return=_METHOD(cli, argument); \
        if (Return == CLI_RETURN_ERROR || Return == CLI_RETURN_FINISH) \
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
bool Separator = false;

//***************************************************************************
// Main
//***************************************************************************

static bool wait_for_another_argument(std::string& argument)
{
    if (argument=="-pc")
    {
        Last_Argument = "--pluginsconfiguration=";
        return true;
    }
    if (argument=="-p")
    {
        Last_Argument = "--policy=";
        return true;
    }
    if (argument=="-d")
    {
        Last_Argument = "--display=";
        return true;
    }
    if (argument=="-c")
    {
        Last_Argument = "--configuration=";
        return true;
    }
    if (argument=="-i")
    {
        Last_Argument = "--implementationschema=";
        return true;
    }
    if (argument=="-iv")
    {
        Last_Argument = "--implementationverbosity=";
        return true;
    }
    return false;
}

static void change_short_options_to_long(std::string& argument)
{
    // Help short options
    if (argument=="-ha")
        argument = "--help=Advanced";
    if (argument=="-v")
        argument = "--version";
    if (argument=="-f")
        argument = "--force";

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

    // Compression short options
    if (argument=="-cz")
        argument = "--compression=zlib";

    // Asynchronous mode
    if (argument=="-as")
        argument = "--async=yes";
}

int Parse(MediaConch::CLI* cli, std::string& argument)
{
    if (argument == "--")
    {
        Separator = true;
        return CLI_RETURN_NONE;
    }

    if (Separator)
        return CLI_RETURN_FILE;

    // With 1 other argument
    if (Last_Argument.length())
    {
        argument = Last_Argument.append(argument);
        Last_Argument = "";
    }
    if (wait_for_another_argument(argument))
        return CLI_RETURN_NONE;
    change_short_options_to_long(argument);

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
    OPTION("--implementationverbosity",                     ImplementationVerbosity)
    OPTION("--compression",                                 Compression)
    OPTION("--force",                                       Force)
    OPTION("--async",                                       Asynchronous)
    OPTION("--pluginsconfiguration",                        PluginsConfiguration)
    OPTION("--defaultvaluesfortype",                        DefaultValuesForType)
    //Default
    OPTION("--",                                            Default)
    else
    {
        if (argument[0] == '-')
            return Help_Usage();
        return CLI_RETURN_FILE;
    }

    return CLI_RETURN_NONE;
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
    {
        Help();
        return CLI_RETURN_ERROR;
    }
        
    // New requested reports
    std::string report_kind = argument.substr(egal_pos + 1);
    cli->set_report_set(report_kind);

    return CLI_RETURN_NONE;
}

//---------------------------------------------------------------------------
CL_OPTION(Format)
{
    //Form : --Inform=Text
    size_t egal_pos = argument.find('=');
    if (egal_pos == std::string::npos)
    {
        Help();
        return CLI_RETURN_ERROR;
    }

    std::string format = argument.substr(egal_pos + 1);
    return cli->set_format(format);
}

//---------------------------------------------------------------------------
CL_OPTION(PolicyOption)
{
    //Form : --Inform=Text
    size_t egal_pos = argument.find('=');
    if (egal_pos == std::string::npos)
    {
        Help_Policy();
        return CLI_RETURN_ERROR;
    }

    std::string file;
    file.assign(argument, egal_pos + 1, std::string::npos);
    cli->add_policy(file);

    return CLI_RETURN_NONE;
}

//---------------------------------------------------------------------------
CL_OPTION(Display)
{
    //Form : --Inform=Text
    size_t egal_pos = argument.find('=');
    if (egal_pos == std::string::npos)
    {
        Help();
        return CLI_RETURN_ERROR;
    }

    std::string file;
    file.assign(argument, egal_pos +1 , std::string::npos);
    cli->set_display_file(file);
    return CLI_RETURN_NONE;
}

//---------------------------------------------------------------------------
CL_OPTION(LogFile)
{
    //Form : --LogFile=Text
    (void)cli;
    LogFile_FileName.assign(ZenLib::Ztring().From_UTF8(argument), 10, std::string::npos);

    return CLI_RETURN_NONE;
}

//---------------------------------------------------------------------------
CL_OPTION(Configuration)
{
    //Form : --Configuration=File
    size_t egal_pos = argument.find('=');
    if (egal_pos == std::string::npos)
    {
        Help();
        return CLI_RETURN_ERROR;
    }

    std::string file;
    file.assign(argument, egal_pos + 1 , std::string::npos);
    cli->set_configuration_file(file);
    return CLI_RETURN_NONE;
}

//---------------------------------------------------------------------------
CL_OPTION(ImplementationSchema)
{
    //Form : --ImplemnetationSchema=File
    size_t egal_pos = argument.find('=');
    if (egal_pos == std::string::npos)
    {
        Help();
        return CLI_RETURN_ERROR;
    }

    std::string file;
    file.assign(argument, egal_pos + 1 , std::string::npos);
    cli->set_implementation_schema_file(file);
    return CLI_RETURN_NONE;
}

//---------------------------------------------------------------------------
CL_OPTION(ImplementationVerbosity)
{
    //Form : --ImplemnetationSchema=File
    size_t egal_pos = argument.find('=');
    if (egal_pos == std::string::npos)
    {
        Help_Usage();
        return CLI_RETURN_ERROR;
    }

    std::string verbosity;
    verbosity.assign(argument, egal_pos + 1 , std::string::npos);
    cli->set_implementation_verbosity(verbosity);
    return CLI_RETURN_NONE;
}

//---------------------------------------------------------------------------
CL_OPTION(Compression)
{
    //Form : --Compression=Mode
    size_t egal_pos = argument.find('=');
    if (egal_pos == std::string::npos)
    {
        Help();
        return CLI_RETURN_ERROR;
    }

    std::string mode;
    mode.assign(argument, egal_pos + 1 , std::string::npos);
    transform(mode.begin(), mode.end(), mode.begin(), (int(*)(int))tolower); //(int(*)(int)) is a patch for unix

    return cli->set_compression_mode(mode);
}

//---------------------------------------------------------------------------
CL_OPTION(Force)
{
    (void)argument;
    cli->set_force_analyze(true);
    return CLI_RETURN_NONE;
}

//---------------------------------------------------------------------------
CL_OPTION(Asynchronous)
{
    //Form : --Compression=Mode
    size_t egal_pos = argument.find('=');
    std::string async;
    if (egal_pos != std::string::npos)
        async.assign(argument, egal_pos + 1 , std::string::npos);
    else
        async = "yes";
    transform(async.begin(), async.end(), async.begin(), (int(*)(int))tolower); //(int(*)(int)) is a patch for unix

    bool is_async = false;
    if (async == "yes")
        is_async = true;

    cli->set_asynchronous(is_async);
    return CLI_RETURN_NONE;
}

//---------------------------------------------------------------------------
CL_OPTION(PluginsConfiguration)
{
    //Form : --PluginsConfiguration=File
    size_t egal_pos = argument.find('=');
    if (egal_pos == std::string::npos)
    {
        Help();
        return CLI_RETURN_ERROR;
    }

    std::string file;
    file.assign(argument, egal_pos + 1 , std::string::npos);
    cli->set_plugins_configuration_file(file);
    return CLI_RETURN_NONE;
}

//---------------------------------------------------------------------------
CL_OPTION(DefaultValuesForType)
{
    (void)cli;
    //Form : --DefaultValuesForType=type,field
    size_t egal_pos = argument.find('=');
    if (egal_pos == std::string::npos)
    {
        Help();
        return CLI_RETURN_ERROR;
    }

    size_t comma_pos = argument.find(',');
    if (comma_pos == std::string::npos)
    {
        Help();
        return CLI_RETURN_ERROR;
    }

    std::string type, field;
    type = argument.substr(egal_pos + 1, comma_pos - egal_pos - 1);
    field.assign(argument, comma_pos + 1 , std::string::npos);

    std::map<std::string, std::map<std::string, std::vector<std::string> > > values;
    if (get_generated_values_from_csv(values) < 0)
        return CLI_RETURN_ERROR;

    if (values.find(type) != values.end())
    {
        if (values[type].find(field) != values[type].end())
        {
            std::stringstream out;
            for (size_t i = 0; i < values[type][field].size(); ++i)
            {
                if (i)
                    out << ",";
                out << values[type][field][i];
            }
            ZenLib::Ztring str;
            str.From_UTF8(out.str());
            STRINGOUT(str);
        }
    }

    return CLI_RETURN_FINISH;
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
