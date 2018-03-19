/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Watch Folder Manager
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef WATCHFOLDER_MANAGERH
#define WATCHFOLDER_MANAGERH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <string>
#include <vector>
#include <map>
#include "ZenLib/CriticalSection.h"

//---------------------------------------------------------------------------
namespace MediaConch {

class Core;
class WatchFolder;

//***************************************************************************
// Class WatchFolderManager
//***************************************************************************

class WatchFoldersManager
{
public:
    WatchFoldersManager(Core *c);
    ~WatchFoldersManager();

    std::map<std::string, std::string>  get_watch_folders();

    int                                 add_watch_folder(const std::string&, const std::string&,
                                                         const std::vector<std::string>&, const std::vector<std::string>&,
                                                         long *in_user, bool recursive,
                                                         const std::vector<std::pair<std::string,std::string> >& options,
                                                         long& user_id, std::string&);
    int                                 edit_watch_folder(const std::string&, const std::string&, std::string&);
    int                                 remove_watch_folder(const std::string&, std::string&);

private:
    WatchFoldersManager(const WatchFoldersManager&);
    WatchFoldersManager&                operator=(const WatchFoldersManager&);

    Core                                *core;
    ZenLib::CriticalSection              CS;
    std::map<std::string, WatchFolder*>  watch_folders;
};

}

#endif // !WATCHFOLDERS_MANAGERH
