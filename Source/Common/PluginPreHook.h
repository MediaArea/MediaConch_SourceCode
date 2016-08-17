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
    PluginPreHook() {}
    virtual ~PluginPreHook() {}

protected:

private:
    PluginPreHook(const PluginPreHook&);
    PluginPreHook&         operator=(const PluginPreHook&);
};

}

#endif // !PLUGINPREHOOKH
