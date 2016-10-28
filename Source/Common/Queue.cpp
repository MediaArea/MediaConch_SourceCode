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
#include "ZenLib/Ztring.h"
#include "Queue.h"
#include "Scheduler.h"

#if !defined(WINDOWS)
#include <unistd.h>
#endif //!defined(WINDOWS)

#include <algorithm>

//---------------------------------------------------------------------------
namespace MediaConch {

//---------------------------------------------------------------------------
QueueElement::QueueElement(Scheduler *s) : Thread(), scheduler(s), MI(NULL)
{
}

//---------------------------------------------------------------------------
QueueElement::~QueueElement()
{
}

//---------------------------------------------------------------------------
void QueueElement::stop()
{
    RequestTerminate();
    while (!IsExited())
    {
#ifdef WINDOWS
        Sleep(0);
#else //WINDOWS
        sleep(0);
#endif //WINDOWS
    }
}

//---------------------------------------------------------------------------
void QueueElement::Entry()
{
    std::string file = filename;
    std::string err;

    //Pre hook plugins
    int ret = 0;
    bool analyze_file = true;
    ret = scheduler->execute_pre_hook_plugins(this, err, analyze_file);

    if (!analyze_file || ret)
    {
        scheduler->work_finished(this, NULL);
        return;
    }

    MI = new MediaInfoNameSpace::MediaInfo;
    // Currently avoiding to have a big trace
    if (options.find("parsespeed") == options.end())
        MI->Option(__T("ParseSpeed"), __T("0"));

    // Configuration of the parsing
    if (options.find("details") == options.end())
        MI->Option(__T("Details"), __T("1"));

    // Partial configuration of the output (note: this options should be removed after libmediainfo has a support of these options after Open() )
    MI->Option(__T("ReadByHuman"), __T("1"));
    MI->Option(__T("Language"), __T("raw"));
    MI->Option(__T("Inform"), __T("MICRO_XML"));

    std::map<std::string, std::string>::iterator it = options.begin();
    for (; it != options.end(); ++it)
        MI->Option(Ztring().From_UTF8(it->first), Ztring().From_UTF8(it->second));

    MI->Open(ZenLib::Ztring().From_UTF8(file));
    scheduler->work_finished(this, MI);
    MI->Close();
    delete MI;
    MI = NULL;
}

//---------------------------------------------------------------------------
double QueueElement::percent_done()
{
    if (!MI)
        return (double)0;

    size_t state = MI->State_Get();
    return (double)state / 100;
}

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Queue::~Queue()
{
    clear();
}

int Queue::add_element(QueuePriority priority, int id, int user, const std::string& filename, long file_id,
                       const std::vector<std::string>& options, const std::vector<std::string>& plugins)
{
    QueueElement *el = new QueueElement(scheduler);

    el->id = id;
    el->user = user;
    el->filename = filename;
    el->file_id = file_id;

    for (size_t i = 0; i < options.size(); ++i)
    {
        std::string option = options[i];
        transform(option.begin(), option.end(), option.begin(), (int(*)(int))tolower);
        size_t pos = option.find("=");
        std::string key;
        std::string value("1");

        if (pos == std::string::npos && option.size() >= 2)
            key.assign(option, 2, pos);
        else if (pos == std::string::npos)
            continue;
        else
        {
            key.assign(option, 2, pos - 2);
            value.assign(options[i], pos + 1, std::string::npos);
        }
        el->options[key] = value;
    }

    for (size_t i = 0; i < plugins.size(); ++i)
        el->plugins.push_back(plugins[i]);

    queue[priority].push_back(el);
    return 0;
}

long Queue::has_element(int user, const std::string& filename)
{
    std::map<QueuePriority, std::list<QueueElement*> >::iterator it = queue.begin();

    for (; it != queue.end(); ++it)
    {
        std::list<QueueElement*>::iterator it_l = it->second.begin();
        for (; it_l != it->second.end() ; ++it_l)
            if ((*it_l)->filename == filename && (*it_l)->user == user)
                return (*it_l)->file_id;
    }

    return -1;
}

int Queue::has_id(int user, long file_id)
{
    std::map<QueuePriority, std::list<QueueElement*> >::iterator it = queue.begin();

    for (; it != queue.end(); ++it)
    {
        std::list<QueueElement*>::iterator it_l = it->second.begin();
        for (; it_l != it->second.end() ; ++it_l)
            if ((*it_l)->file_id == file_id && (*it_l)->user == user)
                return 0;
    }

    return -1;
}

int Queue::remove_element(int id)
{
    std::map<QueuePriority, std::list<QueueElement*> >::iterator it = queue.begin();

    for (; it != queue.end(); ++it)
    {
        std::list<QueueElement*>::iterator it_l = it->second.begin();
        for (; it_l != it->second.end() ; ++it_l)
            if ((*it_l)->id == id)
            {
                delete *it_l;
                it_l = it->second.erase(it_l);
            }
    }
    return 0;
}

int Queue::remove_elements(int user, const std::string& filename)
{
    std::map<QueuePriority, std::list<QueueElement*> >::iterator it = queue.begin();

    for (; it != queue.end(); ++it)
    {
        std::list<QueueElement*>::iterator it_l = it->second.begin();
        for (; it_l != it->second.end() ; ++it_l)
            if ((*it_l)->filename == filename && (*it_l)->user == user)
            {
                delete *it_l;
                it_l = it->second.erase(it_l);
            }
    }
    return 0;
}

void Queue::clear()
{
    std::map<QueuePriority, std::list<QueueElement*> >::iterator it = queue.begin();

    for (; it != queue.end(); ++it)
    {
        std::list<QueueElement*>::iterator it_l = it->second.begin();
        for (; it_l != it->second.end() ; ++it_l)
            delete *it_l;
    }
    queue.clear();
}

QueueElement *Queue::run_next()
{
    QueueElement* el = NULL;

    if (!queue[PRIORITY_HIGH].empty())
    {
        el = queue[PRIORITY_HIGH].front();
        queue[PRIORITY_HIGH].pop_front();
    }
    else if (!queue[PRIORITY_MEDIUM].empty())
    {
        el = queue[PRIORITY_MEDIUM].front();
        queue[PRIORITY_MEDIUM].pop_front();
    }
    else if (!queue[PRIORITY_LOW].empty())
    {
        el = queue[PRIORITY_LOW].front();
        queue[PRIORITY_LOW].pop_front();
    }
    else if (!queue[PRIORITY_NONE].empty())
    {
        el = queue[PRIORITY_NONE].front();
        queue[PRIORITY_NONE].pop_front();
    }

    if (!el)
        return NULL;

    el->Run();
    return el;
}

}
