/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifdef HAVE_SQLITE

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "SQLLite.h"
#include <sstream>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// SQLLite
//***************************************************************************

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
SQLLite::SQLLite() : Database()
{
    db = NULL;
}

//---------------------------------------------------------------------------
SQLLite::~SQLLite()
{
    if (db)
        sqlite3_close(db);
}

//---------------------------------------------------------------------------
int SQLLite::init()
{
    std::string database(db_file.begin(), db_file.end());

    int ret = sqlite3_open(database.c_str(), &db);
    if (ret)
    {
        std::stringstream err("Error to open the DB: ");
        err << sqlite3_errmsg(db);
        errors.push_back(err.str());

        sqlite3_close(db);
        db = NULL;
        return -1;
    }
    create_report_table();
    return 0;
}

//---------------------------------------------------------------------------
int SQLLite::create_report_table()
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

int SQLLite::save_report(MediaConchLib::report reportKind, MediaConchLib::format format, const std::string& filename, const std::string& file_last_modification,
                          const std::string& report)
{
    std::stringstream create;

    reports.clear();
    create << "INSERT INTO " << "Report";
    create << " (FILENAME, FILE_LAST_MODIFICATION, TOOL, FORMAT, REPORT)";
    create << " VALUES ('" << filename << "', '";
    create << file_last_modification << "', ";
    create << reportKind << ", ";
    create << format << ", ";
    create << "'" << report << "');";
    query = create.str();
    return execute();
}

std::string SQLLite::get_report(MediaConchLib::report reportKind, MediaConchLib::format format, const std::string& filename, const std::string& file_last_modification)
{
    std::stringstream create;
    std::string key("REPORT");

    reports.clear();
    create << "SELECT " << key << " FROM " << "Report" << " WHERE ";
    create << "FILENAME = '" << filename << "' ";
    create << "AND FILE_LAST_MODIFICATION = '" << file_last_modification << "' ";
    create << "AND TOOL = " << reportKind << " ";
    create << "AND FORMAT = " << format << ";";
    query = create.str();

    if (execute() || !reports.size() || reports.find(key) == reports.end())
        return std::string();
    return reports[key];
}

bool SQLLite::file_is_registered(MediaConchLib::report reportKind, MediaConchLib::format format, const std::string& filename, const std::string& file_last_modification)
{
    std::stringstream create;
    std::string key("COUNT(REPORT)");

    reports.clear();
    create << "SELECT " << key << " FROM " << "Report" << " WHERE ";
    create << "FILENAME = '" << filename << "' ";
    create << "AND FILE_LAST_MODIFICATION = '" << file_last_modification << "' ";
    create << "AND TOOL = " << reportKind << " ";
    create << "AND FORMAT = " << format << ";";
    query = create.str();

    if (execute() || !reports.size() || reports.find(key) == reports.end())
        return false;

    if (reports[key] == "0")
        return false;
    return true;
}

//---------------------------------------------------------------------------
int SQLLite::execute()
{
    if (!db)
        return -1;

    errors.clear();
    char *error = NULL;

    int ret = sqlite3_exec(db, query.c_str(), callback, this, &error);
    if (ret != SQLITE_OK)
    {
        errors.push_back(std::string(error));
        sqlite3_free(error); //copy
    }

    return ret;
}

//---------------------------------------------------------------------------
int SQLLite::callback(void* data, int columns, char **column_texts, char** column_name)
{
    SQLLite *ptr = (SQLLite*)data;

    for (int i = 0; i < columns; ++i)
        ptr->add_report(column_name[i], column_texts[i]);
    return 0;
}

//---------------------------------------------------------------------------
void SQLLite::add_report(std::string key, std::string report)
{
    reports[key] = report;
}

}

#endif
