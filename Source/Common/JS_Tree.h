/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
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
#include <libxml/tree.h>
#include "MediaConchLib.h"

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
    std::string  format_from_inform_Text(const std::string& text);

    static int   policies_to_js_tree(std::vector<MediaConchLib::Policy_Policy*> vec, std::string& jstree, std::string& error);
    static int   policy_to_js_tree(MediaConchLib::Policy_Policy* policy, std::string& json, std::string& error);
    static int   rule_to_js_tree(MediaConchLib::XSLT_Policy_Rule* rule, std::string& json, std::string&);


    std::string  get_error() const { return error; }

private:
    JsTree (const JsTree&);
    JsTree&      operator=(const JsTree&);

    std::string  error;

    void         find_trace_media_node(xmlNodePtr node, bool& sep, std::string& json);
    void         find_trace_block_node(xmlNodePtr node, bool& sep, std::string& json);
    void         find_trace_data_node(xmlNodePtr node, bool& sep, std::string& json);
    bool         has_block_data(xmlNodePtr child);
    void         find_inform_media_text(std::istringstream& stream, bool& sep, std::string& json);
    void         find_inform_block_text(std::istringstream& stream, bool& sep, std::string& json);
    void         find_inform_track_text(std::istringstream& stream, bool& sep, std::string& json);
    void         find_inform_data_node(xmlNodePtr node, bool& sep, std::string& json);
    void         find_inform_media_node(xmlNodePtr node, bool& sep, std::string& json);
    void         find_inform_track_type(xmlNodePtr node, bool& sep, std::string& json);

    void         interpret_trace_data_in_block(xmlNodePtr block, std::string& json);
    void         interpret_trace_data_in_data(xmlNodePtr data, std::string& json);

    void trim_string(std::string &str);
    std::string decimal_to_hexa(std::string val);
    void interpret_offset(std::string& offset, bool coma, std::string& json);
    void interpret_value(std::string& value, bool coma, std::string& json);

    static std::string unified_json_value(const std::string& value);
};

}

#endif
