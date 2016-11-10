/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef UISETTINGS_H
#define UISETTINGS_H

#include <string>

//---------------------------------------------------------------------------
namespace MediaConch {

class DatabaseUi;

class UiSettings
{
public:
    UiSettings();
    ~UiSettings();

//***************************************************************************
// Functions
//***************************************************************************
    //General
    int          init();
    void         set_database(DatabaseUi* db);

    // Default
    std::string  get_default_policy() const;
    void         change_default_policy(const std::string& policy);
    std::string  get_default_display() const;
    void         change_default_display(const std::string& display);
    int          get_default_verbosity() const;
    void         change_default_verbosity(int verbosity);
    std::string  get_default_save_report_path() const;
    void         change_default_save_report_path(const std::string& path);
    std::string  get_default_save_policy_path() const;
    void         change_default_save_policy_path(const std::string& path);
    std::string  get_default_save_display_path() const;
    void         change_default_save_display_path(const std::string& path);
    std::string  get_default_load_files_path() const;
    void         change_default_load_files_path(const std::string& path);
    std::string  get_default_load_policy_path() const;
    void         change_default_load_policy_path(const std::string& path);
    std::string  get_default_load_display_path() const;
    void         change_default_load_display_path(const std::string& path);

    // Last used
    std::string  get_last_policy() const;
    void         change_last_policy(const std::string& policy);
    std::string  get_last_display() const;
    void         change_last_display(const std::string& display);
    int          get_last_verbosity() const;
    void         change_last_verbosity(int verbosity);
    std::string  get_last_load_files_path() const;
    void         change_last_load_files_path(const std::string& path);
    std::string  get_last_load_policy_path() const;
    void         change_last_load_policy_path(const std::string& path);
    std::string  get_last_load_display_path() const;
    void         change_last_load_display_path(const std::string& path);
    std::string  get_last_save_report_path() const;
    void         change_last_save_report_path(const std::string& path);
    std::string  get_last_save_policy_path() const;
    void         change_last_save_policy_path(const std::string& path);
    std::string  get_last_save_display_path() const;
    void         change_last_save_display_path(const std::string& path);

    // MCO Token
    std::string  get_mco_token() const;
    void         change_mco_token(const std::string& token);

private:
    // default
    std::string  default_policy;
    std::string  default_display;
    int          default_verbosity;
    std::string  default_load_files_path;
    std::string  default_load_policy_path;
    std::string  default_load_display_path;
    std::string  default_save_report_path;
    std::string  default_save_policy_path;
    std::string  default_save_display_path;

    //Last used
    std::string  last_policy;
    std::string  last_display;
    int          last_verbosity;
    std::string  last_load_files_path;
    std::string  last_load_policy_path;
    std::string  last_load_display_path;
    std::string  last_save_report_path;
    std::string  last_save_policy_path;
    std::string  last_save_display_path;

    //MCO Token
    std::string  mco_token;

    DatabaseUi  *database;
};

}

#endif // UISETTINGS_H
