/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "workerfiles.h"
#include "mainwindow.h"
#include "Common/FileRegistered.h"

#include "DatabaseUi.h"

#include <QString>
#include <QDir>
#include <QTimer>

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

//---------------------------------------------------------------------------
WorkerFiles::WorkerFiles(MainWindow* m) : QThread(), mainwindow(m), db(NULL), timer(NULL),
                                          file_index(0)
{
}

//---------------------------------------------------------------------------
WorkerFiles::~WorkerFiles()
{
    if (timer)
    {
        timer->stop();
        delete timer;
        timer = NULL;
    }
}

//---------------------------------------------------------------------------
void WorkerFiles::set_database(DatabaseUi* database)
{
    db = database;
}

//---------------------------------------------------------------------------
void WorkerFiles::run()
{
    timer = new QTimer(0);
    timer->moveToThread(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(update_files_registered()), Qt::DirectConnection);
    timer->start(1000); //TODO
    exec();
    if (timer)
    {
        timer->stop();
        delete timer;
        timer = NULL;
    }
}

//***************************************************************************
// Functions
//***************************************************************************

//---------------------------------------------------------------------------
FileRegistered* WorkerFiles::get_file_registered_from_file(const std::string& file)
{
    FileRegistered* fr = NULL;
    working_files_mutex.lock();
    if (working_files.find(file) != working_files.end() && working_files[file])
        fr = new FileRegistered(*working_files[file]);
    working_files_mutex.unlock();
    return fr;
}

//---------------------------------------------------------------------------
void WorkerFiles::get_registered_files(std::map<std::string, FileRegistered>& files)
{
    working_files_mutex.lock();
    std::map<std::string, FileRegistered*>::iterator it = working_files.begin();
    for (; it != working_files.end(); ++it)
    {
        if (!it->second)
            continue;

        files[it->first] = FileRegistered(*it->second);
    }
    working_files_mutex.unlock();
}

//---------------------------------------------------------------------------
void WorkerFiles::add_file_to_list(const std::string& file, const std::string& path,
                                   int policy, int display, int verbosity)
{
    std::string full_file(path);
    if (path.length())
        full_file += "/";
    full_file += file;

    bool exists = false;
    FileRegistered *fr = NULL;
    working_files_mutex.lock();
    if (working_files.find(full_file) != working_files.end() && working_files[full_file])
    {
        exists = true;
        // nothing to do
        if (policy == working_files[full_file]->policy && display == working_files[full_file]->display
            && verbosity == working_files[full_file]->verbosity)
        {
            working_files_mutex.unlock();
            return;
        }
        else
            fr = new FileRegistered;
    }
    else
        fr = new FileRegistered;
    working_files_mutex.unlock();

    // Keep the old index for the same file
    if (exists)
        fr->index = working_files[full_file]->index;
    else
        fr->index = file_index++;
    fr->filename = file;
    fr->filepath = path;
    fr->policy = policy;
    fr->display = display;
    fr->verbosity = verbosity;

    working_files_mutex.lock();

    if (exists)
        delete working_files[full_file];
    working_files[full_file] = fr;
    working_files_mutex.unlock();

    unfinished_files_mutex.lock();
    unfinished_files.push_back(full_file);
    unfinished_files_mutex.unlock();

    if (exists)
    {
        to_update_files_mutex.lock();
        to_update_files[full_file] = new FileRegistered(*fr);
        to_update_files_mutex.unlock();
        return;
    }

    to_add_files_mutex.lock();
    to_add_files[full_file] = new FileRegistered(*fr);
    to_add_files_mutex.unlock();

    std::vector<std::string> vec;
    vec.push_back(full_file);

    int ret;
    if ((ret = mainwindow->analyze(vec)) < 0)
        mainwindow->set_error_http((MediaConchLib::errorHttp)ret);
}

//---------------------------------------------------------------------------
void WorkerFiles::update_policy_of_file_registered_from_file(const std::string& file, int policy)
{
    working_files_mutex.lock();
    if (working_files.find(file) == working_files.end() || !working_files[file])
    {
        // file is not existing
        working_files_mutex.unlock();
        return;
    }

    bool policy_valid = false;
    if (working_files[file]->analyzed && working_files[file]->report_kind == MediaConchLib::report_MediaConch && policy >= 0)
    {
        working_files_mutex.unlock();

        std::vector<size_t> policies_ids;
        std::vector<std::string> policies_contents;
        std::vector<MediaConchLib::Checker_ValidateRes*> res;
        policies_ids.push_back(policy);

        if (mainwindow->validate(MediaConchLib::report_Max, file,
                                 policies_ids, policies_contents, res) == 0 && res.size() == 1)
        {
            policy_valid = res[0]->valid;
            for (size_t j = 0; j < res.size() ; ++j)
                delete res[j];
            res.clear();
        }

        working_files_mutex.lock();
    }

    working_files[file]->policy = policy;
    working_files[file]->policy_valid = policy_valid;
    FileRegistered fr = *working_files[file];
    working_files_mutex.unlock();

    to_update_files_mutex.lock();
    to_update_files[file] = new FileRegistered(fr);
    to_update_files_mutex.unlock();
}

