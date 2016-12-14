/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "uisettings.h"
#include "DatabaseUi.h"

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

UiSettings::UiSettings()
{
}

UiSettings::~UiSettings()
{
}

//***************************************************************************
// Functions
//***************************************************************************

// General
//---------------------------------------------------------------------------
int UiSettings::init()
{
    default_policy = "last";
    default_display = "last";
    default_verbosity = 5;
    last_verbosity = 5;

    if (!database)
        return -1;
    if (database->ui_settings_get_default_policy(default_policy))
        return -1;
    if (database->ui_settings_get_default_display(default_display))
        return -1;
    if (database->ui_settings_get_default_verbosity(default_verbosity))
        return -1;
    if (database->ui_settings_get_default_save_report_path(default_save_report_path))
        return -1;
    if (database->ui_settings_get_default_save_policy_path(default_save_policy_path))
        return -1;
    if (database->ui_settings_get_default_save_display_path(default_save_display_path))
        return -1;
    if (database->ui_settings_get_default_load_files_path(default_load_files_path))
        return -1;
    if (database->ui_settings_get_default_load_policy_path(default_load_policy_path))
        return -1;
    if (database->ui_settings_get_default_load_display_path(default_load_display_path))
        return -1;

    if (database->ui_settings_get_last_policy(last_policy))
        return -1;
    if (database->ui_settings_get_last_display(last_display))
        return -1;
    if (database->ui_settings_get_last_verbosity(last_verbosity))
        return -1;
    if (database->ui_settings_get_last_save_report_path(last_save_report_path))
        return -1;
    if (database->ui_settings_get_last_save_policy_path(last_save_policy_path))
        return -1;
    if (database->ui_settings_get_last_save_display_path(last_save_display_path))
        return -1;
    if (database->ui_settings_get_last_load_files_path(last_load_files_path))
        return -1;
    if (database->ui_settings_get_last_load_policy_path(last_load_policy_path))
        return -1;
    if (database->ui_settings_get_last_load_display_path(last_load_display_path))
        return -1;

    if (database->ui_settings_get_mco_token(mco_token))
        return -1;
    return 0;
}

//---------------------------------------------------------------------------
void UiSettings::set_database(DatabaseUi* db)
{
    database = db;
}

// Default
//---------------------------------------------------------------------------
std::string UiSettings::get_default_policy() const
{
    return default_policy;
}

//---------------------------------------------------------------------------
void UiSettings::change_default_policy(const std::string& policy)
{
    if (!database || default_policy == policy)
        return;

    if (database->ui_settings_save_default_policy(policy) < 0)
        return;

    default_policy = policy;
}

//---------------------------------------------------------------------------
std::string UiSettings::get_default_display() const
{
    return default_display;
}

//---------------------------------------------------------------------------
void UiSettings::change_default_display(const std::string& display)
{
    if (!database || default_display == display)
        return;

    if (database->ui_settings_save_default_display(display) < 0)
        return;

    default_display = display;
}

//---------------------------------------------------------------------------
int UiSettings::get_default_verbosity() const
{
    return default_verbosity;
}

//---------------------------------------------------------------------------
void UiSettings::change_default_verbosity(int verbosity)
{
    if (!database || default_verbosity == verbosity)
        return;

    if (database->ui_settings_save_default_verbosity(verbosity) < 0)
        return;

    default_verbosity = verbosity;
}

//---------------------------------------------------------------------------
std::string UiSettings::get_default_save_report_path() const
{
    return default_save_report_path;
}

//---------------------------------------------------------------------------
void UiSettings::change_default_save_report_path(const std::string& path)
{
    if (!database || default_save_report_path == path)
        return;

    if (database->ui_settings_save_default_save_report_path(path) < 0)
        return;

    default_save_report_path = path;
}

//---------------------------------------------------------------------------
std::string UiSettings::get_default_save_policy_path() const
{
    return default_save_policy_path;
}

//---------------------------------------------------------------------------
void UiSettings::change_default_save_policy_path(const std::string& path)
{
    if (!database || default_save_policy_path == path)
        return;

    if (database->ui_settings_save_default_save_policy_path(path) < 0)
        return;

    default_save_policy_path = path;
}

//---------------------------------------------------------------------------
std::string UiSettings::get_default_save_display_path() const
{
    return default_save_display_path;
}

//---------------------------------------------------------------------------
void UiSettings::change_default_save_display_path(const std::string& path)
{
    if (!database || default_save_display_path == path)
        return;

    if (database->ui_settings_save_default_save_display_path(path) < 0)
        return;

    default_save_display_path = path;
}

//---------------------------------------------------------------------------
std::string UiSettings::get_default_load_files_path() const
{
    return default_load_files_path;
}

//---------------------------------------------------------------------------
void UiSettings::change_default_load_files_path(const std::string& path)
{
    if (!database || default_load_files_path == path)
        return;

    if (database->ui_settings_save_default_load_files_path(path) < 0)
        return;

    default_load_files_path = path;
}

