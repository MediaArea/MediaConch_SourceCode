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
    }

    //---------------------------------------------------------------------------
    int Scheduler::add_element_to_queue(const std::string& filename)
    {
        static int index = 0;

        queue->add_element(PRIORITY_NONE, index++, filename);
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

    bool Scheduler::element_is_finished(const std::string& filename, double& percent_done)
    {
        bool ret = true;
        CS.Enter();
        if (queue->has_element(filename))
        {
            percent_done = 0.0;
            CS.Leave();
            return false;
        }

        std::map<QueueElement*, QueueElement*>::iterator it = working.begin();
        for (; it != working.end(); ++it)
            if (it->first->filename == filename)
                break;
        if (it != working.end())
        {
            //TODO: find the real percent done
            percent_done = 0.5;
            ret = false;
        }
        CS.Leave();
        return ret;
    }

    bool Scheduler::element_exists(const std::string& filename)
    {
        CS.Enter();
        bool ret = queue->has_element(filename);
        CS.Leave();
        return ret;
    }
}