//---------------------------------------------------------------------------
void WorkerFiles::clear_files()
{
    working_files_mutex.lock();
    std::map<std::string, FileRegistered*>::iterator it = working_files.begin();

    for (; it != working_files.end(); ++it)
    {
        if (!it->second)
            continue;

        delete it->second;
    }
    working_files.clear();
    working_files_mutex.unlock();
    remove_all_registered_file_from_db();
}

//---------------------------------------------------------------------------
void WorkerFiles::update_files_registered()
{
    if (timer)
    {
        delete timer;
        timer = NULL;
    }

    update_unfinished_files();
    update_add_files_registered();
    update_delete_files_registered();
    update_update_files_registered();

    timer = new QTimer(0);
    timer->moveToThread(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(update_files_registered()), Qt::DirectConnection);
    timer->start(1000); //TODO
}

//---------------------------------------------------------------------------
void WorkerFiles::update_add_files_registered()
{
    std::vector<FileRegistered*> vec;
    to_add_files_mutex.lock();
    if (to_add_files.size())
    {
        std::map<std::string, FileRegistered*>::iterator it = to_add_files.begin();
        for (; it != to_add_files.end(); ++it)
            vec.push_back(it->second);
        to_add_files.clear();
    }
    to_add_files_mutex.unlock();

    if (!vec.size())
        return;

    add_registered_files_to_db(vec);
    for (size_t i = 0; i < vec.size(); ++i)
        delete vec[i];
    vec.clear();
}

//---------------------------------------------------------------------------
void WorkerFiles::update_update_files_registered()
{
    std::vector<FileRegistered*> vec;
    to_update_files_mutex.lock();
    if (to_update_files.size())
    {
        std::map<std::string, FileRegistered*>::iterator it = to_update_files.begin();
        for (; it != to_update_files.end(); ++it)
            vec.push_back(it->second);
        to_update_files.clear();
    }
    to_update_files_mutex.unlock();

    if (!vec.size())
        return;

    update_registered_files_in_db(vec);
    for (size_t i = 0; i < vec.size(); ++i)
        delete vec[i];
    vec.clear();
}

//---------------------------------------------------------------------------
void WorkerFiles::update_delete_files_registered()
{
    std::vector<FileRegistered*> vec;

    to_delete_files_mutex.lock();
    if (to_delete_files.size())
    {
        std::map<std::string, FileRegistered*>::iterator it = to_delete_files.begin();
        for (; it != to_delete_files.end(); ++it)
            vec.push_back(it->second);
        to_delete_files.clear();
    }
    to_delete_files_mutex.unlock();

    if (!vec.size())
        return;

    remove_registered_files_from_db(vec);
    for (size_t i = 0; i < vec.size(); ++i)
        delete vec[i];
    vec.clear();
}

//---------------------------------------------------------------------------
void WorkerFiles::update_unfinished_files()
{
    unfinished_files_mutex.lock();
    std::vector<std::string> files = unfinished_files;
    unfinished_files_mutex.unlock();

    std::vector<std::string> vec;
    for (size_t i = 0; i < files.size(); ++i)
    {
        FileRegistered* fr = get_file_registered_from_file(files[i]);
        if (!fr)
        {
            vec.push_back(files[i]);
            continue;
        }

        double percent;

        MediaConchLib::report report_kind;
        int ret = mainwindow->is_analyze_finished(files[i], percent, report_kind);
        if (ret < 0)
        {
            mainwindow->set_error_http((MediaConchLib::errorHttp)ret);
            return;
        }

        fr->analyzed = false;
        if (ret == MediaConchLib::errorHttp_TRUE)
        {
            fr->analyzed = true;
            fr->report_kind = report_kind;
            std::vector<size_t> policies_ids;
            std::vector<std::string> policies_contents;
            std::vector<MediaConchLib::Checker_ValidateRes*> res;

            if (mainwindow->validate(report_kind, files[i],
                                     policies_ids, policies_contents, res) == 0
                && res.size() == 1)
                fr->implementation_valid = res[0]->valid;

            for (size_t j = 0; j < res.size() ; ++j)
                delete res[j];
            res.clear();
            if (report_kind == MediaConchLib::report_MediaConch && fr->policy >= 0)
            {
                policies_ids.push_back(fr->policy);

                if (mainwindow->validate(MediaConchLib::report_Max, files[i],
                                         policies_ids, policies_contents, res) == 0 && res.size() == 1)
                    fr->policy_valid = res[0]->valid;
                for (size_t j = 0; j < res.size() ; ++j)
                    delete res[j];
                res.clear();
            }
        }
        else
        {
            fr->analyze_percent = percent;
            vec.push_back(files[i]);
        }

        to_update_files_mutex.lock();
        if (to_update_files.find(files[i]) != to_update_files.end())
        {
            delete to_update_files[files[i]];
            to_update_files[files[i]] = new FileRegistered(*fr);
        }
        else
            to_update_files[files[i]] = new FileRegistered(*fr);
        to_update_files_mutex.unlock();

        working_files_mutex.lock();
        if (working_files.find(files[i]) != working_files.end())
        {
            delete working_files[files[i]];
            working_files[files[i]] = fr;
        }
        else
            delete fr;
        working_files_mutex.unlock();
    }

    unfinished_files_mutex.lock();
    if (files.size() != unfinished_files.size())
    {
        for (size_t i = 0; i < unfinished_files.size(); ++i)
        {
            size_t j = 0;
            for (; j < files.size(); ++j)
                if (files[j] == unfinished_files[i])
                    break;
            if (j == files.size())
                vec.push_back(unfinished_files[i]);
        }
    }
    unfinished_files = vec;
    unfinished_files_mutex.unlock();
}

