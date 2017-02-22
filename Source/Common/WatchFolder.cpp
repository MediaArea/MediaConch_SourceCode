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
#include "Reports.h"
#include "PluginLog.h"
#include "PluginPreHook.h"

#include <ZenLib/Ztring.h>
#include <ZenLib/ZtringList.h>
#include <ZenLib/File.h>
#include <ZenLib/FileName.h>
#include <ZenLib/Dir.h>

#include <fstream>

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
WatchFolder::WatchFolder(Core* c, long user_id) : user(user_id), core(c), recursive(true), end(false), is_watching(false)
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
                if (!filename.size())
                    continue;

                WatchFolderFile *wffile = new WatchFolderFile;
                wffile->name = filename;
                wffile->time = time_utf8;

                //Create sub directory if needed
                std::string reports_dir = ZenLib::FileName::Path_Get(list[i]).To_UTF8();
                if (reports_dir.size() > folder.size())
                {
                    size_t s = folder.size();
                    if (s && (folder[s - 1] == '/' || folder[s - 1] == '\\'))
                        s -= 1;
                    reports_dir = reports_dir.substr(s);
                }
                else
                    reports_dir = std::string();

                ZenLib::Ztring f_reports = ZenLib::Ztring().From_UTF8(folder_reports);
                if (f_reports[f_reports.size() - 1] != ZenLib::FileName_PathSeparator[0])
                    f_reports += ZenLib::FileName_PathSeparator;
                f_reports += ZenLib::Ztring().From_UTF8(reports_dir);
                if (folder_reports.size() && !ZenLib::Dir::Exists(f_reports))
                    ZenLib::Dir::Create(f_reports);

                if (f_reports[f_reports.size() - 1] != ZenLib::FileName_PathSeparator[0])
                    f_reports += ZenLib::FileName_PathSeparator;
                wffile->report_file = f_reports.To_UTF8();

                wffile->report_file += ZenLib::FileName::Name_Get(list[i]).To_UTF8();
                if (ZenLib::FileName::Extension_Get(list[i]).size())
                    wffile->report_file += "." + ZenLib::FileName::Extension_Get(list[i]).To_UTF8();

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
                continue;

            bool registered = false;
            std::string err;
            bool need_analyze = true;
            for (size_t j = 0; need_analyze && j < plugins.size(); ++j)
            {
                const std::vector<Plugin*>& ps = core->get_pre_hook_plugins();
                for (size_t ps_i = 0; ps_i < ps.size(); ++ps_i)
                    if (ps[ps_i] && plugins[i] == ps[ps_i]->get_id() &&
                        ps[ps_i]->get_type() == MediaConchLib::PLUGIN_PRE_HOOK)
                    {
                        if (((PluginPreHook*)ps[ps_i])->is_creating_files())
                        {
                            need_analyze = false;
                            break;
                        }
                    }
            }

            wffile->file_id = core->checker_analyze(user, filename, registered, options, plugins, err, need_analyze);
            if (wffile->file_id == -1)
            {
                std::stringstream out;
                out << "Cannot parse:" << filename;
                core->plugin_add_log(PluginLog::LOG_LEVEL_ERROR, out.str());
            }
            else
                wffile->state = WatchFolderFile::WFFS_ANALYZING;
        }

        //In case of analysis
        std::map<std::string, WatchFolderFile*>::iterator it = files.begin();
        for (; it != files.end(); ++it)
        {
            WatchFolderFile *wffile = it->second;
            if (!wffile || wffile->state != WatchFolderFile::WFFS_ANALYZING)
                continue;

            MediaConchLib::Checker_StatusRes status;
            std::string err;
            core->checker_status(user, wffile->file_id, status, err);
            if (status.finished)
            {
                wffile->state = WatchFolderFile::WFFS_DONE;

                if (status.generated_id.size())
                {
                    for (size_t j = 0; j < status.generated_id.size(); ++j)
                    {
                        WatchFolderFile *new_wffile = new WatchFolderFile;
                        std::string filename;
                        std::string err;
                        core->checker_file_from_id(user, status.generated_id[j], filename, err);
                        new_wffile->file_id = status.generated_id[j];
                        new_wffile->name = filename;
                        new_wffile->time = ZenLib::File::Modified_Get(ZenLib::Ztring().From_UTF8(filename)).To_UTF8();
                        new_wffile->report_file = wffile->report_file;
                        new_wffile->state = WatchFolderFile::WFFS_ANALYZING;
                        files[filename] = new_wffile;
                    }
                }
                else if (folder_reports.size())
                    ask_report(wffile);
            }
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
    MediaConchLib::Checker_ReportRes result;

    std::string err;

    CheckerReport cr;
    cr.user = user;
    cr.files.push_back(wffile->file_id);
    //Implementation report HTML
    cr.report_set.set(MediaConchLib::report_MediaConch);
    cr.format = MediaConchLib::format_Html;
    int ret = core->reports.checker_get_report(cr, &result, err);

    if (ret < 0 || (result.has_valid && !result.valid))
    {
        std::stringstream out;

        out << wffile->name << ": implementation is not valid";
        core->plugin_add_log(PluginLog::LOG_LEVEL_ERROR, out.str());
    }

    std::string filename = wffile->report_file + ".ImplementationReport.html";
    std::ofstream out(filename.c_str(), std::ofstream::out);
    out << result.report;
    out.close();

    //Implementation report XML
    cr.format = MediaConchLib::format_Xml;
    result.has_valid = false;
    result.report = std::string();
    cr.report_set.reset();
    cr.report_set.set(MediaConchLib::report_MediaConch);
    ret = core->reports.checker_get_report(cr, &result, err);

    filename = wffile->report_file + ".ImplementationReport.xml";
    std::ofstream out_xml(filename.c_str(), std::ofstream::out);
    out_xml << result.report;
    out_xml.close();

    //MediaInfo XML
    result.has_valid = false;
    result.report = std::string();
    cr.report_set.reset();
    cr.report_set.set(MediaConchLib::report_MediaInfo);
    ret = core->reports.checker_get_report(cr, &result, err);

    filename = wffile->report_file + ".MediaInfo.xml";
    std::ofstream out_mi(filename.c_str(), std::ofstream::out);
    out_mi << result.report;
    out_mi.close();

    //MediaTrace XML
    result.has_valid = false;
    result.report = std::string();
    cr.report_set.reset();
    cr.report_set.set(MediaConchLib::report_MediaTrace);
    ret = core->reports.checker_get_report(cr, &result, err);

    filename = wffile->report_file + ".MediaTrace.xml";
    std::ofstream out_mt(filename.c_str(), std::ofstream::out);
    out_mt << result.report;
    out_mt.close();

    //Policies
    if (policies.size())
    {
        result.has_valid = false;
        result.report = std::string();
        cr.report_set.reset();

        for (size_t i = 0; i < policies.size(); ++i)
            cr.policies_contents.push_back(policies[i]);
        ret = core->reports.checker_get_report(cr, &result, err);
        if (ret < 0 || (result.has_valid && !result.valid))
        {
            std::stringstream out;

            out << wffile->name << ": policy is not valid";
            core->plugin_add_log(PluginLog::LOG_LEVEL_ERROR, out.str());
        }
        cr.report_set.reset();

        std::ofstream out_pr(std::string(wffile->report_file + ".PoliciesReport.xml").c_str(), std::ofstream::out);
        out_pr << result.report;
        out_pr.close();
    }

    return ret;
}

};
