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
#include "Common/FileRegistered.h"
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// SQLLite
//***************************************************************************

int SQLLite::current_report_version = 1;
int SQLLite::current_ui_version     = 1;

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
    if (get_db_version(report_version) < 0)
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
    const char* end = NULL;
    int ret = 0;

#define UPDATE_REPORT_TABLE_FOR_VERSION(version)                                      \
    do                                                                                \
    {                                                                                 \
        end = NULL;                                                                   \
        if (report_version > version)                                                 \
            continue;                                                                 \
        get_sql_query_for_update_report_table_v##version(query);                      \
                                                                                      \
        ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end); \
        if (version != 0 && (ret != SQLITE_OK || !stmt || (end && *end)))             \
            return -1;                                                                \
        ret = execute();                                                              \
                                                                                      \
        if (version != 0 && ret < 0)                                                  \
            return ret;                                                               \
    } while(0);

    UPDATE_REPORT_TABLE_FOR_VERSION(0);

#undef UPDATE_REPORT_TABLE_FOR_VERSION

    return set_db_version(current_report_version);
}

int SQLLite::save_report(MediaConchLib::report reportKind, MediaConchLib::format format, const std::string& filename, const std::string& file_last_modification,
                         const std::string& report, MediaConchLib::compression compress)
{
    std::stringstream create;

    if (file_is_registered(reportKind, format, filename))
        return update_report(reportKind, format, filename, file_last_modification, report, compress);

    reports.clear();
    create << "INSERT INTO Report";
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

int SQLLite::update_report(MediaConchLib::report reportKind, MediaConchLib::format format, const std::string& filename, const std::string& file_last_modification,
                         const std::string& report, MediaConchLib::compression compress)
{
    std::stringstream create;

    reports.clear();
    create << "UPDATE Report ";
    create << "SET FILE_LAST_MODIFICATION = ?, REPORT = ?, COMPRESS = ? ";
    create << "WHERE FILENAME = ? AND TOOL = ? AND FORMAT = ?;";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, file_last_modification.c_str(), file_last_modification.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 2, report.c_str(), report.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 3, (int)compress);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 4, filename.c_str(), filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 5, (int)reportKind);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 6, (int)format);
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

