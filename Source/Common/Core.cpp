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
#include "Common/ImplementationReportXsl.h"
#include "Common/ImplementationReportDisplayXsl.h"
#include "Common/ImplementationReportDisplayHtmlXsl.h"
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

    Report.set(report_MediaConch);
    Report_IsDefault=true;
    Format=format_Text;
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
String Core::ReportAndFormatCombination_IsValid ()
{
    // Forcing some formats
    if (List.size()>1 && Format==format_Xml)
        Format=format_MaXml;
    size_t Report_Count=0;
    for (size_t i=0; i<report_Max; i++)
        if (Report[i])
            Report_Count++;
    if (Report_Count>1 && Format==format_Xml)
        Format=format_MaXml;
    
    // Test of incompatibilities
    if (Report[report_MediaConch] && Report[report_MediaTrace] && Format==format_Text)
        return __T("Combination of MediaConch and MediaTrace is currently not possible with Text output");
    if (Report[report_MediaConch] && Report[report_MediaInfo] && Format==format_MaXml)
        return __T("Combination of MediaConch and MediaInfo or MediaTrace is currently not possible with XML output");
    if (Report[report_MediaConch] && Format==format_MaXml)
        return __T("MediaConch is currently not possible with MAXML output");

    return String();
}

//---------------------------------------------------------------------------
void Core::Close ()
{
    MI->Close();
}

//---------------------------------------------------------------------------
void Core::Run (String file)
{
    // Configuration of the parsing
    if (Report[report_MediaConch] || Report[report_MediaTrace])
        MI->Option(__T("Details"), __T("1"));

    // Partial configuration of the output (note: this options should be removed after libmediainfo has a support of these options after Open() )
    switch (Format)
    {
        case format_Text:
                                MI->Option(__T("ReadByHuman"), __T("0"));
                                break;
        default:
                                MI->Option(__T("ReadByHuman"), __T("1"));
    }
    switch (Format)
    {
        case format_Xml:
        case format_JsTree:
                                MI->Option(__T("Language"), __T("raw"));
                                MI->Option(__T("Inform"), __T("XML"));
                                break;
        case format_MaXml:
                                MI->Option(__T("Inform"), __T("MAXML"));
                                break;
        default:
                                MI->Option(__T("Language"), String());
                                MI->Option(__T("Inform"), String());
    }
    if (Report[report_MediaConch])
    {
                                MI->Option(__T("Inform"), __T("MAXML")); // MediaConch report is always based on MAXML output
    }

    // Parsing
    if (file.length())
    {
        MI->Open(file);
    }
    else
    {
        for (size_t Pos=0; Pos<List.size(); Pos++)
            MI->Open(List[Pos]);
    }
}

//---------------------------------------------------------------------------
String Core::GetOutput()
{
    if (!PoliciesFiles.empty())
        return PoliciesCheck();

    switch (Format)
    {
        case format_Text:           return GetOutput_Text();
        case format_Xml:            return GetOutput_Xml();
        case format_MaXml:          return GetOutput_MaXml();
        case format_JsTree:         return GetOutput_JStree();
        case format_Html:           return GetOutput_Html();
        default:                    return String();
    }
}

//---------------------------------------------------------------------------
String Core::GetOutput_Text ()
{
    String ret;

    if (Report[report_MediaInfo])
    {
        MI->Option(__T("Details"), __T("0"));
        MI->Option(__T("Inform"), String());

        ret += MI->Inform();

        ret += __T("\r\n");
    }

    if (Report[report_MediaTrace])
    {
        MI->Option(__T("Details"), __T("1"));
        MI->Option(__T("Inform"), String());

        ret += MI->Inform();

        ret += __T("\r\n");
    }

    if (Report[report_MediaConch])
    {
        MI->Option(__T("Details"), __T("0"));
        MI->Option(__T("Inform"), __T("MAXML")); // MediaConch report is always based on MAXML output

        ret += GetOutput_Text_Implementation();
        ret += __T("\r\n");
    }

    return ret;
}

//---------------------------------------------------------------------------
String Core::GetOutput_Xml ()
{
    String ret;

    if (Report[report_MediaInfo] && !Report[report_MediaTrace])
    {
        MI->Option(__T("Details"), __T("0"));
        MI->Option(__T("Inform"), __T("MAXML")); //TODO: "XML" when new XML output is ready

        ret += MI->Inform();

        ret += __T("\r\n");
    }

    if (Report[report_MediaTrace] && !Report[report_MediaInfo])
    {
        MI->Option(__T("Details"), __T("1"));
        MI->Option(__T("Inform"), __T("XML"));

        ret += MI->Inform();

        ret += __T("\r\n");
    }

    if (Report[report_MediaTrace] && Report[report_MediaInfo])
    {
        MI->Option(__T("Details"), __T("1"));
        MI->Option(__T("Inform"), __T("MAXML"));

        ret += MI->Inform();

        ret += __T("\r\n");
    }

    if (Report[report_MediaConch])
    {
        MI->Option(__T("Details"), __T("0"));
        MI->Option(__T("Inform"), __T("MAXML")); // MediaConch report is always based on MAXML output

        ret += GetOutput_Xml_Implementation();
        ret += __T("\r\n");
    }

    return ret;
}

