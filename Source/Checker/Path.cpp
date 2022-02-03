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
#include  <algorithm>
#include <cstring>

//---------------------------------------------------------------------------
#include "Path.h"

//---------------------------------------------------------------------------
namespace MediaConch {

std::vector<PathElement> parse_path(const std::string& xpath)
{
    if (xpath.empty())
        return std::vector<PathElement>();

    std::vector<PathElement> toReturn;

    // TODO: replace parser
    size_t slash_pos=std::string::npos;

    do {
        PathElement path;
        size_t start=slash_pos==std::string::npos ? 0 : slash_pos + 1;

        slash_pos=xpath.find("/", start);
        size_t bracket_pos=xpath.find("[", start);

        path.name=xpath.substr(start, (bracket_pos < slash_pos ? bracket_pos : slash_pos) - start);
        // strip namespace
        size_t colon_pos=path.name.find(":", 0);
        path.name=colon_pos==std::string::npos ? path.name : path.name.substr(colon_pos + 1);

        if (path.name.empty())
            return std::vector<PathElement>();

        if (bracket_pos < slash_pos)
        {
            std::string parameters=xpath.substr(bracket_pos, slash_pos - bracket_pos);
            for (size_t pos=0; pos < parameters.size(); pos=parameters.find("[", pos + 1))
            {
                size_t end=parameters.find("]", pos + 1);
                if (end==std::string::npos) // unfinished input
                    return std::vector<PathElement>();

                std::string parameter=parameters.substr(pos + 1, end - pos - 1);
                // index
                {
                    if (parameter=="*")
                    {
                        path.occurence=(size_t)-1;
                        continue;
                    }

                    char* end=NULL;
                    size_t val=strtoul(parameter.c_str(), &end, 10);
                    if (val && !strlen(end))
                    {
                        path.occurence=val;
                        continue;
                    }
                }

                // attribute
                {
                    size_t arobase_pos=parameter.find("@", 0);
                    size_t equal_pos=parameter.find("=", 0);
                    if (arobase_pos!=0 || equal_pos==std::string::npos)
                        return std::vector<PathElement>();

                    std::string attribute=parameter.substr(arobase_pos + 1, equal_pos - arobase_pos - 1);
                    if (attribute.empty())
                        return std::vector<PathElement>();

                    size_t quote_start_pos=equal_pos + 1;
                    if (quote_start_pos>=parameter.size())
                        return std::vector<PathElement>();

                    size_t quote_end_pos=parameter.find("'", quote_start_pos + 1);
                    if (quote_end_pos==std::string::npos)
                        return std::vector<PathElement>();

                    std::string value=parameter.substr(quote_start_pos + 1, quote_end_pos - quote_start_pos - 1);

                    path.attributes[attribute]=value;
                }
            }
        }
        path.valid=true;
        toReturn.push_back(path);
    }
    while (slash_pos!=std::string::npos);

    return toReturn;
}


//---------------------------------------------------------------------------
bool path_is_matching(tfsxml_string& tfsxml_priv, tfsxml_string& node, PathElement path, size_t occurence)
{
    tfsxml_string value;

    // compare names
    if (tfsxml_strcmp_charp(node, path.name.c_str()))
        return false;

    // compare occurence
    if (path.occurence!=(size_t)-1 && path.occurence!=occurence)
        return false;

    // compare attributes
    if (!path.attributes.empty())
    {
        std::map<std::string, std::string> attributes;

        tfsxml_string attribute_name;
        tfsxml_string attribute_value;
        tfsxml_string tfsxml_priv_save=tfsxml_priv;
        while (!tfsxml_attr(&tfsxml_priv, &attribute_name, &attribute_value))
            attributes[std::string(attribute_name.buf, attribute_name.len)]=std::string(attribute_value.buf, attribute_value.len);
        tfsxml_priv=tfsxml_priv_save;

        std::map<std::string, std::string>::iterator it=path.attributes.begin();
        while (it!=path.attributes.end())
        {
             std::map<std::string, std::string>::iterator attribute=attributes.find(it->first);
             if (attribute==attributes.end() || attribute->second!=it->second)
                return false;

            it++;
        }
   }
   return true;
}

}

