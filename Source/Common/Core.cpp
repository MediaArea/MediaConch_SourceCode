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
#include "Common/Schema.h"
#include "Common/Schematron.h"
#include "Common/Xslt.h"
#include "Common/JS_Tree.h"
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

namespace MediaConch {


//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Core::Core() : policies(this)
{
    MI=new MediaInfoNameSpace::MediaInfoList;

    Format=format_Text;
    Tool=tool_MediaConch;
    policies.create_values_from_csv();
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
String Core::Run (String file)
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
                                                            MI->Option(__T("Language"), __T("raw"));
                                                            MI->Option(__T("Details"), __T("1"));
                                                            MI->Option(__T("Inform"), __T("MAXML"));
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
                                    case format_JsTree:
                                                            MI->Option(__T("Language"), __T("raw"));
                                                            MI->Option(__T("Inform"), __T("XML"));
                                                            break;
                                    default:                ;
                                }
                                break;
        case tool_Policies:
                                MI->Option(__T("ReadByHuman"), __T("1"));
                                MI->Option(__T("Details"), __T("0"));
                                MI->Option(__T("Complete"), __T("1"));
                                MI->Option(__T("Language"), __T("raw"));
                                MI->Option(__T("Inform"), __T("MAXML"));
                                break;
        case tool_MediaPolicies:
                                MI->Option(__T("ReadByHuman"), __T("1"));
                                MI->Option(__T("Details"), __T("0"));
                                MI->Option(__T("Complete"), __T("1"));
                                MI->Option(__T("Language"), __T("raw"));
                                MI->Option(__T("Inform"), __T("MAXML"));
                                break;
        default:                return String();
    }

    // Parsing
    MI->Close();
    if (file.length())
    {
        MI->Open(file);
    }
    else
    {
        for (size_t Pos=0; Pos<List.size(); Pos++)
            MI->Open(List[Pos]);
    }

    // Output
    switch (Tool)
    {
        case tool_MediaConch:      return MediaConch();
        case tool_MediaInfo:       return MediaInfo();
        case tool_MediaTrace:      return MediaTrace();
        case tool_MediaPolicies:   return MediaPolicies();
        case tool_Policies:        return PoliciesCheck();
        default:                   return String();
    }
}

//---------------------------------------------------------------------------
String Core::MediaConch ()
{
    if (Format==format_Xml)
        return MI->Inform();
    
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
    String ret = MI->Inform();
    if (Format == format_JsTree)
    {
        JsTree js;
        return js.format_from_XML(ret);
    }
    return ret;
}

//---------------------------------------------------------------------------
String Core::PoliciesCheck()
{
    if (PoliciesFiles.size())
    {
        std::vector<String>& vec = PoliciesFiles;
        std::wstringstream Out;
        for (size_t i = 0; i < vec.size(); ++i)
        {
            std::wstringstream tmp;
            if (is_schematron_file(vec[i]))
            {
                if (!PolicySchematron(vec[i], tmp))
                {
                    String retain = tmp.str();
                    tmp.str(String());
                    if (!PolicyXslt(vec[i], tmp))
                        Out << retain;
                    else
                        Out << tmp.str();
                }
                else
                    Out << tmp.str();
            }
            else
            {
                if (!PolicyXslt(vec[i], tmp))
                {
                    String retain = tmp.str();
                    tmp.str(String());
                    if (!PolicySchematron(vec[i], tmp))
                        Out << retain;
                    else
                        Out << tmp.str();
                }
                else
                    Out << tmp.str();
            }
        }
        return Out.str();
    }

    return __T("No policy to apply");
}

//---------------------------------------------------------------------------
bool Core::PolicySchematron(const String& file, std::wstringstream& Out)
{
    Schema* S = new Schematron;
    std::string f=Ztring(file).To_UTF8();
    bool valid = false;

    Out << file << ": ";
    if (S->register_schema_from_file(f.c_str()))
    {
        String report;
        valid = validation(S, report);

        if (!valid)
        {
            Out << __T("NOT VALID\n");
            Out << report;
        }
        else
            Out << __T("VALID");
    }
    else
    {
        std::vector<std::string> errors = S->get_errors();
        Out << "internal error for parsing file" << std::endl;
        for (size_t pos = 0; pos < errors.size(); pos++)
            Out << "\t" << errors[pos].c_str();
    }
    delete S;
    return valid;
}

