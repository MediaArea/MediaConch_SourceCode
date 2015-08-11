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
int Database::create_report_table()
{
    std::stringstream create;
    create << "CREATE TABLE IF NOT EXISTS " << "Report"; //Table name
    create << "(";
    create << "FILENAME               TEXT NOT NULL,";
    create << "FILE_LAST_MODIFICATION INT  NOT NULL,";
    create << "TOOL                   INT  NOT NULL,";
    create << "FORMAT                 INT  NOT NULL,";
    create << "REPORT                 TEXT          ";
    create << ");";

    query = create.str();
    return execute();
}

int Database::save_report(Core::report reportKind, Core::format format, const std::string& filename, time_t file_last_modification,
                          const std::string& report)
{
    std::stringstream create;

    reports.clear();
    create << "INSERT INTO " << "Report";
    create << " (FILENAME, FILE_LAST_MODIFICATION, TOOL, FORMAT, REPORT)";
    create << " VALUES ('" << filename << "', ";
    create << file_last_modification << ", ";
    create << reportKind << ", ";
    create << format << ", ";
    create << "'" << report << "');";
    query = create.str();
    return execute();
}

std::string Database::get_report(Core::report reportKind, Core::format format, const std::string& filename, time_t file_last_modification)
{
    std::stringstream create;
    std::string key("REPORT");

    reports.clear();
    create << "SELECT " << key << " FROM " << "Report" << " WHERE ";
    create << "FILENAME = '" << filename << "' ";
    create << "AND FILE_LAST_MODIFICATION = " << file_last_modification << " ";
    create << "AND TOOL = " << reportKind << " ";
    create << "AND FORMAT = " << format << ";";
    query = create.str();

    if (execute() || !reports.size() || reports.find(key) == reports.end())
        return std::string();
    return reports[key];
}

bool Database::file_is_registered(const std::string& filename, time_t file_last_modification)
{
    std::stringstream create;
    std::string key("COUNT(REPORT)");

    reports.clear();
    create << "SELECT " << key << " FROM " << "Report" << " WHERE ";
    create << "FILENAME = '" << filename << "' ";
    create << "AND FILE_LAST_MODIFICATION = " << file_last_modification << ";";
    query = create.str();

    if (execute() || !reports.size() || reports.find(key) == reports.end())
        return false;

    if (reports[key] == "0")
        return false;
    return true;
}

//---------------------------------------------------------------------------
void Database::set_database_directory(const std::string& dirname)
{
    db_file = dirname + Path_Separator + databaseName;
}

}