bool SQLLite::file_is_registered(MediaConchLib::report reportKind, MediaConchLib::format format, const std::string& filename)
{
    std::stringstream create;
    std::string key("COUNT(REPORT)");

    reports.clear();
    create << "SELECT " << key << " FROM Report WHERE ";
    create << "FILENAME = ? ";
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

    ret = sqlite3_bind_int(stmt, 2, (int)reportKind);
    if (ret != SQLITE_OK)
        return false;

    ret = sqlite3_bind_int(stmt, 3, (int)format);
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
void SQLLite::get_element_report_kind(const std::string& file, MediaConchLib::report& report_kind)
{
    report_kind = MediaConchLib::report_MediaConch;
    reports.clear();
    query = "SELECT TOOL FROM Report WHERE FILENAME = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return;

    ret = sqlite3_bind_blob(stmt, 1, file.c_str(), file.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return;

    if (execute() || !reports.size())
        return;

    for (size_t i = 0; i < reports.size(); ++i)
    {
        if (reports[i].find("TOOL") != reports[i].end())
        {
            MediaConchLib::report tool_i = (MediaConchLib::report)std_string_to_int(reports[i]["TOOL"]);

            if (tool_i == MediaConchLib::report_MediaInfo || tool_i == MediaConchLib::report_MediaTrace)
                continue;
            report_kind = tool_i;
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

    ret = update_ui_table();
    return ret;
}

//---------------------------------------------------------------------------
int SQLLite::update_ui_table()
{
    const char* end = NULL;
    int ret = 0;

#define UPDATE_UI_TABLE_FOR_VERSION(version)                                          \
    do                                                                                \
    {                                                                                 \
        end = NULL;                                                                   \
        if (ui_version > version)                                                     \
            continue;                                                                 \
        get_sql_query_for_update_ui_table_v##version(query);                          \
                                                                                      \
        ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end); \
        if (version != 0 && (ret != SQLITE_OK || !stmt || (end && *end)))             \
            return -1;                                                                \
        ret = execute();                                                              \
                                                                                      \
        if (version != 0 && ret < 0)                                                  \
            return ret;                                                               \
    } while(0);

    UPDATE_UI_TABLE_FOR_VERSION(0);

#undef UPDATE_UI_TABLE_FOR_VERSION

    return set_db_version(current_ui_version);
}

//---------------------------------------------------------------------------
int SQLLite::init_ui()
{
    if (init() < 0)
        return -1;
    if (get_db_version(ui_version) < 0)
        return -1;
    create_ui_table();
    return 0;
}

//---------------------------------------------------------------------------
int SQLLite::ui_add_file(const FileRegistered* file)
{
    std::stringstream create;

    reports.clear();
    create << "INSERT INTO UI ";
    create << "(FILENAME, FILEPATH, POLICY, DISPLAY, ANALYZED, IMPLEMENTATION_VALID, POLICY_VALID, VERBOSITY)";
    create << " VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, file->filename.c_str(), file->filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 2, file->filepath.c_str(), file->filepath.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 3, file->policy);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 4, file->display);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 5, file->analyzed);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 6, file->implementation_valid);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 7, file->policy_valid);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 8, file->verbosity);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLite::ui_add_files(const std::vector<FileRegistered*>& files)
{
    std::stringstream create;

    reports.clear();
    create << "INSERT INTO UI ";
    create << "(FILENAME, FILEPATH, POLICY, DISPLAY, ANALYZED, IMPLEMENTATION_VALID, POLICY_VALID, VERBOSITY) VALUES";
    for (size_t i = 0; i < files.size(); ++i)
    {
        if (i)
            create << ",";
        create << " (?, ?, ?, ?, ?, ?, ?, ?)";
    }

    create << ";";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    int nb_column = 8;
    for (size_t i = 0; i < files.size(); ++i)
    {
        FileRegistered* file = files[i];
        ret = sqlite3_bind_blob(stmt, i * nb_column + 1, file->filename.c_str(), file->filename.length(), SQLITE_STATIC);
        if (ret != SQLITE_OK)
            return -1;

        ret = sqlite3_bind_blob(stmt, i * nb_column + 2, file->filepath.c_str(), file->filepath.length(), SQLITE_STATIC);
        if (ret != SQLITE_OK)
            return -1;

        ret = sqlite3_bind_int(stmt, i * nb_column + 3, file->policy);
        if (ret != SQLITE_OK)
            return -1;

        ret = sqlite3_bind_int(stmt, i * nb_column + 4, file->display);
        if (ret != SQLITE_OK)
            return -1;

        ret = sqlite3_bind_int(stmt, i * nb_column + 5, file->analyzed);
        if (ret != SQLITE_OK)
            return -1;

        ret = sqlite3_bind_int(stmt, i * nb_column + 6, file->implementation_valid);
        if (ret != SQLITE_OK)
            return -1;

        ret = sqlite3_bind_int(stmt, i * nb_column + 7, file->policy_valid);
        if (ret != SQLITE_OK)
            return -1;

        ret = sqlite3_bind_int(stmt, i * nb_column + 8, file->verbosity);
        if (ret != SQLITE_OK)
            return -1;
    }

    return execute();
}

//---------------------------------------------------------------------------
int SQLLite::ui_update_file(const FileRegistered* file)
{
    std::stringstream create;

    reports.clear();
    create << "UPDATE UI ";
    create << "SET POLICY           = ?, ";
    create << "DISPLAY              = ?, ";
    create << "ANALYZED             = ?, ";
    create << "IMPLEMENTATION_VALID = ?, ";
    create << "POLICY_VALID         = ?, ";
    create << "VERBOSITY            = ?  ";
    create << "WHERE FILENAME       = ?  ";
    create << "AND   FILEPATH       = ? ;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, file->policy);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, file->display);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 3, file->analyzed);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 4, file->implementation_valid);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 5, file->policy_valid);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 6, file->verbosity);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 7, file->filename.c_str(), file->filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 8, file->filepath.c_str(), file->filepath.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLite::ui_update_files(const std::vector<FileRegistered*>& files)
{
    std::stringstream create;

    reports.clear();
    create << "WITH Tmp(FILENAME, FILEPATH, POLICY, DISPLAY, ANALYZED, IMPLEMENTATION_VALID, POLICY_VALID, VERBOSITY)";
    create << "AS (VALUES";
    for (size_t i = 0; i < files.size(); ++i)
    {
        if (i)
            create << ", ";
        create << "(?, ?, ?, ?, ?, ?, ?, ?)";
    }
    create << ") ";

    create << "UPDATE UI SET ";
    create << "POLICY = (SELECT POLICY FROM Tmp WHERE UI.FILENAME = Tmp.FILENAME AND UI.FILEPATH = Tmp.FILEPATH), ";
    create << "DISPLAY = (SELECT DISPLAY FROM Tmp WHERE UI.FILENAME = Tmp.FILENAME AND UI.FILEPATH = Tmp.FILEPATH), ";
    create << "ANALYZED = (SELECT ANALYZED FROM Tmp WHERE UI.FILENAME = Tmp.FILENAME AND UI.FILEPATH = Tmp.FILEPATH), ";
    create << "IMPLEMENTATION_VALID = (SELECT IMPLEMENTATION_VALID FROM Tmp WHERE UI.FILENAME = Tmp.FILENAME AND UI.FILEPATH = Tmp.FILEPATH), ";
    create << "POLICY_VALID = (SELECT POLICY_VALID FROM Tmp WHERE UI.FILENAME = Tmp.FILENAME AND UI.FILEPATH = Tmp.FILEPATH), ";
    create << "VERBOSITY = (SELECT VERBOSITY FROM Tmp WHERE UI.FILENAME = Tmp.FILENAME AND UI.FILEPATH = Tmp.FILEPATH) ";

    create << " WHERE FILENAME IN (SELECT FILENAME FROM Tmp) AND  FILEPATH IN (SELECT FILEPATH FROM Tmp)";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);

    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    int nb_column = 8;
    for (size_t i = 0; i < files.size(); ++i)
    {
        FileRegistered* file = files[i];

        ret = sqlite3_bind_blob(stmt, i * nb_column + 1, file->filename.c_str(), file->filename.length(), SQLITE_STATIC);
        if (ret != SQLITE_OK)
            return -1;

        ret = sqlite3_bind_blob(stmt, i * nb_column + 2, file->filepath.c_str(), file->filepath.length(), SQLITE_STATIC);
        if (ret != SQLITE_OK)
            return -1;

        ret = sqlite3_bind_int(stmt, i * nb_column + 3, file->policy);
        if (ret != SQLITE_OK)
            return -1;

        ret = sqlite3_bind_int(stmt, i * nb_column + 4, file->display);
        if (ret != SQLITE_OK)
            return -1;

        ret = sqlite3_bind_int(stmt, i * nb_column + 5, file->analyzed);
        if (ret != SQLITE_OK)
            return -1;

        ret = sqlite3_bind_int(stmt, i * nb_column + 6, file->implementation_valid);
        if (ret != SQLITE_OK)
            return -1;

        ret = sqlite3_bind_int(stmt, i * nb_column + 7, file->policy_valid);
        if (ret != SQLITE_OK)
            return -1;

        ret = sqlite3_bind_int(stmt, i * nb_column + 8, file->verbosity);
        if (ret != SQLITE_OK)
            return -1;
    }

    return execute();
}

//---------------------------------------------------------------------------
int SQLLite::ui_get_file(FileRegistered* file)
{
    std::stringstream create;

    std::string p("POLICY");
    std::string d("DISPLAY");
    std::string a("ANALYZED");
    std::string i_v("IMPLEMENTATION_VALID");
    std::string p_v("POLICY_VALID");
    std::string v("VERBOSITY");

    reports.clear();
    create << "SELECT " << p << ", " << d << ", " << a << ", " << i_v << ", " << p_v << ", " << v << " FROM UI ";
    create << "WHERE FILENAME = ? AND FILEPATH = ?;";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, file->filename.c_str(), file->filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 2, file->filepath.c_str(), file->filepath.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || !reports.size())
        return -1;

    std::map<std::string, std::string> r = reports[0];

    if (r.find(p) != r.end())
        file->policy = std_string_to_int(r[p]);
    if (r.find(d) != r.end())
        file->display = std_string_to_int(r[d]);
    if (r.find(a) != r.end())
        file->analyzed = std_string_to_int(r[a]);
    if (r.find(i_v) != r.end())
        file->implementation_valid = std_string_to_int(r[i_v]);
    if (r.find(p_v) != r.end())
        file->policy_valid = std_string_to_int(r[p_v]);
    if (r.find(v) != r.end())
        file->verbosity = std_string_to_int(r[v]);

    return 0;
}

//---------------------------------------------------------------------------
int SQLLite::ui_remove_file(const FileRegistered* file)
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

    ret = sqlite3_bind_blob(stmt, 1, file->filename.c_str(), file->filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 2, file->filepath.c_str(), file->filepath.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLite::ui_remove_files(const std::vector<FileRegistered*>& files)
{
    std::stringstream create;

    reports.clear();
    create << "DELETE FROM UI WHERE";
    for (size_t i = 0; i < files.size(); ++i)
    {
        if (i)
            create << " OR ";
        create << " FILENAME = ? AND FILEPATH = ?";
    }
    create << ";";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    for (size_t i = 0; i < files.size(); ++i)
    {
        FileRegistered* file = files[i];
        ret = sqlite3_bind_blob(stmt, i * 2 + 1, file->filename.c_str(), file->filename.length(), SQLITE_STATIC);
        if (ret != SQLITE_OK)
            return -1;

        ret = sqlite3_bind_blob(stmt, i * 2 + 2, file->filepath.c_str(), file->filepath.length(), SQLITE_STATIC);
        if (ret != SQLITE_OK)
            return -1;
    }

    return execute();
}

//---------------------------------------------------------------------------
int SQLLite::ui_remove_all_files()
{
    std::stringstream create;

    reports.clear();
    create << "DELETE FROM UI ";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
void SQLLite::ui_get_elements(std::vector<FileRegistered*>& vec)
{
    std::stringstream create;
    std::string name("FILENAME");
    std::string path("FILEPATH");
    std::string p("POLICY");
    std::string d("DISPLAY");
    std::string a("ANALYZED");
    std::string i_v("IMPLEMENTATION_VALID");
    std::string p_v("POLICY_VALID");
    std::string v("VERBOSITY");

    reports.clear();
    create << "SELECT " << name << ", " << path << ", " << p << ", " << d;
    create << ", " << a << ", " << i_v << ", " << p_v << ", " << v;
    create << " FROM UI;";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return;

    if (execute() || !reports.size())
        return;

    for (size_t i = 0; i < reports.size(); ++i)
    {
        if (reports[i].find(name) != reports[i].end())
        {
            FileRegistered* file = new FileRegistered;
            file->filename = reports[i][name];
            if (reports[i].find(path) != reports[i].end())
                file->filepath = reports[i][path];
            if (reports[i].find(p) != reports[i].end())
                file->policy = std_string_to_int(reports[i][p]);
            if (reports[i].find(d) != reports[i].end())
                file->display = std_string_to_int(reports[i][d]);
            if (reports[i].find(a) != reports[i].end())
                file->analyzed = std_string_to_int(reports[i][a]);
            if (reports[i].find(i_v) != reports[i].end())
                file->implementation_valid = std_string_to_int(reports[i][i_v]);
            if (reports[i].find(p_v) != reports[i].end())
                file->policy_valid = std_string_to_int(reports[i][p_v]);
            if (reports[i].find(v) != reports[i].end())
                file->verbosity = std_string_to_int(reports[i][v]);
            if (file->analyzed)
                file->need_update = false;
            vec.push_back(file);
        }
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

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    if (execute() < 0)
        return -1;

    return 0;
}

}

#endif
