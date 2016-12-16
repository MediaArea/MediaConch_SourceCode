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

#include <ZenLib/Ztring.h>
#include <ZenLib/File.h>
#include <ZenLib/Dir.h>
#include <ZenLib/FileName.h>
#include "FFmpeg.h"

//---------------------------------------------------------------------------
namespace MediaConch {

    //***************************************************************************
    // Constructor/Destructor
    //***************************************************************************

    //---------------------------------------------------------------------------
    FFmpeg::FFmpeg() : PluginPreHook()
    {
    }

    //---------------------------------------------------------------------------
    FFmpeg::~FFmpeg()
    {
    }

    //---------------------------------------------------------------------------
    FFmpeg::FFmpeg(const FFmpeg& f) : PluginPreHook(f)
    {
    }

    //---------------------------------------------------------------------------
    int FFmpeg::load_plugin(const std::map<std::string, Container::Value>& obj, std::string& error)
    {
        return PluginPreHook::load_plugin(obj, error);
    }

    //---------------------------------------------------------------------------
    int FFmpeg::run(std::string& error)
    {
        output_file.clear();

        if (!input_file.length())
            return 0;

        if (!outputDir.length())
            outputDir = "./";

        create_output_file_name();

        std::vector<std::string> exec_params;

        exec_params.push_back(bin);
        for (size_t i = 0; i < inputParams.size(); ++i)
            exec_params.push_back(inputParams[i]);

        exec_params.push_back("-i");
        exec_params.push_back(input_file);

        for (size_t i = 0; i < outputParams.size(); ++i)
            exec_params.push_back(outputParams[i]);

        exec_params.push_back(output_file);

        for (size_t i = 0; i < params.size(); ++i)
            exec_params.push_back(params[i]);

        return exec_bin(exec_params, error);
    }

}
