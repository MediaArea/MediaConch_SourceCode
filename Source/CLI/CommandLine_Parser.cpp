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
        int Return=_METHOD(MI, Argument); \
        if (Return<0) \
            return Return; \
    } \

#define OPTION(_TEXT, _TOLAUNCH) \
    else if (Argument.find(_TEXT)==0)        LAUNCH(_TOLAUNCH) \

#define OPTION2(_TEXT, _TOLAUNCH) \
    else if (Argument.find(_TEXT)==0)        _TOLAUNCH(); \


//***************************************************************************
// Defaults
//***************************************************************************

ZenLib::Ztring LogFile_FileName;
std::string Last_Argument;

//***************************************************************************
// Main
//***************************************************************************

int Parse(MediaConch::Core& MI, std::string& Argument)
{
    // With 1 other argument
    if (Last_Argument.length())
    {
        Argument = Last_Argument.append(Argument);
        Last_Argument = "";
    }
    if (Argument=="-p") {
        Last_Argument = "--policy=";
        return 0;
    }
    if (Argument=="-d") {
        Last_Argument = "--display=";
        return 0;
    }

    // Help short options
    if (Argument=="-ha")
        Argument = "--help=Advanced";
    if (Argument=="-v")
        Argument = "--version";

    // Backward compatibility
    if (Argument=="-tc")
        Argument = "--report=MediaConch";
    if (Argument=="-ti")
        Argument = "--report=MediaInfo";
    if (Argument=="-tt")
        Argument = "--report=MediaTrace";

    // Report other options
    if (Argument=="--mediaconch")
        Argument = "-mc";
    if (Argument=="--mediainfo")
        Argument = "-mi";
    if (Argument=="--mediatrace")
        Argument = "-mt";
    
    // Report short options
    if (Argument=="-mc")
        Argument = "--report=MediaConch";
    if (Argument=="-mi")
        Argument = "--report=MediaInfo";
    if (Argument=="-mt")
        Argument = "--report=MediaTrace";

    // Format short options
    if (Argument=="-ft")
        Argument = "--format=Text";
    if (Argument=="-fx")
        Argument = "--format=XML";
    if (Argument=="-fa")
        Argument = "--format=MAXML";
    if (Argument=="-fj")
        Argument = "--format=JSTREE";
    if (Argument=="-fh")
        Argument = "--format=HTML";
    
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
    //Default
    OPTION("--",                                            Default)
    else
        return 1;

    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(Help)
{
    (void)MI;
    //Form : --Help=Advanced
    size_t Egal_Pos=Argument.find('=');
    if (Egal_Pos!=std::string::npos)
    {
        std::string level=Argument.substr(Egal_Pos+1);
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
    (void)MI;
    (void)Argument;
    return Version();
}

//---------------------------------------------------------------------------
CL_OPTION(Report)
{
    //Form : --Inform=Text
    size_t Egal_Pos=Argument.find('=');
    if (Egal_Pos==std::string::npos)
        return Help();

    // Removing defaults
    if (MI.Report_IsDefault)
    {
        MI.Report.reset();
        MI.Report_IsDefault=false;
    }
        
    // New requested reports
    std::string Tool=Argument.substr(Egal_Pos+1);
    if (Tool=="MediaConch" || Tool=="mediaconch")
        MI.Report.set(MediaConch::Core::report_MediaConch);
    if (Tool=="MediaInfo" || Tool=="mediainfo")
        MI.Report.set(MediaConch::Core::report_MediaInfo);
    if (Tool=="MediaTrace" || Tool=="mediatrace")
        MI.Report.set(MediaConch::Core::report_MediaTrace);

    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(Format)
{
    //Form : --Inform=Text
    size_t Egal_Pos=Argument.find('=');
    if (Egal_Pos==std::string::npos)
        return Help();

    std::string Format=Argument.substr(Egal_Pos+1);
    if (Format=="Text" || Format=="text")
        MI.Format=MediaConch::Core::format_Text;
    if (Format=="XML" || Format=="xml")
        MI.Format=MediaConch::Core::format_Xml;
    if (Format=="MAXML" || Format=="maxml")
        MI.Format=MediaConch::Core::format_MaXml;
    if (Format=="JSTREE" || Format=="jstree")
        MI.Format=MediaConch::Core::format_JsTree;
    if (Format=="HTML" || Format=="html")
        MI.Format=MediaConch::Core::format_Html;

    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(PolicyOption)
{
    //Form : --Inform=Text
    size_t Egal_Pos=Argument.find('=');
    if (Egal_Pos==std::string::npos)
        return Help_Policy();

    std::string file;
    file.assign(Argument, Egal_Pos+1, std::string::npos);
    MI.PoliciesFiles.push_back(file);
    Report(MI, "--report=MediaConch");
    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(Display)
{
    //Form : --Inform=Text
    size_t Egal_Pos=Argument.find('=');
    if (Egal_Pos==std::string::npos)
        return Help();

    std::string file;
    file.assign(Argument, Egal_Pos+1, std::string::npos);
    MI.xsltDisplay = file;
    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(LogFile)
{
    //Form : --LogFile=Text
    (void)MI;
    LogFile_FileName.assign(Ztring().From_UTF8(Argument), 10, std::string::npos);

    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(Default)
{
    //Form : --Option=Text
    size_t Egal_Pos=Argument.find(__T('='));
    if (Egal_Pos<2)
        return 0;
    MediaInfoNameSpace::String Option(Ztring().From_UTF8(Argument), 2, Egal_Pos-2);
    MediaInfoNameSpace::String Value;
    if (Egal_Pos!=std::string::npos)
        Value.assign(Ztring().From_UTF8(Argument), Egal_Pos+1, std::string::npos);
    else
        Value=__T('1');

    String Result=MI.Menu_Option_Preferences_Option(Option, Value);
    if (!Result.empty())
    {
        STRINGOUT(Result);
        return -1;
    }

    return 0;
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
void CallBack_Set(MediaConch::Core &MI, void* Event_CallBackFunction)
{
    //CallBack configuration
    MI.Menu_Option_Preferences_Option(__T("Event_CallBackFunction"), __T("CallBack=memory://")+ZenLib::Ztring::ToZtring((size_t)Event_CallBackFunction));
}
