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
#include "Schema.h"
#include <iostream>
#include <sstream>
#include <string.h>
//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// Policy
//***************************************************************************

//---------------------------------------------------------------------------
    Policy::Policy(Policies *p, Policies::PolicyType t, bool n_https) : type(t), is_system(false), no_https(n_https),
                                                                        is_public(false), keep_public(true), policies(p)
{
    this->id = p->get_an_id();
}

//---------------------------------------------------------------------------
Policy::Policy(const Policy* p)
{
    if (p == this)
        return;

    this->name = p->name;
    this->description = p->description;
    this->license = p->license;
    this->is_system = false;
    this->is_public = p->is_public;
    this->keep_public = p->keep_public;
    this->no_https = p->no_https;
    this->policies = p->policies;
    this->id = p->policies->get_an_id();
}

//---------------------------------------------------------------------------
Policy::Policy(const Policy& p)
{
    if (&p == this)
        return;

    this->name = p.name;
    this->description = p.description;
    this->license = p.license;
    this->is_system = p.is_system;
    this->is_public = p.is_public;
    this->keep_public = p.keep_public;
    this->no_https = p.no_https;
    this->policies = p.policies;
    this->id = p.id;
}

//---------------------------------------------------------------------------
Policy::~Policy()
{
}

//---------------------------------------------------------------------------
int Policy::import_schema(const std::string& filename, const std::string& save_name)
{
    Schema s(no_https);
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
    xmlCleanupCharEncodingHandlers();
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

    Schema s(no_https);
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
    xmlSetGenericErrorFunc(NULL, NULL);
    return ret;
}

//---------------------------------------------------------------------------
int Policy::export_schema(const char* filename, std::string& err)
{
    Schema s(no_https);
    xmlSetGenericErrorFunc(&s, &s.manage_generic_error);

    xmlDocPtr new_doc = create_doc();
    if (!new_doc)
    {
        err = error;
        xmlSetGenericErrorFunc(NULL, NULL);
        return -1;
    }

    int ret = xmlSaveFormatFile(filename, new_doc, 2);
    xmlFreeDoc(new_doc);
    xmlSetGenericErrorFunc(NULL, NULL);
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
