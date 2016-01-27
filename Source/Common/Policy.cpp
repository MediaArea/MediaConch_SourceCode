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
int Policy::import_schema(const std::string& filename)
{
    Schematron s;
    xmlSetGenericErrorFunc(&s, &s.manage_generic_error);

    xmlDocPtr doc = xmlParseFile(filename.c_str());
    if (!doc)
    {
        // maybe put the errors from s.errors
        error = "The schema cannot be parsed";
        return -1;
    }

    int ret = import_schema_from_doc(filename, doc);
    printf("import value in policy: %d\n", ret);
    xmlFreeDoc(doc);
    saved = true;
    return ret;
}

//---------------------------------------------------------------------------
int Policy::import_schema_from_memory(const std::string& filename, const char* buffer, int len)
{
    if (!buffer || !len)
    {
        error = "The schematron does not exist";
        return -1;
    }

    Schematron s;
    xmlSetGenericErrorFunc(&s, &s.manage_generic_error);

    xmlDocPtr doc = xmlParseMemory(buffer, len);
    if (!doc)
    {
        // maybe put the errors from s.errors
        error = "The schema given cannot be parsed";
        return -1;
    }

    int ret = import_schema_from_doc(filename, doc);
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

//---------------------------------------------------------------------------
int Policy::dump_schema(std::string& data)
{
    xmlDocPtr new_doc = create_doc();

    if (!new_doc)
        return -1;

    xmlChar *mem = NULL;
    int size = 0;
    xmlDocDumpFormatMemory(new_doc, &mem, &size, 2);
    if (size > 0)
    {
        data = std::string((const char*)mem, size);
        xmlFree(mem);
    }
    xmlFreeDoc(new_doc);
    return 0;
}

}
