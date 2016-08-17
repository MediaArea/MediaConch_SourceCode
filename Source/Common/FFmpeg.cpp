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
#include "FFmpeg.h"

//---------------------------------------------------------------------------
namespace MediaConch {

    //***************************************************************************
    // Constructor/Destructor
    //***************************************************************************

    //---------------------------------------------------------------------------
    FFmpeg::FFmpeg()
    {
        type = MediaConchLib::PLUGIN_PRE_HOOK;
    }

    //---------------------------------------------------------------------------
    FFmpeg::~FFmpeg()
    {
    }

    //---------------------------------------------------------------------------
    int FFmpeg::load_plugin(const std::map<std::string, Container::Value>& obj, std::string& error)
    {
        if (obj.find("name") == obj.end() || obj.at("name").type != Container::Value::CONTAINER_TYPE_STRING)
        {
            error += "Field 'name' is not present\n";
            return -1;
        }
        name = obj.at("name").s;

        if (obj.find("bin") == obj.end() || obj.at("bin").type != Container::Value::CONTAINER_TYPE_STRING)
        {
            error += "Field 'bin' is not present\n";
            return -1;
        }
        bin = obj.at("bin").s;

        if (obj.find("outputFile") == obj.end() || obj.at("outputFile").type != Container::Value::CONTAINER_TYPE_STRING)
        {
            error += "Field 'outputFile' is not present\n";
            return -1;
        }
        outputFile = obj.at("outputFile").s;

        if (obj.find("inputParams") != obj.end() && obj.at("inputParams").type == Container::Value::CONTAINER_TYPE_ARRAY)
        {
            for (size_t i = 0; i < obj.at("inputParams").array.size(); ++i)
            {
                const Container::Value& val = obj.at("inputParams").array[i];
                if (val.type == Container::Value::CONTAINER_TYPE_STRING)
                    inputParams.push_back(val.s);
            }
        }

        if (obj.find("outputParams") != obj.end() && obj.at("outputParams").type == Container::Value::CONTAINER_TYPE_ARRAY)
        {
            for (size_t i = 0; i < obj.at("outputParams").array.size(); ++i)
            {
                const Container::Value& val = obj.at("outputParams").array[i];
                if (val.type == Container::Value::CONTAINER_TYPE_STRING)
                    outputParams.push_back(val.s);
            }
        }

        return 0;
    }

    //---------------------------------------------------------------------------
    int FFmpeg::run(std::string& error)
    {
        // std::vector<std::string> exec_params;

        // exec_params.push_back(bin);
        // for (size_t i = 0; i < params.size(); ++i)
        //     exec_params.push_back(params[i]);

        // std::string file(filename);
        // //unified_string(file);
        // exec_params.push_back(file);

        // report.clear();
        // return exec_bin(exec_params, error);
        return 0;
    }

}
