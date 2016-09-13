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
#include "Core.h"
#include "PluginsManager.h"
#include "Plugin.h"
#include "VeraPDF.h"
#include "DpfManager.h"

//---------------------------------------------------------------------------
namespace MediaConch {

    //***************************************************************************
    // Constructor/Destructor
    //***************************************************************************

    //---------------------------------------------------------------------------
    PluginsManager::PluginsManager(Core* c) : core(c)
    {
    }

    //---------------------------------------------------------------------------
    PluginsManager::~PluginsManager()
    {
        format_plugins.clear();
        for (size_t i = 0; i < plugins.size(); ++i)
            delete plugins[i];

        plugins.clear();
    }

    //---------------------------------------------------------------------------
    const std::vector<Plugin*>& PluginsManager::get_plugins() const
    {
        return plugins;
    }


    //---------------------------------------------------------------------------
    int PluginsManager::load_plugin(const std::map<std::string, Container::Value>& obj, std::string& error)
    {
        Plugin *p = NULL;

        if (obj.at("name").s == "VeraPDF")
            p = new VeraPDF;
        else if (obj.at("name").s == "DPFManager")
            p = new DPFManager;
        else
        {
            error += std::string("The plugin ") + obj.at("name").s + " is not managed yet.\n";
            return -1;
        }

        p->load_plugin(obj, error);
        plugins.push_back(p);

        if (p->get_type() == MediaConchLib::PLUGIN_FORMAT)
            format_plugins[((PluginFormat*)p)->get_format()] = p;
        return 0;
    }

}
