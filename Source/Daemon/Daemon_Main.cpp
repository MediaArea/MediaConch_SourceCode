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
#include "Daemon_Main.h"
#include "Common/MediaConchLib.h"
#include "Help.h"
#include "Daemon.h"
#if defined(_MSC_VER) && defined(UNICODE)
#include "io.h"
#include "fcntl.h"
#endif

//***************************************************************************
// Main
//***************************************************************************

//---------------------------------------------------------------------------
int main(int argc, char* argv_ansi[])
{
    //Localisation
    setlocale(LC_ALL, "");
    MediaInfoLib::MediaInfo::Option_Static(__T("CharSet"), __T(""));

    //Initialize terminal (to fix Unicode output on Win32)
#if defined(_MSC_VER) && defined(UNICODE)
    //_setmode(_fileno(stdout), _O_U8TEXT);
    //_setmode(_fileno(stderr), _O_U8TEXT);
#endif
    MediaInfoLib::MediaInfo::Option_Static(__T("LineSeparator"), __T("\n")); //Using sdtout

#ifdef _WIN32
    //Initialize sockets (needed before using sockets on Windows)
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        printf("WSAStartup failed\n");
        return 1;
    }
#endif //_WIN32

    // TODO: Retrieve command line (mainly for Unicode)
    // GETCOMMANDLINE();

    MediaConch::Daemon daemon;
    std::string error;
    //Parse command line
    int ret = daemon.parse_args(argc, argv_ansi);
    if (ret == DAEMON_RETURN_ERROR)
        ret = 1;
    else if (ret == DAEMON_RETURN_FINISH)
        ret = 0;
    else if (daemon.init(error) < 0)
        ret = 2;
    else if (daemon.run(error) < 0)
        ret = 2;

    if (ret == 2)
        STRINGERR(ZenLib::Ztring().From_UTF8(error));

#ifdef _WIN32
    WSACleanup();
#endif //_WIN32

    return ret;
}
