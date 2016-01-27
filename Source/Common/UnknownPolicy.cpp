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
#include "Core.h"
#include "Policies.h"
#include "Policy.h"
#include "UnknownPolicy.h"
#include "ZenLib/Ztring.h"
#include "ZenLib/File.h"
#include <iostream>
#include <sstream>
#include <string.h>

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// UnknownPolicy
//***************************************************************************

//---------------------------------------------------------------------------
UnknownPolicy::UnknownPolicy(const UnknownPolicy* s) : Policy(s)
{
    type = Policies::POLICY_UNKNOWN;
    this->filename = s->filename;
}

//---------------------------------------------------------------------------
UnknownPolicy::~UnknownPolicy()
{
}

//---------------------------------------------------------------------------
int UnknownPolicy::import_schema_from_doc(const std::string& filename, xmlDocPtr doc)
{
    if (!doc)
    {
        error = "The document is not valid";
        return -1;
    }

    std::string path = Core::get_local_data_path() + "policies/";

    size_t pos = filename.rfind("/");
    std::string file;
    if (pos != std::string::npos)
    {
        if (filename.substr(0, pos + 1) == path)
        {
            this->filename = filename;
            this->title = this->filename;
            return 0;
        }
        file = filename.substr(pos + 1);
    }
    path += file;

    std::string current_path = path;
    for (size_t i = 0; ; ++i)
    {
        ZenLib::Ztring z_path = ZenLib::Ztring().From_UTF8(current_path);
        if (!ZenLib::File::Exists(z_path))
            break;

        std::stringstream ss;
        if (path.substr(path.length() - 4) == ".xsl")
            ss << path.substr(0, path.length() - 4) << i << ".xsl";
        else
            ss << path << i;
        current_path = ss.str();
    }
    this->filename = current_path;
    this->title = this->filename;
    xmlSaveFormatFile(this->filename.c_str(), doc, 2);

    return 0;
}

//---------------------------------------------------------------------------
xmlDocPtr UnknownPolicy::create_doc()
{
    xmlDocPtr doc = xmlParseFile(filename.c_str());
    if (!doc)
    {
        error = "The schema cannot be parsed";
        return NULL;
    }
    return doc;
}

}
