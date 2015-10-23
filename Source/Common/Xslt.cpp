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
#include <libxml/tree.h>
#include "Xslt.h"
#include <fstream>
#include <sstream>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Xslt::Xslt() : Schema()
{
    xslt_ctx = NULL;
    doc_ctx = NULL;
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
    return xslt_ctx != NULL;
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
    xmlDocPtr doc = xmlReadMemory(schem.c_str(), schem.length(), NULL, NULL, doc_flags);
    if (doc == NULL)
        return -1;

    bool ret = register_schema_from_doc(doc);

    xmlFreeDoc(doc);
    xmlSetGenericErrorFunc(NULL, NULL);
    return ret;
}

//---------------------------------------------------------------------------
int Xslt::validate_xml(std::string& xml, bool)
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

    xmlDocPtr res = xsltApplyStylesheet(xslt_ctx, doc, NULL);

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
    report = std::string((const char*)doc_txt_ptr, doc_txt_len);

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
    else if (outcome_has_fail(report))
        return 1;
    return 0;
}

//---------------------------------------------------------------------------
bool Xslt::outcome_has_fail(std::string& report)
{
    if (report.find(" outcome=\"fail\"") != std::string::npos)
        return true;
    return false;
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
