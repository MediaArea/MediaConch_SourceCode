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

//---------------------------------------------------------------------------
#include "Core.h"
#include "Common/Schematron.h"
#include "ZenLib/Ztring.h"
#include "ZenLib/File.h"
#include <sstream>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
const MediaInfoNameSpace::Char* MEDIAINFO_TITLE=__T("MediaArea.net/MediaConch");
ZenLib::Ztring Text_Temp;
using namespace ZenLib;
using namespace std;
//---------------------------------------------------------------------------


//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Core::Core()
{
    MI=new MediaInfoNameSpace::MediaInfoList;

    Format=format_Text;
    Tool=tool_MediaConch;
}

Core::~Core()
{
    delete MI;
}

//***************************************************************************
// Menu
//***************************************************************************

//---------------------------------------------------------------------------
void Core::Menu_Option_Preferences_Inform (const String& Inform)
{
    MI->Option(__T("Inform"), Inform);
}

//---------------------------------------------------------------------------
String Core::Menu_Option_Preferences_Option (const String& Param, const String& Value)
{
    return MI->Option(Param, Value);
}

//***************************************************************************
// Tools
//***************************************************************************

//---------------------------------------------------------------------------
String Core::Run ()
{
    // Config
    switch (Tool)
    {
        case tool_MediaConch: 
                                MI->Option(__T("Complete"), __T("0"));
                                MI->Option(__T("ReadByHuman"), __T("1"));
                                MI->Option(__T("Language"), String());
                                MI->Option(__T("Details"), __T("0"));
                                switch (Format)
                                {
                                    case format_Xml:
                                                            return __T("Conch XML output not yet implemented");
                                    default:                ;
                                }
                                break;
        case tool_MediaInfo:
                                MI->Option(__T("ReadByHuman"), __T("1"));
                                MI->Option(__T("Details"), __T("0"));
                                switch (Format)
                                {
                                    case format_Text:
                                                            MI->Option(__T("Complete"), __T("0"));
                                                            MI->Option(__T("Language"), String());
                                                            MI->Option(__T("Inform"), String());
                                                            break;
                                    case format_Xml:
                                                            MI->Option(__T("Complete"), __T("1"));
                                                            MI->Option(__T("Language"), __T("raw"));
                                                            MI->Option(__T("Inform"), __T("XML"));
                                                            break;
                                    default:                ;
                                }
                                break;
        case tool_MediaTrace:
                                MI->Option(__T("Complete"), __T("0"));
                                MI->Option(__T("ReadByHuman"), __T("1"));
                                MI->Option(__T("Details"), __T("1"));
                                switch (Format)
                                {
                                    case format_Text:
                                                            MI->Option(__T("Language"), String());
                                                            MI->Option(__T("Inform"), String());
                                                            break;
                                    case format_Xml:
                                                            MI->Option(__T("Language"), __T("raw"));
                                                            MI->Option(__T("Inform"), __T("XML"));
                                                            break;
                                    default:                ;
                                }
                                break;
        case tool_MediaSchematron:
                                MI->Option(__T("ReadByHuman"), __T("1"));
                                MI->Option(__T("Details"), __T("0"));
                                MI->Option(__T("Complete"), __T("1"));
                                MI->Option(__T("Language"), __T("raw"));
                                MI->Option(__T("Inform"), __T("XML"));
                                break;
        case tool_MediaPolicies:
                                MI->Option(__T("ReadByHuman"), __T("1"));
                                MI->Option(__T("Details"), __T("0"));
                                MI->Option(__T("Complete"), __T("1"));
                                MI->Option(__T("Language"), __T("raw"));
                                MI->Option(__T("Inform"), __T("XML"));
                                break;
        default:                return String();
    }

    // Parsing
    MI->Close();
    for (size_t Pos=0; Pos<List.size(); Pos++)
        MI->Open(List[Pos]);

    // Output
    switch (Tool)
    {
        case tool_MediaConch:      return MediaConch();
        case tool_MediaInfo:       return MediaInfo();
        case tool_MediaTrace:      return MediaTrace();
        case tool_MediaSchematron: return MediaSchematron();
        case tool_MediaPolicies:   return MediaPolicies();
        default:                   return String();
    }
}

