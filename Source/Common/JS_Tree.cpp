/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
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
std::string JsTree::format_from_inform_Text(const std::string& text)
{
    std::istringstream stream(text);
    std::string json("[");

    bool media_sep = false;
    find_inform_media_text(stream, media_sep, json);

    json += "]";

    return json;
}

//---------------------------------------------------------------------------
void JsTree::find_inform_track_text(std::istringstream& stream, bool& sep, std::string& json)
{
    std::streampos pos = stream.tellg();
    std::string line;

    while (std::getline(stream, line))
    {
        if (!line.empty())
        {
            if (line.find(":") == std::string::npos)
            {
                stream.clear();
                stream.seekg(pos);
                return;
            }

            if (sep)
                json += ", ";
            else
                sep = true;

            std::string name = line.substr(0, line.find_first_of(":"));
            trim_string(name);
            std::string value = line.substr(line.find_first_of(":") + 1);
            trim_string(value);

            json += "{\"type\":\"data\"";

            if (!name.empty())
            {
                json += ", \"text\":\"";
                json += name;
                json += "\"";
            }

            if (!value.empty())
            {
                json += ", \"data\":{\"dataValue\":\"";
                json += unified_json_value(value);
                json += "\"}";
            }

            json += "}";
        }

        pos = stream.tellg();
    }

    stream.clear();
    stream.seekg(pos);
}

//---------------------------------------------------------------------------
void JsTree::find_inform_block_text(std::istringstream& stream, bool& sep, std::string& json)
{
    std::streampos pos = stream.tellg();
    std::string line;

    while (std::getline(stream, line))
    {
        trim_string(line);
        if (!line.empty() && line.find(":") == std::string::npos)
        {
            if (line == "General")
            {
                stream.clear();
                stream.seekg(pos);
                return;
            }

            if (sep)
                json += ", ";
            else
                sep = true;

            json += "{\"type\":\"block\", \"text\":\"";
            json += line;
            json += "\"";
            json += ", \"children\":[";
            bool track_sep = false;
            find_inform_track_text(stream, track_sep, json);
            json += "]}";
        }

        pos = stream.tellg();
    }

    stream.clear();
    stream.seekg(pos);
}

