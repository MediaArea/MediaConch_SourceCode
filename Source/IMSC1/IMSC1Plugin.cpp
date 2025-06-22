#include "IMSC1Plugin.h"
#include "Common/Version.h"
#include "Common/generated/XmlXsd.h"
#include "Common/generated/Ttml1Xsd.h"
#include "Common/generated/Ttml1DocumentXsd.h"
#include "Common/generated/Ttml1MetadataXsd.h"
#include "Common/generated/Ttml1MetadataAttribsXsd.h"
#include "Common/generated/Ttml1MetadataItemsXsd.h"
#include "Common/generated/Ttml1ParametersXsd.h"
#include "Common/generated/Ttml1ParameterAttribsXsd.h"
#include "Common/generated/Ttml1ParameterItemsXsd.h"
#include "Common/generated/Ttml1StylingXsd.h"
#include "Common/generated/Ttml1StylingAttribsXsd.h"
#include "Common/generated/Ttml1CoreAttribsXsd.h"
#include "Common/generated/Ttml1TimingAttribsXsd.h"
#include "Common/generated/Ttml1AnimationXsd.h"
#include "Common/generated/Ttml1ContentXsd.h"
#include "Common/generated/Ttml1DatatypesXsd.h"
#include "Common/generated/Ttml1HeadXsd.h"
#include "Common/generated/Ttml1LayoutXsd.h"
#include "Common/generated/Ttml1ProfileXsd.h"
#include "Common/generated/St205212010smpteTtXsd.h"
#include "Common/generated/TtmlWrapperXsd.h"

