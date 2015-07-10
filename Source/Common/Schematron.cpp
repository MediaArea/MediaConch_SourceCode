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
using namespace std;
//---------------------------------------------------------------------------


//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Schematron::Schematron()
{
    schematron_ctx = NULL;
}

//---------------------------------------------------------------------------
Schematron::~Schematron()
{
    if (schematron_ctx != NULL) {
        xmlSchematronFree(schematron_ctx);
        schematron_ctx = NULL;
    }
    schema.clear();
}

//---------------------------------------------------------------------------
bool Schematron::register_schema_from_doc(xmlDocPtr doc)
{
    if (doc == NULL) {
        return false;
    }
    xmlLoadExtDtdDefaultValue |= 1;
    xmlSetGenericErrorFunc(this, &manage_generic_error);

    if (schematron_ctx != NULL) {
        xmlSchematronFree(schematron_ctx);
        schematron_ctx = NULL;
    }
    xmlSchematronParserCtxtPtr parser = xmlSchematronNewDocParserCtxt(doc);
    if (!parser) {
        return false;
    }

    schematron_ctx = xmlSchematronParse(parser); //TODO: Leak?
    xmlSchematronFreeParserCtxt(parser);
    if (schematron_ctx == NULL) {
        return false;
    }
    xmlSetGenericErrorFunc(NULL, NULL);
    return true;
}

//---------------------------------------------------------------------------
bool Schematron::register_schema_from_memory()
{
    xmlLoadExtDtdDefaultValue |= 1;
    xmlSetGenericErrorFunc(this, &manage_generic_error);

    if (schematron_ctx != NULL) {
        xmlSchematronFree(schematron_ctx);
        schematron_ctx = NULL;
    }
    xmlSchematronParserCtxtPtr parser =
        xmlSchematronNewMemParserCtxt(schema.c_str(), schema.length());
    if (!parser) {
        return false;
    }

    schematron_ctx = xmlSchematronParse(parser); //TODO: Leak?
    xmlSchematronFreeParserCtxt(parser);
    if (schematron_ctx == NULL) {
        return false;
    }
    xmlSetGenericErrorFunc(NULL, NULL);
    return true;
}

//---------------------------------------------------------------------------
bool Schematron::register_schema_from_file(const char* filename)
{
    schema = read_file(filename);
    if (!schema.length()) {
        stringstream error;

        error << "Schematron file:'";
        error << filename;
        error << "' does not exist\n";
        errors.push_back(error.str());
        return false;
    }

    return register_schema_from_memory();
}

//---------------------------------------------------------------------------
int Schematron::validate_xml(const char* xml, size_t len, bool silent)
{
    if (schematron_ctx == NULL)
        return -1;

    int doc_flags = XML_PARSE_COMPACT | XML_PARSE_DTDLOAD;
    xmlSetGenericErrorFunc(this, &manage_generic_error);

#ifdef XML_PARSE_BIG_LINES
    doc_flags =| XML_PARSE_BIG_LINES;
#endif //  | XML_PARSE_BIG_LINES

    xmlDocPtr doc = xmlReadMemory(xml, len, NULL, NULL, doc_flags);
    if (doc == NULL)
        return -1;

    xmlSchematronValidCtxtPtr ctx = NULL;
    int validation_flags = XML_SCHEMATRON_OUT_TEXT;

    if (silent) {
        validation_flags |= XML_SCHEMATRON_OUT_QUIET;
    }

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
    return ret;
}

//---------------------------------------------------------------------------
int Schematron::validate_xml_from_file(const char* filename, bool silent)
{
    if (schematron_ctx == NULL)
        return -1;

    string xml = read_file(filename);
    if (!xml.length())
        return -1;
    return validate_xml(xml.c_str(), xml.length(), silent);
}

//---------------------------------------------------------------------------
string Schematron::read_file(const char* filename)
{
    // open at the end (ate) to get the length of the file
    std::ifstream file_handler(filename, ios_base::ate);
    string buffer;

    if (!file_handler) {
        return buffer;
    }
    buffer.reserve(file_handler.tellg());
    file_handler.seekg(0, file_handler.beg);

    buffer.assign(std::istreambuf_iterator<char>(file_handler),
                  std::istreambuf_iterator<char>());
    file_handler.close();
    return buffer;
}

void Schematron::manage_error(void *userData, xmlErrorPtr err)
{
    Schematron *obj = (Schematron *)userData;
    if (!err || err->code == XML_ERR_OK) {
        return;
    }
    obj->errors.push_back(err->message);
}

void Schematron::manage_generic_error(void *userData, const char* msg, ...)
{
    Schematron *obj = (Schematron *)userData;
    va_list args;
    char buf[4096] = {0};

    va_start(args, msg);
    int ret = vsnprintf(buf, sizeof(buf), msg, args);
    if (ret < 0)
        ret = 0;
    buf[ret] = '\0';
    obj->errors.push_back(buf);
    va_end(args);
}
