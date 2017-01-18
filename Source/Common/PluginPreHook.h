/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Scheduler functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef PLUGINPREHOOKH
#define PLUGINPREHOOKH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "Plugin.h"

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Class Plugin
//***************************************************************************

class PluginPreHook : public Plugin
{
public:
    PluginPreHook();
    virtual      ~PluginPreHook();
    PluginPreHook(const PluginPreHook& p);

    struct Output
    {
        Output() : create_file(true), analyze(true) {}
        Output(const Output* o)
        {
            if (!o || o == this)
                return;

            name = o->name;
            output_file = o->output_file;
            outputDir = o->outputDir;
            outputExt = o->outputExt;
            for (size_t i = 0; i < o->outputParams.size(); ++i)
                outputParams.push_back(o->outputParams[i]);

            create_file = o->create_file;
            analyze = o->analyze;
        }

        // Set at run-time
        std::string               output_file;

        // Set by the configuration
        std::string               name;
        std::string               outputDir;
        std::string               outputExt;
        std::vector<std::string>  outputParams;
        bool                      create_file;
        bool                      analyze;
    };

    virtual int   load_plugin(const std::map<std::string, Container::Value>& obj, std::string& error);
    virtual int   run(std::string& error);

    void          set_input_file(const std::string& file) { input_file = file; }

    void          get_outputs(std::vector<Output*>& files);

    bool          is_creating_files();
    bool          need_analyze();

protected:
    // Set at run-time
    std::string               input_file;

    // Set by the configuration
    std::string               bin;
    std::string               formatting;
    std::vector<std::string>  inputParams;
    std::vector<std::string>  params;
    std::vector<Output*>      outputs;

    void    create_output_file_name(Output* o);
    Output *get_output_from_name(const std::string& str, size_t start, std::string& err);

    static const std::string output_params_str;
    static const std::string output_file_str;

private:
    PluginPreHook &operator=(const PluginPreHook&);
};

}

#endif // !PLUGINPREHOOKH
