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
}

Database::~Database()
{
}

int Database::create_report_table()
{
    wstringstream create;
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

int Database::save_report(Core::format format, Core::report reportKind, String& filename, time_t file_last_modification,
                          String& report)
{
    wstringstream create;

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

String Database::report_exists(Core::format format, Core::report reportKind, String& filename, time_t file_last_modification)
{
    wstringstream create;

    reports.clear();
    create << "SELECT report FROM " << "Report" << " WHERE ";
    create << "FILENAME = '" << filename << "' ";
    create << "AND FILE_LAST_MODIFICATION = " << file_last_modification << " ";
    create << "AND TOOL = " << reportKind << " ";
    create << "AND FORMAT = " << format << ";";
    query = create.str();

    if (execute() || !reports.size())
        return String();
    return reports[0];
}

}
