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
#include "FFmpeg.h"
#include <ZenLib/Ztring.h>

#if defined(_WIN32) || defined(WIN32)
#include <Winbase.h>
#else
#include <sys/time.h>
#endif

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
    int Scheduler::add_element_to_queue(const std::string& filename, long file_id,
                                        const std::vector<std::string>& options, bool do_pre_hook)
    {
        static int index = 0;

        queue->add_element(PRIORITY_NONE, index++, filename, file_id, options, do_pre_hook);
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

    void Scheduler::work_finished(QueueElement *el, MediaInfoNameSpace::MediaInfo* MI)
    {
        if (!el)
            return;

        core->set_file_analyzed_to_database(el->file_id);

        if (!MI)
        {
            CS.Enter();
            remove_element(el);
            CS.Leave();
            run_element();
            return;
        }

        if (another_work_to_do(el, MI) <= 0)
            return;

        CS.Enter();
        core->register_reports_to_database(el->file_id, MI);
        remove_element(el);
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

    bool Scheduler::element_is_finished(long file_id, double& percent_done)
    {
        bool ret = true;
        CS.Enter();

        std::map<QueueElement*, QueueElement*>::iterator it = working.begin();
        for (; it != working.end(); ++it)
            if (it->first->file_id == file_id)
                break;

        if (it != working.end())
        {
            percent_done = it->first->percent_done();
            CS.Leave();
            return false;
        }

        if (queue->has_id(file_id) >= 0)
        {
            percent_done = 0.0;
            ret = false;
        }

        CS.Leave();
        return ret;
    }

    long Scheduler::element_exists(const std::string& filename)
    {
        CS.Enter();
        long file_id = queue->has_element(filename);
        if (file_id >= 0)
        {
            CS.Leave();
            return file_id;
        }

        std::map<QueueElement*, QueueElement*>::iterator it = working.begin();
        for (; it != working.end(); ++it)
            if (it->first->filename == filename)
                break;

        if (it != working.end())
            file_id = it->first->file_id;
        CS.Leave();
        return file_id;
    }

    int Scheduler::another_work_to_do(QueueElement *el, MediaInfoNameSpace::MediaInfo* MI)
    {
        // Before registering, check the format
        String format = MI->Get(Stream_General, 0, __T("Format"));
        std::string format_str = ZenLib::Ztring(format).To_UTF8().c_str();

        std::map<std::string, Plugin*> plugins = core->get_format_plugins();
        if (plugins.find(format_str) == plugins.end() || !plugins[format_str])
            return 1;

        std::string error;
        ((PluginFormat*)plugins[format_str])->set_file(el->filename);
        plugins[format_str]->run(error);
        const std::string& report = plugins[format_str]->get_report();
        MediaConchLib::report report_kind = ((PluginFormat*)plugins[format_str])->get_report_kind();

        CS.Enter();
        core->register_reports_to_database(el->file_id, report, report_kind, MI);
        remove_element(el);
        CS.Leave();
        run_element();

        return 0;
    }

    void Scheduler::remove_element(QueueElement *el)
    {
        std::map<QueueElement*, QueueElement*>::iterator it = working.find(el);
        if (it != working.end())
            working.erase(it);
    }

    int Scheduler::execute_pre_hook_plugins(QueueElement *el, std::string& err)
    {
        // Before registering, check the format
        std::vector<Plugin*> plugins = core->get_pre_hook_plugins();
        bool                 analyze_file = true;
        std::string          new_file = el->filename;
        long                 old_id = el->file_id;

        for (size_t i = 0; i < plugins.size(); ++i)
        {
            if (!plugins[i])
                continue;

            ((PluginPreHook*)plugins[i])->set_input_file(new_file);

#if defined(_WIN32) || defined(WIN32)
            unsigned long time_before = GetTickCount();
#else
            struct timeval time_before;
            gettimeofday(&time_before, NULL);
#endif

            if (plugins[i]->run(err) < 0)
                return -1;

#if defined(_WIN32) || defined(WIN32)
            unsigned long time_after = GetTickCount();
            size_t time_passed = time_after - time_before;
#else
            struct timeval time_after;
            gettimeofday(&time_after, NULL);

            size_t time_passed = (time_after.tv_sec - time_before.tv_sec) * 1000 + (time_after.tv_usec - time_before.tv_usec) / 1000;
#endif

            if (((PluginPreHook*)plugins[i])->is_creating_file())
            {
                std::string generated_log = ((PluginPreHook*)plugins[i])->get_report();
                new_file = ((PluginPreHook*)plugins[i])->get_output_file();

                std::vector<std::string> options;
                std::map<std::string, std::string>::iterator it = el->options.begin();
                for (; it != el->options.end(); ++it)
                {
                    if (it->second.length())
                        options.push_back(it->first + "=" + it->second);
                    else
                        options.push_back(it->first);
                }
                //TODO: Add it to analyzer with source id and add generated id
                long id = core->checker_analyze(new_file, old_id, time_passed, generated_log, generated_log, options, false);
                if (id >= 0)
                    core->file_update_generated_file(old_id, id);
                old_id = id;
            }

            if (!((PluginPreHook*)plugins[i])->analyzing_source())
                analyze_file = false;
        }

        if (!analyze_file)
        {
            remove_element(el);
            return 1;
        }

        return analyze_file ? 0 : 1;
    }

}
