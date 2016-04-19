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
#ifndef SQLLiteUiH
#define SQLLiteUiH
//---------------------------------------------------------------------------

#include "DatabaseUi.h"
#include "Common/SQLLite.h"
#include <sqlite3.h>
//---------------------------------------------------------------------------

namespace MediaConch {

class FileRegistered;

//***************************************************************************
// Class SQLLite
//***************************************************************************

class SQLLiteUi : public DatabaseUi, public SQLLite
{
public:
    //Constructor/Destructor
    SQLLiteUi();
    virtual ~SQLLiteUi();

    // General
    virtual int init();
    virtual int init_ui();

    // UI Settings
    int         ui_settings_create_table();
    int         ui_settings_has_user_id(int user_id, bool& has_row);
    int         ui_settings_add_default_values_for_user(int user_id);
    int         ui_settings_check_user_id(int user_id);

    virtual int ui_settings_save_default_policy(const std::string&, int user_id=-1);
    virtual int ui_settings_get_default_policy(std::string&, int user_id = -1);

    virtual int ui_settings_save_default_display(const std::string&, int user_id = -1);
    virtual int ui_settings_get_default_display(std::string&, int user_id = -1);

    virtual int ui_settings_save_default_verbosity(int, int user_id = -1);
    virtual int ui_settings_get_default_verbosity(int&, int user_id = -1);

    // UI
    int         ui_create_table();
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

private:
    int           ui_version;
    static int    current_ui_version;

    SQLLiteUi (const SQLLiteUi&);
    SQLLiteUi& operator=(const SQLLiteUi&);
};

}

#endif /* !SQLLiteUiH */

#endif /* !HAVE_SQLITE */
