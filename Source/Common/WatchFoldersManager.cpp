/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#include "WatchFoldersManager.h"
#include "WatchFolder.h"
#include "Core.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Watch Folders Manager
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

namespace MediaConch {

//---------------------------------------------------------------------------
WatchFoldersManager::WatchFoldersManager(Core *c) : core(c)
{
}

WatchFoldersManager::~WatchFoldersManager()
{
    CS.Enter();
    std::map<std::string, WatchFolder*>::iterator it = watch_folders.begin();
    for (; it != watch_folders.end(); ++it)
    {
        if (it->second)
        {
            delete it->second;
            it->second = NULL;
        }
    }
    watch_folders.clear();
    CS.Leave();
}

//---------------------------------------------------------------------------
std::map<std::string, std::string> WatchFoldersManager::get_watch_folders()
{
    std::map<std::string, std::string> tmp;
    CS.Enter();
    std::map<std::string, WatchFolder*>::iterator it = watch_folders.begin();
    for (; it != watch_folders.end(); ++it)
    {
        if (!it->second)
            continue;
        tmp[it->first] = it->second->folder_reports;
    }
    CS.Leave();
    return tmp;
}

//---------------------------------------------------------------------------
long WatchFoldersManager::add_watch_folder(const std::string& folder, const std::string& folder_reports, std::string& error)
{
    CS.Enter();
    std::map<std::string, WatchFolder*>::iterator it = watch_folders.find(folder);

    if (it != watch_folders.end())
    {
        error = "Already watching this folder";
        CS.Leave();
        return -1;
    }

    WatchFolder *wf = new WatchFolder;
    wf->folder = folder;
    wf->folder_reports = folder_reports;
    watch_folders[folder] = wf;
    CS.Leave();
    return 0;
}

//---------------------------------------------------------------------------
int WatchFoldersManager::edit_watch_folder(const std::string& folder,
                                           const std::string& folder_reports,
                                           std::string& error)
{
    CS.Enter();
    std::map<std::string, WatchFolder*>::iterator it = watch_folders.find(folder);

    if (it == watch_folders.end() || !it->second)
    {
        error = "Not watching this folder";
        CS.Leave();
        return -1;
    }

    it->second->folder_reports = folder_reports;
    CS.Leave();
    return 0;
}

//---------------------------------------------------------------------------
int WatchFoldersManager::remove_watch_folder(const std::string& folder, std::string& error)
{
    CS.Enter();
    std::map<std::string, WatchFolder*>::iterator it = watch_folders.find(folder);

    if (it == watch_folders.end() || !it->second)
    {
        error = "Not watching this folder";
        CS.Leave();
        return -1;
    }

    delete it->second;
    it->second = NULL;
    watch_folders.erase(it);
    CS.Leave();
    return 0;
}

};