//---------------------------------------------------------------------------
bool Core::PolicyXslt(const String& file, std::wstringstream& Out)
{
    Schema *S = new Xslt;
    std::string f=Ztring(file).To_UTF8();
    bool valid = false;

    if (S->register_schema_from_file(f.c_str()))
    {
        String report;
        valid = validation(S, report);
        if (xsltDisplay.length())
            report = Core::transformWithXslt(report, xsltDisplay);
        Out << report;
    }
    else
    {
        std::vector<std::string> errors = S->get_errors();
        Out << file << ": ";
        Out << "internal error for parsing file" << endl;
        for (size_t pos = 0; pos < errors.size(); pos++)
            Out << "\t" << errors[pos].c_str();
    }
    delete S;
    return valid;
}

//---------------------------------------------------------------------------
String Core::MediaPolicies ()
{
    return String();
}

//***************************************************************************
// API
//***************************************************************************

//---------------------------------------------------------------------------
bool Core::ValidatePolicy(int policy, bool& valid, String& report)
{
    if (policy == -1 && PoliciesFiles.size())
    {
        if (is_schematron_file(PoliciesFiles[0]))
        {
            validateSchematronPolicy(-1, valid, report);
            if (!valid)
            {
                String tmp = report;
                validateXsltPolicy(-1, valid, report);
                if (!valid)
                    report = tmp;
            }
        }
        else
        {
            validateXsltPolicy(-1, valid, report);
            if (!valid)
            {
                String tmp;
                validateSchematronPolicy(-1, valid, tmp);
                if (valid)
                    report = tmp;
            }
        }
        return true;
    }

    if (policy == -1)
    {
        report = __T("Policy not found");
        return false;
    }

    if (policies.policies[policy]->type == Policies::POLICY_SCHEMATRON)
        validateSchematronPolicy(policy, valid, report);
    else if (policies.policies[policy]->type == Policies::POLICY_XSLT)
        validateXsltPolicy(policy, valid, report);
    return true;
}

//---------------------------------------------------------------------------
String Core::transformWithXslt(String& report, String& xslt)
{
    Schema *S = new Xslt;

    std::string x(xslt.begin(), xslt.end());
    if (!S->register_schema_from_file(x.c_str()))
        return report;

    std::string re = Ztring(report).To_UTF8();
    int valid = S->validate_xml(re);
    if (valid < 0)
        return report;
    std::string r = S->get_report();
    delete S;
    return String(r.begin(), r.end());
}

//***************************************************************************
// HELPER
//***************************************************************************

//---------------------------------------------------------------------------
void Core::validateSchematronPolicy(int pos, bool& valid, String& report)
{
    std::string file;
    xmlDocPtr doc = NULL;
    Schema *S = new Schematron;

    if (pos < 0)
        file=Ztring(PoliciesFiles[0]).To_UTF8();
    else
        doc = policies.create_doc(pos);

    if (doc && S->register_schema_from_doc(doc))
        valid = validation(S, report);
    else if (!doc && S->register_schema_from_file(file.c_str()))
        valid = validation(S, report);
    else
    {
        valid = false;

        wstringstream Out;
        std::vector<std::string> errors = S->get_errors();

        Out << "internal error for parsing Policies" << endl;
        for (size_t i = 0; i < errors.size(); i++)
            Out << "\t" << errors[i].c_str();
        report = Out.str();
    }
    xmlFreeDoc(doc);
    delete S;
}

//---------------------------------------------------------------------------
void Core::validateXsltPolicy(int pos, bool& valid, String& report)
{
    std::string file;
    xmlDocPtr doc = NULL;
    Schema *S = new Xslt;

    if (pos < 0)
        file=Ztring(PoliciesFiles[0]).To_UTF8();
    else
        doc = policies.create_doc(pos);
    if (doc && S->register_schema_from_doc(doc))
        valid = validation(S, report);
    else if (!doc && S->register_schema_from_file(file.c_str()))
        valid = validation(S, report);
    else
    {
        valid = false;

        wstringstream Out;
        std::vector<std::string> errors = S->get_errors();

        Out << "internal error for parsing Policies" << endl;
        for (size_t i = 0; i < errors.size(); i++)
            Out << "\t" << errors[i].c_str();
        report = Out.str();
    }
    delete S;
}

//---------------------------------------------------------------------------
bool Core::validation(Schema* S, String& report)
{
    String tmp = MI->Inform();
    std::string xml=Ztring(tmp).To_UTF8();
    bool valid = true;

    int ret = S->validate_xml(xml);
    if (ret < 0)
    {
        report = __T("Validation generated an internal error");
        valid = false;
    }
    else
    {
        std::string r = S->get_report();
        report = Ztring().From_UTF8(r);
        valid = ret == 0 ? true : false;
    }
    return valid;
}

bool Core::is_schematron_file(const String& file)
{
    if (file.compare(file.length() - 4, 4, __T(".sch")))
        return false;

    return true;
}

}
