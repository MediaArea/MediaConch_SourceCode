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
#include "Policies.h"
#include "Policy.h"
#include "Schematron.h"
#include <iostream>
#include <sstream>
#include <string.h>
//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// Policy
//***************************************************************************

//---------------------------------------------------------------------------
Policy::Policy(const Policy* p)
{
    if (p == this) {
        return;
    }

    this->title = p->title;
    this->description = p->description;
    this->saved = false;
}

//---------------------------------------------------------------------------
Policy::~Policy()
{
}

//---------------------------------------------------------------------------
std::string Policy::import_schema(const std::string& filename)
{
    Schematron s;
    xmlSetGenericErrorFunc(&s, &s.manage_generic_error);

    xmlDocPtr doc = xmlParseFile(filename.c_str());
    if (!doc)
    {
        // maybe put the errors from s.errors
        return "The schema cannot be parsed";
    }

    std::string ret = import_schema_from_doc(filename, doc);
    xmlFreeDoc(doc);
    saved = true;
    return ret;
}

//---------------------------------------------------------------------------
std::string Policy::import_schema_from_memory(const std::string& filename, const char* buffer, int len)
{
    if (!buffer || !len)
        return "The schematron does not exist";

    Schematron s;
    xmlSetGenericErrorFunc(&s, &s.manage_generic_error);

    xmlDocPtr doc = xmlParseMemory(buffer, len);
    if (!doc)
    {
        // maybe put the errors from s.errors
        return "The schema given cannot be parsed";
    }

    std::string ret = import_schema_from_doc(filename, doc);
    xmlFreeDoc(doc);
    saved = true;
    return ret;
}

//---------------------------------------------------------------------------
void Policy::export_schema(const char* filename)
{
    xmlDocPtr new_doc = create_doc();

    xmlSaveFormatFile(filename, new_doc, 2);
    xmlFreeDoc(new_doc);
    saved = true;
}

}
