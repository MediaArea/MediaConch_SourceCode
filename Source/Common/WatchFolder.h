/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Watch Folder
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef WATCHFOLDERH
#define WATCHFOLDERH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <string>
#include <vector>
#include <map>
#include <ZenLib/Thread.h>
#include <ZenLib/CriticalSection.h>

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Class WatchFolderFile
//***************************************************************************

class Core;

class WatchFolderFile
{
public:
    WatchFolderFile();
    ~WatchFolderFile();

    enum WatchFolderFileState
    {
        WFFS_NOT_READY,
        WFFS_ANALYZING,
        WFFS_DONE,
    };

    std::string           name;
    std::string           time;
    std::string           report_file;
    long                  file_id;
    WatchFolderFileState  state;
};

//***************************************************************************
// Class WatchFolder
//***************************************************************************

class WatchFolder : public ZenLib::Thread
{
public:
    WatchFolder(Core* c, long user);
    ~WatchFolder();

    void                                     Entry();
    void                                     stop();
    void                                     set_waiting_time(size_t t);
    void                                     set_recursive(bool);

    std::string                              folder;
    std::string                              folder_reports;
    std::vector<std::string>                 plugins;
    long                                     user;

private:
    WatchFolder(const WatchFolder&);
    WatchFolder&                             operator=(const WatchFolder&);

    int                                      ask_report(WatchFolderFile *wffile);

    Core                                    *core;
    std::map<std::string, WatchFolderFile*>  files;
    size_t                                   waiting_time;
    bool                                     recursive;
    bool                                     end;
    bool                                     is_watching;
};

}

#endif // !WATCHFOLDERS_MANAGERH
