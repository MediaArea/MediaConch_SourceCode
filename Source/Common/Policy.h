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
    Policy(Policies *p, Policies::PolicyType t, bool n_https);
    virtual ~Policy();

    Policy(const Policy*);
    Policy(const Policy&);

    int                  import_schema(const std::string& filename, const std::string& save_name);
    int                  import_schema_from_memory(const char* memory, int len, const std::string& save_name);
    int                  export_schema(const char* filename, std::string& err);
    int                  dump_schema(std::string& data);
    std::string          get_error() const { return error; }
    virtual xmlDocPtr    create_doc() = 0;
    void                 set_keep_public(bool b) { keep_public = b;}

    std::string          filename;
    std::string          name;
    std::string          description;
    std::string          license;
    Policies::PolicyType type;
    size_t               id;
    bool                 is_system;
    bool                 no_https;
    bool                 is_public;
    bool                 keep_public;

protected:
    Policies            *policies;
    std::string          error;

    // HELPER
    virtual int          import_schema_from_doc(xmlDocPtr doc, const std::string& filename) = 0;

private:
    Policy& operator=(const Policy&);
};

}

#endif
