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
#ifndef PLUGINS_MANAGERH
#define PLUGINS_MANAGERH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <vector>
#include <map>
#include "Container.h"

//---------------------------------------------------------------------------
namespace MediaConch {

class Plugin;
class Core;

//***************************************************************************
// Class Scheduler
//***************************************************************************

class PluginsManager
{
public:
    PluginsManager(Core *c);
    ~PluginsManager();

    const std::vector<Plugin*>& get_plugins() const;
    const std::map<std::string, Plugin*>& get_format_plugins() const { return format_plugins; }
    int   load_plugin(const std::map<std::string, Container::Value>& obj, std::string& error);

private:
    PluginsManager(const PluginsManager&);
    PluginsManager&     operator=(const PluginsManager&);

    Core                           *core;
    std::vector<Plugin*>            plugins;
    std::map<std::string, Plugin*>  format_plugins;
};

}

#endif // !PLUGIN_MANAGERH
