/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// JsTree functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef JsTreeH
#define JsTreeH

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
//---------------------------------------------------------------------------
#include <libxml/tree.h>

//---------------------------------------------------------------------------
using namespace MediaInfoNameSpace;

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Class JsTree
//***************************************************************************

class JsTree
{
public:
    //Constructor/Destructor
    JsTree() {}
    ~JsTree() {}

    String       format_from_XML(String& xml);
    void         find_block_node(xmlNodePtr node, bool& sep, String& json);
    void         find_data_node(xmlNodePtr node, bool& sep, String& json);
    bool         has_block_data(xmlNodePtr child);

    void         interpret_data_in_block(xmlNodePtr block, String& json);
    void         interpret_data_in_data(xmlNodePtr data, String& json);

    String       get_error() const { return error; }

private:
    JsTree (const JsTree&);
    JsTree&      operator=(const JsTree&);

    String       error;

    std::string decimal_to_hexa(std::string val);
};

}

#endif
