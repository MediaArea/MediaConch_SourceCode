/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Queue functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef QueueH
#define QueueH
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
#include "ZenLib/Ztring.h"
#include "ZenLib/Thread.h"
#include "ZenLib/CriticalSection.h"
#include <MediaInfo/MediaInfo_Events.h>
//---------------------------------------------------------------------------
#include <map>
#include <list>
#include <vector>

namespace MediaConch
{
    //***************************************************************************
    // Class QueueElement
    //***************************************************************************

    class Queue;
    class Scheduler;

    struct Attachment
    {
        std::string filename;
        std::string realname;
    };

    class QueueElement : public ZenLib::Thread
    {
    public:
        QueueElement(Scheduler *s);
        virtual ~QueueElement();
        int                                user;
        int                                id;
        std::string                        filename;
        std::string                        real_filename;
        std::string                        options_str;
        std::vector<std::pair<std::string, std::string> > options;
        std::vector<std::string>           plugins;
        std::vector<Attachment*>           attachments;
        long                               file_id;
        bool                               mil_analyze;

        void                               Entry();
        void                               stop();
        double                             percent_done();
        int                                attachment_cb(struct MediaInfo_Event_Global_AttachedFile_0 *Event);
        int                                log_cb(struct MediaInfo_Event_Log_0 *Event);

    private:
        Scheduler*                         scheduler;
        MediaInfoNameSpace::MediaInfo     *MI;
        ZenLib::CriticalSection            MI_CS;
    };

    //***************************************************************************
    // Enum priority
    //***************************************************************************

    enum QueuePriority
    {
        PRIORITY_NONE,
        PRIORITY_LOW,
        PRIORITY_MEDIUM,
        PRIORITY_HIGH,
    };

    //***************************************************************************
    // Class Queue
    //***************************************************************************

    class Queue
    {
    public:
        Queue(Scheduler *s) : scheduler(s){}
        ~Queue();

        int add_element(QueuePriority priority, int id, int user, const std::string& filename, long file_id,
                        const std::vector<std::pair<std::string,std::string> >& options,
                        const std::vector<std::string>& plugins, bool mil_analyze,
                        const std::string& alias="");
        long has_element(int user, const std::string& filename);
        int  has_id(int user, long file_id);
        int remove_element(int id);
        int remove_elements(int user, const std::string& filename);
        void clear();

        QueueElement* run_next();
        size_t queue_size() const { return queue.size(); }

    private:
        std::map<QueuePriority, std::list<QueueElement*> > queue;
        Scheduler *scheduler;
    };
}

#endif
