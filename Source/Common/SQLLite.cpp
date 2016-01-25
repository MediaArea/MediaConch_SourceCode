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
#include <stdio.h>
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
    stmt = NULL;
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
    int ret = sqlite3_open(db_file.c_str(), &db);
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
    get_sql_query_for_create_report_table(query);

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;
    ret = execute();
    if (ret < 0)
        return -1;

    return update_report_table();
}

//---------------------------------------------------------------------------
int SQLLite::update_report_table()
{
    get_sql_query_for_update_report_table(query);

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;
    return execute();
}

int SQLLite::save_report(MediaConchLib::report reportKind, MediaConchLib::format format, const std::string& filename, const std::string& file_last_modification,
                         const std::string& report, MediaConchLib::compression compress)
{
    std::stringstream create;

    reports.clear();
    create << "INSERT INTO " << "Report";
    create << " (FILENAME, FILE_LAST_MODIFICATION, TOOL, FORMAT, REPORT, COMPRESS)";
    create << " VALUES (?, ?, ?, ?, ?, ?);";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, filename.c_str(), filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 2, file_last_modification.c_str(), file_last_modification.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 3, (int)reportKind);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 4, (int)format);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 5, report.c_str(), report.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 6, (int)compress);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

int SQLLite::remove_report(const std::string& filename)
{
    std::stringstream delete_query;

    reports.clear();
    query = "DELETE FROM Report WHERE FILENAME = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, filename.c_str(), filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

void SQLLite::get_report(MediaConchLib::report reportKind, MediaConchLib::format format,
                         const std::string& filename, const std::string& file_last_modification,
                         std::string& report, MediaConchLib::compression& compress)
{
    std::stringstream create;

    reports.clear();
    create << "SELECT REPORT, COMPRESS FROM " << "Report" << " WHERE ";
    create << "FILENAME = ? ";
    create << "AND FILE_LAST_MODIFICATION = ? ";
    create << "AND TOOL = ? ";
    create << "AND FORMAT = ?;";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return;

    ret = sqlite3_bind_blob(stmt, 1, filename.c_str(), filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return;

    ret = sqlite3_bind_blob(stmt, 2, file_last_modification.c_str(), file_last_modification.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return;

    ret = sqlite3_bind_int(stmt, 3, (int)reportKind);
    if (ret != SQLITE_OK)
        return;

    ret = sqlite3_bind_int(stmt, 4, (int)format);
    if (ret != SQLITE_OK)
        return;

    if (execute() || !reports.size() || reports.find("REPORT") == reports.end())
        return;

    if (reports.find("COMPRESS") != reports.end())
    {
        const std::string& c = reports["COMPRESS"];
        if (c == "0")
            compress = MediaConchLib::compression_None;
        else if (c == "1")
            compress = MediaConchLib::compression_ZLib;
        else
            compress = MediaConchLib::compression_None;
    }
    report += reports["REPORT"];
}

bool SQLLite::file_is_registered(MediaConchLib::report reportKind, MediaConchLib::format format, const std::string& filename, const std::string& file_last_modification)
{
    std::stringstream create;
    std::string key("COUNT(REPORT)");

    reports.clear();
    create << "SELECT " << key << " FROM " << "Report" << " WHERE ";
    create << "FILENAME = ? ";
    create << "AND FILE_LAST_MODIFICATION = ? ";
    create << "AND TOOL = ? ";
    create << "AND FORMAT = ?;";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return false;

    ret = sqlite3_bind_blob(stmt, 1, filename.c_str(), filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return false;

    ret = sqlite3_bind_blob(stmt, 2, file_last_modification.c_str(), file_last_modification.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return false;

    ret = sqlite3_bind_int(stmt, 3, (int)reportKind);
    if (ret != SQLITE_OK)
        return false;

    ret = sqlite3_bind_int(stmt, 4, (int)format);
    if (ret != SQLITE_OK)
        return false;

    if (execute() || !reports.size() || reports.find(key) == reports.end())
        return false;

    if (reports[key] == "0")
        return false;
    return true;
}

//---------------------------------------------------------------------------
int SQLLite::execute()
{
    if (!db || !stmt)
        return -1;

    errors.clear();
    int ret;

    while (1)
    {
        ret = sqlite3_step(stmt);
        if (ret == SQLITE_DONE)
            break;
        else if (ret == SQLITE_ROW)
        {
            for (int i = 0; i < sqlite3_column_count(stmt); ++i)
            {
                std::string name(sqlite3_column_name(stmt, i));
                const void *blob = sqlite3_column_blob(stmt, i);
                std::string value((const char *)blob, sqlite3_column_bytes(stmt, i));
                add_report(name, value);
            }
        }
        else
        {
            ret = sqlite3_reset(stmt);
            #if SQLITE_VERSION_NUMBER >= 3007015
            errors.push_back(sqlite3_errstr(ret));
            #else
            errors.push_back("Error occurs during execution");
            #endif
            break;
        }
    }

    sqlite3_finalize(stmt);
    stmt = NULL;
    if(ret == SQLITE_DONE)
        return 0;
    return -1;
}

//---------------------------------------------------------------------------
void SQLLite::add_report(const std::string& key, const std::string& report)
{
    reports[key] = report;
}

}

#endif
