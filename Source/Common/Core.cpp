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
#include "SQLLite.h"
#include "Common/Schematron.h"
#include "Common/Xslt.h"
#include "Common/JS_Tree.h"
#include "Common/ImplementationReportXsl.h"
#if defined(_WIN32) || defined(WIN32)
#include "Common/ImplementationReportDisplayTextXsl.h"
#else //defined(_WIN32) || defined(WIN32)
#include "Common/ImplementationReportDisplayTextUnicodeXsl.h"
#endif //defined(_WIN32) || defined(WIN32)
#include "Common/ImplementationReportDisplayHtmlXsl.h"
#include "ZenLib/Ztring.h"
#include "ZenLib/File.h"
#include <sstream>
#include <sys/stat.h>
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
    db = new SQLLite;
}

Core::~Core()
{
    delete db;
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
    if (Format==format_Text && !xsltDisplay.empty())
        Format=format_Xml; //Forcing XML output in order to apply the Display XSL
    
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
    // Currently avoiding to have a big trace
    MI->Option(__T("ParseSpeed"), __T("0"));
    
    // Configuration of the parsing
    MI->Option(__T("Details"), __T("1"));

    // Partial configuration of the output (note: this options should be removed after libmediainfo has a support of these options after Open() )
    MI->Option(__T("ReadByHuman"), __T("1"));
    MI->Option(__T("Language"), __T("raw"));
    MI->Option(__T("Inform"), __T("XML"));

    // Parsing
    if (file.length())
    {
        bool registered = file_is_registered_in_db(file);
        if (!registered)
        {
            MI->Open(file);
            register_file_to_database(file);
            MI->Close();
        }
    }
    else
    {
        for (size_t Pos=0; Pos<List.size(); Pos++)
        {
            bool registered = file_is_registered_in_db(file);
            if (!registered)
            {
                MI->Open(List[Pos]);
                register_file_to_database(List[Pos]);
                MI->Close();
            }
        }
    }
}

//---------------------------------------------------------------------------
String Core::GetOutput()
{
    if (!PoliciesFiles.empty())
        return PoliciesCheck();

    String report;
    for (size_t i = 0; i < List.size(); ++i)
    {
        switch (Format)
        {
            case format_Text:
            case format_Xml:
            case format_MaXml:
                get_Reports_Output(List[i], report);
                break;
            case format_JsTree:
                GetOutput_JStree(List[i], report);
                break;
            case format_Html:
                GetOutput_Html(List[i], report);
                break;
            default:
                break;
        }
    }
    return report;
}

//---------------------------------------------------------------------------
String Core::GetOutput_Text (const String& file)
{
    String report;
    Format = format_Text;
    get_Reports_Output(file, report);
    return report;
}

//---------------------------------------------------------------------------
String Core::GetOutput_Xml (const String& file)
{
    String report;
    Format = format_Xml;
    get_Reports_Output(file, report);
    return report;
}

//---------------------------------------------------------------------------
void Core::GetOutput_JStree (const String& file, String& report)
{
    JsTree js;
    if (Report[report_MediaInfo])
    {
        String ret = get_report_saved(file, report_MediaInfo, format_Xml);
        report += js.format_from_inform_XML(ret);
        return;
    }

    if (Report[report_MediaTrace])
    {
        String ret = get_report_saved(file, report_MediaTrace, format_Xml);
        report += js.format_from_trace_XML(ret);
        return;
    }
}

//---------------------------------------------------------------------------
void Core::GetOutput_Html (const String& file, String& report)
{
    if (Report[report_MediaConch])
    {
        String tmp;
        bool valid;
        std::string memory(implementation_report_xsl);
        validateXsltPolicyFromMemory(file, memory, valid, tmp);

        // Apply an XSLT to have HTML
        memory = std::string(implementation_report_display_html_xsl);
        report += transformWithXsltMemory(tmp, memory);
    }
}

//---------------------------------------------------------------------------
String Core::GetOutput_Text_Implementation (const String& file)
{
    String report;
    bool valid;
    std::string memory(implementation_report_xsl);
    validateXsltPolicyFromMemory(file, memory, valid, report);

    // Apply an XSLT to have Text
#if defined(_WIN32) || defined(WIN32)
    memory = std::string(implementation_report_display_text_xsl);
#else //defined(_WIN32) || defined(WIN32)
    memory = std::string(implementation_report_display_textunicode_xsl);
#endif //defined(_WIN32) || defined(WIN32)
    report = transformWithXsltMemory(report, memory);
    return report;
}

