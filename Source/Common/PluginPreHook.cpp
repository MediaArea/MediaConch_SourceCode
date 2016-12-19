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
#include "PluginPreHook.h"

//---------------------------------------------------------------------------
namespace MediaConch {

    //***************************************************************************
    // Constructor/Destructor
    //***************************************************************************

    //---------------------------------------------------------------------------
    PluginPreHook::PluginPreHook() : create_file(false), analyze_source(true)
    {
        type = MediaConchLib::PLUGIN_PRE_HOOK;
    }

    //---------------------------------------------------------------------------
    PluginPreHook::~PluginPreHook()
    {
    }

    //---------------------------------------------------------------------------
    PluginPreHook::PluginPreHook(const PluginPreHook& p) : Plugin(p)
    {
        type = MediaConchLib::PLUGIN_PRE_HOOK;
        input_file = p.input_file;
        output_file = p.output_file;
        create_file = p.create_file;
        analyze_source = p.analyze_source;
        bin = p.bin;
        formatting = p.formatting;
        outputDir = p.outputDir;
        outputExt = p.outputExt;

        for (size_t i = 0; i < p.inputParams.size(); ++i)
            inputParams.push_back(p.inputParams[i]);
        for (size_t i = 0; i < p.outputParams.size(); ++i)
            outputParams.push_back(p.outputParams[i]);
        for (size_t i = 0; i < p.params.size(); ++i)
            params.push_back(p.params[i]);
    }

    //---------------------------------------------------------------------------
    int PluginPreHook::load_plugin(const std::map<std::string, Container::Value>& obj, std::string& error)
    {
        if (obj.find("bin") == obj.end() || obj.at("bin").type != Container::Value::CONTAINER_TYPE_STRING)
        {
            error += "Field 'bin' is not present\n";
            return -1;
        }
        bin = obj.at("bin").s;

        if (obj.find("createFile") == obj.end() || obj.at("createFile").type != Container::Value::CONTAINER_TYPE_BOOL ||
            obj.at("createFile").b == false)
        {
            error += "Field 'createFile' must be set true\n";
            return -1;
        }
        create_file = true;

        if (obj.find("analyzeSource") != obj.end() && obj.at("analyzeSource").type == Container::Value::CONTAINER_TYPE_BOOL)
            analyze_source = obj.at("analyzeSource").b;

        if (obj.find("outputDir") == obj.end() || obj.at("outputDir").type != Container::Value::CONTAINER_TYPE_STRING)
        {
            error += "Field 'outputDir' is not present\n";
            return -1;
        }
        outputDir = obj.at("outputDir").s;
#if defined(_WIN32) || defined(WIN32)
        if (outputDir[outputDir.length() - 1] != '\\')
            outputDir += "\\";
#else
        if (outputDir[outputDir.length() - 1] != '/')
            outputDir += "/";
#endif

        if (!ZenLib::Dir::Exists(ZenLib::Ztring().From_UTF8(outputDir)))
            ZenLib::Dir::Create(ZenLib::Ztring().From_UTF8(outputDir));

        if (obj.find("outputExt") == obj.end() || obj.at("outputExt").type != Container::Value::CONTAINER_TYPE_STRING)
        {
            error += "Field 'outputExt' is not present\n";
            return -1;
        }
        outputExt = obj.at("outputExt").s;

        if (obj.find("formatting") != obj.end() && obj.at("formatting").type == Container::Value::CONTAINER_TYPE_STRING)
            formatting = obj.at("formatting").s;
        else
            formatting = "$BIN $INPUTPARAMS $INPUTFILE $OUTPUTPARAMS $OUTPUTFILE $PARAMS";

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
    int PluginPreHook::run(std::string& error)
    {
        output_file.clear();

        if (!input_file.length())
            return 0;

        if (!outputDir.length())
            outputDir = "./";

        create_output_file_name();

        std::vector<std::string> exec_params;

        size_t pos = 0;
        size_t start = 0;
        while (pos != std::string::npos && pos < formatting.size())
        {
            start = pos;
            while (formatting[start] == ' ')
                ++start;

            pos = formatting.find(" ", start);
            std::string var;
            if (pos != std::string::npos)
            {
                var = formatting.substr(start, pos - start);
                pos += 1;
            }
            else
                var = formatting.substr(start);

            if (!var.size())
                continue;

            else if (var == "$BIN")
                exec_params.push_back(bin);

            else if (var == "$INPUTPARAMS")
            {
                for (size_t i = 0; i < inputParams.size(); ++i)
                    exec_params.push_back(inputParams[i]);
            }

            else if (var == "$INPUTFILE")
            {
                exec_params.push_back(input_file);
            }

            else if (var == "$OUTPUTPARAMS")
            {
                for (size_t i = 0; i < outputParams.size(); ++i)
                    exec_params.push_back(outputParams[i]);
            }

            else if (var == "$OUTPUTFILE")
            {
                exec_params.push_back(output_file);
            }

            else if (var == "$PARAMS")
            {
                for (size_t i = 0; i < params.size(); ++i)
                    exec_params.push_back(params[i]);
            }

            else
                exec_params.push_back(var);
        }

        return exec_bin(exec_params, error);
    }

    //---------------------------------------------------------------------------
    void PluginPreHook::create_output_file_name()
    {
        ZenLib::Ztring z_in = ZenLib::Ztring().From_UTF8(input_file);
        ZenLib::FileName file(z_in);
        std::string out = file.Name_Get().To_UTF8();

        for (size_t i = 0; ; ++i)
        {
            std::stringstream path;

            path << outputDir << out;
            if (i)
                path << i;
            path << "." << outputExt;

            output_file = path.str();
            if (!ZenLib::File::Exists(ZenLib::Ztring().From_UTF8(output_file)))
                break;
        }
    }

}
