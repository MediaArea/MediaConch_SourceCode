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
#include "VeraPDF.h"
#include <stdio.h>
#include <stdlib.h>

//---------------------------------------------------------------------------
namespace MediaConch {

    //***************************************************************************
    // Constructor/Destructor
    //***************************************************************************

    //---------------------------------------------------------------------------
    VeraPDF::VeraPDF()
    {
        type = MediaConchLib::PLUGIN_FORMAT;
        report_kind = MediaConchLib::report_MediaVeraPdf;
    }

    //---------------------------------------------------------------------------
    VeraPDF::~VeraPDF()
    {
    }

    //---------------------------------------------------------------------------
    VeraPDF::VeraPDF(const VeraPDF& v): PluginFormat(v)
    {
        bin = v.bin;

        for (size_t i = 0; i < v.params.size(); ++i)
            params.push_back(v.params[i]);
    }

    //---------------------------------------------------------------------------
    int VeraPDF::load_plugin(const std::map<std::string, Container::Value>& obj, std::string& error)
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

        return 0;
    }

    //---------------------------------------------------------------------------
    int VeraPDF::run(std::string& error)
    {
        std::vector<std::string> exec_params;

        exec_params.push_back(bin);
        for (size_t i = 0; i < params.size(); ++i)
            exec_params.push_back(params[i]);

        // std::string report_dir;
        // if (create_report_dir("VeraTemp", "VeraReportDir", report_dir) < 0)
        //     return -1;

        // std::string report_file = report_dir + "summary.xml";
        // exec_params.push_back("--reportfile");
        // exec_params.push_back(report_file);

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
        int ret = exec_bin(exec_params, error);

        // if (ret >= 0)
        // {
        //     report.clear();
        //     read_report(report_file, report);
        // }

        return ret;
    }

}
