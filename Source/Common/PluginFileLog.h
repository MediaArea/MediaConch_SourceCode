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
#ifndef PLUGINFILELOGH
#define PLUGINFILELOGH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <fstream>
#include <ZenLib/CriticalSection.h>
#include "PluginLog.h"

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Class Plugin
//***************************************************************************

class PluginFileLog : public PluginLog
{
public:
    PluginFileLog();
    virtual                 ~PluginFileLog();

    virtual int              load_plugin(const std::map<std::string, Container::Value>& obj, std::string& error);
    virtual int              run(std::string& error);
    virtual void             add_log(const std::string& time, int level, const std::string& log);
    void                     set_file(const std::string& file);
    const std::string&       get_file() const { return filename; }

private:
    std::string              filename;
    std::ofstream            file_handle;
    ZenLib::CriticalSection  file_mutex;

    void                     close();
    bool                     open();
    bool                     is_open();

    PluginFileLog(const PluginFileLog&);
    PluginFileLog&           operator=(const PluginFileLog&);
};

}

#endif // !PLUGINFILELOGH