#include <fstream>
#include <streambuf>
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
    IMSC1Plugin::IMSC1Plugin(const std::string& wdir) : wdir(wdir)
    {
    }

    //---------------------------------------------------------------------------
    IMSC1Plugin::~IMSC1Plugin()
    {
    }

    //---------------------------------------------------------------------------
    std::string IMSC1Plugin::validate_file(const std::string& filename)
    {
        bool result;
        std::string error;
        std::string message;

        if(validate_ttml_file(filename, result, message, error))
            return std::string();

        std::string report;
        if (create_report(report, filename, result, message, error))
            return std::string();
        return report;
    }

    //---------------------------------------------------------------------------
    std::string IMSC1Plugin::validate_buffer(const std::string& buffer, const std::string& filename)
    {
        bool result;
        std::string error;
        std::string message;

        if(validate_ttml_buffer(buffer, filename, result, message, error))
            return std::string();

        std::string report;
        if (create_report(report, filename, result, message, error))
            return std::string();
        return report;
    }

    //---------------------------------------------------------------------------
    int IMSC1Plugin::create_report(std::string& report, const std::string& filename, bool result, const std::string& message, std::string&)
    {

        xmlDocPtr doc = xmlNewDoc((const xmlChar *)"1.0");

        xmlNodePtr root_node = xmlNewNode(NULL, (const xmlChar*)"MediaConch");
        xmlNewNs(root_node, (const xmlChar*)"https://mediaarea.net/mediaconch", NULL);
        xmlSetProp(root_node, (const xmlChar*)"version", (const xmlChar*)"0.2");
        xmlDocSetRootElement(doc, root_node);

        xmlNodePtr version_node = xmlNewNode(NULL, (const xmlChar*)"creatingApplication");
        xmlSetProp(version_node, (const xmlChar*)"version", (const xmlChar*)MEDIACONCH_VERSION);
        xmlSetProp(version_node, (const xmlChar*)"url", (const xmlChar*)"https://mediaarea.net/MediaConch");
        xmlNodeSetContent(version_node, (const xmlChar*)"MediaConch");
        xmlAddChild(root_node, version_node);

        xmlNodePtr media_node = xmlNewNode(NULL, (const xmlChar*)"media");
        xmlSetProp(media_node, (const xmlChar*)"ref", (const xmlChar*)filename.c_str());
        xmlAddChild(root_node, media_node);

        xmlNodePtr implementation_checks_node = xmlNewNode(NULL, (const xmlChar*)"implementationChecks");
        xmlSetProp(implementation_checks_node, (const xmlChar*)"checks_run", (const xmlChar*)"1");
        xmlSetProp(implementation_checks_node, (const xmlChar*)"fail_count", result ? (const xmlChar*)"0" : (const xmlChar*)"1");
        xmlSetProp(implementation_checks_node, (const xmlChar*)"pass_count", result ? (const xmlChar*)"1" : (const xmlChar*)"0");
        xmlAddChild(media_node, implementation_checks_node);

        xmlNodePtr name_node = xmlNewNode(NULL, (const xmlChar*)"name");
        xmlNodeSetContent(name_node, (const xmlChar*)"MediaConch IMSC1 Implementation Checker");
        xmlAddChild(implementation_checks_node, name_node);


        xmlNodePtr check_node = xmlNewNode(NULL, (const xmlChar*)"check");
        xmlSetProp(check_node, (const xmlChar*)"icid", (const xmlChar*)"XML conformance");
        xmlSetProp(check_node, (const xmlChar*)"checks_run", (const xmlChar*)"1");
        xmlSetProp(check_node, (const xmlChar*)"fail_count", result ? (const xmlChar*)"0" : (const xmlChar*)"1");
        xmlSetProp(check_node, (const xmlChar*)"pass_count", result ? (const xmlChar*)"1" : (const xmlChar*)"0");
        xmlAddChild(implementation_checks_node, check_node);

        xmlNodePtr test_node = xmlNewNode(NULL, (const xmlChar*)"test");
        xmlSetProp(test_node, (const xmlChar*)"outcome", result ? (const xmlChar*)"pass" : (const xmlChar*)"fail");
        if(!result && !message.empty())
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
    int IMSC1Plugin::validate_ttml_file(const std::string& filename, bool& result, std::string& message, std::string& error)
    {
        if(write_shemas(error))
            return -1;

        xmlSchemaPtr schema = NULL;

        xmlSchemaParserCtxtPtr pctxt = NULL;
        pctxt = xmlSchemaNewParserCtxt((wdir + "ttml-wrapper.xsd").c_str());
        xmlSchemaSetParserErrors(pctxt, NULL, xml_error_discard, NULL);
        schema = xmlSchemaParse(pctxt);
        if (!schema)
        {
            error = "Failed to compile schema.";
            clean_shemas(error);
            return -1;
        }
        xmlSchemaFreeParserCtxt(pctxt);

        xmlParserInputBufferPtr buf = NULL;
        buf = xmlParserInputBufferCreateFilename(filename.c_str(), XML_CHAR_ENCODING_NONE);
        if (!buf)
        {
            error = "Failed to open " + filename + ".";
            clean_shemas(error);
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
        clean_shemas(error);

        return 0;
    }

    //---------------------------------------------------------------------------
    int IMSC1Plugin::validate_ttml_buffer(const std::string& buffer, const std::string& filename, bool& result, std::string& message, std::string& error)
    {
        if(write_shemas(error))
            return -1;

        xmlSchemaPtr schema = NULL;

        xmlSchemaParserCtxtPtr pctxt = NULL;
        pctxt = xmlSchemaNewParserCtxt((wdir + "ttml-wrapper.xsd").c_str());
        xmlSchemaSetParserErrors(pctxt, NULL, xml_error_discard, NULL);
        schema = xmlSchemaParse(pctxt);
        if (!schema)
        {
            error = "Failed to compile schema.";
            clean_shemas(error);
            return -1;
        }
        xmlSchemaFreeParserCtxt(pctxt);

        xmlParserInputBufferPtr buf = NULL;
        buf = xmlParserInputBufferCreateMem(buffer.c_str(), buffer.size(), XML_CHAR_ENCODING_NONE);
        if (!buf)
        {
            error = "Failed to create buffer.";
            clean_shemas(error);
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
        clean_shemas(error);

        return 0;
    }

    //---------------------------------------------------------------------------
    int IMSC1Plugin::write_shema(const std::string& path, const std::string& file, const char* schema, std::string& error)
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
    int IMSC1Plugin::write_shemas(std::string& error)
    {
        if (write_shema(wdir, "xml.xsd", xml_xsd, error) ||
            write_shema(wdir, "ttml1.xsd", ttml1_xsd, error) ||
            write_shema(wdir, "ttml1-document.xsd", ttml1_document_xsd, error) ||
            write_shema(wdir, "ttml1-metadata.xsd", ttml1_metadata_xsd, error) ||
            write_shema(wdir, "ttml1-metadata-attribs.xsd", ttml1_metadata_attribs_xsd, error) ||
            write_shema(wdir, "ttml1-metadata-items.xsd", ttml1_metadata_items_xsd, error) ||
            write_shema(wdir, "ttml1-parameters.xsd", ttml1_parameters_xsd, error) ||
            write_shema(wdir, "ttml1-parameter-attribs.xsd", ttml1_parameter_attribs_xsd, error) ||
            write_shema(wdir, "ttml1-parameter-items.xsd", ttml1_parameter_items_xsd, error) ||
            write_shema(wdir, "ttml1-styling.xsd", ttml1_styling_xsd, error) ||
            write_shema(wdir, "ttml1-styling-attribs.xsd", ttml1_styling_attribs_xsd, error) ||
            write_shema(wdir, "ttml1-core-attribs.xsd", ttml1_core_attribs_xsd, error) ||
            write_shema(wdir, "ttml1-timing-attribs.xsd", ttml1_timing_attribs_xsd, error) ||
            write_shema(wdir, "ttml1-animation.xsd", ttml1_animation_xsd, error) ||
            write_shema(wdir, "ttml1-content.xsd", ttml1_content_xsd, error) ||
            write_shema(wdir, "ttml1-datatypes.xsd", ttml1_datatypes_xsd, error) ||
            write_shema(wdir, "ttml1-head.xsd", ttml1_head_xsd, error) ||
            write_shema(wdir, "ttml1-layout.xsd", ttml1_layout_xsd, error) ||
            write_shema(wdir, "ttml1-profile.xsd", ttml1_profile_xsd, error) ||
            write_shema(wdir, "st-2052-1-2010-smpte-tt.xsd", st_2052_1_2010_smpte_tt_xsd, error) ||
            write_shema(wdir, "ttml-wrapper.xsd", ttml_wrapper_xsd, error))
        {
            clean_shemas(error);
            return -1;
        }

        return 0;
    }

    //---------------------------------------------------------------------------
    int IMSC1Plugin::clean_shemas(std::string&)
    {
        std::remove((wdir + "xml.xsd").c_str());
        std::remove((wdir + "ttml1.xsd").c_str());
        std::remove((wdir + "ttml1-document.xsd").c_str());
        std::remove((wdir + "ttml1-metadata.xsd").c_str());
        std::remove((wdir + "ttml1-metadata-attribs.xsd").c_str());
        std::remove((wdir + "ttml1-metadata-items.xsd").c_str());
        std::remove((wdir + "ttml1-parameters.xsd").c_str());
        std::remove((wdir + "ttml1-parameter-attribs.xsd").c_str());
        std::remove((wdir + "ttml1-parameter-items.xsd").c_str());
        std::remove((wdir + "ttml1-styling.xsd").c_str());
        std::remove((wdir + "ttml1-styling-attribs.xsd").c_str());
        std::remove((wdir + "ttml1-core-attribs.xsd").c_str());
        std::remove((wdir + "ttml1-timing-attribs.xsd").c_str());
        std::remove((wdir + "ttml1-animation.xsd").c_str());
        std::remove((wdir + "ttml1-content.xsd").c_str());
        std::remove((wdir + "ttml1-datatypes.xsd").c_str());
        std::remove((wdir + "ttml1-head.xsd").c_str());
        std::remove((wdir + "ttml1-layout.xsd").c_str());
        std::remove((wdir + "ttml1-profile.xsd").c_str());
        std::remove((wdir + "st-2052-1-2010-smpte-tt.xsd").c_str());
        std::remove((wdir + "ttml-wrapper.xsd").c_str());

        return 0;
    }

    //---------------------------------------------------------------------------
    void IMSC1Plugin::xml_error_string(void* data, const char* format, ...)
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
    void IMSC1Plugin::xml_error_discard(void*, const char*, ...)
    {
    }
}

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(mediaconch_imsc1plugin) {
    emscripten::class_<MediaConch::IMSC1Plugin>("IMSC1Plugin")
        .constructor<const std::string&>()
        .function("validate_file", &MediaConch::IMSC1Plugin::validate_file)
        .function("validate_buffer", &MediaConch::IMSC1Plugin::validate_buffer)
    ;
}
#endif //__EMSCRIPTEN__
