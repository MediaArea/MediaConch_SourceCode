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
#include "WatchFolder.h"
#include "MediaConchLib.h"
#include "Core.h"

#include <ZenLib/Ztring.h>
#include <ZenLib/ZtringList.h>
#include <ZenLib/File.h>
#include <ZenLib/Dir.h>

#if defined(UNIX)
#include <unistd.h>
#elif defined(MACOS) || defined(MACOSX)
#include <unistd.h>
#elif defined(WINDOWS)
#include <windows.h>
#include <Lmcons.h>
#endif

//---------------------------------------------------------------------------
namespace MediaConch {

//---------------------------------------------------------------------------
WatchFolderFile::WatchFolderFile() : state(WFFS_NOT_READY)
{
}

//---------------------------------------------------------------------------
WatchFolderFile::~WatchFolderFile()
{
}

//---------------------------------------------------------------------------
WatchFolder::WatchFolder(Core* c, long user_id) : user(user_id), core(c), recursive(false), end(false), is_watching(false)
{
#ifdef WINDOWS
        waiting_time = 10;
#else
        waiting_time = 1000000;
#endif
}

//---------------------------------------------------------------------------
WatchFolder::~WatchFolder()
{
    stop();
}

//---------------------------------------------------------------------------
void WatchFolder::set_waiting_time(size_t time)
{
    waiting_time = time;
}

//---------------------------------------------------------------------------
void WatchFolder::set_recursive(bool r)
{
    recursive = r;
}

//---------------------------------------------------------------------------
void WatchFolder::Entry()
{
    is_watching = true;
    while (!end)
    {
        ZenLib::Ztring dir_name = ZenLib::Ztring().From_UTF8(folder);

        int flags = ZenLib::Dir::Include_Files | ZenLib::Dir::Include_Hidden;
        if (recursive)
            flags |= ZenLib::Dir::Parse_SubDirs;

        ZenLib::ZtringList list = ZenLib::Dir::GetAllFileNames(dir_name, (ZenLib::Dir::dirlist_t)flags);

        for (size_t i = 0; i < list.size(); ++i)
        {
            ZenLib::Ztring modified_time = ZenLib::File::Modified_Get(list[i]);
            std::string filename = list[i].To_UTF8();
            std::string time_utf8 = modified_time.To_UTF8();

            if (files.find(filename) == files.end() || !files[filename])
            {
                WatchFolderFile *wffile = new WatchFolderFile;
                wffile->name = filename;
                wffile->time = time_utf8;
                files[filename] = wffile;
                continue;
            }

            WatchFolderFile *wffile = files[filename];
            if (wffile->time != time_utf8)
            {
                wffile->time = time_utf8;
                wffile->state = WatchFolderFile::WFFS_NOT_READY;
                continue;
            }

            if (wffile->state == WatchFolderFile::WFFS_DONE)
                continue;

            if (wffile->state == WatchFolderFile::WFFS_ANALYZING)
            {
                MediaConchLib::Checker_StatusRes status;
                core->checker_status(user, wffile->file_id, status);
                if (status.finished)
                {
                    //TODO: write report if folder_reports is set
                    ask_report(wffile);
                    wffile->state = WatchFolderFile::WFFS_DONE;
                }
                continue;
            }

            std::vector<std::string> options;
            bool registered = false;
            wffile->file_id = core->checker_analyze(user, filename, registered, options, plugins, false);
            if (wffile->file_id == -1)
            {
                std::stringstream out;
                out << "Cannot parse:" << filename;
                core->plugin_add_log(out.str());
            }
            else
                wffile->state = WatchFolderFile::WFFS_ANALYZING;
        }

#ifdef WINDOWS
        ::Sleep((DWORD)waiting_time);
#else
        usleep(waiting_time);
#endif
    }
    is_watching = false;
}

//---------------------------------------------------------------------------
void WatchFolder::stop()
{
    end = true;
    RequestTerminate();
    while (!IsExited())
    {
#ifdef WINDOWS
        ::Sleep(0);
#else //WINDOWS
        sleep(0);
#endif //WINDOWS
    }
}

//---------------------------------------------------------------------------
int WatchFolder::ask_report(WatchFolderFile *wffile)
{
    std::vector<long> files;
    files.push_back(wffile->file_id);

    MediaConchLib::format format = MediaConchLib::format_Html;


    std::bitset<MediaConchLib::report_Max> report_set;

    std::vector<size_t> policies_ids;
    std::vector<std::string> policies_contents;

    std::map<std::string, std::string> options;

    MediaConchLib::Checker_ReportRes result;

    std::string display_name, display_content;

    report_set.set(MediaConchLib::report_MediaConch);
    int ret = core->checker_get_report(user, report_set, format,
                                       files, policies_ids, policies_contents, options,
                                       &result, &display_name, &display_content);
    report_set.reset();

    if (!result.valid)
    {
        std::stringstream out;

        out << wffile->name << ": is not valid";
        core->plugin_add_log(out.str());
    }
    return ret;
}

};
