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
    PluginPreHook() : create_file(false), analyze_source(true) {}
    virtual      ~PluginPreHook() {}
    PluginPreHook(const PluginPreHook& p) : Plugin(p)
    {
        input_file = p.input_file;
        output_file = p.output_file;
        create_file = p.create_file;
        analyze_source = p.analyze_source;
    }

    void          set_input_file(const std::string& file) { input_file = file; }

    std::string   get_output_file() const { return output_file; }

    bool          is_creating_file() { return create_file; }
    bool          analyzing_source() { return analyze_source; }

protected:
    std::string   input_file;
    std::string   output_file;
    bool          create_file;
    bool          analyze_source;

private:
    PluginPreHook &operator=(const PluginPreHook&);
};

}

#endif // !PLUGINPREHOOKH
