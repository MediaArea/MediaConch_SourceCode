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
#include <vector>
#include "Policies.h"

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
    enum tool
    {
        tool_MediaConch,
        tool_MediaInfo,
        tool_MediaTrace,
        tool_MediaSchematron,
        tool_MediaXslt,
        tool_MediaPolicies,
    };
    tool Tool;
    enum format
    {
        format_Text,
        format_Xml,
    };
    format Format;
    vector<String> List;

    String Run(String file = String());

    bool ValidatePolicy(String& policy, bool& valid, String& report);

    enum policyType
    {
        policyType_Schematron,
        policyType_Xslt,
    };
    std::map<policyType, std::vector<String> > PoliciesFiles;
    Policies policies;

private:
    Core (const Core&);

    MediaInfoNameSpace::MediaInfoList* MI;

    String MediaConch();
    String MediaInfo();
    String MediaTrace();
    String MediaSchematron();
    String MediaXslt();
    String MediaPolicies();

    //Helper
    bool validation(Schema* S, String& report);
    void validateSchematronPolicy(int pos, bool& valid, String& report);
    void validateXsltPolicy(int pos, bool& valid, String& report);
};

}

#endif
