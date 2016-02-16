/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef WORKERFILES_H
#define WORKERFILES_H

#include <map>
#include <string>

namespace MediaConch {

class FileRegistered;
class MainWindow;
class Database;

class WorkerFiles
{
public:
    explicit WorkerFiles(MainWindow* m);
    ~WorkerFiles();

//***************************************************************************
// Functions
//***************************************************************************

    void add_file_to_list(const std::string& file, const std::string& path, int policy, int display);
    void clear_files();
    FileRegistered* get_file_registered_from_file(const std::string& file);
    void remove_file_registered_from_file(const std::string& file);
    void update_file_registered(const std::string& file, FileRegistered* fr);
    const std::map<std::string, FileRegistered*>& get_registered_files() const;

    void fill_registered_files_from_db();
    void create_and_configure_database();
    void load_database();

private:
    void add_registered_file_to_db(const FileRegistered* file);
    void update_registered_file_in_db(const FileRegistered* file);
    void get_registered_file_from_db(FileRegistered* file);
    void remove_registered_file_from_db(const FileRegistered* file);


private:
    MainWindow                             *mainwindow;
    Database                               *db;
    static const std::string                database_filename;

    std::map<std::string, FileRegistered*>  working_files;
    std::map<std::string, FileRegistered*>  to_delete_files;
    std::map<std::string, FileRegistered*>  to_add_files;
    std::map<std::string, FileRegistered*>  to_update_files;
};

}

#endif // WORKERFILES_H
