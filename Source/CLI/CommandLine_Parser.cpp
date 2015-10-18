/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#include <string>
#include <vector>
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
    if (Argument==__T("-ti"))
        Argument = __T("--tool=Info");
    if (Argument==__T("-tt"))
        Argument = __T("--tool=Trace");
    if (Argument==__T("-fx"))
        Argument = __T("--format=XML");
    if (Argument==__T("-fj"))
        Argument = __T("--format=JSTREE");
    if (0);
    OPTION("--help",                                        Help)
    OPTION("-h",                                            Help)
    OPTION("--version",                                     Version)
    OPTION("--tool",                                        Tool)
    OPTION("--format",                                      Format)
    OPTION("--output",                                      Output)
    OPTION("--policy",                                      Policies)
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
    (void)Argument;
    Version();
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
CL_OPTION(Tool)
{
    //Form : --Inform=Text
    size_t Egal_Pos=Argument.find(__T('='));
    if (Egal_Pos==String::npos)
        return Help_Output();

    String Tool=Argument.substr(Egal_Pos+1);
    if (Tool==__T("Info") || Tool==__T("info"))
        MI.Tool=MediaConch::Core::tool_MediaInfo;
    if (Tool==__T("Trace") || Tool==__T("trace"))
        MI.Tool=MediaConch::Core::tool_MediaTrace;

    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(Format)
{
    //Form : --Inform=Text
    size_t Egal_Pos=Argument.find(__T('='));
    if (Egal_Pos==String::npos)
        return Help_Output();

    String Format=Argument.substr(Egal_Pos+1);
    if (Format==__T("XML") || Format==__T("xml"))
        MI.Format=MediaConch::Core::format_Xml;
    if (Format==__T("JSTREE") || Format==__T("jstree"))
        MI.Format=MediaConch::Core::format_JsTree;

    return 0;
}

//---------------------------------------------------------------------------
CL_OPTION(Policies)
{
    //Form : --Inform=Text
    size_t Egal_Pos=Argument.find(__T('='));
    if (Egal_Pos==String::npos)
        return Help_Policy();

    String file;
    file.assign(Argument, Egal_Pos+1, std::string::npos);
    MI.PoliciesFiles.push_back(file);
    MI.Tool=MediaConch::Core::tool_Policies;
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