//---------------------------------------------------------------------------
std::string UiSettings::get_default_load_policy_path() const
{
    return default_load_policy_path;
}

//---------------------------------------------------------------------------
void UiSettings::change_default_load_policy_path(const std::string& path)
{
    if (!database || default_load_policy_path == path)
        return;

    if (database->ui_settings_save_default_load_policy_path(path) < 0)
        return;

    default_load_policy_path = path;
}

//---------------------------------------------------------------------------
std::string UiSettings::get_default_load_display_path() const
{
    return default_load_display_path;
}

//---------------------------------------------------------------------------
void UiSettings::change_default_load_display_path(const std::string& path)
{
    if (!database || default_load_display_path == path)
        return;

    if (database->ui_settings_save_default_load_display_path(path) < 0)
        return;

    default_load_display_path = path;
}

// Last used
//---------------------------------------------------------------------------
std::string UiSettings::get_last_policy() const
{
    return last_policy;
}

//---------------------------------------------------------------------------
void UiSettings::change_last_policy(const std::string& policy)
{
    if (!database || last_policy == policy)
        return;

    if (database->ui_settings_save_last_policy(policy) < 0)
        return;

    last_policy = policy;
}

//---------------------------------------------------------------------------
std::string UiSettings::get_last_display() const
{
    return last_display;
}

//---------------------------------------------------------------------------
void UiSettings::change_last_display(const std::string& display)
{
    if (!database || last_display == display)
        return;

    if (database->ui_settings_save_last_display(display) < 0)
        return;

    last_display = display;
}

//---------------------------------------------------------------------------
int UiSettings::get_last_verbosity() const
{
    return last_verbosity;
}

//---------------------------------------------------------------------------
void UiSettings::change_last_verbosity(int verbosity)
{
    if (!database || last_verbosity == verbosity)
        return;

    if (database->ui_settings_save_last_verbosity(verbosity) < 0)
        return;

    last_verbosity = verbosity;
}

//---------------------------------------------------------------------------
std::string UiSettings::get_last_save_report_path() const
{
    return last_save_report_path;
}

//---------------------------------------------------------------------------
void UiSettings::change_last_save_report_path(const std::string& path)
{
    if (!database || last_save_report_path == path)
        return;

    if (database->ui_settings_save_last_save_report_path(path) < 0)
        return;

    last_save_report_path = path;
}

//---------------------------------------------------------------------------
std::string UiSettings::get_last_save_policy_path() const
{
    return last_save_policy_path;
}

//---------------------------------------------------------------------------
void UiSettings::change_last_save_policy_path(const std::string& path)
{
    if (!database || last_save_policy_path == path)
        return;

    if (database->ui_settings_save_last_save_policy_path(path) < 0)
        return;

    last_save_policy_path = path;
}

//---------------------------------------------------------------------------
std::string UiSettings::get_last_save_display_path() const
{
    return last_save_display_path;
}

//---------------------------------------------------------------------------
void UiSettings::change_last_save_display_path(const std::string& path)
{
    if (!database || last_save_display_path == path)
        return;

    if (database->ui_settings_save_last_save_display_path(path) < 0)
        return;

    last_save_display_path = path;
}

//---------------------------------------------------------------------------
std::string UiSettings::get_last_load_files_path() const
{
    return last_load_files_path;
}

//---------------------------------------------------------------------------
void UiSettings::change_last_load_files_path(const std::string& path)
{
    if (!database || last_load_files_path == path)
        return;

    if (database->ui_settings_save_last_load_files_path(path) < 0)
        return;

    last_load_files_path = path;
}

//---------------------------------------------------------------------------
std::string UiSettings::get_last_load_policy_path() const
{
    return last_load_policy_path;
}

//---------------------------------------------------------------------------
void UiSettings::change_last_load_policy_path(const std::string& path)
{
    if (!database || last_load_policy_path == path)
        return;

    if (database->ui_settings_save_last_load_policy_path(path) < 0)
        return;

    last_load_policy_path = path;
}

//---------------------------------------------------------------------------
std::string UiSettings::get_last_load_display_path() const
{
    return last_load_display_path;
}

//---------------------------------------------------------------------------
void UiSettings::change_last_load_display_path(const std::string& path)
{
    if (!database || last_load_display_path == path)
        return;

    if (database->ui_settings_save_last_load_display_path(path) < 0)
        return;

    last_load_display_path = path;
}

// Last used
//---------------------------------------------------------------------------
std::string UiSettings::get_mco_token() const
{
    return mco_token;
}

//---------------------------------------------------------------------------
void UiSettings::change_mco_token(const std::string& token)
{
    if (!database || mco_token == token)
        return;

    if (database->ui_settings_save_mco_token(token) < 0)
        return;

    mco_token = token;
}

}