//---------------------------------------------------------------------------
String Core::GetOutput_MaXml ()
{
    String ret;

    MI->Option(__T("Details"), Report[report_MediaTrace]?__T("1"):__T("0"));
    MI->Option(__T("Inform"), __T("MAXML"));

    ret += MI->Inform();

    return ret;
}

//---------------------------------------------------------------------------
String Core::GetOutput_JStree ()
{
    JsTree js;
    if (Report[report_MediaInfo])
    {
        MI->Option(__T("Details"), __T("0"));
        MI->Option(__T("Inform"), __T("MAXML"));

        const String& ret = MI->Inform();
        return js.format_from_inform_XML(ret);
    }

    if (Report[report_MediaTrace])
    {
        MI->Option(__T("Details"), __T("1"));
        MI->Option(__T("Inform"), __T("XML"));

        const String& ret = MI->Inform();
        return js.format_from_trace_XML(ret);
    }

    return String();
}

//---------------------------------------------------------------------------
String Core::GetOutput_Html ()
{
    if (Report[report_MediaConch])
    {
        MI->Option(__T("Details"), __T("0"));
        MI->Option(__T("Inform"), __T("MAXML")); // MediaConch report is always based on MAXML output

        String report;
        bool valid;
        std::string memory(implementation_report_xsl);
        validateXsltPolicyFromMemory(memory, valid, report);

        // Apply an XSLT to have HTML
        memory = std::string(implementation_report_display_html_xsl);
        report = transformWithXsltMemory(report, memory);
        return report;
    }

    return String();
}

//---------------------------------------------------------------------------
String Core::GetOutput_Text_Implementation ()
{
    String report;
    bool valid;
    std::string memory(implementation_report_xsl);
    validateXsltPolicyFromMemory(memory, valid, report);

    // Apply an XSLT to have Text
    memory = std::string(implementation_report_display_xsl);
    report = transformWithXsltMemory(report, memory);
    return report;
}

//---------------------------------------------------------------------------
String Core::GetOutput_Xml_Implementation ()
{
    String report;
    bool valid;
    std::string memory(implementation_report_xsl);
    validateXsltPolicyFromMemory(memory, valid, report);

    if (!valid)
        return report;

    // Apply an XSLT to have XML
    return report;
}

//---------------------------------------------------------------------------
String Core::PoliciesCheck()
{
    MI->Option(__T("Complete"), __T("0"));
    MI->Option(__T("Language"), __T("raw"));
    MI->Option(__T("Details"), __T("1"));
    MI->Option(__T("Inform"), __T("MAXML"));

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
            report = Core::transformWithXsltFile(report, xsltDisplay);
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

//***************************************************************************
// API
//***************************************************************************

//---------------------------------------------------------------------------
bool Core::ValidatePolicy(int policy, bool& valid, String& report)
{
    MI->Option(__T("Complete"), __T("0"));
    MI->Option(__T("Language"), __T("raw"));
    MI->Option(__T("Details"), __T("1"));
    MI->Option(__T("Inform"), __T("MAXML"));

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
String Core::transformWithXsltFile(String& report, String& xslt)
{
    Schema *S = new Xslt;

    std::string x = Ztring(xslt).To_UTF8();
    if (!S->register_schema_from_file(x.c_str()))
        return report;

    std::string re = Ztring(report).To_UTF8();
    int valid = S->validate_xml(re);
    if (valid < 0)
        return report;
    std::string r = S->get_report();
    delete S;
    return Ztring().From_UTF8(r);
}

//---------------------------------------------------------------------------
String Core::transformWithXsltMemory(String& report, std::string& memory)
{
    Schema *S = new Xslt;

    if (!S->register_schema_from_memory(memory))
        return report;

    std::string re = Ztring(report).To_UTF8();
    int valid = S->validate_xml(re);
    if (valid < 0)
        return report;
    std::string r = S->get_report();
    delete S;
    return Ztring().From_UTF8(r);
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
void Core::validateXsltPolicyFromMemory(const std::string& memory, bool& valid, String& report)
{
    Schema *S = new Xslt;

    if (S->register_schema_from_memory(memory))
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
