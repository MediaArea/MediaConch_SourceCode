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
#include "Help.h"
#include "Config.h"
//---------------------------------------------------------------------------

//***************************************************************************
//
//***************************************************************************

//---------------------------------------------------------------------------
int Help()
{
    Version();
    Help_Usage();

    TEXTOUT("");
    TEXTOUT("Options:");
    TEXTOUT("Help:");
    TEXTOUT("--Help, -h");
    TEXTOUT("                    Display this help and exit");

    TEXTOUT("Daemon fork:");
    TEXTOUT("--Fork=[Yes/No]");
    TEXTOUT("                    If No, MediaConchd will not fork, default it forks");
    TEXTOUT("-n");
    TEXTOUT("                    Same as --Fork=No");
    return -1;
}

//---------------------------------------------------------------------------
int Help_Usage()
{
    TEXTOUT("Usage: \"MediaConchD [-Options...]\"");

    return -1;
}

//---------------------------------------------------------------------------
int Version()
{
    TEXTOUT("MediaConch Daemon 0.1.0");

    return -1;
}