//---------------------------------------------------------------------------
void WorkerFiles::remove_file_registered_from_file(const std::string& file)
{
    working_files_mutex.lock();
    std::map<std::string, FileRegistered*>::iterator it = working_files.find(file);
    if (it == working_files.end() || !working_files[file])
    {
        working_files_mutex.unlock();
        return;
    }

    FileRegistered* fr = working_files[file];

    working_files[file] = NULL;
    working_files.erase(it);
    working_files_mutex.unlock();

    to_delete_files_mutex.lock();
    to_delete_files[file] = fr;
    to_delete_files_mutex.unlock();
}

//---------------------------------------------------------------------------
void WorkerFiles::add_registered_file_to_db(const FileRegistered* file)
{
    if (!db)
        return;

    db->ui_add_file(file);
}

//---------------------------------------------------------------------------
void WorkerFiles::add_registered_files_to_db(const std::vector<FileRegistered*>& files)
{
    if (!db)
        return;

    db->ui_add_files(files);
}

//---------------------------------------------------------------------------
void WorkerFiles::update_registered_file_in_db(const FileRegistered* file)
{
    if (!db)
        return;

    db->ui_update_file(file);
}

//---------------------------------------------------------------------------
void WorkerFiles::update_registered_files_in_db(const std::vector<FileRegistered*>& files)
{
    if (!db)
        return;

    db->ui_update_files(files);
}

//---------------------------------------------------------------------------
void WorkerFiles::remove_registered_file_from_db(const FileRegistered* file)
{
    if (!db)
        return;

    db->ui_remove_file(file);
}

//---------------------------------------------------------------------------
void WorkerFiles::remove_registered_files_from_db(const std::vector<FileRegistered*>& files)
{
    if (!db)
        return;

    db->ui_remove_files(files);
}

//---------------------------------------------------------------------------
void WorkerFiles::remove_all_registered_file_from_db()
{
    if (!db)
        return;

    db->ui_remove_all_files();
}

//---------------------------------------------------------------------------
void WorkerFiles::get_registered_file_from_db(FileRegistered* file)
{
    if (!db || !file)
        return;

    db->ui_get_file(file);
}

//---------------------------------------------------------------------------
void WorkerFiles::fill_registered_files_from_db()
{
    if (!db)
        return;

    std::vector<FileRegistered*> vec;
    db->ui_get_elements(vec);

    std::vector<std::string> files;
    working_files_mutex.lock();
    for (size_t i = 0; i < vec.size(); ++i)
    {
        FileRegistered *fr = vec[i];

        std::string full_file(fr->filepath);
        if (full_file.length())
            full_file += "/";
        full_file += fr->filename;

        //check if policy still exists
        MediaConchLib::Get_Policy p;
        if (mainwindow->policy_get(fr->policy, p) < 0)
            fr->policy = -1;

        fr->index = file_index++;
        fr->analyzed = false;

        working_files[full_file] = fr;
        files.push_back(full_file);
    }
    working_files_mutex.unlock();

    unfinished_files_mutex.lock();
    for (size_t i = 0; i < files.size(); ++i)
        unfinished_files.push_back(files[i]);
    unfinished_files_mutex.unlock();

    int ret;
    if ((ret = mainwindow->analyze(files)) < 0)
        mainwindow->set_error_http((MediaConchLib::errorHttp)ret);
}

}
