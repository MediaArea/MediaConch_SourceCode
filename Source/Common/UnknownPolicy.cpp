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
int UnknownPolicy::import_schema_from_doc(xmlDocPtr doc, const std::string& filename)
{
    if (!doc)
    {
        error = "The document is not valid";
        return -1;
    }

    this->filename = filename;
    this->title = this->filename;

    ZenLib::Ztring z_path = ZenLib::Ztring().From_UTF8(filename);
    if (ZenLib::File::Exists(z_path))
        return 0;

    int ret = xmlSaveFormatFile(this->filename.c_str(), doc, 2);

    if (ret < 0)
    {
        error = "Cannot save the policy";
        return -1;
    }

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
