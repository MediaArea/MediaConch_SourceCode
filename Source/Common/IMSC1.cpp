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
#include "IMSC1.h"
#include "generated/XmlXsd.h"
#include "generated/Ttml1Xsd.h"
#include "generated/Ttml1DocumentXsd.h"
#include "generated/Ttml1MetadataXsd.h"
#include "generated/Ttml1MetadataAttribsXsd.h"
#include "generated/Ttml1MetadataItemsXsd.h"
#include "generated/Ttml1ParametersXsd.h"
#include "generated/Ttml1ParameterAttribsXsd.h"
#include "generated/Ttml1ParameterItemsXsd.h"
#include "generated/Ttml1StylingXsd.h"
#include "generated/Ttml1StylingAttribsXsd.h"
#include "generated/Ttml1CoreAttribsXsd.h"
#include "generated/Ttml1TimingAttribsXsd.h"
#include "generated/Ttml1AnimationXsd.h"
#include "generated/Ttml1ContentXsd.h"
#include "generated/Ttml1DatatypesXsd.h"
#include "generated/Ttml1HeadXsd.h"
#include "generated/Ttml1LayoutXsd.h"
#include "generated/Ttml1ProfileXsd.h"
#include "generated/St205212010smpteTtXsd.h"
#include "generated/TtmlWrapperXsd.h"

#include <fstream>
#include <algorithm>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xmlschemas.h>

//---------------------------------------------------------------------------

namespace MediaConch {

    //***************************************************************************
    // Constructor/Destructor
    //***************************************************************************

    //---------------------------------------------------------------------------
    IMSC1::IMSC1()
    {
        type = MediaConchLib::PLUGIN_FORMAT;
        report_kind = MediaConchLib::report_MediaImsc1Validation;
    }

    //---------------------------------------------------------------------------
    IMSC1::~IMSC1()
    {
    }

    //---------------------------------------------------------------------------
    IMSC1::IMSC1(const IMSC1& p): PluginFormat(p)
    {
    }

    //---------------------------------------------------------------------------
    int IMSC1::load_plugin(const std::map<std::string, Container::Value>& obj, std::string& error)
    {
        if (obj.find("format") == obj.end() || obj.at("format").type != Container::Value::CONTAINER_TYPE_STRING)
        {
            error += "Field 'format' is not present\n";
            return -1;
        }
        format = obj.at("format").s;

        return 0;
    }

    //---------------------------------------------------------------------------
    int IMSC1::run(std::string& error)
    {
        bool pass = false;
        std::string message;

        if (validate_ttml_file(pass, message, error))
            return -1;

        xmlDocPtr doc = xmlNewDoc((const xmlChar *)"1.0");

        xmlNodePtr root_node = xmlNewNode(NULL, (const xmlChar*)"MediaConch");
        xmlNewNs(root_node, (const xmlChar*)"https://mediaarea.net/mediaconch", NULL);
        xmlSetProp(root_node, (const xmlChar*)"version", (const xmlChar*)"0.2");
        xmlDocSetRootElement(doc, root_node);

        xmlNodePtr media_node = xmlNewNode(NULL, (const xmlChar*)"media");
        xmlSetProp(media_node, (const xmlChar*)"ref", (const xmlChar*)filename.c_str());
        xmlAddChild(root_node, media_node);

        xmlNodePtr implementation_checks_node = xmlNewNode(NULL, (const xmlChar*)"implementationChecks");
        xmlSetProp(implementation_checks_node, (const xmlChar*)"checks_run", (const xmlChar*)"1");
        xmlSetProp(implementation_checks_node, (const xmlChar*)"fail_count", pass ? (const xmlChar*)"0" : (const xmlChar*)"1");
        xmlSetProp(implementation_checks_node, (const xmlChar*)"pass_count", pass ? (const xmlChar*)"1" : (const xmlChar*)"0");
        xmlAddChild(media_node, implementation_checks_node);

        xmlNodePtr name_node = xmlNewNode(NULL, (const xmlChar*)"name");
        xmlNodeSetContent(name_node, (const xmlChar*)"MediaConch IMSC1 Implementation Checker");
        xmlAddChild(implementation_checks_node, name_node);


        xmlNodePtr check_node = xmlNewNode(NULL, (const xmlChar*)"check");
        xmlSetProp(check_node, (const xmlChar*)"icid", (const xmlChar*)"XML conformance");
        xmlSetProp(check_node, (const xmlChar*)"checks_run", (const xmlChar*)"1");
        xmlSetProp(check_node, (const xmlChar*)"fail_count", pass ? (const xmlChar*)"0" : (const xmlChar*)"1");
        xmlSetProp(check_node, (const xmlChar*)"pass_count", pass ? (const xmlChar*)"1" : (const xmlChar*)"0");
        xmlAddChild(implementation_checks_node, check_node);

        xmlNodePtr test_node = xmlNewNode(NULL, (const xmlChar*)"test");
        xmlSetProp(test_node, (const xmlChar*)"outcome", pass ? (const xmlChar*)"pass" : (const xmlChar*)"fail");
        if(!pass && !message.empty())
            xmlSetProp(test_node, (const xmlChar*)"reason", (const xmlChar*)message.c_str());
        xmlAddChild(check_node, test_node);

        xmlChar *buf;
        int buf_size;
        xmlDocDumpFormatMemoryEnc(doc, &buf, &buf_size, "UTF-8", 1);

        report = std::string((char*)buf);

        xmlFree(buf);
        xmlFreeDoc(doc);
        xmlCleanupParser();

        return 0;
    }

