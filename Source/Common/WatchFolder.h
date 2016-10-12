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
#include <map>
#include "ZenLib/Thread.h"
#include "ZenLib/CriticalSection.h"

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Class WatchFolder
//***************************************************************************

class WatchFolder : public ZenLib::Thread
{
public:
    WatchFolder();
    ~WatchFolder();

    void          Entry();

    std::string   folder;
    std::string   folder_reports;

private:
    WatchFolder(const WatchFolder&);
    WatchFolder&  operator=(const WatchFolder&);
};

}

#endif // !WATCHFOLDERS_MANAGERH
