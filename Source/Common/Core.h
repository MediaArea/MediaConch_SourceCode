/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Core functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef CoreH
#define CoreH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifdef MEDIAINFO_DLL_RUNTIME
    #include "MediaInfoDLL/MediaInfoDLL.h"
    #define MediaInfoNameSpace MediaInfoDLL
#elif defined MEDIAINFO_DLL_STATIC
    #include "MediaInfoDLL/MediaInfoDLL_Static.h"
    #define MediaInfoNameSpace MediaInfoDLL
#else
    #include "MediaInfo/MediaInfoList.h"
    #define MediaInfoNameSpace MediaInfoLib
#endif
#include <map>
#include <bitset>
#include <vector>
#include "Policies.h"
#include "Policy.h"
#include "Configuration.h"

using namespace MediaInfoNameSpace;
using namespace std;
//---------------------------------------------------------------------------

namespace MediaConch {

class Schema;
class Database;

//***************************************************************************
// Class Core
//***************************************************************************

class Core
{
public:
    //Constructor/Destructor
    Core();
    ~Core();

    //Menu
    void    Menu_Option_Preferences_Inform  (const MediaInfoNameSpace::String &Inform);
    String  Menu_Option_Preferences_Option  (const MediaInfoNameSpace::String &Param, const MediaInfoNameSpace::String &Value);

    //Config
    enum report
    {
        report_MediaConch,
        report_MediaInfo,
        report_MediaTrace,
        report_Max
    };
    bitset<report_Max> Report;
    bool Report_IsDefault;
    enum format
    {
        format_Text,
        format_Xml,         // XML corresponding to only one of MediaConch, MediaInfo, MediaTrace
        format_MaXml,       // MAXML, can contain one or more of MediaConch, MediaInfo, MediaTrace
        format_JsTree,
        format_Html,
        format_Max,
    };
    format Format;
    String ReportAndFormatCombination_IsValid();
    vector<String> List;

    void   Close();
    void   Run(String file = String());
    String GetOutput();
    String GetOutput_Text(const String& file);
    String GetOutput_Text_Implementation(const String& file);
    String GetOutput_Xml(const String& file);
    String GetOutput_Xml_Implementation(const String& file);
    void   GetOutput_JStree(const String& file, String& report);
    void   GetOutput_Html(const String& file, String& report);
    String PoliciesCheck();

    bool ValidatePolicy(const String& file, int policy, bool& valid, String& report);
    String transformWithXsltFile(String& report, String& Xslt);
    String transformWithXsltMemory(String& report, std::string& memory);

    std::vector<String> PoliciesFiles;
    Policies policies;
    String xsltDisplay;

private:
    Core (const Core&);

    MediaInfoNameSpace::MediaInfoList* MI;
    Database*                          db;
    Configuration                      config;

    bool PolicySchematron(const String& file, std::wstringstream& Out);
    bool PolicyXslt(const String& file, std::wstringstream& Out);

    //Helper
    bool validation(const String& file, Schema* S, String& report);
    void validateSchematronPolicy(const String& file, int pos, bool& valid, String& report);
    void validateXsltPolicy(const String& file, int pos, bool& valid, String& report);
    void validateXsltPolicyFromMemory(const String& file, const std::string& memory, bool& valid, String& report);
    bool is_schematron_file(const String& file);

    bool   file_is_registered_in_db(String& file);
    time_t get_last_modification_file(const std::string& file);
    String get_report_saved(const String& file, report reportKind, format f);
    void get_Reports_Output(const String& file, String& report);

    void register_file_to_database(String& file);
    void register_report_mediainfo_text_to_database(std::string& file, time_t time);
    void register_report_mediainfo_xml_to_database(std::string& file, time_t time);
    void register_report_mediatrace_text_to_database(std::string& file, time_t time);
    void register_report_mediatrace_xml_to_database(std::string& file, time_t time);
    void register_report_mediainfo_and_mediatrace_xml_to_database(std::string& file, time_t time);
    std::string get_config_path();
};

}

#endif
