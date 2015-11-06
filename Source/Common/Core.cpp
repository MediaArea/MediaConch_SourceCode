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
#include "Database.h"
#include "SQLLite.h"
#include "Configuration.h"
#include "Json.h"
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
#include <fstream>
#include <sys/stat.h>
//---------------------------------------------------------------------------

#if defined(UNIX)
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

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
    config = NULL;
    db = NULL;
    scheduler = new Scheduler(this);
}

Core::~Core()
{
    if (scheduler)
        delete scheduler;
    if (db)
        delete db;
    delete MI;
    if (config)
        delete config;
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
// General
//***************************************************************************

//---------------------------------------------------------------------------
void Core::load_configuration()
{
    config = new Configuration;
    std::string config_path = get_config_path();
    config->set_path(config_path);
    config->parse();

    long scheduler_max_threads = 1;
    if (scheduler && !config->get("Scheduler_Max_Threads", scheduler_max_threads))
    {
        if (scheduler_max_threads <= 0)
            scheduler_max_threads = 1;
        scheduler->set_max_threads((size_t)scheduler_max_threads);
    }
}

//---------------------------------------------------------------------------
void Core::load_database()
{
#ifdef HAVE_SQLITE
    std::string db_path;
    if (!config || config->get("SQLite_Path", db_path))
        db_path = std::string(".");
    db_path += Path_Separator;

    db = new SQLLite;

    db->set_database_directory(db_path);
    db->init();
#endif
}

//***************************************************************************
// Tools
//***************************************************************************

//---------------------------------------------------------------------------
std::string Core::ReportAndFormatCombination_IsValid ()
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
        return "Combination of MediaConch and MediaTrace is currently not possible with Text output";
    if (Report[report_MediaConch] && Report[report_MediaInfo] && Format==format_MaXml)
        return "Combination of MediaConch and MediaInfo or MediaTrace is currently not possible with XML output";
    if (Report[report_MediaConch] && Format==format_MaXml)
        return "MediaConch is currently not possible with MAXML output";

    return std::string();
}

//---------------------------------------------------------------------------
void Core::Close ()
{
    MI->Close();
}

//---------------------------------------------------------------------------
void Core::Run (std::string file)
{
    // Parsing
    if (file.length())
        open_file(file);
    else
    {
        for (size_t Pos=0; Pos<List.size(); Pos++)
            open_file(List[Pos]);
    }
}

//---------------------------------------------------------------------------
void Core::open_file(std::string& file)
{
    bool registered = file_is_registered_in_db(file);
    if (!registered)
    {
        if (get_db())
            scheduler->add_element_to_queue(file);
        else
            MI->Open(Ztring().From_UTF8(file));
    }
}

