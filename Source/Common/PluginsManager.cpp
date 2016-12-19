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
#include "PluginPreHook.h"
#include "PluginFileLog.h"
#if defined(WIN32)
#include <Windows.h>
#else
#include <sys/time.h>
#endif
#include <sstream>

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
        else if (obj.at("name").s == "PreHook")
            p = new PluginPreHook;
        else if (obj.at("name").s == "FileLog")
            p = new PluginFileLog;
        else
        {
            error += std::string("The plugin ") + obj.at("name").s + " is not managed yet.\n";
            return -1;
        }

        //Common plugin field
        if (obj.find("id") == obj.end() || obj.at("id").type != Container::Value::CONTAINER_TYPE_STRING)
        {
            error += "Field 'id' is not present\n";
            return -1;
        }
        p->set_id(obj.at("id").s);

        if (obj.find("name") == obj.end() || obj.at("name").type != Container::Value::CONTAINER_TYPE_STRING)
        {
            error += "Field 'name' is not present\n";
            return -1;
        }
        p->set_name(obj.at("name").s);

        if (p->load_plugin(obj, error) < 0)
            return -1;

        plugins.push_back(p);

        if (p->get_type() == MediaConchLib::PLUGIN_FORMAT)
            format_plugins[((PluginFormat*)p)->get_format()] = p;
        else if (p->get_type() == MediaConchLib::PLUGIN_PRE_HOOK)
            pre_hook_plugins.push_back(p);
        else if (p->get_type() == MediaConchLib::PLUGIN_LOG)
            log_plugins.push_back(p);
        return 0;
    }

    //---------------------------------------------------------------------------
    int PluginsManager::write_log(int level, const std::string& log)
    {
        int ret = 0;
        std::string time = Core::get_date();

        for (size_t i = 0; i < log_plugins.size(); ++i)
        {
            if (!log_plugins[i])
                continue;

            ((PluginLog*)log_plugins[i])->add_log(time, level, log);
        }

        return ret;
    }

    //---------------------------------------------------------------------------
    int PluginsManager::write_log_timestamp(int level, const std::string& log)
    {
        int ret = 0;
        std::stringstream t;
#if defined(WIN32)
        t << GetTickCount64();
#else
        struct timeval tv;
        gettimeofday(&tv, NULL);
        suseconds_t micro = tv.tv_usec / 1000;
        t << tv.tv_sec;
        if (micro < 10)
            t << "00";
        else if (micro < 100)
            t << "0";
        t << micro;
#endif

        for (size_t i = 0; i < log_plugins.size(); ++i)
        {
            if (!log_plugins[i])
                continue;

            ((PluginLog*)log_plugins[i])->add_log(t.str(), level, log);
        }

        return ret;
    }

}
