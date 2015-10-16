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
#include "Schematron.h"
#include <fstream>
#include <sstream>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Schematron::Schematron() : Schema()
{
    schematron_ctx = NULL;
}

//---------------------------------------------------------------------------
Schematron::~Schematron()
{
    if (schematron_ctx != NULL)
    {
        xmlSchematronFree(schematron_ctx);
        schematron_ctx = NULL;
    }
}

//---------------------------------------------------------------------------
bool Schematron::register_schema_from_doc(void* data)
{
    xmlDocPtr doc = (xmlDocPtr)data;
    if (doc == NULL)
        return false;

    xmlLoadExtDtdDefaultValue |= 1;
    xmlSetGenericErrorFunc(this, &manage_generic_error);

    if (schematron_ctx != NULL)
    {
        xmlSchematronFree(schematron_ctx);
        schematron_ctx = NULL;
    }

    xmlSchematronParserCtxtPtr parser = xmlSchematronNewDocParserCtxt(doc);
    if (!parser)
        return false;

    schematron_ctx = xmlSchematronParse(parser); //TODO: Leak?
    xmlSchematronFreeParserCtxt(parser);
    if (schematron_ctx == NULL)
        return false;

    xmlSetGenericErrorFunc(NULL, NULL);
    return true;
}

//---------------------------------------------------------------------------
bool Schematron::register_schema_from_memory(const std::string& schem)
{
    xmlLoadExtDtdDefaultValue |= 1;
    xmlSetGenericErrorFunc(this, &manage_generic_error);

    if (schematron_ctx != NULL)
    {
        xmlSchematronFree(schematron_ctx);
        schematron_ctx = NULL;
    }

    xmlSchematronParserCtxtPtr parser =
        xmlSchematronNewMemParserCtxt(schem.c_str(), schem.length());
    if (!parser)
        return false;

    schematron_ctx = xmlSchematronParse(parser); //TODO: Leak?
    xmlSchematronFreeParserCtxt(parser);
    if (schematron_ctx == NULL)
        return false;

    xmlSetGenericErrorFunc(NULL, NULL);
    return true;
}

//---------------------------------------------------------------------------
int Schematron::validate_xml(std::string& xml, bool silent)
{
    if (schematron_ctx == NULL)
        return -1;

    //Hack for removing namespace so we use .sch without namespace. TODO: find a way to keep namespace with .sch policy input
    std::string xmlns("xmlns=\"https://mediaarea.net/mediaarea\"");
    size_t xmlns_pos=xml.rfind(xmlns, 1000);
    if (xmlns_pos!=std::string::npos)
        xml.erase(xmlns_pos, xmlns.size());

    int doc_flags = XML_PARSE_COMPACT | XML_PARSE_DTDLOAD;
    xmlSetGenericErrorFunc(this, &manage_generic_error);

#ifdef XML_PARSE_BIG_LINES
    doc_flags =| XML_PARSE_BIG_LINES;
#endif // !XML_PARSE_BIG_LINES

    xmlDocPtr doc = xmlReadMemory(xml.c_str(), xml.length(), NULL, NULL, doc_flags);
    if (doc == NULL)
        return -1;

    xmlSchematronValidCtxtPtr ctx = NULL;
    int validation_flags = XML_SCHEMATRON_OUT_TEXT;

    if (silent)
        validation_flags |= XML_SCHEMATRON_OUT_QUIET;

#if LIBXML_VERSION >= 20632
    validation_flags |= XML_SCHEMATRON_OUT_ERROR;
#endif
    ctx = xmlSchematronNewValidCtxt(schematron_ctx, validation_flags);

#if LIBXML_VERSION >= 20632
    xmlSchematronSetValidStructuredErrors(ctx, manage_error, this);
#endif

    int ret = xmlSchematronValidateDoc(ctx, doc);
    xmlSchematronFreeValidCtxt(ctx);
    xmlFreeDoc(doc);
    xmlSetGenericErrorFunc(NULL, NULL);

    std::stringstream Out;
    if (ret > 0)
    {
        for (size_t pos = 0; pos < errors.size(); pos++)
            Out << "\t" << errors[pos].c_str();
        if (!errors.size())
            Out << std::endl;
        report = Out.str();
    }
    return ret;
}

//***************************************************************************
// Callbacks
//***************************************************************************

//---------------------------------------------------------------------------
void Schematron::manage_error(void *userData, xmlErrorPtr err)
{
    Schematron *obj = (Schematron *)userData;
    if (!err || err->code == XML_ERR_OK) {
        return;
    }
    obj->errors.push_back(err->message);
}

//---------------------------------------------------------------------------
void Schematron::manage_generic_error(void *userData, const char* msg, ...)
{
    Schematron *obj = (Schematron *)userData;
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
