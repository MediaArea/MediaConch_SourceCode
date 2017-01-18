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
    // Const global
    //***************************************************************************
    //---------------------------------------------------------------------------
    const std::string PluginPreHook::output_params_str = "$OUTPUTPARAMS";
    const std::string PluginPreHook::output_file_str = "$OUTPUTFILE";

    //***************************************************************************
    // Constructor/Destructor
    //***************************************************************************
    //---------------------------------------------------------------------------
    PluginPreHook::PluginPreHook()
    {
        type = MediaConchLib::PLUGIN_PRE_HOOK;
    }

    //---------------------------------------------------------------------------
    PluginPreHook::~PluginPreHook()
    {
        for (size_t i = 0; i < outputs.size(); ++i)
        {
            delete outputs[i];
            outputs[i] = NULL;
        }

        outputs.clear();
    }

    //---------------------------------------------------------------------------
    PluginPreHook::PluginPreHook(const PluginPreHook& p) : Plugin(p)
    {
        type = MediaConchLib::PLUGIN_PRE_HOOK;
        input_file = p.input_file;
        bin = p.bin;
        formatting = p.formatting;

        for (size_t i = 0; i < p.inputParams.size(); ++i)
            inputParams.push_back(p.inputParams[i]);
        for (size_t i = 0; i < p.params.size(); ++i)
            params.push_back(p.params[i]);

        for (size_t i = 0; i < p.outputs.size(); ++i)
            if (p.outputs[i])
                outputs.push_back(new Output(p.outputs[i]));
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

        if (obj.find("params") != obj.end() && obj.at("params").type == Container::Value::CONTAINER_TYPE_ARRAY)
        {
            for (size_t i = 0; i < obj.at("params").array.size(); ++i)
            {
                const Container::Value& val = obj.at("params").array[i];
                if (val.type == Container::Value::CONTAINER_TYPE_STRING)
                    params.push_back(val.s);
            }
        }

        if (obj.find("outputs") != obj.end() && obj.at("outputs").type == Container::Value::CONTAINER_TYPE_ARRAY)
        {
            for (size_t i = 0; i < obj.at("outputs").array.size(); ++i)
            {
                if (obj.at("outputs").array[i].type != Container::Value::CONTAINER_TYPE_OBJECT)
                    continue;

                const std::map<std::string, Container::Value>& val = obj.at("outputs").array[i].obj;
                Output *out = new Output;

                if (obj.find("analyze") != obj.end() && obj.at("analyze").type == Container::Value::CONTAINER_TYPE_BOOL)
                    out->analyze = obj.at("analyze").b;

                if (val.find("createFile") != val.end() && val.at("createFile").type == Container::Value::CONTAINER_TYPE_BOOL)
                    out->create_file = val.at("createFile").b;

                if (val.find("name") != val.end() && val.at("name").type == Container::Value::CONTAINER_TYPE_STRING)
                    out->name = val.at("name").s;

                if (val.find("outputDir") != val.end() && val.at("outputDir").type == Container::Value::CONTAINER_TYPE_STRING)
                {
                    out->outputDir = val.at("outputDir").s;
#if defined(_WIN32) || defined(WIN32)
                    if (out->outputDir.length() && out->outputDir[out->outputDir.length() - 1] != '\\')
                        out->outputDir += "\\";
#else
                    if (out->outputDir.length() && out->outputDir[out->outputDir.length() - 1] != '/')
                        out->outputDir+=  "/";
#endif
                    if (out->outputDir.size() && !ZenLib::Dir::Exists(ZenLib::Ztring().From_UTF8(out->outputDir)))
                        ZenLib::Dir::Create(ZenLib::Ztring().From_UTF8(out->outputDir));
                }

                if (val.find("outputExt") != val.end() && val.at("outputExt").type == Container::Value::CONTAINER_TYPE_STRING)
                    out->outputExt = val.at("outputExt").s;

                if (val.find("outputParams") != val.end() && val.at("outputParams").type == Container::Value::CONTAINER_TYPE_ARRAY)
                {
                    for (size_t j = 0; j < val.at("outputParams").array.size(); ++j)
                    {
                        const Container::Value& val2 = val.at("outputParams").array[j];
                        if (val2.type == Container::Value::CONTAINER_TYPE_STRING)
                            out->outputParams.push_back(val2.s);
                    }
                }

                outputs.push_back(out);
            }
        }

        return 0;
    }

    //---------------------------------------------------------------------------
    bool PluginPreHook::is_creating_files()
    {
        for (size_t i = 0; i < outputs.size(); ++i)
            if (outputs[i] && outputs[i]->create_file)
                return true;

        return false;
    }

    //---------------------------------------------------------------------------
    bool PluginPreHook::need_analyze()
    {
        for (size_t i = 0; i < outputs.size(); ++i)
            if (outputs[i] && outputs[i]->analyze)
                return true;

        return false;
    }

    //---------------------------------------------------------------------------
    void PluginPreHook::get_outputs(std::vector<Output*>& files)
    {
        for (size_t i = 0; i < outputs.size(); ++i)
            files.push_back(outputs[i]);
    }

    //---------------------------------------------------------------------------
    int PluginPreHook::run(std::string& error)
    {
        if (!input_file.length())
            return 0;

        for (size_t i = 0; i < outputs.size(); ++i)
        {
            if (!outputs[i])
                continue;

            outputs[i]->output_file.clear();
            if (!outputs[i]->outputDir.length())
                outputs[i]->outputDir = "./";

            create_output_file_name(outputs[i]);
        }

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

            else if (var == "$PARAMS")
            {
                for (size_t i = 0; i < params.size(); ++i)
                    exec_params.push_back(params[i]);
            }

            else if (var.find(output_params_str) == 0)
            {
                Output *o = get_output_from_name(var, output_params_str.size(), error);
                if (!o)
                    return -1;

                for (size_t i = 0; i < o->outputParams.size(); ++i)
                    exec_params.push_back(o->outputParams[i]);
            }

            else if (var.find(output_file_str) == 0)
            {
                Output* o = get_output_from_name(var, output_file_str.size(), error);
                if (!o)
                    return -1;

                exec_params.push_back(o->output_file);
            }

            else
                exec_params.push_back(var);
        }

        return exec_bin(exec_params, error);
    }

    //---------------------------------------------------------------------------
    PluginPreHook::Output* PluginPreHook::get_output_from_name(const std::string& str, size_t start, std::string& err)
    {
        // If no colon, guessing only one output is there, otherwise return NULL
        if (str.size() == start)
        {
            if (outputs.size() == 1)
                return outputs[0];

            err = "if no colon following $OUTPUTPARAMS or $OUTPUTFILE, only one output can be there.";
            return NULL;
        }

        // if no colon, return NULL
        if (str[start] != ':')
        {
            err = "$OUTPUTPARAMS and $OUTPUTFILE should be followed by colon.";
            return NULL;
        }

        start += 1;
        std::string name = str.substr(start);
        for (size_t i = 0; i < outputs.size(); ++i)
        {
            if (!outputs[i])
                continue;

            if (outputs[i]->name == name)
                return outputs[i];
        }

        err = "Cannot find the name following $OUTPUTPARAMS or $OUTPUTFILE in the outputs configured.";
        return NULL;
    }

    //---------------------------------------------------------------------------
    void PluginPreHook::create_output_file_name(Output* o)
    {
        ZenLib::Ztring z_in = ZenLib::Ztring().From_UTF8(input_file);
        ZenLib::FileName file(z_in);
        std::string out = file.Name_Get().To_UTF8();

        for (size_t i = 0; ; ++i)
        {
            std::stringstream path;

            path << o->outputDir << out;
            if (i)
                path << i;
            path << "." << o->outputExt;

            o->output_file = path.str();
            if (!ZenLib::File::Exists(ZenLib::Ztring().From_UTF8(o->output_file)))
                break;
        }
    }

}
