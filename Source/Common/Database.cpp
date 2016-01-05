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
#include "Database.h"
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
Database::Database()
{
    db_file = databaseName;
}

//---------------------------------------------------------------------------
Database::~Database()
{
}

//---------------------------------------------------------------------------
void Database::set_database_directory(const std::string& dirname)
{
    db_file = dirname + databaseName;
}

//---------------------------------------------------------------------------
void Database::get_sql_query_for_create_report_table(std::string& q)
{
    std::stringstream create;
    create << "CREATE TABLE IF NOT EXISTS Report "; // Table name
    create << "(FILENAME              TEXT NOT NULL,";
    create << "FILE_LAST_MODIFICATION INT  NOT NULL,";
    create << "TOOL                   INT  NOT NULL,";
    create << "FORMAT                 INT  NOT NULL,";
    create << "REPORT                 TEXT         );";
    q = create.str();
}

//---------------------------------------------------------------------------
void Database::get_sql_query_for_update_report_table(std::string& q)
{
    std::stringstream create;
    create << "ALTER TABLE Report "; // Table name
    create << "ADD COMPRESS INT DEFAULT 0 NOT NULL;";
    q = create.str();
}

}
