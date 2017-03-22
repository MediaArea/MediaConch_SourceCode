/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#include <vector>
#include <algorithm>
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifdef MEDIAINFO_DLL_RUNTIME
    #include "MediaInfoDLL/MediaInfoDLL.h"
    #define MediaInfoNameSpace MediaInfoDLL
#elif defined MEDIAINFO_DLL_STATIC
    #include "MediaInfoDLL/MediaInfoDLL_Static.h"
    #define MediaInfoNameSpace MediaInfoDLL
#else
    #include "MediaInfo/MediaInfoList.h"
    #define MediaInfoNameSpace MediaInfoLib
#endif
#if defined(_MSC_VER) && defined(UNICODE)
    #include "io.h"
    #include "fcntl.h"
#endif
#include "CLI.h"
#include "CommandLine_Parser.h"
#include "MediaInfo/MediaInfo.h"
#include "MediaInfo/MediaInfo_Events.h"
//---------------------------------------------------------------------------

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
        _setmode(_fileno(stdout), _O_U8TEXT);
        _setmode(_fileno(stderr), _O_U8TEXT);
    #endif
    MediaInfoLib::MediaInfo::Option_Static(__T("LineSeparator"), __T("\n")); //Using sdtout

    // TODO: Retrieve command line (mainly for Unicode)
    GETCOMMANDLINE();
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i)
    {
        MediaInfoLib::String Argument(argv[i]);
        args.push_back(ZenLib::Ztring(Argument).To_UTF8());
    }

    MediaConch::CLI cli;

    int ret = cli.parse_args(args);
    if (ret == CLI_RETURN_ERROR)
        return 1;
    else if (ret == CLI_RETURN_FINISH)
        return 0;

    std::string err;
    if (cli.init(err) == CLI_RETURN_ERROR)
    {
        STRINGOUT(ZenLib::Ztring().From_UTF8(err));
        return 1;
    }

    if (cli.run(err) < 0)
        STRINGOUT(ZenLib::Ztring().From_UTF8(err));

    cli.finish();

    return 0;
}
