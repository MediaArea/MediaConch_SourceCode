/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
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
#include <stdlib.h>
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
SQLLite::SQLLite()
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
int SQLLite::init(const std::string& db_dirname, const std::string& db_filename)
{
    std::string db_file(db_dirname + db_filename);
    int ret = sqlite3_open(db_file.c_str(), &db);
    if (ret)
    {
        std::stringstream err("Error to open the DB: ");
        err << sqlite3_errmsg(db);
        error = err.str();

        sqlite3_close(db);
        db = NULL;
        return -1;
    }
    return 0;
}

//---------------------------------------------------------------------------
int SQLLite::execute()
{
    if (!db || !stmt)
        return -1;

    error.clear();
    int ret;

    while (1)
    {
        ret = sqlite3_step(stmt);
        if (ret == SQLITE_DONE)
            break;
        else if (ret == SQLITE_ROW)
        {
            std::map<std::string, std::string> tmp;
            for (int i = 0; i < sqlite3_column_count(stmt); ++i)
            {
                std::string name(sqlite3_column_name(stmt, i));
                const void *blob = sqlite3_column_blob(stmt, i);
                std::string value((const char *)blob, sqlite3_column_bytes(stmt, i));
                tmp[name] = value;
            }
            reports.push_back(tmp);
        }
        else
        {
            ret = sqlite3_reset(stmt);
            error = get_sqlite_error(ret);
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
long SQLLite::std_string_to_int(const std::string& str)
{
    long val;
    char *end = NULL;

    val = strtol(str.c_str(), &end, 10);
    // if (!end || *end != '\0')
    //     error;
    return val;
}

//---------------------------------------------------------------------------
size_t SQLLite::std_string_to_uint(const std::string& str)
{
    size_t val;
    char *end = NULL;

    val = strtoul(str.c_str(), &end, 10);
    // if (!end || *end != '\0')
    //     error;
    return val;
}

//---------------------------------------------------------------------------
unsigned long long SQLLite::std_string_to_ullong(const std::string& str)
{
    unsigned long long val;
    char *end = NULL;

    val = strtoull(str.c_str(), &end, 10);
    // if (!end || *end != '\0')
    //     error;
    return val;
}


//---------------------------------------------------------------------------
int SQLLite::get_db_version(int& version)
{
    reports.clear();
    query = std::string("PRAGMA user_version;");

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    if (execute() < 0 || reports.size() != 1 || reports[0].find("user_version") == reports[0].end())
        return -1;

    version = std_string_to_int(reports[0]["user_version"]);
    return 0;
}

//---------------------------------------------------------------------------
int SQLLite::set_db_version(int version)
{
    reports.clear();

    std::stringstream create;
    create << "PRAGMA user_version=" << version << ";";
    query = create.str();

    std::string err;
    if (prepare_v2(query, err) < 0)
        return -1;

    if (execute() < 0)
        return -1;

    return 0;
}

//---------------------------------------------------------------------------
const std::string& SQLLite::get_error() const
{
    return error;
}

//---------------------------------------------------------------------------
std::string SQLLite::get_sqlite_error(int err)
{
#if SQLITE_VERSION_NUMBER >= 3007015
    return sqlite3_errstr(err);
#else
    return "An error occurs during execution";
#endif
}

//---------------------------------------------------------------------------
int SQLLite::prepare_v2(std::string& query, std::string& err)
{
    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
    {
        if (ret != SQLITE_OK)
            err = get_sqlite_error(ret);
        else
            err = "Internal error when creating SQLLite query.";
        return -1;
    }

    return 0;
}

}

#endif
