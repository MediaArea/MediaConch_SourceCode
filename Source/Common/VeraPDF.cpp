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
    int VeraPDF::load_plugin(const std::map<std::string, Container::Value>& obj, std::string& error)
    {
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

        if (obj.find("params") != obj.end() && obj.at("params").type == Container::Value::CONTAINER_TYPE_STRING)
            params = obj.at("params").s;

        return 0;
    }

    //---------------------------------------------------------------------------
    int VeraPDF::run(std::string& error)
    {
        std::vector<std::string> exec_params;

        exec_params.push_back(bin);
        if (params.length())
            exec_params.push_back(params);

        std::string file(filename);
        unified_string(file);
        exec_params.push_back(file);

        report.clear();
        return exec_bin(exec_params, error);
    }

}
