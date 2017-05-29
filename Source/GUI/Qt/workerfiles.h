/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef WORKERFILES_H
#define WORKERFILES_H

#include <map>
#include <vector>
#include <string>
#include <QThread>
#include <QMutex>

class QTimer;

namespace MediaConch {

class FileRegistered;
class MainWindow;
class DatabaseUi;

class WorkerFiles : public QThread
{
    Q_OBJECT

public:
    explicit WorkerFiles(MainWindow* m);
    virtual ~WorkerFiles();

    // Thread
    void run();

    // Configuration
    void fill_registered_files_from_db();
    void set_database(DatabaseUi* database);

    // functions
    int add_file_to_list(const std::string& file, const std::string& path, int policy, int display,
                         int verbosity, bool fixer, bool force, bool, const std::vector<std::string>&, std::string& err);
    int add_file_to_list(long id, const std::string& file, const std::string&, const std::string&, std::string& err);
    int add_attachment_to_list(const std::string& file, int policy, int display, int verbosity,
                               const std::vector<std::string>& options, std::string& err);
    void clear_files();
    FileRegistered* get_file_registered_from_file(const std::string& file);
    FileRegistered* get_file_registered_from_id(long id);
    void remove_file_registered_from_id(long file_id);
    void remove_all_files_registered();
    void get_registered_files(std::map<std::string, FileRegistered*>& files);
    long get_id_from_registered_file(const std::string& file);
    std::string get_filename_from_registered_file_id(long file_id);
    int update_policy_of_file_registered_from_file(long file_id, long policy, std::string& error);

private:
    void add_registered_file_to_db(const FileRegistered* file);
    void add_registered_files_to_db(const std::vector<FileRegistered*>& files);
    void update_registered_file_in_db(const FileRegistered* file);
    void update_registered_files_in_db(const std::vector<FileRegistered*>& files);
    void get_registered_file_from_db(FileRegistered* file);
    void remove_registered_file_from_db(const FileRegistered* file);
    void remove_registered_files_from_db(const std::vector<FileRegistered*>& files);
    void remove_all_registered_file_from_db();

    void update_unfinished_files();
    void update_add_files_registered();
    void update_delete_files_registered();
    void update_update_files_registered();

private Q_SLOTS:
    void update_files_registered();

private:
    MainWindow                             *mainwindow;
    DatabaseUi                             *db;
    QTimer                                 *timer;
    size_t                                  file_index;

    std::map<std::string, FileRegistered*>  working_files;
    QMutex                                  working_files_mutex;

    std::map<std::string, FileRegistered*>  to_delete_files;
    QMutex                                  to_delete_files_mutex;

    std::map<std::string, FileRegistered*>  to_add_files;
    QMutex                                  to_add_files_mutex;

    std::map<std::string, FileRegistered*>  to_update_files;
    QMutex                                  to_update_files_mutex;

    std::vector<std::string>                unfinished_files;
    QMutex                                  unfinished_files_mutex;
};

}

#endif // WORKERFILES_H
