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
#include "Daemon.h"
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
    TEXTOUT("--Version, -v");
    TEXTOUT("                    Display the version and exit");
    TEXTOUT("");

    TEXTOUT("Implementation Checker:");
    TEXTOUT("--ImplementationSchema=File");
    TEXTOUT("                    Use the specified File for implementation validation");
    TEXTOUT("--ImplementationVerbosity=V, -iv V");
    TEXTOUT("                    Select verbosity (V) of the implementation check, default 5");
    TEXTOUT("                    <= 4, show only fails and N/A");
    TEXTOUT("                    >= 5, show fails, N/A and pass");
    TEXTOUT("");

    TEXTOUT("Daemon fork:");
    TEXTOUT("--Fork=[Yes/No]");
    TEXTOUT("                    If No, MediaConchd will not fork, default it forks");
    TEXTOUT("-n");
    TEXTOUT("                    Same as --Fork=No");
    return DAEMON_RETURN_FINISH;
}

//---------------------------------------------------------------------------
int Help_Usage()
{
    TEXTOUT("Usage: \"MediaConchD [-Options...]\"");

    return DAEMON_RETURN_ERROR;
}

//---------------------------------------------------------------------------
int Version()
{
    TEXTOUT(std::string("MediaConch Daemon " + MediaConch::Daemon::version).c_str());

    return DAEMON_RETURN_FINISH;
}
