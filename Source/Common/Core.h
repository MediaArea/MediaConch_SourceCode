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

using namespace MediaInfoNameSpace;
using namespace std;
//---------------------------------------------------------------------------

namespace MediaConch {

class Schema;

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
    };
    format Format;
    String ReportAndFormatCombination_IsValid();
    vector<String> List;

    void   Close();
    void   Run(String file = String());
    String GetOutput();
    String GetOutput_Text();
    String GetOutput_Text_Implementation();
    String GetOutput_Xml();
    String GetOutput_Xml_Implementation();
    String GetOutput_MaXml();
    String GetOutput_JStree();
    String GetOutput_Html();
    String PoliciesCheck();

    bool ValidatePolicy(int policy, bool& valid, String& report);
    String transformWithXsltFile(String& report, String& Xslt);
    String transformWithXsltMemory(String& report, std::string& memory);

    std::vector<String> PoliciesFiles;
    Policies policies;
    String xsltDisplay;

private:
    Core (const Core&);

    MediaInfoNameSpace::MediaInfoList* MI;

    bool PolicySchematron(const String& file, std::wstringstream& Out);
    bool PolicyXslt(const String& file, std::wstringstream& Out);

    //Helper
    bool validation(Schema* S, String& report);
    void validateSchematronPolicy(int pos, bool& valid, String& report);
    void validateXsltPolicy(int pos, bool& valid, String& report);
    void validateXsltPolicyFromMemory(const std::string& memory, bool& valid, String& report);
    bool is_schematron_file(const String& file);
};

}

#endif
