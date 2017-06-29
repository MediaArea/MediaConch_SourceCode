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
#ifndef SchedulerH
#define SchedulerH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifdef MEDIAINFO_DLL_RUNTIME
    #include "MediaInfoDLL/MediaInfoDLL.h"
    #define MediaInfoNameSpace MediaInfoDLL
#elif defined MEDIAINFO_DLL_STATIC
    #include "MediaInfoDLL/MediaInfoDLL_Static.h"
    #define MediaInfoNameSpace MediaInfoDLL
#else
    #include "MediaInfo/MediaInfo.h"
    #define MediaInfoNameSpace MediaInfoLib
#endif
#include "ZenLib/CriticalSection.h"
#include <map>
#include <vector>

using namespace MediaInfoNameSpace;
using namespace ZenLib;

//---------------------------------------------------------------------------
namespace MediaConch {

class Queue;
class QueueElement;
class Core;

//***************************************************************************
// Class Scheduler
//***************************************************************************

class Scheduler
{
public:
    //Constructor/Destructor
    Scheduler(Core *c);
    virtual ~Scheduler();

    int  add_element_to_queue(int user, const std::string& filename, long file_id,
                              const std::vector<std::pair<std::string,std::string> >& options,
                              const std::vector<std::string>& plugins, bool mil_analyze,
                              const std::string& alias="");
    void work_finished(QueueElement* el, MediaInfoNameSpace::MediaInfo* MI);
    bool is_finished();

    long element_exists(int user, const std::string& filename,
                        const std::string& options, std::string& err);
    bool element_is_finished(int user, long file_id, double& percent_done);
    int  get_elements(int user, std::vector<std::string>& vec, std::string& err);
    int  get_elements(int user, std::vector<long>& vec, std::string& err);
    int  stop_elements(int user, const std::vector<long>& vec, std::string& err);

    int  another_work_to_do(QueueElement* el, MediaInfoNameSpace::MediaInfo* MI);
    int  execute_pre_hook_plugins(QueueElement *el, std::string& err);
    int  attachments_to_add(QueueElement *el);

    void write_log_timestamp(int level, std::string log);
    void log_cb(struct MediaInfo_Event_Log_0 *Event);

    void set_default_max_threads(size_t nb) {} // { if (max_threads_modified) return; max_threads = nb; } //TODO: check issues with the GUI when there are tons of files
    void set_max_threads(size_t nb) { max_threads_modified = true; max_threads = nb; }

private:
    Scheduler(const Scheduler&);
    Scheduler&     operator=(const Scheduler&);

    Core                                   *core;
    Queue                                  *queue;
    int                                     threads_launch;
    size_t                                  max_threads;
    bool                                    max_threads_modified;
    std::map<QueueElement*, QueueElement*>  working;
    CriticalSection                         CS;

    void run_element();
    void remove_element(QueueElement *el);
};

}

#endif
