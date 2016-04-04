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
    #include "MediaInfo/MediaInfoList.h"
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

    int add_element_to_queue(const std::string& filename, const std::vector<std::string>& options);
    void work_finished(QueueElement* el, MediaInfoNameSpace::MediaInfoList* MI);
    bool is_finished();
    bool element_exists(const std::string& filename);
    bool element_is_finished(const std::string& filename, double& percent_done);
    void set_max_threads(size_t nb) { max_threads = nb; }
    void get_elements(std::vector<std::string>& vec);
    int another_work_to_do(QueueElement* el, MediaInfoNameSpace::MediaInfoList* MI);

private:
    Scheduler(const Scheduler&);
    Scheduler&     operator=(const Scheduler&);

    Core                                   *core;
    Queue                                  *queue;
    int                                     threads_launch;
    size_t                                  max_threads;
    std::map<QueueElement*, QueueElement*>  working;
    CriticalSection                         CS;

    void run_element();
};

}

#endif
