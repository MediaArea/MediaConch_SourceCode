/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "DatabaseUi.h"
#include <sstream>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Database
//***************************************************************************

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
DatabaseUi::DatabaseUi()
{
}

//---------------------------------------------------------------------------
DatabaseUi::~DatabaseUi()
{
}

//---------------------------------------------------------------------------
void DatabaseUi::set_database_directory(const std::string& dirname)
{
    Database::set_database_directory(dirname);
}

//---------------------------------------------------------------------------
void DatabaseUi::set_database_filename(const std::string& filename)
{
    Database::set_database_filename(filename);
}

//---------------------------------------------------------------------------
void DatabaseUi::get_sql_query_for_create_ui_table(std::string& q)
{
    std::stringstream create;
    create << "CREATE TABLE IF NOT EXISTS UI "; // Table name
    create << "(FILENAME              TEXT NOT NULL,";
    create << " FILEPATH              TEXT NOT NULL,";
    create << " POLICY                INT  NOT NULL,";
    create << " DISPLAY               INT  NOT NULL,";
    create << " ANALYZED              BOOL NOT NULL,";
    create << " IMPLEMENTATION_VALID  BOOL         ,";
    create << " POLICY_VALID          BOOL          );";
    q = create.str();
}

//---------------------------------------------------------------------------
void DatabaseUi::get_sql_query_for_update_ui_table_v0(std::string& q)
{
    std::stringstream create;
    create << "ALTER TABLE UI "; // Table name
    create << "ADD VERBOSITY INT DEFAULT -1 NOT NULL;";
    q = create.str();
}

//---------------------------------------------------------------------------
void DatabaseUi::get_sql_query_for_create_ui_settings_table(std::string& q)
{
    std::stringstream create;
    create << "CREATE TABLE IF NOT EXISTS UI_SETTINGS "; // Table name
    create << "(USER_ID               INT     PRIMARY KEY DEFAULT -1,";
    create << " DEFAULT_POLICY        TEXT    NOT NULL,";
    create << " DEFAULT_DISPLAY       TEXT    NOT NULL,";
    create << " DEFAULT_VERBOSITY     INT     NOT NULL);";
    q = create.str();
}

//---------------------------------------------------------------------------
void DatabaseUi::get_sql_query_for_update_ui_settings_table_v1(std::string& q)
{
    std::stringstream create;
    create << "ALTER TABLE UI_SETTINGS"; // Table name
    create << " ADD LAST_POLICY    TEXT DEFAULT NULL;";
    q = create.str();
}

//---------------------------------------------------------------------------
void DatabaseUi::get_sql_query_for_update_ui_settings_table_v2(std::string& q)
{
    std::stringstream create;
    create << "ALTER TABLE UI_SETTINGS"; // Table name
    create << " ADD LAST_DISPLAY   TEXT DEFAULT NULL;";
    q = create.str();
}

//---------------------------------------------------------------------------
void DatabaseUi::get_sql_query_for_update_ui_settings_table_v3(std::string& q)
{
    std::stringstream create;
    create << "ALTER TABLE UI_SETTINGS"; // Table name
    create << " ADD LAST_VERBOSITY INT  DEFAULT 5;";
    q = create.str();
}

//---------------------------------------------------------------------------
void DatabaseUi::get_sql_query_for_update_ui_settings_table_v4(std::string& q)
{
    std::stringstream create;
    create << "ALTER TABLE UI_SETTINGS"; // Table name
    create << " ADD DEFAULT_SAVE_REPORT_PATH TEXT DEFAULT NULL;";
    q = create.str();
}

//---------------------------------------------------------------------------
void DatabaseUi::get_sql_query_for_update_ui_settings_table_v5(std::string& q)
{
    std::stringstream create;
    create << "ALTER TABLE UI_SETTINGS"; // Table name
    create << " ADD DEFAULT_LOAD_FILES_PATH TEXT DEFAULT NULL;";
    q = create.str();
}

//---------------------------------------------------------------------------
void DatabaseUi::get_sql_query_for_update_ui_settings_table_v6(std::string& q)
{
    std::stringstream create;
    create << "ALTER TABLE UI_SETTINGS"; // Table name
    create << " ADD LAST_SAVE_REPORT_PATH TEXT DEFAULT NULL;";
    q = create.str();
}

//---------------------------------------------------------------------------
void DatabaseUi::get_sql_query_for_update_ui_settings_table_v7(std::string& q)
{
    std::stringstream create;
    create << "ALTER TABLE UI_SETTINGS"; // Table name
    create << " ADD LAST_LOAD_FILES_PATH TEXT DEFAULT NULL;";
    q = create.str();
}

//---------------------------------------------------------------------------
void DatabaseUi::get_sql_query_for_update_ui_settings_table_v8(std::string& q)
{
    std::stringstream create;
    create << "ALTER TABLE UI_SETTINGS"; // Table name
    create << " ADD DEFAULT_SAVE_POLICY_PATH TEXT DEFAULT NULL;";
    create << "ALTER TABLE UI_SETTINGS"; // Table name
    create << " ADD DEFAULT_SAVE_DISPLAY_PATH TEXT DEFAULT NULL;";
    create << "ALTER TABLE UI_SETTINGS"; // Table name
    create << " ADD LAST_SAVE_POLICY_PATH TEXT DEFAULT NULL;";
    create << "ALTER TABLE UI_SETTINGS"; // Table name
    create << " ADD LAST_SAVE_DISPLAY_PATH TEXT DEFAULT NULL;";
    create << "ALTER TABLE UI_SETTINGS"; // Table name
    create << " ADD DEFAULT_LOAD_POLICY_PATH TEXT DEFAULT NULL;";
    create << "ALTER TABLE UI_SETTINGS"; // Table name
    create << " ADD DEFAULT_LOAD_DISPLAY_PATH TEXT DEFAULT NULL;";
    create << "ALTER TABLE UI_SETTINGS"; // Table name
    create << " ADD LAST_LOAD_POLICY_PATH TEXT DEFAULT NULL;";
    create << "ALTER TABLE UI_SETTINGS"; // Table name
    create << " ADD LAST_LOAD_DISPLAY_PATH TEXT DEFAULT NULL;";
    q = create.str();
}

//---------------------------------------------------------------------------
void DatabaseUi::get_sql_query_for_update_ui_settings_table_v9(std::string& q)
{
    std::stringstream create;
    create << "ALTER TABLE UI_SETTINGS"; // Table name
    create << " ADD MCO_TOKEN    TEXT DEFAULT NULL;";
    q = create.str();
}

//---------------------------------------------------------------------------
void DatabaseUi::get_sql_query_for_update_ui_table_v10(std::string& q)
{
    std::stringstream create;
    create << "ALTER TABLE UI"; // Table name
    create << " ADD OPTIONS TEXT DEFAULT NULL;";
    q = create.str();
}

}
