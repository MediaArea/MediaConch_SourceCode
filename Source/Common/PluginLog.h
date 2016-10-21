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
#ifndef PLUGINLOGH
#define PLUGINLOGH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "Plugin.h"

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Class PluginLog
//***************************************************************************

class PluginLog : public Plugin
{
public:
    PluginLog() {}
    virtual      ~PluginLog() {}

    virtual void  add_log(const std::string& time, const std::string& log) = 0;

private:
    PluginLog(const PluginLog&);
    PluginLog&    operator=(const PluginLog&);
};

}

#endif // !PLUGINLOGH