//---------------------------------------------------------------------------
std::string Core::GetOutput()
{
    if (!PoliciesFiles.empty())
        return PoliciesCheck();

    std::string report;
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
std::string Core::GetOutput_Text (const std::string& file)
{
    std::string report;
    Format = format_Text;
    get_Reports_Output(file, report);
    return report;
}

//---------------------------------------------------------------------------
std::string Core::GetOutput_Xml (const std::string& file)
{
    std::string report;
    Format = format_Xml;
    get_Reports_Output(file, report);
    return report;
}

//---------------------------------------------------------------------------
void Core::GetOutput_JStree (const std::string& file, std::string& report)
{
    JsTree js;
    if (Report[report_MediaInfo])
    {
        std::string ret = get_report_saved(file, report_MediaInfo, format_Xml);
        report += js.format_from_inform_XML(ret);
        return;
    }

    if (Report[report_MediaTrace])
    {
        std::string ret = get_report_saved(file, report_MediaTrace, format_Xml);
        report += js.format_from_trace_XML(ret);
        return;
    }
}

//---------------------------------------------------------------------------
void Core::GetOutput_Html (const std::string& file, std::string& report)
{
    if (Report[report_MediaConch])
    {
        std::string tmp;
        bool valid;
        std::string memory(implementation_report_xsl);
        validateXsltPolicyFromMemory(file, memory, valid, tmp);

        // Apply an XSLT to have HTML
        memory = std::string(implementation_report_display_html_xsl);
        report += transformWithXsltMemory(tmp, memory);
    }
}

//---------------------------------------------------------------------------
std::string Core::GetOutput_Text_Implementation (const std::string& file)
{
    std::string report;
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
std::string Core::GetOutput_Xml_Implementation (const std::string& file)
{
    std::string report;
    bool valid;
    std::string memory(implementation_report_xsl);
    validateXsltPolicyFromMemory(file, memory, valid, report);

    // Apply an XSLT to have XML
    if (xsltDisplay.length())
        report = Core::transformWithXsltFile(report, xsltDisplay);
    return report;
}

//---------------------------------------------------------------------------
std::string Core::PoliciesCheck()
{
    if (PoliciesFiles.size())
    {
        std::vector<std::string>& vec = PoliciesFiles;
        std::stringstream Out;
        for (size_t i = 0; i < vec.size(); ++i)
        {
            std::stringstream tmp;
            if (is_schematron_file(vec[i]))
            {
                if (!PolicySchematron(vec[i], tmp))
                {
                    std::string retain = tmp.str();
                    tmp.str(std::string());
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
                    std::string retain = tmp.str();
                    tmp.str(std::string());
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

    return "No policy to apply";
}

//---------------------------------------------------------------------------
bool Core::PolicySchematron(const std::string& file, std::stringstream& Out)
{
    Schema* S = new Schematron;
    bool valid = false;

    Out << file << ": ";
    if (S->register_schema_from_file(file.c_str()))
    {
        std::string report;
        valid = validation(file, S, report);

        if (!valid)
        {
            Out << "NOT VALID\n";
            Out << report;
        }
        else
            Out << "VALID";
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
bool Core::PolicyXslt(const std::string& file, std::stringstream& Out)
{
    Schema *S = new Xslt;
    bool valid = false;

    if (S->register_schema_from_file(file.c_str()))
    {
        std::string report;
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
bool Core::ValidatePolicy(const std::string& file, int policy, bool& valid, std::string& report)
{
    if (policy == -1 && PoliciesFiles.size())
    {
        if (is_schematron_file(PoliciesFiles[0]))
        {
            validateSchematronPolicy(file, -1, valid, report);
            if (!valid)
            {
                std::string tmp = report;
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
                std::string tmp;
                validateSchematronPolicy(file, -1, valid, tmp);
                if (valid)
                    report = tmp;
            }
        }
        return true;
    }

    if (policy == -1)
    {
        report = "Policy not found";
        return false;
    }

    if (policies.policies[policy]->type == Policies::POLICY_SCHEMATRON)
        validateSchematronPolicy(file, policy, valid, report);
    else if (policies.policies[policy]->type == Policies::POLICY_XSLT)
        validateXsltPolicy(file, policy, valid, report);
    return true;
}

//---------------------------------------------------------------------------
std::string Core::transformWithXsltFile(std::string& report, std::string& xslt)
{
    Schema *S = new Xslt;

    if (!S->register_schema_from_file(xslt.c_str()))
        return report;

    int valid = S->validate_xml(report);
    if (valid < 0)
        return report;

    std::string r = S->get_report();
    delete S;
    return r;
}

//---------------------------------------------------------------------------
std::string Core::transformWithXsltMemory(std::string& report, std::string& memory)
{
    Schema *S = new Xslt;

    if (!S->register_schema_from_memory(memory))
        return report;

    int valid = S->validate_xml(report);
    if (valid < 0)
        return report;
    std::string r = S->get_report();
    delete S;
    return r;
}

//***************************************************************************
// HELPER
//***************************************************************************

//---------------------------------------------------------------------------
void Core::validateSchematronPolicy(const std::string& file, int pos, bool& valid, std::string& report)
{
    std::string policyFile;
    xmlDocPtr doc = NULL;
    Schema *S = new Schematron;

    if (pos >= 0)
        doc = policies.create_doc(pos);

    if (doc && S->register_schema_from_doc(doc))
        valid = validation(file, S, report);
    else if (!doc && PoliciesFiles.size() > 0 && S->register_schema_from_file(PoliciesFiles[0].c_str()))
        valid = validation(file, S, report);
    else
    {
        valid = false;

        std::stringstream Out;
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
void Core::validateXsltPolicy(const std::string& file, int pos, bool& valid, std::string& report)
{
    std::string policyFile;
    xmlDocPtr doc = NULL;
    Schema *S = new Xslt;

    if (pos >= 0)
        doc = policies.create_doc(pos);

    if (doc && S->register_schema_from_doc(doc))
        valid = validation(file, S, report);
    else if (!doc && PoliciesFiles.size() > 0 && S->register_schema_from_file(PoliciesFiles[0].c_str()))
        valid = validation(file, S, report);
    else
    {
        valid = false;

        std::stringstream Out;
        std::vector<std::string> errors = S->get_errors();

        Out << "internal error for parsing Policies" << endl;
        for (size_t i = 0; i < errors.size(); i++)
            Out << "\t" << errors[i].c_str();
        report = Out.str();
    }
    delete S;
}

//---------------------------------------------------------------------------
void Core::validateXsltPolicyFromMemory(const std::string& file, const std::string& memory, bool& valid, std::string& report)
{
    Schema *S = new Xslt;

    if (S->register_schema_from_memory(memory))
        valid = validation(file, S, report);
    else
    {
        valid = false;

        std::stringstream Out;
        std::vector<std::string> errors = S->get_errors();

        Out << "internal error for parsing Policies" << endl;
        for (size_t i = 0; i < errors.size(); i++)
            Out << "\t" << errors[i].c_str();
        report = Out.str();
    }
    delete S;
}

//---------------------------------------------------------------------------
bool Core::validation(const std::string& file, Schema* S, std::string& report)
{
    std::string xml = get_report_saved(file, report_MediaConch, format_MaXml);
    bool valid = true;

    int ret = S->validate_xml(xml);
    if (ret < 0)
    {
        report = "Validation generated an internal error";
        valid = false;
    }
    else
    {
        report = S->get_report();
        valid = ret == 0 ? true : false;
    }
    return valid;
}

bool Core::is_schematron_file(const std::string& file)
{
    if (file.compare(file.length() - 4, 4, ".sch"))
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
void Core::register_report_mediainfo_text_to_database(std::string& file, time_t time,
                                                      MediaInfoNameSpace::MediaInfoList* curMI)
{
    curMI->Option(__T("Details"), __T("0"));
    curMI->Option(__T("Inform"), String());
    std::string report = Ztring(curMI->Inform()).To_UTF8();
    db->save_report(report_MediaInfo, format_Text, file, time, report);
}

//---------------------------------------------------------------------------
void Core::register_report_mediainfo_xml_to_database(std::string& file, time_t time,
                                                     MediaInfoNameSpace::MediaInfoList* curMI)
{
    curMI->Option(__T("Details"), __T("0"));
    curMI->Option(__T("Inform"), __T("MIXML"));
    std::string report = Ztring(curMI->Inform()).To_UTF8();
    db->save_report(report_MediaInfo, format_Xml, file, time, report);
}

//---------------------------------------------------------------------------
void Core::register_report_mediatrace_text_to_database(std::string& file, time_t time,
                                                       MediaInfoNameSpace::MediaInfoList* curMI)
{
    curMI->Option(__T("Details"), __T("1"));
    curMI->Option(__T("Inform"), String());
    std::string report = Ztring(curMI->Inform()).To_UTF8();
    db->save_report(report_MediaTrace, format_Text, file, time, report);
}

//---------------------------------------------------------------------------
void Core::register_report_mediatrace_xml_to_database(std::string& file, time_t time,
                                                      MediaInfoNameSpace::MediaInfoList* curMI)
{
    curMI->Option(__T("Details"), __T("1"));
    curMI->Option(__T("Inform"), __T("XML"));
    std::string report = Ztring(curMI->Inform()).To_UTF8();
    db->save_report(report_MediaTrace, format_Xml, file, time, report);
}

//---------------------------------------------------------------------------
void Core::register_report_mediainfo_and_mediatrace_xml_to_database(std::string& file, time_t time,
                                                                    MediaInfoNameSpace::MediaInfoList* curMI)
{
    curMI->Option(__T("Details"), __T("1"));
    curMI->Option(__T("Inform"), __T("MAXML"));
    std::string report = Ztring(curMI->Inform()).To_UTF8();
    db->save_report(report_MediaConch, format_MaXml, file, time, report);
}

//---------------------------------------------------------------------------
void Core::register_file_to_database(std::string& filename, MediaInfoNameSpace::MediaInfoList* curMI)
{
    time_t time = get_last_modification_file(filename);

    // MediaInfo
    register_report_mediainfo_text_to_database(filename, time, curMI);
    register_report_mediainfo_xml_to_database(filename, time, curMI);

    // MediaTrace
    register_report_mediatrace_text_to_database(filename, time, curMI);
    register_report_mediatrace_xml_to_database(filename, time, curMI);

    // MediaInfo + MediaTrace (MaXML)
    register_report_mediainfo_and_mediatrace_xml_to_database(filename, time, curMI);
}

//---------------------------------------------------------------------------
void Core::register_file_to_database(std::string& filename)
{
    time_t time = get_last_modification_file(filename);

    // MediaInfo
    register_report_mediainfo_text_to_database(filename, time, MI);
    register_report_mediainfo_xml_to_database(filename, time, MI);

    // MediaTrace
    register_report_mediatrace_text_to_database(filename, time, MI);
    register_report_mediatrace_xml_to_database(filename, time, MI);

    // MediaInfo + MediaTrace (MaXML)
    register_report_mediainfo_and_mediatrace_xml_to_database(filename, time, MI);
}

//---------------------------------------------------------------------------
std::string Core::get_report_saved(const std::string& filename, report reportKind, format f)
{
    if (reportKind >= report_Max || f >= format_Max)
        return std::string();

    time_t time = get_last_modification_file(filename);
    return db->get_report(reportKind, f, filename, time);
}

//---------------------------------------------------------------------------
void Core::get_Reports_Output(const std::string& file, std::string& report)
{
    if (Report[report_MediaConch])
    {
        if (Format == format_Xml)
            report += GetOutput_Xml_Implementation(file);
        else
            report += GetOutput_Text_Implementation(file);
        report += "\r\n";
    }

    if (Report[report_MediaTrace] && Report[report_MediaInfo] && Format == format_MaXml)
    {
        report += get_report_saved(file, report_MediaConch, Format);
        report += "\r\n";
    }
    else
    {
        if (Report[report_MediaInfo])
        {
            report += get_report_saved(file, report_MediaInfo, Format);
            report += "\r\n";
        }
        if (Report[report_MediaTrace])
        {
            report += get_report_saved(file, report_MediaTrace, Format);
            report += "\r\n";
        }
    }
}

//---------------------------------------------------------------------------
bool Core::file_is_registered_in_db(std::string& filename)
{
    if (!get_db())
        return false;

    time_t time = get_last_modification_file(filename);

    return db->file_is_registered(filename, time);
}

//---------------------------------------------------------------------------
std::string Core::get_config_path()
{
    if (configuration_path.length())
        return configuration_path;

    std::string config_path(".");
#if defined(WINDOWS)
    char username[UNLEN+1];
    DWORD username_len = UNLEN+1;
    GetUserName(username, &username_len);

    std::string wuser(username, username_len);
    std::string user_name(wuser.begin(), wuser.end());
    std::stringstream path;

    path << "C:/Users/" << user_name << "/AppData/Local/MediaConch/";
    config_path = path.str();
#elif defined(UNIX)
    const char* home = NULL;

    if ((home = getenv("HOME")) == NULL)
    {
        struct passwd *pw = getpwuid(getuid());
        if (pw)
            home = pw->pw_dir;
    }
    config_path = std::string(home) + Path_Separator + std::string(".config/");
#elif defined(MACOS) || defined(MACOSX)
    config_path = std::string("~/Library/Preferences/");
#endif

    std::ifstream ifile(config_path.c_str());
    if (!ifile)
        config_path = std::string(".") + Path_Separator;
    return config_path;
}

//---------------------------------------------------------------------------
void Core::set_configuration_path(std::string& path)
{
    configuration_path = path;
}

//---------------------------------------------------------------------------
const std::string& Core::get_configuration_path() const
{
    return configuration_path;
}

//---------------------------------------------------------------------------
bool Core::database_is_enabled() const
{
    if (!config)
        return false;
    bool enabled = false;
    if (config->get("SQLite_Path", enabled))
        return false;
    return enabled;
}

//---------------------------------------------------------------------------
Database *Core::get_db()
{
    if (!db)
        load_database();
    return db;
}

//---------------------------------------------------------------------------
void Core::WaitRunIsFinished()
{
    while (1)
    {
        if (scheduler->is_finished())
            break;
        usleep(50000);
    }
}

}
