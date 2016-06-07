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
#include "Scheduler.h"
#include "Queue.h"
#include "Plugin.h"
#include "PluginFormat.h"
#include "PluginsManager.h"
#include <ZenLib/Ztring.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

    //***************************************************************************
    // Constructor/Destructor
    //***************************************************************************

    //---------------------------------------------------------------------------
    Scheduler::Scheduler(Core* c) : core(c), max_threads(1)
    {
        queue = new Queue(this);
    }

    //---------------------------------------------------------------------------
    Scheduler::~Scheduler()
    {
        queue->clear();
        CS.Enter();
        std::map<QueueElement*, QueueElement*>::iterator it = working.begin();
        for (; it != working.end(); ++it)
        {
            if (it->first)
            {
                it->first->stop();
                delete it->first;
            }
        }
        working.clear();
        CS.Leave();
        delete queue;
    }

    //---------------------------------------------------------------------------
    int Scheduler::add_element_to_queue(const std::string& filename, const std::vector<std::string>& options)
    {
        static int index = 0;

        queue->add_element(PRIORITY_NONE, index++, filename, options);
        run_element();
        return index - 1;
    }

    //---------------------------------------------------------------------------
    void Scheduler::run_element()
    {
        CS.Enter();
        if (working.size() >= max_threads)
        {
            CS.Leave();
            return;
        }

        QueueElement *ret = queue->run_next();
        if (!ret)
        {
            CS.Leave();
            return;
        }

        working[ret] = ret;
        CS.Leave();
    }

    void Scheduler::work_finished(QueueElement *el, MediaInfoNameSpace::MediaInfoList* MI)
    {
        if (!el)
            return;

        if (another_work_to_do(el, MI) <= 0)
            return;

        CS.Enter();
        core->register_file_to_database(el->filename, MI);
        std::map<QueueElement*, QueueElement*>::iterator it = working.find(el);
        if (it != working.end())
            working.erase(it);
        CS.Leave();

        run_element();
    }

    bool Scheduler::is_finished()
    {
        CS.Enter();
        size_t size = working.size();
        CS.Leave();
        return size == 0;
    }

    void Scheduler::get_elements(std::vector<std::string>& vec)
    {
        CS.Enter();
        std::map<QueueElement*, QueueElement*>::iterator it = working.begin();
        for (; it != working.end(); ++it)
            if (it->first)
                vec.push_back(it->first->filename);
        CS.Leave();
    }

    bool Scheduler::element_is_finished(const std::string& filename, double& percent_done)
    {
        bool ret = true;
        CS.Enter();

        std::map<QueueElement*, QueueElement*>::iterator it = working.begin();
        for (; it != working.end(); ++it)
            if (it->first->filename == filename)
                break;
        if (it != working.end())
        {
            percent_done = it->first->percent_done();
            CS.Leave();
            return false;
        }
        if (queue->has_element(filename))
        {
            percent_done = 0.0;
            ret = false;
        }

        CS.Leave();
        return ret;
    }

    bool Scheduler::element_exists(const std::string& filename)
    {
        CS.Enter();
        bool ret = queue->has_element(filename);

        std::map<QueueElement*, QueueElement*>::iterator it = working.begin();
        for (; it != working.end(); ++it)
            if (it->first->filename == filename)
                break;
        if (it != working.end())
            ret = true;
        CS.Leave();
        return ret;
    }

    int Scheduler::another_work_to_do(QueueElement *el, MediaInfoNameSpace::MediaInfoList* MI)
    {
        // Before registering, check the format
        String format = MI->Get(0, Stream_General, 0, __T("Format"));
        std::string format_str = ZenLib::Ztring(format).To_UTF8().c_str();

        std::map<std::string, Plugin*> plugins = core->get_format_plugins();
        if (plugins.find(format_str) != plugins.end() && plugins[format_str])
        {
            std::string error;
            ((PluginFormat*)plugins[format_str])->set_file(el->filename);
            plugins[format_str]->run(error);
            const std::string& report = plugins[format_str]->get_report();
            MediaConchLib::report report_kind = ((PluginFormat*)plugins[format_str])->get_report_kind();
            core->register_file_to_database(el->filename, report, report_kind, MI);
        }
        else
            return 1;

        CS.Enter();
        //core->register_plugin_report_to_database(el->filename, MI);
        std::map<QueueElement*, QueueElement*>::iterator it = working.find(el);
        if (it != working.end())
            working.erase(it);
        CS.Leave();

        run_element();
        return 0;
    }

}
