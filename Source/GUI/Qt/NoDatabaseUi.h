/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// NoDatabase functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef NODATABASEUIH
#define NODATABASEUIH

//---------------------------------------------------------------------------
#include "DatabaseUi.h"

//---------------------------------------------------------------------------
namespace MediaConch {

class FileRegistered;

//***************************************************************************
// Class NoDatabase
//***************************************************************************

class NoDatabaseUi : public DatabaseUi
{
public:
    //Constructor/Destructor
    NoDatabaseUi();
    virtual ~NoDatabaseUi();

    // General
    virtual int init();
    virtual int init_ui();

    // UI Settings
    virtual int ui_settings_save_default_policy(const std::string&, int user_id = -1);
    virtual int ui_settings_get_default_policy(std::string&, int user_id = -1);
    virtual int ui_settings_save_last_policy(const std::string&, int user_id = -1);
    virtual int ui_settings_get_last_policy(std::string&, int user_id = -1);

    virtual int ui_settings_save_default_display(const std::string&, int user_id = -1);
    virtual int ui_settings_get_default_display(std::string&, int user_id = -1);
    virtual int ui_settings_save_last_display(const std::string&, int user_id = -1);
    virtual int ui_settings_get_last_display(std::string&, int user_id = -1);

    virtual int ui_settings_save_default_verbosity(int, int user_id = -1);
    virtual int ui_settings_get_default_verbosity(int&, int user_id = -1);
    virtual int ui_settings_save_last_verbosity(int, int user_id = -1);
    virtual int ui_settings_get_last_verbosity(int&, int user_id = -1);

    virtual int ui_settings_save_default_load_files_path(const std::string&, int user_id = -1);
    virtual int ui_settings_get_default_load_files_path(std::string&, int user_id = -1);
    virtual int ui_settings_save_last_load_files_path(const std::string&, int user_id = -1);
    virtual int ui_settings_get_last_load_files_path(std::string&, int user_id = -1);

    virtual int ui_settings_save_default_load_policy_path(const std::string&, int user_id = -1);
    virtual int ui_settings_get_default_load_policy_path(std::string&, int user_id = -1);
    virtual int ui_settings_save_last_load_policy_path(const std::string&, int user_id = -1);
    virtual int ui_settings_get_last_load_policy_path(std::string&, int user_id = -1);

    virtual int ui_settings_save_default_load_display_path(const std::string&, int user_id = -1);
    virtual int ui_settings_get_default_load_display_path(std::string&, int user_id = -1);
    virtual int ui_settings_save_last_load_display_path(const std::string&, int user_id = -1);
    virtual int ui_settings_get_last_load_display_path(std::string&, int user_id = -1);

    virtual int ui_settings_save_default_save_report_path(const std::string&, int user_id = -1);
    virtual int ui_settings_get_default_save_report_path(std::string&, int user_id = -1);
    virtual int ui_settings_save_last_save_report_path(const std::string&, int user_id = -1);
    virtual int ui_settings_get_last_save_report_path(std::string&, int user_id = -1);

    virtual int ui_settings_save_default_save_policy_path(const std::string&, int user_id = -1);
    virtual int ui_settings_get_default_save_policy_path(std::string&, int user_id = -1);
    virtual int ui_settings_save_last_save_policy_path(const std::string&, int user_id = -1);
    virtual int ui_settings_get_last_save_policy_path(std::string&, int user_id = -1);

    virtual int ui_settings_save_default_save_display_path(const std::string&, int user_id = -1);
    virtual int ui_settings_get_default_save_display_path(std::string&, int user_id = -1);
    virtual int ui_settings_save_last_save_display_path(const std::string&, int user_id = -1);
    virtual int ui_settings_get_last_save_display_path(std::string&, int user_id = -1);

    virtual int  ui_settings_save_mco_token(const std::string&, int user_id=-1);
    virtual int  ui_settings_get_mco_token(std::string&, int user_id = -1);

    // UI
    virtual int ui_add_file(const FileRegistered* file);
    virtual int ui_add_files(const std::vector<FileRegistered*>& files);
    virtual int ui_update_file(const FileRegistered* file);
    virtual int ui_update_files(const std::vector<FileRegistered*>& files);
    virtual int ui_get_file(FileRegistered* file);
    virtual int ui_remove_file(const FileRegistered* file);
    virtual int ui_remove_files(const std::vector<FileRegistered*>& files);
    virtual int ui_remove_all_files();
    virtual void ui_get_elements(std::vector<FileRegistered*>& vec);
    virtual int ui_reset_file_for_reload(FileRegistered* file);

protected:
    virtual int execute();

private:
    NoDatabaseUi (const NoDatabaseUi&);
    NoDatabaseUi& operator=(const NoDatabaseUi&);
};

}

#endif /* !NODATABASEUIH */
