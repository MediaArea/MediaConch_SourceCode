/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Path helper
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef PathH
#define PathH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "ThirdParty/tfsxml/tfsxml.h"

#include <string>
#include <vector>
#include <map>

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

//---------------------------------------------------------------------------
struct PathElement
{
    std::string name;
    std::map<std::string, std::string> attributes;
    size_t occurrence;
    bool valid;

    PathElement() : occurrence((size_t)-1), valid(false) {};
};

std::vector<PathElement> parse_path(const std::string& xpath);
bool path_is_matching(tfsxml_string& tfsxml_priv, tfsxml_string& node, PathElement path, size_t& occurrence);
}

#endif
