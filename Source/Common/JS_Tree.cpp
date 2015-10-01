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
#include "ZenLib/Ztring.h"
#include "JS_Tree.h"
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
String JsTree::format_from_XML(String& xml)
{
    String json(__T("["));
    std::string xml_not_unicode=Ztring(xml).To_UTF8();

    xmlDocPtr doc = xmlParseMemory(xml_not_unicode.c_str(), xml_not_unicode.length());
    if (!doc)
    {
        error = __T("The report given cannot be parsed");
        return String();
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (!root)
    {
        error = __T("No root node, leaving");
        return String();
    }

    xmlNodePtr child = root->children;
    bool new_sep = false;
    while (child)
    {
        find_block_node(child, new_sep, json);
        child = child->next;
    }

    json += String(__T("]"));

    xmlFreeDoc(doc);
    return json;
}

//---------------------------------------------------------------------------
void JsTree::find_block_node(xmlNodePtr node, bool& sep, String& json)
{
    std::string def("block");
    if (!node || node->type != XML_ELEMENT_NODE ||
        !node->name || def.compare((const char*)node->name))
        return;

    if (sep)
        json += __T(", ");
    else
        sep = true;

    json += __T("{\"type\":\"block\"");

    interpret_data_in_block(node, json);
    xmlNodePtr child = node->children;
    if (has_block_data(child))
    {
        json += __T(", \"children\":[");
        bool new_sep = false;
        while (child)
        {
            find_block_node(child, new_sep, json);
            find_data_node(child, new_sep, json);
            child = child->next;
        }
        json += __T("]");
    }

    json += __T("}");
}

//---------------------------------------------------------------------------
void JsTree::find_data_node(xmlNodePtr node, bool& sep, String& json)
{
    std::string def("data");
    if (!node || node->type != XML_ELEMENT_NODE ||
        !node->name || def.compare((const char*)node->name))
        return;

    if (sep)
        json += __T(", ");
    else
        sep = true;

    json += __T("{\"type\":\"data\"");

    interpret_data_in_data(node, json);

    xmlNodePtr child = node->children;
    if (has_block_data(child))
    {
        json += __T(", \"children\":[");
        bool new_sep = false;
        while (child)
        {
            find_block_node(child, new_sep, json);
            find_data_node(child, new_sep, json);
            child = child->next;
        }
        json += __T("]");
    }

    json += __T("}");
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
void JsTree::interpret_data_in_block(xmlNodePtr block, String& json)
{
    //Format: offset_hexa name[ - info] (size bytes)]
    std::string offset;
    std::string name;
    std::string info;
    std::string size;

    xmlChar *offset_c = xmlGetNoNsProp(block, (const unsigned char*)"offset");
    if (offset_c != NULL)
        offset = decimal_to_hexa(std::string((const  char *)offset_c));
    xmlChar *name_c = xmlGetNoNsProp(block, (const unsigned char*)"name");
    if (name_c != NULL)
        name = std::string((const  char *)name_c);
    xmlChar *info_c = xmlGetNoNsProp(block, (const unsigned char*)"info");
    if (info_c != NULL)
        info = std::string((const  char *)info_c);
    xmlChar *size_c = xmlGetNoNsProp(block, (const unsigned char*)"size");
    if (size_c != NULL)
        size = std::string((const  char *)size_c);

    json += __T(", \"text\":\"");
    json += String(offset.begin(), offset.end()); //TODO: Transform to hexa
    json += __T(" ");
    json += String(name.begin(), name.end());
    if (info.length())
    {
        json += __T(" - ");
        json += String(info.begin(), info.end());
    }
    json += __T(" (");
    json += String(size.begin(), size.end());
    json += __T(" bytes)");
    json += __T("\"");
}

//---------------------------------------------------------------------------
void JsTree::interpret_data_in_data(xmlNodePtr data, String& json)
{
    //Format: offset_hexa name: value (value_in_hexa)]
    std::string offset; //Decimal to hexa
    std::string name;
    std::string value; //decimal + hexa if numerical

    xmlChar *offset_c = xmlGetNoNsProp(data, (const unsigned char*)"offset");
    if (offset_c != NULL)
        offset = decimal_to_hexa(std::string((const char *)offset_c));
    xmlChar *name_c = xmlGetNoNsProp(data, (const unsigned char*)"name");
    if (name_c != NULL)
        name = std::string((const char *)name_c);

    xmlChar *value_c = xmlNodeGetContent(data);
    if (value_c != NULL)
        value = std::string((const char *)value_c);

    json += __T(", \"text\":\"");
    json += String(offset.begin(), offset.end());
    json += __T(" ");
    json += String(name.begin(), name.end());
    json += __T(": ");
    if (value.length())
    {
        json += String(value.begin(), value.end());

        // Not numerical
        std::size_t found = value.find_first_not_of("0123456789.");
        if (found != std::string::npos)
            value = "0";
        json += __T(" (0x");
        std::string hexa = decimal_to_hexa(value);
        json += String(hexa.begin(), hexa.end());
        json += __T(")");
    }
    json += __T("\"");
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
