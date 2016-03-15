/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// SQLLite functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef HAVE_SQLITE

//---------------------------------------------------------------------------
#ifndef SQLLiteH
#define SQLLiteH
//---------------------------------------------------------------------------

#include "Database.h"
#include <sqlite3.h>
//---------------------------------------------------------------------------

namespace MediaConch {

class FileRegistered;

//***************************************************************************
// Class SQLLite
//***************************************************************************

class SQLLite : public Database
{
public:
    //Constructor/Destructor
    SQLLite();
    virtual ~SQLLite();

    // Report
    virtual int create_report_table();
    virtual int update_report_table();
    virtual int save_report(MediaConchLib::report reportKind, MediaConchLib::format format,
                            const std::string& filename, const std::string& file_last_modification,
                            const std::string& report, MediaConchLib::compression);
    virtual int update_report(MediaConchLib::report reportKind, MediaConchLib::format format,
                              const std::string& filename, const std::string& file_last_modification,
                              const std::string& report, MediaConchLib::compression);
    virtual int remove_report(const std::string& filename);
    virtual void get_report(MediaConchLib::report reportKind, MediaConchLib::format format,
                            const std::string& filename, const std::string& file_last_modification,
                            std::string& report, MediaConchLib::compression&);
    virtual bool file_is_registered(MediaConchLib::report reportKind, MediaConchLib::format format,
                                    const std::string& filename, const std::string& file_last_modification);
    virtual bool file_is_registered(MediaConchLib::report reportKind, MediaConchLib::format format,
                                    const std::string& filename);
    virtual void get_elements(std::vector<std::string>& vec);
    virtual void get_element_report_kind(const std::string& file, MediaConchLib::report& report_kind);

    // UI
    virtual int create_ui_table();
    virtual int update_ui_table();
    virtual int ui_add_file(const FileRegistered* file);
    virtual int ui_add_files(const std::vector<FileRegistered*>& files);
    virtual int ui_update_file(const FileRegistered* file);
    virtual int ui_update_files(const std::vector<FileRegistered*>& files);
    virtual int ui_get_file(FileRegistered* file);
    virtual int ui_remove_file(const FileRegistered* file);
    virtual int ui_remove_files(const std::vector<FileRegistered*>& files);
    virtual int ui_remove_all_files();
    virtual void ui_get_elements(std::vector<FileRegistered*>& vec);

protected:
    virtual int execute();
    virtual int init();
    virtual int init_report();
    virtual int init_ui();

private:
    sqlite3      *db;
    sqlite3_stmt *stmt; // Statement handler

    int           report_version;
    int           ui_version;

    static int    current_report_version;
    static int    current_ui_version;

    // Helper
    int std_string_to_int(const std::string& str);
    int get_db_version(int& version);
    int set_db_version(int version);

    SQLLite (const SQLLite&);
    SQLLite& operator=(const SQLLite&);
};

}

#endif /* !SQLLiteH */

#endif /* !HAVE_SQLITE */
