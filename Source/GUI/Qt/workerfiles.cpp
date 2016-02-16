/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "workerfiles.h"
#include "mainwindow.h"
#include "Common/FileRegistered.h"

#include "Common/Database.h"
#include "Common/NoDatabase.h"
#include "Common/SQLLite.h"

#include <QString>
#include <QDir>

namespace MediaConch {

//***************************************************************************
// Constant
//***************************************************************************

const std::string WorkerFiles::database_filename = std::string("MediaConchUi.db");

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

WorkerFiles::WorkerFiles(MainWindow* m): mainwindow(m), db(NULL)
{
}

WorkerFiles::~WorkerFiles()
{
    if (db)
        delete db;
}

//***************************************************************************
// Functions
//***************************************************************************

//---------------------------------------------------------------------------
FileRegistered* WorkerFiles::get_file_registered_from_file(const std::string& file)
{
    if (working_files.find(file) == working_files.end() || !working_files[file])
        return NULL;
    return working_files[file];
}

//---------------------------------------------------------------------------
const std::map<std::string, FileRegistered*>& WorkerFiles::get_registered_files() const
{
    return working_files;
}

//---------------------------------------------------------------------------
void WorkerFiles::add_file_to_list(const std::string& file, const std::string& path,
                                   int policy, int display)
{
    std::string full_file(path);
    if (path.length())
        full_file += "/";
    full_file += file;

    bool exists = false;
    FileRegistered *fr = NULL;
    if (working_files.find(full_file) == working_files.end() || !working_files[full_file])
        fr = new FileRegistered;
    else
    {
        fr = working_files[full_file];
        if (policy != fr->policy)
            fr->need_update = true;
        else
            fr->need_update = false;
        exists = true;
    }

    fr->filename = file;
    fr->filepath = path;
    fr->policy = policy;
    fr->display = display;

    if (exists)
    {
        if (fr->need_update)
            update_registered_file_in_db(fr);
        return;
    }

    working_files[full_file] = fr;
    std::vector<std::string> vec;
    vec.push_back(full_file);

    int ret;
    if ((ret = mainwindow->analyze(vec)) < 0)
        mainwindow->set_error_http((MediaConchLib::errorHttp)ret);
    add_registered_file_to_db(fr);
}

//---------------------------------------------------------------------------
void WorkerFiles::clear_files()
{
    std::map<std::string, FileRegistered*>::iterator it = working_files.begin();

    for (; it != working_files.end(); ++it)
    {
        if (!it->second)
            continue;

        remove_registered_file_from_db(it->second);
        delete it->second;
    }
    working_files.clear();
}

//---------------------------------------------------------------------------
void WorkerFiles::update_file_registered(const std::string& file, FileRegistered* fr)
{
    if (!fr)
        return;

    double percent;
    std::vector<std::string> files;
    files.push_back(file);

    int ret = mainwindow->is_analyze_finished(files, percent);
    if (ret < 0)
    {
        mainwindow->set_error_http((MediaConchLib::errorHttp)ret);
        return;
    }

    fr->analyzed = false;
    if (ret == MediaConchLib::errorHttp_TRUE)
    {
        fr->analyzed = true;
        std::vector<std::string> policies_names, policies_contents;
        std::vector<MediaConchLib::ValidateRes*> res;

        if (mainwindow->validate(MediaConchLib::report_MediaConch, files,
                                 policies_names, policies_contents, res) == 0
            && res.size() == 1)
            fr->implementation_valid = res[0]->valid;

        for (size_t i = 0; i < res.size() ; ++i)
            delete res[i];
        res.clear();
        if (fr->policy >= 0)
        {
            Policy *p = mainwindow->get_policy((size_t)fr->policy);
            if (p)
            {
                std::string policy_content;
                p->dump_schema(policy_content);
                policies_contents.push_back(policy_content);
            }

            if (p && mainwindow->validate(MediaConchLib::report_Max, files,
                         policies_names, policies_contents, res) == 0 && res.size() == 1)
                fr->policy_valid = res[0]->valid;
            for (size_t i = 0; i < res.size() ; ++i)
                delete res[i];
            res.clear();
        }
        update_registered_file_in_db(fr);
        fr->need_update = false;
    }
}

//---------------------------------------------------------------------------
void WorkerFiles::remove_file_registered_from_file(const std::string& file)
{
    std::map<std::string, FileRegistered*>::iterator it = working_files.find(file);
    if (it == working_files.end() || !working_files[file])
        return;

    FileRegistered* fr = working_files[file];

    remove_registered_file_from_db(fr);
    delete fr;
    working_files[file] = NULL;
    working_files.erase(it);
}

//---------------------------------------------------------------------------
void WorkerFiles::load_database()
{
#ifdef HAVE_SQLITE
    std::string db_path;
    if (mainwindow->get_ui_database_path(db_path) < 0)
    {
        db_path = Core::get_local_data_path();
        QDir f(QString().fromStdString(db_path));
        if (!f.exists())
            db_path = ".";
    }

    db = new SQLLite;

    db->set_database_directory(db_path);
    db->set_database_filename(database_filename);
    if (db->init_ui() < 0)
    {
        const std::vector<std::string>& errors = db->get_errors();
        std::string error;
        for (size_t i = 0; i < errors.size(); ++i)
        {
            if (i)
                error += " ";
            error += errors[i];
        }
        QString msg = QString().fromStdString(error);
        mainwindow->set_msg_error_to_status_bar(msg);
    }
#else
    db = new NoDatabase;
    db->init_ui();
#endif
}

//---------------------------------------------------------------------------
void WorkerFiles::create_and_configure_database()
{
    load_database();
}

//---------------------------------------------------------------------------
void WorkerFiles::add_registered_file_to_db(const FileRegistered* file)
{
    if (!db)
        return;

    db->ui_add_file(file);
}

//---------------------------------------------------------------------------
void WorkerFiles::update_registered_file_in_db(const FileRegistered* file)
{
    if (!db)
        return;

    db->ui_update_file(file);
}

//---------------------------------------------------------------------------
void WorkerFiles::remove_registered_file_from_db(const FileRegistered* file)
{
    if (!db)
        return;

    db->ui_remove_file(file);
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
    for (size_t i = 0; i < vec.size(); ++i)
    {
        FileRegistered *fr = vec[i];

        std::string full_file(fr->filepath);
        if (full_file.length())
            full_file += "/";
        full_file += fr->filename;

        working_files[full_file] = fr;
        files.push_back(full_file);
    }

    int ret;
    if ((ret = mainwindow->analyze(files)) < 0)
        mainwindow->set_error_http((MediaConchLib::errorHttp)ret);
}

}