    //---------------------------------------------------------------------------
    int IMSC1::validate_ttml_file(bool& result, std::string& message, std::string& error)
    {
        std::string schema_dir;
        if (create_report_dir("TTMLTemp", "TTMLSchemaDir", schema_dir) < 0)
        {
            error = "Unable to create temporary directory.";
            return -1;
        }

        if (write_shema(schema_dir, "xml.xsd", xml_xsd, error) ||
            write_shema(schema_dir, "ttml1.xsd", ttml1_xsd, error) ||
            write_shema(schema_dir, "ttml1-document.xsd", ttml1_document_xsd, error) ||
            write_shema(schema_dir, "ttml1-metadata.xsd", ttml1_metadata_xsd, error) ||
            write_shema(schema_dir, "ttml1-metadata-attribs.xsd", ttml1_metadata_attribs_xsd, error) ||
            write_shema(schema_dir, "ttml1-metadata-items.xsd", ttml1_metadata_items_xsd, error) ||
            write_shema(schema_dir, "ttml1-parameters.xsd", ttml1_parameters_xsd, error) ||
            write_shema(schema_dir, "ttml1-parameter-attribs.xsd", ttml1_parameter_attribs_xsd, error) ||
            write_shema(schema_dir, "ttml1-parameter-items.xsd", ttml1_parameter_items_xsd, error) ||
            write_shema(schema_dir, "ttml1-styling.xsd", ttml1_styling_xsd, error) ||
            write_shema(schema_dir, "ttml1-styling-attribs.xsd", ttml1_styling_attribs_xsd, error) ||
            write_shema(schema_dir, "ttml1-core-attribs.xsd", ttml1_core_attribs_xsd, error) ||
            write_shema(schema_dir, "ttml1-timing-attribs.xsd", ttml1_timing_attribs_xsd, error) ||
            write_shema(schema_dir, "ttml1-animation.xsd", ttml1_animation_xsd, error) ||
            write_shema(schema_dir, "ttml1-content.xsd", ttml1_content_xsd, error) ||
            write_shema(schema_dir, "ttml1-datatypes.xsd", ttml1_datatypes_xsd, error) ||
            write_shema(schema_dir, "ttml1-head.xsd", ttml1_head_xsd, error) ||
            write_shema(schema_dir, "ttml1-layout.xsd", ttml1_layout_xsd, error) ||
            write_shema(schema_dir, "ttml1-profile.xsd", ttml1_profile_xsd, error) ||
            write_shema(schema_dir, "st-2052-1-2010-smpte-tt.xsd", st_2052_1_2010_smpte_tt_xsd, error) ||
            write_shema(schema_dir, "ttml-wrapper.xsd", ttml_wrapper_xsd, error))
        {
            delete_report_dir(schema_dir);
            return -1;
        }

        xmlSchemaPtr schema = NULL;

        xmlSchemaParserCtxtPtr pctxt = NULL;
        pctxt = xmlSchemaNewParserCtxt((schema_dir + "ttml-wrapper.xsd").c_str());
        xmlSchemaSetParserErrors(pctxt, NULL, xml_error_discard, NULL);
        schema = xmlSchemaParse(pctxt);
        if (!schema)
        {
            error = "Failed to compile schema.";
            delete_report_dir(schema_dir);
            return -1;
        }
        xmlSchemaFreeParserCtxt(pctxt);

        xmlParserInputBufferPtr buf = NULL;
        buf = xmlParserInputBufferCreateFilename(filename.c_str(), XML_CHAR_ENCODING_NONE);
        if (!buf)
        {
            error = "Failed to open " + filename + ".";
            delete_report_dir(schema_dir);
            return -1;
        }

        xmlSchemaValidCtxtPtr vctxt = NULL;
        vctxt = xmlSchemaNewValidCtxt(schema);
        xmlSchemaSetValidErrors(vctxt, xml_error_string, NULL, (void*)&message);
        xmlSchemaValidateSetFilename(vctxt, filename.c_str());

        if(!xmlSchemaValidateStream(vctxt, buf, XML_CHAR_ENCODING_NONE, NULL, NULL))
            result = true;
        else
            result = false;

        xmlSchemaFreeValidCtxt(vctxt);
        delete_report_dir(schema_dir);

        return 0;
    }

    //---------------------------------------------------------------------------
    int IMSC1::write_shema(const std::string& path, const std::string& file, const char* schema, std::string& error)
    {
        std::ofstream ofs;
        ofs.open((path + file).c_str(), std::ofstream::out | std::ofstream::binary);

        if (!ofs.is_open())
        {
            error = "Unable to write schema file.";
            return -1;
        }

        ofs << schema;
        ofs.close();

        return 0;
    }

    //---------------------------------------------------------------------------
    void IMSC1::xml_error_string(void* data, const char* format, ...)
    {
        if(!data)
            return;


        va_list args;

        std::string* obj=(std::string*)data;


        va_start(args, format);
        int ret = vsnprintf(NULL, 0, format, args);
        va_end(args);

        if(ret < 0)
            return;

        char* buf = new char[ret+1];
        if (!buf)
            return;

        va_start(args, format);
        vsnprintf(buf, ret + 1, format, args);
        buf[ret] = '\0';
        va_end(args);

        std::string tmp(buf);
        delete[] buf;

        tmp.erase(std::remove(tmp.begin(), tmp.end(), '\n'), tmp.end());

        if (!obj->empty())
            *obj += " ";
        obj->append(tmp);


        return;
    }

    //---------------------------------------------------------------------------
    void IMSC1::xml_error_discard(void* user_data, const char* format, ...)
    {
    }
}
