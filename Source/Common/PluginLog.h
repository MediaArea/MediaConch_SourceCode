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

    enum LogLevel
    {
        LOG_LEVEL_NOTHING = 0,
        LOG_LEVEL_DEBUG,
        LOG_LEVEL_WARNING,
        LOG_LEVEL_ERROR,
    };

    PluginLog() : level(LOG_LEVEL_ERROR) {}
    virtual      ~PluginLog() {}
    PluginLog(const PluginLog& p) : Plugin(p) { level = p.level; }

    virtual void  add_log(const std::string& time, int level, const std::string& log) = 0;
    void          set_level(LogLevel l) { level = (int)l; }

protected:
    int           level;

private:
    PluginLog&    operator=(const PluginLog&);
};

}

#endif // !PLUGINLOGH
