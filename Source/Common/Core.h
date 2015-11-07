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
#include "Scheduler.h"

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
    std::string ReportAndFormatCombination_IsValid();
    vector<std::string> List;

    void        Close();
    void        Run(std::string file = std::string());
    std::string GetOutput();
    std::string GetOutput_Text(const std::string& file);
    std::string GetOutput_Text_Implementation(const std::string& file);
    std::string GetOutput_Xml(const std::string& file);
    std::string GetOutput_Xml_Implementation(const std::string& file);
    void        GetOutput_JStree(const std::string& file, std::string& report);
    void        GetOutput_Html(const std::string& file, std::string& report);
    std::string PoliciesCheck();

    bool ValidatePolicy(const std::string& file, int policy, bool& valid, std::string& report);
    std::string transformWithXsltFile(std::string& report, std::string& Xslt);
    std::string transformWithXsltMemory(std::string& report, std::string& memory);

    std::vector<std::string> PoliciesFiles;
    Policies policies;
    std::string xsltDisplay;

    //General Configuration
    void load_configuration();
    void set_configuration_path(std::string& path);
    const std::string& get_configuration_path() const;

    //General Database
    void load_database();
    bool database_is_enabled() const;
    void register_file_to_database(std::string& file, MediaInfoNameSpace::MediaInfoList* MI);
    void create_report_mi_xml(const std::string& filename, std::string& report);
    void create_report_mt_xml(const std::string& filename, std::string& report);
    void create_report_ma_xml(const std::string& filename, std::string& report, bitset<report_Max> reports);

    // TODO: removed and manage waiting time otherway
    void WaitRunIsFinished();

private:
    Core (const Core&);

    MediaInfoNameSpace::MediaInfoList* MI;
    Database*                          db;
    Configuration*                     config;
    std::string                        configuration_path;
    //TODO: remove with the daemon
    Scheduler                         *scheduler;

    bool PolicySchematron(const std::string& file, std::stringstream& Out);
    bool PolicyXslt(const std::string& file, std::stringstream& Out);

    //Helper
    bool validation(const std::string& file, Schema* S, std::string& report);
    void validateSchematronPolicy(const std::string& file, int pos, bool& valid, std::string& report);
    void validateXsltPolicy(const std::string& file, int pos, bool& valid, std::string& report);
    void validateXsltPolicyFromMemory(const std::string& file, const std::string& memory, bool& valid, std::string& report);
    bool is_schematron_file(const std::string& file);

    bool   file_is_registered_in_db(std::string& file);
    time_t get_last_modification_file(const std::string& file);
    std::string get_report_saved(const std::string& file, report reportKind, format f);
    void get_Reports_Output(const std::string& file, std::string& report);
    void get_implementation_report(const std::string& file, std::string& report);

    void register_file_to_database(std::string& file);
    void register_report_mediainfo_text_to_database(std::string& file, time_t time,
                                                    MediaInfoNameSpace::MediaInfoList* MI);
    void register_report_mediainfo_xml_to_database(std::string& file, time_t time,
                                                   MediaInfoNameSpace::MediaInfoList* MI);
    void register_report_mediatrace_text_to_database(std::string& file, time_t time,
                                                     MediaInfoNameSpace::MediaInfoList* MI);
    void register_report_mediatrace_xml_to_database(std::string& file, time_t time,
                                                    MediaInfoNameSpace::MediaInfoList* MI);
    void register_report_implementation_xml_to_database(const std::string& file, time_t time,
                                                        std::string& report);
    void get_content_of_media_in_xml(std::string& report);
    //No idea how to do it better way
    bitset<report_Max> get_bitset_with_mi_mt();

    std::string get_config_path();
    Database *get_db();
    void open_file(std::string& filename);
};

}

#endif
