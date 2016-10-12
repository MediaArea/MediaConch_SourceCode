/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#include "WatchFoldersManager.h"
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
}

//---------------------------------------------------------------------------
std::map<std::string, std::string> WatchFoldersManager::get_watch_folders()
{
    std::map<std::string, std::string> tmp;
    CS.Enter();
    std::map<std::string, std::string>::iterator it = watch_folders.begin();
    for (; it != watch_folders.end(); ++it)
        tmp[it->first] = it->second;
    CS.Leave();
    return tmp;
}

//---------------------------------------------------------------------------
long WatchFoldersManager::add_watch_folder(const std::string& folder, const std::string& folder_reports, std::string& error)
{
    CS.Enter();
    std::map<std::string, std::string>::iterator it = watch_folders.find(folder);

    if (it != watch_folders.end())
    {
        error = "Already watching this folder";
        CS.Leave();
        return -1;
    }

    watch_folders[folder] = folder_reports;
    CS.Leave();
    return 0;
}

//---------------------------------------------------------------------------
int WatchFoldersManager::edit_watch_folder(const std::string& folder,
                                                      const std::string& folder_reports,
                                                      std::string& error)
{
    CS.Enter();
    std::map<std::string, std::string>::iterator it = watch_folders.find(folder);

    if (it == watch_folders.end())
    {
        error = "Not watching this folder";
        CS.Leave();
        return -1;
    }

    it->second = folder_reports;
    CS.Leave();
    return 0;
}

//---------------------------------------------------------------------------
int WatchFoldersManager::remove_watch_folder(const std::string& folder, std::string& error)
{
    CS.Enter();
    std::map<std::string, std::string>::iterator it = watch_folders.find(folder);

    if (it == watch_folders.end())
    {
        error = "Not watching this folder";
        CS.Leave();
        return -1;
    }

    watch_folders.erase(it);
    CS.Leave();
    return 0;
}

};
