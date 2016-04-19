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

}
