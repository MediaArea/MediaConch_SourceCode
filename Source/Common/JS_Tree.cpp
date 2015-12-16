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
#include <string>
#include <sstream>
#include "JS_Tree.h"
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
std::string JsTree::format_from_trace_XML(const std::string& xml)
{
    std::string json("[");

    xmlDocPtr doc = xmlParseMemory(xml.c_str(), xml.length());
    if (!doc)
    {
        error = "The report given cannot be parsed";
        return std::string();
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (!root)
    {
        error = "No root node, leaving";
        return std::string();
    }

    xmlNodePtr child = root->children;
    bool new_sep = false;
    while (child)
    {
        find_trace_media_node(child, new_sep, json);
        child = child->next;
    }

    json += "]";

    xmlFreeDoc(doc);
    return json;
}

//---------------------------------------------------------------------------
void JsTree::find_trace_media_node(xmlNodePtr node, bool& sep, std::string& json)
{
    std::string name("media");

    if (!node || node->type != XML_ELEMENT_NODE || !node->name ||
        name.compare((const char*)node->name))
        return;

    xmlNodePtr child = node->children;
    while (child)
    {
        find_trace_block_node(child, sep, json);
        child = child->next;
    }
}

//---------------------------------------------------------------------------
void JsTree::find_trace_block_node(xmlNodePtr node, bool& sep, std::string& json)
{
    std::string def("block");
    if (!node || node->type != XML_ELEMENT_NODE ||
        !node->name || def.compare((const char*)node->name))
        return;

    if (sep)
        json += ", ";
    else
        sep = true;

    json += "{\"type\":\"block\"";

    interpret_trace_data_in_block(node, json);
    xmlNodePtr child = node->children;
    if (has_block_data(child))
    {
        json += ", \"children\":[";
        bool new_sep = false;
        while (child)
        {
            find_trace_block_node(child, new_sep, json);
            find_trace_data_node(child, new_sep, json);
            child = child->next;
        }
        json += "]";
    }

    json += "}";
}

//---------------------------------------------------------------------------
void JsTree::find_trace_data_node(xmlNodePtr node, bool& sep, std::string& json)
{
    std::string def("data");
    if (!node || node->type != XML_ELEMENT_NODE ||
        !node->name || def.compare((const char*)node->name))
        return;

    if (sep)
        json += ", ";
    else
        sep = true;

    json += "{\"type\":\"data\"";

    interpret_trace_data_in_data(node, json);

    xmlNodePtr child = node->children;
    if (has_block_data(child))
    {
        json += ", \"children\":[";
        bool new_sep = false;
        while (child)
        {
            find_trace_block_node(child, new_sep, json);
            find_trace_data_node(child, new_sep, json);
            child = child->next;
        }
        json += "]";
    }

    json += "}";
}

//---------------------------------------------------------------------------
bool JsTree::has_block_data(xmlNodePtr child)
{
    std::string block("block");
    std::string data("data");

    while (child)
    {
        if (child && child->type == XML_ELEMENT_NODE && child->name &&
            (block.compare((const char*)child->name) || data.compare((const char*)child->name)))
            return true;
        child = child->next;
    }

    return false;
}

//---------------------------------------------------------------------------
std::string JsTree::format_from_inform_XML(const std::string& xml)
{
    std::string json("[");

    xmlDocPtr doc = xmlParseMemory(xml.c_str(), xml.length());
    if (!doc)
    {
        error = "The report given cannot be parsed";
        return std::string();
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (!root)
    {
        error = "No root node, leaving";
        return std::string();
    }

    xmlNodePtr child = root->children;
    bool new_sep = false;
    while (child)
    {
        find_inform_media_node(child, new_sep, json);
        child = child->next;
    }

    json += "]";
    xmlFreeDoc(doc);
    return json;
}

//---------------------------------------------------------------------------
void JsTree::find_inform_data_node(xmlNodePtr node, bool& sep, std::string& json)
{
    if (!node || node->type != XML_ELEMENT_NODE)
        return;

    if (sep)
        json += ", ";
    else
        sep = true;

    json += "{\"type\":\"data\"";

    xmlChar *value_c = xmlNodeGetContent(node);

    std::string name;
    if (node->name != NULL)
        name = (const  char *)node->name;

    if (name == "extra")
    {
        xmlNodePtr child = node->children;
        while (child)
        {
            if (child->type != XML_ELEMENT_NODE)
            {
                child = child->next;
                continue;
            }

            if (child->name != NULL)
                name = (const  char *)child->name;
            value_c = xmlNodeGetContent(child);
            break;
        }
    }

    if (name.length())
    {
        json += ", \"text\":\"";
        json += name;
        json += "\"";
    }

    if (value_c != NULL)
    {
        std::string value((const char *)value_c);
        json += ", \"data\":{\"dataValue\":\"";
        json += value;
        json += "\"}";
    }

    json += "}";
}

//---------------------------------------------------------------------------
void JsTree::find_inform_track_type(xmlNodePtr node, bool& sep, std::string& json)
{
    std::string name("track");

    if (!node || node->type != XML_ELEMENT_NODE || !node->name ||
        name.compare((const char*)node->name))
        return;

    xmlChar *type_c = xmlGetNoNsProp(node, (const unsigned char*)"type");
    std::string type;
    if (type_c != NULL)
        type = (const  char *)type_c;

    if (sep)
        json += ", ";
    else
        sep = true;

    json += "{\"type\":\"block\", \"text\":\"";
    json += type;
    json += "\"";

    xmlNodePtr child = node->children;
    if (child)
    {
        json += ", \"children\":[";
        bool new_sep = false;
        while (child)
        {
            find_inform_data_node(child, new_sep, json);
            child = child->next;
        }
        json += "]";
    }

    json += "}";
}

//---------------------------------------------------------------------------
void JsTree::find_inform_media_node(xmlNodePtr node, bool& sep, std::string& json)
{
    std::string name("media");

    if (!node || node->type != XML_ELEMENT_NODE || !node->name ||
        name.compare((const char*)node->name))
        return;

    const char* file = "";
    xmlChar *file_c = xmlGetNoNsProp(node, (const unsigned char*)"ref");
    if (file_c != NULL)
        file = (const char *)file_c;

    if (sep)
        json += ", ";
    else
        sep = true;

    json += "{\"type\":\"block\", \"text\":\"";
    json += file;
    json += "\"";

    xmlNodePtr child = node->children;
    if (child)
    {
        json += ", \"children\":[";
        bool new_sep = false;
        while (child)
        {
            find_inform_track_type(child, new_sep, json);
            child = child->next;
        }
        json += "]";
    }
    json += "}";
}

//---------------------------------------------------------------------------
void JsTree::interpret_trace_data_in_block(xmlNodePtr block, std::string& json)
{
    //Format: "text": "name[ - info] (size bytes)", "data": {"offset": "offset_hexa"}
    std::string offset;
    std::string name;
    std::string info;
    std::string size;

    xmlChar *offset_c = xmlGetNoNsProp(block, (const unsigned char*)"offset");
    if (offset_c != NULL)
        offset = (const  char *)offset_c;
    xmlChar *name_c = xmlGetNoNsProp(block, (const unsigned char*)"name");
    if (name_c != NULL)
        name = std::string((const  char *)name_c);
    xmlChar *info_c = xmlGetNoNsProp(block, (const unsigned char*)"info");
    if (info_c != NULL)
        info = std::string((const  char *)info_c);
    xmlChar *size_c = xmlGetNoNsProp(block, (const unsigned char*)"size");
    if (size_c != NULL)
        size = std::string((const  char *)size_c);

    //Block data
    json += ", \"text\":\"";
    json += name;
    if (info.length())
    {
        json += " - ";
        json += info;
    }
    json += " (";
    json += size;
    json += " bytes)\"";

    //Block data
    json += ", \"data\":{";
    bool coma = false;
    if (offset_c)
    {
        interpret_offset(offset, coma, json);
        coma = true;
    }
    json += "}";
}

//---------------------------------------------------------------------------
void JsTree::interpret_trace_data_in_data(xmlNodePtr data, std::string& json)
{
    //Format: "text": "name", "data": {"offset": "offset_hexa", "dataValue": "value (value_in_hexa)"}
    std::string offset; //Decimal to hexa
    std::string name;
    std::string value; //decimal + hexa if numerical

    xmlChar *offset_c = xmlGetNoNsProp(data, (const unsigned char*)"offset");
    if (offset_c != NULL)
        offset = (const char *)offset_c;
    xmlChar *name_c = xmlGetNoNsProp(data, (const unsigned char*)"name");
    if (name_c != NULL)
        name = std::string((const char *)name_c);

    xmlChar *value_c = xmlNodeGetContent(data);
    if (value_c != NULL)
        value = std::string((const char *)value_c);

    json += ", \"text\":\"";
    json += name;

    //Block data
    json += "\", \"data\":{";
    bool coma = false;
    if (offset_c)
    {
        interpret_offset(offset, coma, json);
        coma = true;
    }
    if (value.length())
    {
        interpret_value(value, coma, json);
        coma = true;
    }
    json += "}";
}

//---------------------------------------------------------------------------
void JsTree::interpret_offset(std::string& offset, bool coma, std::string& json)
{
    if (coma)
        json += ", ";
    json += "\"offset\":";

    // Not numerical
    std::size_t found = offset.find_first_not_of("0123456789.");
    if (found != std::string::npos)
        offset = "0";
    json += "\"0x";

    std::string hexa = decimal_to_hexa(offset);
    int zero = 8 - hexa.length();
    while (zero > 0)
    {
        json += "0";
        --zero;
    }
    json += hexa;

    json += "\"";
}

//---------------------------------------------------------------------------
void JsTree::interpret_value(std::string& value, bool coma, std::string& json)
{
    if (coma)
        json += ", ";
    json += "\"dataValue\":\"";

    json += value;

    // Not numerical
    std::size_t found = value.find_first_not_of("0123456789.");
    if (found == std::string::npos)
    {
        json += " (0x";
        json += decimal_to_hexa(value);
        json += ")";
    }
    json += "\"";
}

//---------------------------------------------------------------------------
std::string JsTree::decimal_to_hexa(std::string str)
{
    std::stringstream ss;

    long long int val = strtoll(str.c_str(), NULL, 10);

    ss << std::hex << val;
    return ss.str();
}

}
