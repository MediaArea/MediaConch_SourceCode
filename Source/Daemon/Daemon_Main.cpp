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
#include "Common/MediaConchLib.h"
#include "Help.h"
#include "Daemon.h"

//***************************************************************************
// Main
//***************************************************************************

//---------------------------------------------------------------------------
int main(int argc, char* argv_ansi[])
{
    // TODO: Retrieve command line (mainly for Unicode)
    // GETCOMMANDLINE();

    MediaConch::Daemon daemon;
    //Parse command line
    if (daemon.parse_args(argc, argv_ansi) < 0)
        return 1;

    if (daemon.init() < 0)
        return 1;

    if (daemon.run() < 0)
        return 1;

    return 0;
}
