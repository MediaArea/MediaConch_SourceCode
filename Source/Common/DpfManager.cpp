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
#include "DpfManager.h"
#include "Core.h"
#include <ZenLib/File.h>
#include <ZenLib/Dir.h>
#include <sstream>
#include <fstream>
#include <libxml/tree.h>

#if defined(WINDOWS)
#include <windows.h>
#include <conio.h>
#else
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#endif

//---------------------------------------------------------------------------
namespace MediaConch {

    //***************************************************************************
    // Constructor/Destructor
    //***************************************************************************

    //---------------------------------------------------------------------------
    DPFManager::DPFManager()
    {
        type = MediaConchLib::PLUGIN_FORMAT;
        report_kind = MediaConchLib::report_MediaDpfManager;
    }

    //---------------------------------------------------------------------------
    DPFManager::~DPFManager()
    {
    }

    //---------------------------------------------------------------------------
    DPFManager::DPFManager(const DPFManager& d) : PluginFormat(d)
    {
        bin = d.bin;

        for (size_t i = 0; i < d.isos.size(); ++i)
            isos.push_back(isos[i]);
        for (size_t i = 0; i < d.params.size(); ++i)
            params.push_back(params[i]);
    }

    //---------------------------------------------------------------------------
    int DPFManager::load_plugin(const std::map<std::string, Container::Value>& obj, std::string& error)
    {
        if (obj.find("format") == obj.end() || obj.at("format").type != Container::Value::CONTAINER_TYPE_STRING)
        {
            error += "Field 'format' is not present\n";
            return -1;
        }
        format = obj.at("format").s;

        if (obj.find("bin") == obj.end() || obj.at("bin").type != Container::Value::CONTAINER_TYPE_STRING)
        {
            error += "Field 'bin' is not present\n";
            return -1;
        }
        bin = obj.at("bin").s;

        if (obj.find("params") != obj.end() && obj.at("params").type == Container::Value::CONTAINER_TYPE_ARRAY)
        {
            for (size_t i = 0; i < obj.at("params").array.size(); ++i)
            {
                const Container::Value& val = obj.at("params").array[i];
                if (val.type == Container::Value::CONTAINER_TYPE_STRING)
                    params.push_back(val.s);
            }
        }

        if (obj.find("isos") != obj.end() && obj.at("isos").type == Container::Value::CONTAINER_TYPE_ARRAY)
        {
            for (size_t i = 0; i < obj.at("isos").array.size(); ++i)
            {
                const Container::Value& val = obj.at("isos").array[i];
                if (val.type == Container::Value::CONTAINER_TYPE_STRING)
                    isos.push_back(val.s);
            }
        }

        return 0;
    }

    //---------------------------------------------------------------------------
    int DPFManager::run(std::string& error)
    {
        std::vector<std::string> exec_params;

        exec_params.push_back(bin);

        for (size_t i = 0; i < params.size(); ++i)
            exec_params.push_back(params[i]);

        std::string report_dir;
        if (create_report_dir("DPFTemp/", "DPFReportDir", report_dir) < 0)
            return -1;

        std::string config_file;
        if (create_configuration_file(report_dir, config_file) < 0)
            return -1;

        exec_params.push_back("-configuration");
        exec_params.push_back(config_file);

        std::string file(filename);
#if !defined(_WIN32)
        char *path = realpath(filename.c_str(), NULL);
        if (path)
        {
            file = std::string(path);
            free(path);
        }
#endif //!_WIN32
        exec_params.push_back(file);

        report.clear();
        int ret =  exec_bin(exec_params, error);

        if (ret >= 0)
        {
            report.clear();
            std::string report_file = report_dir + "summary.xml";
            read_report(report_file, report);
        }

        delete_configuration_file(config_file);
        delete_report_dir(report_dir);
        return ret;
    }

    //---------------------------------------------------------------------------
    int DPFManager::create_configuration_file(const std::string& report_dir, std::string& file)
    {
        std::string local_data = Core::get_local_data_path();
        local_data += "DPFTemp/";

        Ztring z_local = ZenLib::Ztring().From_UTF8(local_data);
        if (!ZenLib::Dir::Exists(z_local))
            if (!ZenLib::Dir::Create(z_local))
                return -1;

        std::stringstream path;
        for (size_t i = 0; ; ++i)
        {
            path.str("");
            path << local_data << "DPFConfig";
            if (i)
                path << i;
            path << ".dpf";

            Ztring z_path = ZenLib::Ztring().From_UTF8(path.str());
            if (!ZenLib::File::Exists(z_path))
                break;
        }

        xmlDocPtr doc = xmlNewDoc((const xmlChar *)"1.0");
        xmlNodePtr root_node = xmlNewNode(NULL, (const xmlChar*)"configuration");
        xmlDocSetRootElement(doc, root_node);

        //Version
        xmlNodePtr version_node = xmlNewNode(NULL, (const xmlChar*)"version");
        xmlNodeSetContent(version_node, (const xmlChar*)"1");
        xmlAddChild(root_node, version_node);

        //Isos
        xmlNodePtr isos_node = xmlNewNode(NULL, (const xmlChar*)"isos");
        for (size_t i = 0; i < isos.size(); ++i)
        {
            xmlNodePtr iso_node = xmlNewNode(NULL, (const xmlChar*)"iso");
            xmlNodeSetContent(iso_node, (const xmlChar*)isos[i].c_str());
            xmlAddChild(isos_node, iso_node);
        }
        xmlAddChild(root_node, isos_node);

        //Formats
        xmlNodePtr formats_node = xmlNewNode(NULL, (const xmlChar*)"formats");
        xmlNodePtr format_node = xmlNewNode(NULL, (const xmlChar*)"format");
        xmlNodeSetContent(format_node, (const xmlChar*)"XML");
        xmlAddChild(formats_node, format_node);
        xmlAddChild(root_node, formats_node);

        //Rules
        xmlNodePtr rules_node = xmlNewNode(NULL, (const xmlChar*)"rules");
        xmlAddChild(root_node, rules_node);

        //Fixes
        xmlNodePtr fixes_node = xmlNewNode(NULL, (const xmlChar*)"fixes");
        xmlAddChild(root_node, fixes_node);

        //Output
        xmlNodePtr output_node = xmlNewNode(NULL, (const xmlChar*)"output");
        xmlNodeSetContent(output_node, (const xmlChar*)report_dir.c_str());
        xmlAddChild(root_node, output_node);

        //Description
        xmlNodePtr description_node = xmlNewNode(NULL, (const xmlChar*)"description");
        xmlAddChild(root_node, description_node);

        xmlSaveFormatFileEnc(path.str().c_str(), doc, "UTF-8", 1);
        xmlFreeDoc(doc);
        xmlCleanupParser();

        file = path.str();
        return 0;
    }

    //---------------------------------------------------------------------------
    int DPFManager::delete_configuration_file(const std::string& file)
    {
        Ztring z_path = ZenLib::Ztring().From_UTF8(file);
        if (!ZenLib::File::Exists(z_path))
            return 0;

        ZenLib::File::Delete(z_path);
        return 0;
    }

}
