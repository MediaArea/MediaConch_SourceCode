/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Scheduler functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef PLUGINSCONFIGH
#define PLUGINSCONFIGH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "Container.h"

//---------------------------------------------------------------------------
namespace MediaConch {

class PluginsManager;

//***************************************************************************
// Class Scheduler
//***************************************************************************

class PluginsConfig
{
public:
    PluginsConfig(PluginsManager* pm);
    ~PluginsConfig();

    int load_file(const std::string& filename, std::string& error);
    int parse_struct(const std::vector<Container::Value>& conf, std::string& error);

private:
    PluginsConfig(const PluginsConfig&);
    PluginsConfig&     operator=(const PluginsConfig&);

    PluginsManager* manager;
};

}

#endif // !PLUGINSCONFIGH
