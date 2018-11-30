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
#include <libxml/tree.h>
#include <libexslt/exslt.h>
#include "Xslt.h"
#include "Core.h"
#include <fstream>
#include <sstream>
#include <string.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

size_t Xslt::instances = 0;

//---------------------------------------------------------------------------
Xslt::Xslt(bool no_https) : Schema(no_https)
{
    xslt_ctx = NULL;
    doc_ctx = NULL;

    CS.Enter();
    if (instances == 0)
        exsltRegisterAll();

    instances++;
    CS.Leave();
}

//---------------------------------------------------------------------------
Xslt::~Xslt()
{
    if (xslt_ctx)
    {
        xsltFreeStylesheet(xslt_ctx);
        xslt_ctx = NULL;
        doc_ctx = NULL;
    }
    if (doc_ctx)
    {
        xmlFreeDoc(doc_ctx);
        doc_ctx = NULL;
    }

    CS.Enter();
    instances--;

    if (instances == 0)
        xsltCleanupGlobals();
    CS.Leave();
}

//---------------------------------------------------------------------------
bool Xslt::register_schema_from_doc(void* data)
{
    xmlDocPtr doc = (xmlDocPtr)data;
    if (doc == NULL)
        return false;

    xsltSetGenericErrorFunc(this, &manage_generic_error);
    xmlLoadExtDtdDefaultValue = 1;

    if (xslt_ctx)
    {
        xsltFreeStylesheet(xslt_ctx);
        xslt_ctx = NULL;
        doc_ctx = NULL;
    }

    if (doc_ctx)
    {
        xmlFreeDoc(doc_ctx);
        doc_ctx = NULL;
    }

    doc_ctx = xmlCopyDoc(doc, 1);
    xslt_ctx = xsltParseStylesheetDoc(doc_ctx);

    xsltSetGenericErrorFunc(NULL, NULL);

    return true;
}

//---------------------------------------------------------------------------
bool Xslt::register_schema_from_memory(const std::string& schem)
{
    xmlLoadExtDtdDefaultValue = 1;
    xmlSetGenericErrorFunc(this, &manage_generic_error);

    int doc_flags = XML_PARSE_COMPACT | XML_PARSE_DTDLOAD;
#ifdef XML_PARSE_BIG_LINES
    doc_flags =| XML_PARSE_BIG_LINES;
#endif // !XML_PARSE_BIG_LINES

    xmlDocPtr doc = NULL;
    if (no_https)
    {
        std::string memory = schem;
        Core::unify_no_https(memory);
        doc = xmlReadMemory(memory.c_str(), memory.length(), NULL, NULL, doc_flags);
    }
    else
        doc = xmlReadMemory(schem.c_str(), schem.length(), NULL, NULL, doc_flags);

    if (doc == NULL)
        return false;

    bool ret = register_schema_from_doc(doc);

    xmlFreeDoc(doc);
    xmlSetGenericErrorFunc(NULL, NULL);
    return ret;
}

//---------------------------------------------------------------------------
int Xslt::validate_xml(const std::string& xml, bool)
{
    report.clear();
    if (!xslt_ctx)
        return -1;

    xmlSubstituteEntitiesDefault(1);
    xmlLoadExtDtdDefaultValue = 1;

    int doc_flags = XML_PARSE_COMPACT | XML_PARSE_DTDLOAD;
    xmlSetGenericErrorFunc(this, &manage_generic_error);

#ifdef XML_PARSE_BIG_LINES
    doc_flags =| XML_PARSE_BIG_LINES;
#endif // !XML_PARSE_BIG_LINES

    xmlDocPtr doc = xmlReadMemory(xml.c_str(), xml.length(), NULL, NULL, doc_flags);
    if (!doc)
        return -1;

    const char** params = NULL;
    std::vector<std::string> vec;

    if (options.size())
    {
        std::map<std::string, std::string>::iterator it = options.begin();
        for (; it != options.end(); ++it)
        {
            if (!it->second.size())
                continue;
            vec.push_back(std::string(it->first));
            vec.push_back(std::string(it->second));
        }

        params = new const char* [vec.size() + 1];
        size_t i = 0;
        for(; i < vec.size(); ++i)
            params[i] = vec[i].c_str();
        params[i] = NULL;
    }

    xmlDocPtr res = xsltApplyStylesheet(xslt_ctx, doc, params);

    if (params)
        delete [] params;

    if (!res)
    {
        xmlFreeDoc(doc);
        return -1;
    }

    xmlChar *doc_txt_ptr = NULL;
    int doc_txt_len = 0;
    if (xsltSaveResultToString(&doc_txt_ptr, &doc_txt_len, res, xslt_ctx) < 0)
    {
        xmlFreeDoc(doc);
        xmlFreeDoc(res);
        return -1;
    }
    size_t Prefix = 0;
    if (doc_txt_len>3 && doc_txt_ptr[0]=='\n' && doc_txt_ptr[1]==' ' && doc_txt_ptr[2]==' ')
        Prefix=3; // TODO: we see such prefix in output, we need to understand the reason and remove it fro XSL instead of this hack
    report = std::string((const char*)doc_txt_ptr+Prefix, doc_txt_len);
    free(doc_txt_ptr);

    xmlFreeDoc(doc);
    xmlFreeDoc(res);
    xmlSetGenericErrorFunc(NULL, NULL);

    std::stringstream Out;
    if (errors.size())
    {
        for (size_t pos = 0; pos < errors.size(); pos++)
            Out << "\t" << errors[pos].c_str();
        if (!errors.size())
            Out << std::endl;
        report = Out.str();
        return 1;
    }
    return 0;
}

//***************************************************************************
// Callbacks
//***************************************************************************

//---------------------------------------------------------------------------
void Xslt::manage_error(void *userData, xmlErrorPtr err)
{
    Xslt *obj = (Xslt *)userData;
    if (!err || err->code == XML_ERR_OK) {
        return;
    }
    obj->errors.push_back(err->message);
}

//---------------------------------------------------------------------------
void Xslt::manage_generic_error(void *userData, const char* msg, ...)
{
    Xslt *obj = (Xslt *)userData;
    va_list args;
    char buf[4096] = {0};

    va_start(args, msg);
#ifdef _MSC_VER
    int ret = vsnprintf_s(buf, sizeof(buf), _TRUNCATE, msg, args);
#else //_MSC_VER
    int ret = vsnprintf(buf, sizeof(buf), msg, args);
    if (ret < 0)
        ret = 0;
    buf[ret] = '\0';
#endif //_MSC_VER
    obj->errors.push_back(buf);
    va_end(args);
}

}
