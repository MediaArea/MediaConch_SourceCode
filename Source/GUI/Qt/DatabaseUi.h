/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Database functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef DatabaseUiH
#define DatabaseUiH
//---------------------------------------------------------------------------

#include <map>
#include <vector>
#include "Common/Database.h"
#include "Common/MediaConchLib.h"
//---------------------------------------------------------------------------

namespace MediaConch {

class FileRegistered;

//***************************************************************************
// Class Database
//***************************************************************************

class DatabaseUi : public Database
{
public:
    //Constructor/Destructor
    DatabaseUi();
    virtual ~DatabaseUi();

    void        set_database_directory(const std::string& dirname);
    void        set_database_filename(const std::string& name);

    // General
    virtual int init_ui() = 0;

    // UI
    virtual int ui_add_file(const FileRegistered* file) = 0;
    virtual int ui_add_files(const std::vector<FileRegistered*>& files) = 0;
    virtual int ui_update_file(const FileRegistered* file) = 0;
    virtual int ui_update_files(const std::vector<FileRegistered*>& files) = 0;
    virtual int ui_get_file(FileRegistered* file) = 0;
    virtual int ui_remove_file(const FileRegistered* file) = 0;
    virtual int ui_remove_files(const std::vector<FileRegistered*>& files) = 0;
    virtual int ui_remove_all_files() = 0;
    virtual void ui_get_elements(std::vector<FileRegistered*>& vec) = 0;

    // UI Settings
    virtual int ui_settings_save_default_policy(const std::string&, int user_id = -1) = 0;
    virtual int ui_settings_get_default_policy(std::string&, int user_id = -1) = 0;
    virtual int ui_settings_save_last_policy(const std::string&, int user_id = -1) = 0;
    virtual int ui_settings_get_last_policy(std::string&, int user_id = -1) = 0;

    virtual int ui_settings_save_default_display(const std::string&, int user_id = -1) = 0;
    virtual int ui_settings_get_default_display(std::string&, int user_id = -1) = 0;
    virtual int ui_settings_save_last_display(const std::string&, int user_id = -1) = 0;
    virtual int ui_settings_get_last_display(std::string&, int user_id = -1) = 0;

    virtual int ui_settings_save_default_verbosity(int, int user_id = -1) = 0;
    virtual int ui_settings_get_default_verbosity(int&, int user_id = -1) = 0;
    virtual int ui_settings_save_last_verbosity(int, int user_id = -1) = 0;
    virtual int ui_settings_get_last_verbosity(int&, int user_id = -1) = 0;

protected:
    //Database dependant
    void        get_sql_query_for_create_ui_table(std::string& q);
    void        get_sql_query_for_update_ui_table_v0(std::string& q);
    void        get_sql_query_for_create_ui_settings_table(std::string& q);
    void        get_sql_query_for_update_ui_settings_table_v1(std::string& q);
    void        get_sql_query_for_update_ui_settings_table_v2(std::string& q);
    void        get_sql_query_for_update_ui_settings_table_v3(std::string& q);

private:
    DatabaseUi (const DatabaseUi&);
    DatabaseUi& operator=(const DatabaseUi&);
};

}

#endif
