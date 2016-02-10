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
    std::string db_file(db_dirname + db_filename);
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
    return 0;
}

//---------------------------------------------------------------------------
int SQLLite::init_report()
{
    if (init() < 0)
        return -1;
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

    if (execute() < 0 || !reports.size())
        return;

    std::map<std::string, std::string> r = reports[0];

    if (r.find("REPORT") == r.end())
        return;

    if (r.find("COMPRESS") != r.end())
    {
        const std::string& c = r["COMPRESS"];
        if (c == "0")
            compress = MediaConchLib::compression_None;
        else if (c == "1")
            compress = MediaConchLib::compression_ZLib;
        else
            compress = MediaConchLib::compression_None;
    }
    report += r["REPORT"];
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

    if (execute() || !reports.size() || reports[0].find(key) == reports[0].end())
        return false;

    if (reports[0][key] == "0")
        return false;
    return true;
}

void SQLLite::get_elements(std::vector<std::string>& vec)
{
    std::stringstream create;
    std::string key("FILENAME");

    reports.clear();
    create << "SELECT " << key << " FROM " << "Report;";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return;

    if (execute() || !reports.size())
        return;

    for (size_t i = 0; i < reports.size(); ++i)
    {
        if (!(i % 3)) // 3 kinds of reports
        {
            if (reports[i].find(key) != reports[i].end())
                vec.push_back(reports[i][key]);
        }
    }
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
int SQLLite::create_ui_table()
{
    get_sql_query_for_create_ui_table(query);

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
int SQLLite::update_ui_table()
{
    get_sql_query_for_update_ui_table(query);

    if (!query.length())
        return 0;

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;
    return execute();
}

//---------------------------------------------------------------------------
int SQLLite::init_ui()
{
    if (init() < 0)
        return -1;
    create_ui_table();
    return 0;
}

//---------------------------------------------------------------------------
int SQLLite::ui_add_file(const std::string& filename, const std::string& filepath,
                         int policy, int display, bool analyzed,
                         bool implementation_valid, bool policy_valid)
{
    std::stringstream create;

    reports.clear();
    create << "INSERT INTO UI ";
    create << "(FILENAME, FILEPATH, POLICY, DISPLAY, ANALYZED, IMPLEMENTATION_VALID, POLICY_VALID)";
    create << " VALUES (?, ?, ?, ?, ?, ?, ?);";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, filename.c_str(), filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 2, filepath.c_str(), filepath.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 3, policy);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 4, display);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 5, analyzed);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 6, implementation_valid);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 7, policy_valid);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLite::ui_update_file(const std::string& filename, const std::string& filepath,
                            int policy, int display, bool analyzed,
                            bool implementation_valid, bool policy_valid)
{
    std::stringstream create;

    reports.clear();
    create << "UPDATE UI ";
    create << "SET POLICY           = ?, ";
    create << "DISPLAY              = ?, ";
    create << "ANALYZED             = ?, ";
    create << "IMPLEMENTATION_VALID = ?, ";
    create << "POLICY_VALID         = ?  ";
    create << "WHERE FILENAME       = ?  ";
    create << "AND   FILEPATH       = ? ;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, policy);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, display);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 3, analyzed);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 4, implementation_valid);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 5, policy_valid);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 6, filename.c_str(), filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 7, filepath.c_str(), filepath.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLite::ui_get_file(const std::string& filename, const std::string& filepath,
                         int& policy, int& display, bool& analyzed,
                         bool& implementation_valid, bool& policy_valid)
{
    std::stringstream create;

    std::string p("POLICY");
    std::string d("DISPLAY");
    std::string a("ANALYZED");
    std::string i_v("IMPLEMENTATION_VALID");
    std::string p_v("POLICY_VALID");

    reports.clear();
    create << "SELECT " << p << ", " << d << ", " << a << ", " << i_v << ", " << p_v << " FROM UI ";
    create << "WHERE FILENAME = ? AND FILEPATH = ?;";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, filename.c_str(), filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 2, filepath.c_str(), filepath.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || !reports.size())
        return -1;

    std::map<std::string, std::string> r = reports[0];

    if (r.find(p) != r.end())
        policy = std_string_to_int(r[p]);
    if (r.find(d) != r.end())
        display = std_string_to_int(r[d]);
    if (r.find(a) != r.end())
        analyzed = std_string_to_int(r[a]);
    if (r.find(i_v) != r.end())
        implementation_valid = std_string_to_int(r[i_v]);
    if (r.find(p_v) != r.end())
        policy_valid = std_string_to_int(r[p_v]);

    return 0;
}

//---------------------------------------------------------------------------
int SQLLite::ui_remove_file(const std::string& filename, const std::string& filepath)
{
    std::stringstream create;

    reports.clear();
    create << "DELETE FROM UI ";
    create << "WHERE FILENAME = ? AND FILEPATH = ?;";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, filename.c_str(), filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 2, filepath.c_str(), filepath.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
void SQLLite::ui_get_elements(std::vector<std::pair<std::string, std::string> >& vec)
{
    std::stringstream create;
    std::string name("FILENAME");
    std::string path("FILEPATH");

    reports.clear();
    create << "SELECT " << name << ", " << path << " FROM UI;";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return;

    if (execute() || !reports.size())
        return;

    for (size_t i = 0; i < reports.size(); ++i)
    {
        if (reports[i].find(name) != reports[i].end() && reports[i].find(path) != reports[i].end())
            vec.push_back(std::make_pair(reports[i][name], reports[i][path]));
    }
}

//---------------------------------------------------------------------------
int SQLLite::std_string_to_int(const std::string& str)
{
    int val;
    char *end = NULL;

    val = strtol(str.c_str(), &end, 10);
    // if (!end || *end != '\0')
    //     error;
    return val;
}

}

#endif
