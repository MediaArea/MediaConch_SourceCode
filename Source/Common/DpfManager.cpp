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
    int DPFManager::load_plugin(const std::map<std::string, Container::Value>& obj, std::string& error)
    {
        if (obj.find("id") == obj.end() || obj.at("id").type != Container::Value::CONTAINER_TYPE_STRING)
        {
            error += "Field 'id' is not present\n";
            return -1;
        }
        id = obj.at("id").s;

        if (obj.find("name") == obj.end() || obj.at("name").type != Container::Value::CONTAINER_TYPE_STRING)
        {
            error += "Field 'name' is not present\n";
            return -1;
        }
        name = obj.at("name").s;

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
        if (create_report_dir(report_dir) < 0)
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
    int DPFManager::create_report_dir(std::string& report_dir)
    {
        std::string local_data = Core::get_local_data_path();
        local_data += "DPFTemp/";

        Ztring z_local = ZenLib::Ztring().From_UTF8(local_data);
        if (!ZenLib::Dir::Exists(z_local))
            ZenLib::Dir::Create(z_local);

        if (!ZenLib::Dir::Exists(z_local))
            return -1;

        std::stringstream path;
        for (size_t i = 0; ; ++i)
        {
            path.str("");
            path << local_data << "DPFReportDir";
            if (i)
                path << i;
            path << "/";

            Ztring z_path = ZenLib::Ztring().From_UTF8(path.str());
            if (!ZenLib::Dir::Exists(z_path))
                break;
        }

        Ztring z_path = ZenLib::Ztring().From_UTF8(path.str());
        ZenLib::Dir::Create(z_path);

        if (!ZenLib::Dir::Exists(z_path))
            return -1;

        report_dir = path.str();
        return 0;
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

        std::ofstream ofs;
        ofs.open(path.str().c_str(), std::ofstream::out | std::ofstream::trunc);
        if (!ofs.is_open())
            return -1;

        ofs << "ISO\tBaseline\n";
        ofs << "ISO\tTiff/EP\n";
        ofs << "ISO\tTiff/IT\n";
        ofs << "ISO\tTiff/IT-1\n";
        ofs << "ISO\tTiff/IT-2\n";
        ofs << "FORMAT\tXML\n";
        ofs << "OUTPUT\t" << report_dir << "\n";

        ofs.close();

        file = path.str();
        return 0;
    }

    //---------------------------------------------------------------------------
    int DPFManager::delete_report_dir(const std::string& report_dir)
    {
        Ztring z_path = ZenLib::Ztring().From_UTF8(report_dir);
        if (!ZenLib::Dir::Exists(z_path))
            return 0;

        //ZenLib::Dir::Delete(z_path);
#if defined(WINDOWS)
        WIN32_FIND_DATA find_file_data;
        HANDLE handler = FindFirstFile(z_path.c_str(), &find_file_data);

        if (handler == INVALID_HANDLE_VALUE)
            return -1;

        do
        {
            if (ZenLib::Ztring(__T(".")) == find_file_data.cFileName || ZenLib::Ztring(__T("..")) == find_file_data.cFileName)
                continue;

            ZenLib::Ztring full_path(z_path);
            full_path += find_file_data.cFileName;
            if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                delete_report_dir(full_path.To_UTF8());
            else
                DeleteFile(full_path.c_str());
        } while (FindNextFile(handler, &find_file_data));
        FindClose(handler);

        if (!RemoveDirectory(z_path.c_str()))
            return -1;
#else
        DIR* handler = opendir(report_dir.c_str());
        if (!handler)
            return -1;

        struct dirent* entry = NULL;
        while ((entry = readdir(handler)))
        {
            std::string full_path(entry->d_name);
            if (full_path == "." || full_path == "..")
                continue;

            full_path = report_dir + full_path;
            if (entry->d_type & DT_DIR)
                delete_report_dir(full_path.c_str());
            else
                unlink(full_path.c_str());
        }
        closedir(handler);

        if (rmdir(report_dir.c_str()) < 0)
            return -1;
#endif
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
