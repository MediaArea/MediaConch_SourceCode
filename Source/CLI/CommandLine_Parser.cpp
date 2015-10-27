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
    else if (Argument.find(__T(_TEXT))==0)        LAUNCH(_TOLAUNCH) \

#define OPTION2(_TEXT, _TOLAUNCH) \
    else if (Argument.find(__T(_TEXT))==0)        _TOLAUNCH(); \


//***************************************************************************
// Defaults
//***************************************************************************

ZenLib::Ztring LogFile_FileName;
String Last_Argument;

//***************************************************************************
// Main
//***************************************************************************

int Parse(MediaConch::Core &MI, MediaInfoNameSpace::String Argument)
{
    // With 1 other argument
    if (Last_Argument.length())
    {
        Argument = Last_Argument.append(Argument);
        Last_Argument = __T("");
    }
    if (Argument==__T("-p")) {
        Last_Argument = __T("--policy=");
        return 0;
    }
    if (Argument==__T("-d")) {
        Last_Argument = __T("--display=");
        return 0;
    }

    // Help short options
    if (Argument==__T("-ha"))
        Argument = __T("--help=Advanced");

    // Backward compatibility
    if (Argument==__T("-tc"))
        Argument = __T("--report=MediaConch");
    if (Argument==__T("-ti"))
        Argument = __T("--report=MediaInfo");
    if (Argument==__T("-tt"))
        Argument = __T("--report=MediaTrace");

    // Report other options
    if (Argument==__T("--mediaconch"))
        Argument = __T("-mc");
    if (Argument==__T("--mediainfo"))
        Argument = __T("-mi");
    if (Argument==__T("--mediatrace"))
        Argument = __T("-mt");
    
    // Report short options
    if (Argument==__T("-mc"))
        Argument = __T("--report=MediaConch");
    if (Argument==__T("-mi"))
        Argument = __T("--report=MediaInfo");
    if (Argument==__T("-mt"))
        Argument = __T("--report=MediaTrace");

    // Format short options
    if (Argument==__T("-ft"))
        Argument = __T("--format=Text");
    if (Argument==__T("-fx"))
        Argument = __T("--format=XML");
    if (Argument==__T("-fa"))
        Argument = __T("--format=MAXML");
    if (Argument==__T("-fj"))
        Argument = __T("--format=JSTREE");
    
    // Listing
    if (0);
    OPTION("--help",                                        Help)
    OPTION("-h",                                            Help)
    OPTION("--version",                                     Version)
    OPTION("--report",                                      Report)
    OPTION("--format",                                      Format)
    OPTION("--output",                                      Output)
    OPTION("--policy",                                      PolicyOption)
    OPTION("--display",                                     Display)
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
    size_t Egal_Pos=Argument.find(__T('='));
    if (Egal_Pos!=String::npos)
    {
        String level=Argument.substr(Egal_Pos+1);
        transform(level.begin(), level.end(), level.begin(), (int(*)(int))tolower); //(int(*)(int)) is a patch for unix

        if (level == __T("advanced"))
            return Help_Advanced();
        else if (level == __T("ssl"))
            return Help_Ssl();
        else if (level == __T("ssh"))
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
    size_t Egal_Pos=Argument.find(__T('='));
    if (Egal_Pos==String::npos)
        return Help();

    // Removing defaults
    if (MI.Report_IsDefault)
    {
        MI.Report.reset();
        MI.Report_IsDefault=false;
    }
        
    // New requested reports
    String Tool=Argument.substr(Egal_Pos+1);
    if (Tool==__T("MediaConch") || Tool==__T("mediaconch"))
        MI.Report.set(MediaConch::Core::report_MediaConch);
    if (Tool==__T("MediaInfo") || Tool==__T("mediainfo"))
        MI.Report.set(MediaConch::Core::report_MediaInfo);
    if (Tool==__T("MediaTrace") || Tool==__T("mediatrace"))
        MI.Report.set(MediaConch::Core::report_MediaTrace);

    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(Format)
{
    //Form : --Inform=Text
    size_t Egal_Pos=Argument.find(__T('='));
    if (Egal_Pos==String::npos)
        return Help();

    String Format=Argument.substr(Egal_Pos+1);
    if (Format==__T("Text") || Format==__T("text"))
        MI.Format=MediaConch::Core::format_Text;
    if (Format==__T("XML") || Format==__T("xml"))
        MI.Format=MediaConch::Core::format_Xml;
    if (Format==__T("MAXML") || Format==__T("maxml"))
        MI.Format=MediaConch::Core::format_MaXml;
    if (Format==__T("JSTREE") || Format==__T("jstree"))
        MI.Format=MediaConch::Core::format_JsTree;

    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(PolicyOption)
{
    //Form : --Inform=Text
    size_t Egal_Pos=Argument.find(__T('='));
    if (Egal_Pos==String::npos)
        return Help_Policy();

    String file;
    file.assign(Argument, Egal_Pos+1, std::string::npos);
    MI.PoliciesFiles.push_back(file);
    Report(MI, __T("--report=MediaConch"));
    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(Display)
{
    //Form : --Inform=Text
    size_t Egal_Pos=Argument.find(__T('='));
    if (Egal_Pos==String::npos)
        return Help();

    String file;
    file.assign(Argument, Egal_Pos+1, std::string::npos);
    MI.xsltDisplay = file;
    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(Output)
{
    //Form : --Inform=Text
    size_t Egal_Pos=Argument.find(__T('='));
    if (Egal_Pos==String::npos)
        return Help_Output();

    MI.Menu_Option_Preferences_Inform(Argument.substr(Egal_Pos+1));

    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(Default)
{
    //Form : --Option=Text
    size_t Egal_Pos=Argument.find(__T('='));
    if (Egal_Pos<2)
        return 0;
    MediaInfoNameSpace::String Option(Argument, 2, Egal_Pos-2);
    MediaInfoNameSpace::String Value;
    if (Egal_Pos!=std::string::npos)
        Value.assign(Argument, Egal_Pos+1, std::string::npos);
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
void CallBack_Set(MediaConch::Core &MI, void* Event_CallBackFunction)
{
    //CallBack configuration
    MI.Menu_Option_Preferences_Option(__T("Event_CallBackFunction"), __T("CallBack=memory://")+ZenLib::Ztring::ToZtring((size_t)Event_CallBackFunction));
}
