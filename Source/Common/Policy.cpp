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
    this->is_system = false;
    this->no_https = p->no_https;
}

//---------------------------------------------------------------------------
Policy::~Policy()
{
}

//---------------------------------------------------------------------------
int Policy::import_schema(const std::string& filename, const std::string& save_name)
{
    Schematron s(no_https);
    xmlSetGenericErrorFunc(&s, &s.manage_generic_error);

    xmlDocPtr doc = xmlParseFile(filename.c_str());
    if (!doc)
    {
        // maybe put the errors from s.errors
        error = "The schema cannot be parsed";
        xmlSetGenericErrorFunc(NULL, NULL);
        return -1;
    }

    int ret = import_schema_from_doc(doc, save_name);
    xmlFreeDoc(doc);
    saved = true;
    xmlSetGenericErrorFunc(NULL, NULL);
    return ret;
}

//---------------------------------------------------------------------------
int Policy::import_schema_from_memory(const char* buffer, int len, const std::string& save_name)
{
    if (!buffer || !len)
    {
        error = "The schema does not exist";
        return -1;
    }

    Schematron s(no_https);
    xmlSetGenericErrorFunc(&s, &s.manage_generic_error);

    xmlDocPtr doc = xmlParseMemory(buffer, len);
    if (!doc)
    {
        // maybe put the errors from s.errors
        error = "The schema given cannot be parsed";
        xmlSetGenericErrorFunc(NULL, NULL);
        return -1;
    }

    int ret = import_schema_from_doc(doc, save_name);
    xmlFreeDoc(doc);
    saved = true;
    xmlSetGenericErrorFunc(NULL, NULL);
    return ret;
}

//---------------------------------------------------------------------------
int Policy::export_schema(const char* filename, std::string& err)
{
    Schematron s(no_https);
    xmlSetGenericErrorFunc(&s, &s.manage_generic_error);

    xmlDocPtr new_doc = create_doc();
    if (!new_doc)
    {
        err = "cannot create the XML Document";
        xmlSetGenericErrorFunc(NULL, NULL);
        return -1;
    }

    int ret = xmlSaveFormatFile(filename, new_doc, 2);
    xmlFreeDoc(new_doc);
    xmlSetGenericErrorFunc(NULL, NULL);
    saved = true;
    if (ret < 0)
    {
        err = "cannot save the schema";
        return -1;
    }

    return 0;
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
