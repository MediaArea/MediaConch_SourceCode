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

    std::string  format_from_trace_XML(const std::string& xml);
    std::string  format_from_inform_XML(const std::string& xml);

    std::string  get_error() const { return error; }

private:
    JsTree (const JsTree&);
    JsTree&      operator=(const JsTree&);

    std::string  error;

    void         find_trace_media_node(xmlNodePtr node, bool& sep, std::string& json);
    void         find_trace_block_node(xmlNodePtr node, bool& sep, std::string& json);
    void         find_trace_data_node(xmlNodePtr node, bool& sep, std::string& json);
    bool         has_block_data(xmlNodePtr child);
    void         find_inform_data_node(xmlNodePtr node, bool& sep, std::string& json);
    void         find_inform_media_node(xmlNodePtr node, bool& sep, std::string& json);
    void         find_inform_track_type(xmlNodePtr node, bool& sep, std::string& json);

    void         interpret_trace_data_in_block(xmlNodePtr block, std::string& json);
    void         interpret_trace_data_in_data(xmlNodePtr data, std::string& json);

    std::string decimal_to_hexa(std::string val);
    void interpret_offset(std::string& offset, bool coma, std::string& json);
    void interpret_value(std::string& value, bool coma, std::string& json);
};

}

#endif
