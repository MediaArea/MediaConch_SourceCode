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
UnknownPolicy::UnknownPolicy(const UnknownPolicy* s) : Policy(s), system_doc(NULL)
{
    type = Policies::POLICY_UNKNOWN;
}

//---------------------------------------------------------------------------
UnknownPolicy::~UnknownPolicy()
{
    if (system_doc)
        xmlFreeDoc(system_doc);
}

//---------------------------------------------------------------------------
int UnknownPolicy::import_schema_from_doc(xmlDocPtr doc, const std::string& filename)
{
    if (!doc)
    {
        error = "The document is not valid";
        return -1;
    }

    if (!filename.length())
    {
        error = "Cannot copy the policy";
        return -1;
    }

    this->filename = filename;
    std::string title = filename;
    size_t pos = title.rfind("/");
    this->title = title.substr(pos == std::string::npos? 0 : pos + 1);

    size_t ext_pos;
    if ((ext_pos = this->title.rfind(".")) != std::string::npos)
        this->title = this->title.substr(0, ext_pos);

    if (!this->title.length())
        this->title = "Policy example";

    if (filename.find(":/") == 0)
    {
        system_doc = xmlCopyDoc(doc, 1);
        return 0;
    }

    ZenLib::Ztring z_path = ZenLib::Ztring().From_UTF8(filename);
    if (ZenLib::File::Exists(z_path))
        return 0;

    int ret = xmlSaveFormatFile(this->filename.c_str(), doc, 2);

    if (ret < 0)
    {
        error = "Cannot copy the policy";
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
xmlDocPtr UnknownPolicy::create_doc()
{
    xmlDocPtr doc = NULL;

    if (system_doc)
        doc = xmlCopyDoc(system_doc, 1);
    else
        doc = xmlParseFile(filename.c_str());
    if (!doc)
    {
        error = "Unknwn policy should be a valid XML";
        return NULL;
    }
    return doc;
}

}