//---------------------------------------------------------------------------
String Core::MediaConch ()
{
    //Output
    wstringstream Out;
    for (size_t FilePos=0; FilePos<MI->Count_Get(); FilePos++)
    {
        Out<<__T("************************************************************************")<<endl;
        Out<<__T("       Container")<<endl;
        Out<<__T("       ---------")<<endl;
        Out<<__T("     | File name                     : ")<<MI->Get(FilePos, Stream_General, 0, __T("CompleteName"))<<endl;
        Out<<(MI->Get(FilePos, Stream_General, 0, __T("Format"))==__T("Matroska")?__T("Pass"):__T("Fail"));
        Out<<    __T(" | Format                        : ")<<MI->Get(FilePos, Stream_General, 0, __T("Format"))<<endl;
        for (size_t StreamPos=0; StreamPos<MI->Count_Get(FilePos, Stream_Video); StreamPos++)
        {
            Out<<endl;
            Out<<__T("       Video")<<endl;
            Out<<__T("       -----")<<endl;
            Out<<(MI->Get(FilePos, Stream_Video, StreamPos, __T("Format"))==__T("FFV1")?__T("Pass"):__T("Fail"));
            Out<<    __T(" | Format                        : ")<<MI->Get(FilePos, Stream_Video, StreamPos, __T("Format"))<<endl;
            if (MI->Get(FilePos, Stream_Video, StreamPos, __T("Format")) == __T("FFV1"))
            {
                Out<<(MI->Get(FilePos, Stream_Video, StreamPos, __T("Format_Version"))!=__T("Version 2")?__T("Pass"):__T("Fail"));
                Out<<    __T(" | Version                       : ")<<MI->Get(FilePos, Stream_Video, StreamPos, __T("Format_Version")).substr(8)<<endl;
                Out<<__T("     | Width                         : ")<<MI->Get(FilePos, Stream_Video, StreamPos, __T("Width"))<<endl;
                Out<<__T("     | Height                        : ")<<MI->Get(FilePos, Stream_Video, StreamPos, __T("Height"))<<endl;
                Out<<__T("     | DAR                           : ")<<MI->Get(FilePos, Stream_Video, StreamPos, __T("DisplayAspectRatio"))<<endl;
                Out<<__T("     | Frame rate                    : ")<<MI->Get(FilePos, Stream_Video, StreamPos, __T("FrameRate"))<<endl;
                Out<<__T("     | Color space                   : ")<<MI->Get(FilePos, Stream_Video, StreamPos, __T("ColorSpace"))<<endl;
                if (!MI->Get(FilePos, Stream_Video, StreamPos, __T("ChromaSubsampling")).empty())
                    Out<<__T("     | Chroma subsampling            : ")<<MI->Get(FilePos, Stream_Video, StreamPos, __T("ChromaSubsampling"))<<endl;
                Out<<__T("     | Bit depth                     : ")<<MI->Get(FilePos, Stream_Video, StreamPos, __T("BitDepth"))<<endl;
                Out<<__T("     | Coder type                    : ")<<MI->Get(FilePos, Stream_Video, StreamPos, __T("coder_type"))<<endl;
                if (MI->Get(FilePos, Stream_Video, StreamPos, __T("Format_Version")).substr(8) >= __T("2"))
                    Out<<__T("     | Max count of slices per frame : ")<<MI->Get(FilePos, Stream_Video, StreamPos, __T("MaxSlicesCount"))<<endl;
                if (MI->Get(FilePos, Stream_Video, StreamPos, __T("Format_Version")).substr(8) >= __T("3.1"))
                    Out<<__T("     | Is intra only                 : ")<<(MI->Get(FilePos, Stream_Video, StreamPos, __T("Format_GOP"))==__T("N=1")?__T("Yes"):__T("No"))<<endl;
                if (MI->Get(FilePos, Stream_Video, StreamPos, __T("Format_Version")).substr(8) >= __T("3.0"))
                {
                    Out<<(!MI->Get(FilePos, Stream_Video, StreamPos, __T("ErrorDetection")).empty()?__T("Pass"):__T("Fail"));
                    Out<<    __T(" | Error detection type          : ")<<(MI->Get(FilePos, Stream_Video, StreamPos, __T("ErrorDetectionType")).empty()?__T("No"):MI->Get(FilePos, Stream_Video, StreamPos, __T("ErrorDetection")))<<endl;
                }
            }
        }
        for (size_t StreamPos=0; StreamPos<MI->Count_Get(FilePos, Stream_Audio); StreamPos++)
        {
            Out<<endl;
            Out<<__T("       Audio")<<endl;
            Out<<__T("       -----")<<endl;
            Out<<__T("     | Format                        : ")<<MI->Get(FilePos, Stream_Audio, StreamPos, __T("Format"))<<endl;
        }
    }

    return Out.str();
}

//---------------------------------------------------------------------------
String Core::MediaInfo ()
{
    return MI->Inform();
}

//---------------------------------------------------------------------------
String Core::MediaTrace ()
{
    return MI->Inform();
}

//---------------------------------------------------------------------------
String Core::MediaSchematron ()
{
    wstringstream Out;
    Schematron S;
    xmlDocPtr doc = NULL;

    if (policies.patterns.size())
    {
        doc = policies.create_doc();
        S.register_schema_from_doc(doc);
    } else {
        std::string file(SchematronFile.begin(), SchematronFile.end());
        S.register_schema_from_file(file.c_str());
    }

    String tmp = MI->Inform();
    std::string xml(tmp.begin(), tmp.end());

    if (!S.validate_xml(xml.c_str(), xml.length()))
    {
        Out << __T("NOT VALID\n");
        for (size_t pos = 0; pos < S.errors.size(); pos++) {
            Out << "\t" << S.errors[pos].c_str();
        }
    } else {
        Out << __T("VALID");
    }
    if (doc)
    {
        xmlFreeDoc(doc);
    }
    return Out.str();
}

//---------------------------------------------------------------------------
String Core::MediaPolicies ()
{
    return String();
}
