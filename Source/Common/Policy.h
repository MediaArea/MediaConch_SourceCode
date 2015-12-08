/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Policies functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef PolicYH
#define PolicYH
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
#include <vector>
#include <libxml/tree.h>
#include "Policies.h"
using namespace MediaInfoNameSpace;
//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// Policy
//***************************************************************************

class Policy
{
public:
    Policy(Policies::PolicyType t) : type(t), saved(false) {}
    virtual ~Policy();
    Policy(const Policy*);

    std::string       import_schema(const std::string& filename);
    std::string       import_schema_from_memory(const std::string& filename, const char* memory, int len);
    void              export_schema(const char* filename);
    virtual xmlDocPtr create_doc() = 0;

    std::string          filename;
    std::string          title;
    std::string          description;
    Policies::PolicyType type;
    bool                 saved;

protected:
    // HELPER
    virtual std::string import_schema_from_doc(const std::string& filename, xmlDocPtr doc) = 0;

private:
    Policy& operator=(const Policy&);
};

}

#endif