//---------------------------------------------------------------------------
void JsTree::find_inform_media_text(std::istringstream& stream, bool& sep, std::string& json)
{
    std::string line;

    while (std::getline(stream, line))
    {
        trim_string(line);
        if (line == "General")
        {
            if (sep)
                json += ", ";
            else
                sep = true;

            json += "{\"type\":\"block\"";

            std::streampos pos = stream.tellg();

            while (std::getline(stream, line))
            {
                if(!line.empty() && line.find(":") == std::string::npos)
                    break;

                if (line.find("CompleteName") == 0 || line.find("Complete name") == 0)
                {
                    std::string name = line.substr(0, line.find_first_of(":"));
                    trim_string(name);
                    std::string value = line.substr(line.find_first_of(":") + 1);
                    trim_string(value);

                    if (!value.empty())
                    {
                        json +=", \"text\":\"";
                        json += value;
                        json += "\"";
                    }
                }
            }

            stream.clear();
            stream.seekg(pos);

            json += ", \"children\":[";
            json += "{\"type\":\"block\", \"text\":\"General\"";
            json += ", \"children\":[";
            bool track_sep = false;
            find_inform_track_text(stream, track_sep, json);
            json += "]}";
            bool block_sep = true;
            find_inform_block_text(stream, block_sep, json);
            json += "]}";
        }
    }
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

    xmlChar *value_c = xmlNodeGetContent(node);

    std::string name;
    if (node->name != NULL)
        name = (const  char *)node->name;

    if (name == "extra")
    {
        for (xmlNodePtr child = node->children; child; child = child->next)
        {
            find_inform_data_node(child, sep, json);
        }
        return;
    }

    if (sep)
        json += ", ";
    else
        sep = true;

    json += "{\"type\":\"data\"";

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
std::string JsTree::unified_json_value(const std::string& v)
{
    std::string value(v);

    // \n
    size_t pos = 0;
    while (pos != std::string::npos)
    {
        pos = value.find("\\", pos);
        if (pos != std::string::npos)
        {
            value.replace(pos, 1, "\\\\");
            pos += 2;
        }
    }

    // \r
    pos = 0;
    while (pos != std::string::npos)
    {
        pos = value.find("\r", pos);
        if (pos != std::string::npos)
        {
            value.replace(pos, 1, "\\r");
            pos += 2;
        }
    }

    // "
    pos = 0;
    while (pos != std::string::npos)
    {
        pos = value.find("\"", pos);
        if (pos != std::string::npos)
        {
            value.replace(pos, 1, "\\\"");
            pos += 2;
        }
    }

    // back-slash
    pos = 0;
    while (pos != std::string::npos)
    {
        pos = value.find("\n", pos);
        if (pos != std::string::npos)
        {
            value.replace(pos, 1, "\\n");
            pos += 2;
        }
    }

    return value;
}

//---------------------------------------------------------------------------
void JsTree::interpret_value(std::string& value, bool coma, std::string& json)
{
    if (coma)
        json += ", ";
    json += "\"dataValue\":\"";

    json += unified_json_value(value);

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
void JsTree::trim_string(std::string& str)
{
    size_t ltrim = str.find_first_not_of(" \t\n\r\f\v");
    str.erase(0, ltrim);

    if (ltrim == std::string::npos)
        return;

    size_t rtrim = str.find_last_not_of(" \t\n\r\f\v");
    if (rtrim < str.size() - 1)
        str.erase(rtrim + 1);
}

//---------------------------------------------------------------------------
std::string JsTree::decimal_to_hexa(std::string str)
{
    std::stringstream ss;

    long long int val = strtoll(str.c_str(), NULL, 10);

    ss << std::hex << val;
    return ss.str();
}

//---------------------------------------------------------------------------
int JsTree::rule_to_js_tree(MediaConchLib::XSLT_Policy_Rule* rule, std::string& json, std::string&)
{
    if (!rule)
        return 0;

    std::stringstream ss;
    ss << "{\"text\":\"" << unified_json_value(rule->name);
    ss << "\",\"type\":\"r\"";
    ss << ",\"data\":{";
    ss << "\"ruleId\":" << rule->id;
    ss << ",\"tracktype\":\"" << unified_json_value(rule->tracktype);
    ss << "\",\"field\":\"" << unified_json_value(rule->field);
    ss << "\",\"scope\":\"" << unified_json_value(rule->scope);
    ss << "\",\"occurrence\":" << rule->occurrence;
    ss << ",\"ope\":\"" << unified_json_value(rule->ope);
    ss << "\",\"value\":\"" << unified_json_value(rule->value);
    ss <<"\"}}";

    json = ss.str();
    return 0;
}

//---------------------------------------------------------------------------
int JsTree::policy_to_js_tree(MediaConchLib::Policy_Policy* policy, std::string& json, std::string& error)
{
    if (!policy)
        return 0;

    std::stringstream ss;
    ss << "{\"text\":\"" << unified_json_value(policy->name);
    if (policy->kind == "XSLT")
        ss << " (" << unified_json_value(policy->type) << ")";
    ss << "\",\"type\":\"" << (policy->is_system ? "s" : "u");
    ss << "\",\"data\":{";
    ss << "\"policyId\":" << policy->id;
    ss << ",\"kind\":\"" << unified_json_value(policy->kind);
    ss << "\",\"type\":\"" << unified_json_value(policy->type) << "\"";
    if (policy->kind == "XSLT")
    {
        ss << ",\"isEditable\":true";
        ss << ",\"description\":\"" << unified_json_value(policy->description) << "\"";
        if (policy->parent_id == -1 && policy->is_public)
            ss << ",\"isPublic\":" << std::boolalpha << policy->is_public;
        if (policy->license.size())
            ss << ",\"license\":\"" << unified_json_value(policy->license) << "\"";
    }
    else
        ss << ",\"isEditable\":false";
    ss <<"}";

    std::stringstream children;
    for (size_t i = 0; i < policy->children.size(); ++i)
    {
        if (!policy->children[i].second.policy)
            continue;

        if (i)
            children << ",";
        std::string child;
        if (policy->children[i].first == 0)
            policy_to_js_tree(policy->children[i].second.policy, child, error);
        else if (policy->children[i].first == 1)
            rule_to_js_tree(policy->children[i].second.rule, child, error);

        children << child;
    }

    ss << ",\"children\":[" << children.str() << "]";
    ss << "}";

    json = ss.str();
    return 0;
}

//---------------------------------------------------------------------------
int JsTree::policies_to_js_tree(std::vector<MediaConchLib::Policy_Policy*> vec, std::string& jstree, std::string& error)
{
    std::stringstream ss;
    ss << "{\"policiesTree\":[";

    std::stringstream users, systems;
    for (size_t i = 0; i < vec.size(); ++i)
    {
        if (!vec[i])
            continue;

        if (vec[i]->is_system)
        {
            std::string json;
            if (policy_to_js_tree(vec[i], json, error) < 0)
                return -1;
            if (systems.str().size())
                systems << ",";
            systems << json;
        }
        else
        {
            std::string json;
            if (policy_to_js_tree(vec[i], json, error) < 0)
                return -1;
            if (users.str().size())
                users << ",";
            users << json;
        }
    }
    ss << "{\"id\":\"u_p\",\"text\":\"User policies\",\"type\":\"up\",\"state\":{\"opened\":true,\"selected\":true},\"children\":[" << users.str() << "]}";
    ss << ",{\"id\":\"s_p\",\"text\":\"System policies\",\"type\":\"sp\",\"state\":{\"opened\":true},\"children\":[" << systems.str() << "]}";
    ss << "]}";

    jstree = ss.str();
    return 0;
}

}