//---------------------------------------------------------------------------
String Core::GetOutput_Xml_Implementation (const String& file)
{
    String report;
    bool valid;
    std::string memory(implementation_report_xsl);
    validateXsltPolicyFromMemory(file, memory, valid, report);

    // Apply an XSLT to have XML
    if (xsltDisplay.length())
        report = Core::transformWithXsltFile(report, xsltDisplay);
    return report;
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
        valid = validation(file, S, report);

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
        valid = validation(file, S, report);
        if (xsltDisplay.length())
            report = Core::transformWithXsltFile(report, xsltDisplay);
        else 
            switch (Format)
            {
                case format_Text:
                                    {
                                        // Apply an XSLT to have Text
                                    #if defined(_WIN32) || defined(WIN32)
                                        std::string memory(implementation_report_display_text_xsl);
                                    #else //defined(_WIN32) || defined(WIN32)
                                        std::string memory(implementation_report_display_textunicode_xsl);
                                    #endif //defined(_WIN32) || defined(WIN32)
                                        report = transformWithXsltMemory(report, memory);
                                    }
                                    break;
                case format_Html:
                                    {
                                        // Apply an XSLT to have Text
                                        std::string memory(implementation_report_display_html_xsl);
                                        report = transformWithXsltMemory(report, memory);
                                    }
                                    break;
                default:            ;
            }

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
bool Core::ValidatePolicy(const String& file, int policy, bool& valid, String& report)
{
    if (policy == -1 && PoliciesFiles.size())
    {
        if (is_schematron_file(PoliciesFiles[0]))
        {
            validateSchematronPolicy(file, -1, valid, report);
            if (!valid)
            {
                String tmp = report;
                validateXsltPolicy(file, -1, valid, report);
                if (!valid)
                    report = tmp;
            }
        }
        else
        {
            validateXsltPolicy(file, -1, valid, report);
            if (!valid)
            {
                String tmp;
                validateSchematronPolicy(file, -1, valid, tmp);
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
        validateSchematronPolicy(file, policy, valid, report);
    else if (policies.policies[policy]->type == Policies::POLICY_XSLT)
        validateXsltPolicy(file, policy, valid, report);
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
void Core::validateSchematronPolicy(const String& file, int pos, bool& valid, String& report)
{
    std::string policyFile;
    xmlDocPtr doc = NULL;
    Schema *S = new Schematron;

    if (pos < 0)
        policyFile=Ztring(PoliciesFiles[0]).To_UTF8();
    else
        doc = policies.create_doc(pos);

    if (doc && S->register_schema_from_doc(doc))
        valid = validation(file, S, report);
    else if (!doc && S->register_schema_from_file(policyFile.c_str()))
        valid = validation(file, S, report);
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
void Core::validateXsltPolicy(const String& file, int pos, bool& valid, String& report)
{
    std::string policyFile;
    xmlDocPtr doc = NULL;
    Schema *S = new Xslt;

    if (pos < 0)
        policyFile=Ztring(PoliciesFiles[0]).To_UTF8();
    else
        doc = policies.create_doc(pos);
    if (doc && S->register_schema_from_doc(doc))
        valid = validation(file, S, report);
    else if (!doc && S->register_schema_from_file(policyFile.c_str()))
        valid = validation(file, S, report);
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
void Core::validateXsltPolicyFromMemory(const String& file, const std::string& memory, bool& valid, String& report)
{
    Schema *S = new Xslt;

    if (S->register_schema_from_memory(memory))
        valid = validation(file, S, report);
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
bool Core::validation(const String& file, Schema* S, String& report)
{
    String tmp;
    tmp = get_report_saved(file, report_MediaConch, format_MaXml);
    std::string xml = Ztring(tmp).To_UTF8();
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

//---------------------------------------------------------------------------
time_t Core::get_last_modification_file(const std::string& filename)
{
    struct stat stat_base;
    if (lstat(filename.c_str(), &stat_base))
        return 0;
    return stat_base.st_mtime;
}

//---------------------------------------------------------------------------
void Core::register_report_mediainfo_text_to_database(std::string& file, time_t time)
{
    MI->Option(__T("Details"), __T("0"));
    MI->Option(__T("Inform"), String());
    std::string report = Ztring(MI->Inform()).To_UTF8();
    db->save_report(report_MediaInfo, format_Text, file, time, report);
}

//---------------------------------------------------------------------------
void Core::register_report_mediainfo_xml_to_database(std::string& file, time_t time)
{
    MI->Option(__T("Details"), __T("0"));
    MI->Option(__T("Inform"), __T("MIXML"));
    std::string report = Ztring(MI->Inform()).To_UTF8();
    db->save_report(report_MediaInfo, format_Xml, file, time, report);
}

//---------------------------------------------------------------------------
void Core::register_report_mediatrace_text_to_database(std::string& file, time_t time)
{
    MI->Option(__T("Details"), __T("1"));
    MI->Option(__T("Inform"), String());
    std::string report = Ztring(MI->Inform()).To_UTF8();
    //printf("reporttext=%s\n\n\n\n\n", report.c_str());
    db->save_report(report_MediaTrace, format_Text, file, time, report);
}

//---------------------------------------------------------------------------
void Core::register_report_mediatrace_xml_to_database(std::string& file, time_t time)
{
    MI->Option(__T("Details"), __T("1"));
    MI->Option(__T("Inform"), __T("XML"));
    std::string report = Ztring(MI->Inform()).To_UTF8();
    db->save_report(report_MediaTrace, format_Xml, file, time, report);
}

//---------------------------------------------------------------------------
void Core::register_report_mediainfo_and_mediatrace_xml_to_database(std::string& file, time_t time)
{
    MI->Option(__T("Details"), __T("1"));
    MI->Option(__T("Inform"), __T("MAXML"));
    std::string report = Ztring(MI->Inform()).To_UTF8();
    db->save_report(report_MediaConch, format_MaXml, file, time, report);
}

//---------------------------------------------------------------------------
void Core::register_file_to_database(String& file)
{
    std::string filename = Ztring(file).To_UTF8();
    time_t time = get_last_modification_file(filename);

    // MediaInfo
    register_report_mediainfo_text_to_database(filename, time);
    register_report_mediainfo_xml_to_database(filename, time);

    // MediaTrace
    register_report_mediatrace_text_to_database(filename, time);
    register_report_mediatrace_xml_to_database(filename, time);

    // MediaInfo + MediaTrace (MaXML)
    register_report_mediainfo_and_mediatrace_xml_to_database(filename, time);
}

//---------------------------------------------------------------------------
String Core::get_report_saved(const String& file, report reportKind, format f)
{
    if (reportKind >= report_Max || f >= format_Max)
        return String();

    std::string filename = Ztring(file).To_UTF8();
    time_t time = get_last_modification_file(filename);
    std::string report = db->get_report(reportKind, f, filename, time);

    return Ztring().From_UTF8(report);
}

//---------------------------------------------------------------------------
void Core::get_Reports_Output(const String& file, String& report)
{
    if (Report[report_MediaConch])
    {
        if (Format == format_Xml)
            report += GetOutput_Xml_Implementation(file);
        else
            report += GetOutput_Text_Implementation(file);
        report += __T("\r\n");
    }

    if (Report[report_MediaTrace] && Report[report_MediaInfo] && Format == format_MaXml)
    {
        report += get_report_saved(file, report_MediaConch, Format);
        report += __T("\r\n");
    }
    else
    {
        if (Report[report_MediaInfo])
        {
            report += get_report_saved(file, report_MediaInfo, Format);
            report += __T("\r\n");
        }
        if (Report[report_MediaTrace])
        {
            report += get_report_saved(file, report_MediaTrace, Format);
            report += __T("\r\n");
        }
    }
}

//---------------------------------------------------------------------------
bool Core::file_is_registered_in_db(String& file)
{
    std::string filename = Ztring(file).To_UTF8();
    time_t time = get_last_modification_file(filename);

    return db->file_is_registered(filename, time);
}

}
