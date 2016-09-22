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
#include "SQLLiteReport.h"
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// SQLLiteReport
//***************************************************************************

int SQLLiteReport::current_report_version = 4;

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
SQLLiteReport::SQLLiteReport() : DatabaseReport(), SQLLite()
{
}

//---------------------------------------------------------------------------
SQLLiteReport::~SQLLiteReport()
{
}

//---------------------------------------------------------------------------
int SQLLiteReport::init()
{
    return SQLLite::init(db_dirname, db_filename);
}

//---------------------------------------------------------------------------
int SQLLiteReport::init_report()
{
    if (init() < 0)
        return -1;
    if (get_db_version(report_version) < 0)
        return -1;
    return create_report_table();
}

//---------------------------------------------------------------------------
int SQLLiteReport::create_report_table()
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
int SQLLiteReport::update_report_table()
{
    const char* end = NULL;
    int ret = 0;

#define UPDATE_REPORT_TABLE_FOR_VERSION(version)                        \
    do                                                                  \
    {                                                                   \
        end = NULL;                                                     \
        if (report_version > version)                                   \
            continue;                                                   \
        get_sql_query_for_update_report_table_v##version(query);        \
        if (!query.length())                                            \
            continue;                                                   \
                                                                        \
        const char* q = query.c_str();                                  \
        end = NULL;                                                     \
        while (1)                                                       \
        {                                                               \
            ret = sqlite3_prepare_v2(db, q, -1, &stmt, &end);           \
            if (version > 1 && (ret != SQLITE_OK || !stmt))             \
                return -1;                                              \
            ret = execute();                                            \
                                                                        \
            if (version > 1 && ret < 0)                                 \
                return ret;                                             \
            if (!end || !*end)                                          \
                break;                                                  \
                                                                        \
            q = end;                                                    \
        }                                                               \
                                                                        \
    } while(0);

    UPDATE_REPORT_TABLE_FOR_VERSION(0);
    UPDATE_REPORT_TABLE_FOR_VERSION(1);
    UPDATE_REPORT_TABLE_FOR_VERSION(2);
    UPDATE_REPORT_TABLE_FOR_VERSION(3);

#undef UPDATE_REPORT_TABLE_FOR_VERSION

    return set_db_version(current_report_version);
}

long SQLLiteReport::add_file(const std::string& filename, const std::string& file_last_modification,
                             std::string& err,
                             long source_id,
                             const std::string& generated_file, const std::string& log)
{
    long id = get_file_id(filename, file_last_modification);
    if (id >= 0)
        return id;

    std::stringstream create;

    reports.clear();
    create << "INSERT INTO MEDIACONCH_FILE";
    create << " (FILENAME, FILE_LAST_MODIFICATION, SOURCE_FILE_ID, GENERATED_FILE, LOG)";
    create << " VALUES (?, ?, ?, ?, ?);";
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

    ret = sqlite3_bind_int(stmt, 3, source_id);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 4, generated_file.c_str(), generated_file.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 5, log.c_str(), log.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() < 0)
    {
        err = error;
        return -1;
    }

    id = get_file_id(filename, file_last_modification);
    return id;
}

long SQLLiteReport::get_file_id(const std::string& filename, const std::string& file_last_modification)
{
    std::stringstream create;
    std::string key("ID");

    reports.clear();
    create << "SELECT " << key << " FROM MEDIACONCH_FILE";
    create << " WHERE FILENAME = ?";
    if (file_last_modification.size())
        create << " AND FILE_LAST_MODIFICATION = ?";
    create << ";";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, filename.c_str(), filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    if (file_last_modification.size())
    {
        ret = sqlite3_bind_blob(stmt, 2, file_last_modification.c_str(), file_last_modification.length(), SQLITE_STATIC);
        if (ret != SQLITE_OK)
            return -1;
    }

    if (execute() || !reports.size() || reports[0].find(key) == reports[0].end())
        return -1;

    return std_string_to_int(reports[0][key]);
}

//---------------------------------------------------------------------------
void SQLLiteReport::get_file_from_id(long id, std::string& file)
{
    std::stringstream create;
    std::string key("FILENAME");

    reports.clear();
    create << "SELECT " << key << " FROM MEDIACONCH_FILE";
    create << " WHERE ID = ?;";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return;

    ret = sqlite3_bind_int(stmt, 1, id);
    if (ret != SQLITE_OK)
        return;

    if (execute() || !reports.size() || reports[0].find(key) == reports[0].end())
        return;

    file =reports[0][key];
}

int SQLLiteReport::save_report(long file_id, MediaConchLib::report reportKind, MediaConchLib::format format,
                               const std::string& report, MediaConchLib::compression compress,
                               int mil_version)
{
    std::stringstream create;

    if (report_is_registered(file_id, reportKind, format))
        return update_report(file_id, reportKind, format, report, compress, mil_version);

    reports.clear();
    create << "INSERT INTO MEDIACONCH_REPORT";
    create << " (FILE_ID, TOOL, FORMAT, REPORT, COMPRESS, MIL_VERSION)";
    create << " VALUES (?, ?, ?, ?, ?, ?);";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, file_id);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, (int)reportKind);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 3, (int)format);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 4, report.c_str(), report.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 5, (int)compress);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 6, mil_version);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

int SQLLiteReport::update_report(long file_id, MediaConchLib::report reportKind, MediaConchLib::format format,
                                 const std::string& report, MediaConchLib::compression compress,
                                 int mil_version)
{
    std::stringstream create;

    reports.clear();
    create << "UPDATE MEDIACONCH_REPORT ";
    create << "REPORT = ?, COMPRESS = ?, MIL_VERSION = ? ";
    create << "WHERE FILE_ID = ? AND TOOL = ? AND FORMAT = ?;";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, report.c_str(), report.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, (int)compress);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 3, mil_version);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 4, file_id);
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

int SQLLiteReport::remove_report(long file_id)
{
    std::stringstream delete_query;

    reports.clear();
    query = "DELETE FROM MEDIACONCH_REPORT WHERE FILE_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, file_id);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

void SQLLiteReport::get_report(long file_id, MediaConchLib::report reportKind, MediaConchLib::format format,
                               std::string& report, MediaConchLib::compression& compress)
{
    std::stringstream create;

    reports.clear();
    create << "SELECT REPORT, COMPRESS FROM MEDIACONCH_REPORT WHERE ";
    create << "FILE_ID = ? ";
    create << "AND TOOL = ? ";
    create << "AND FORMAT = ?;";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return;

    ret = sqlite3_bind_int(stmt, 1, file_id);
    if (ret != SQLITE_OK)
        return;

    ret = sqlite3_bind_int(stmt, 2, (int)reportKind);
    if (ret != SQLITE_OK)
        return;

    ret = sqlite3_bind_int(stmt, 3, (int)format);
    if (ret != SQLITE_OK)
        return;

    if (execute() < 0 || !reports.size())
        return;

    std::map<std::string, std::string> r = reports[0];

    if (r.find("REPORT") == r.end())
        return;
    report += r["REPORT"];

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
}

bool SQLLiteReport::report_is_registered(long file_id, MediaConchLib::report reportKind, MediaConchLib::format format)
{
    std::stringstream create;
    std::string key("COUNT(REPORT)");

    reports.clear();
    create << "SELECT " << key << " FROM MEDIACONCH_REPORT WHERE ";
    create << "FILE_ID = ? ";
    create << "AND TOOL = ? ";
    create << "AND FORMAT = ?;";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return false;

    ret = sqlite3_bind_int(stmt, 1, file_id);
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

int SQLLiteReport::version_registered(long file_id)
{
    std::stringstream create;
    std::string key("MIL_VERSION");

    reports.clear();
    create << "SELECT " << key << " FROM MEDIACONCH_REPORT WHERE ";
    create << "FILE_ID = ?;";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, file_id);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || !reports.size() || reports[0].find(key) == reports[0].end())
        return -1;

    return std_string_to_int(reports[0][key]);
}

void SQLLiteReport::get_elements(std::vector<std::string>& vec)
{
    std::stringstream create;
    std::string key("FILENAME");

    reports.clear();
    create << "SELECT " << key << " FROM MEDIACONCH_FILE;";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return;

    if (execute() || !reports.size())
        return;

    for (size_t i = 0; i < reports.size(); ++i)
    {
        if (reports[i].find(key) != reports[i].end())
            vec.push_back(reports[i][key]);
    }
}

//---------------------------------------------------------------------------
void SQLLiteReport::get_element_report_kind(long file_id, MediaConchLib::report& report_kind)
{
    report_kind = MediaConchLib::report_MediaConch;
    reports.clear();
    query = "SELECT TOOL FROM MEDIACONCH_REPORT WHERE FILE_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return;

    ret = sqlite3_bind_int(stmt, 1, file_id);
    if (ret != SQLITE_OK)
        return;

    if (execute() || !reports.size())
        return;

    for (size_t i = 0; i < reports.size(); ++i)
    {
        if (reports[i].find("TOOL") != reports[i].end())
        {
            MediaConchLib::report tool_i = (MediaConchLib::report)std_string_to_int(reports[i]["TOOL"]);

            if (tool_i == MediaConchLib::report_MediaInfo || tool_i == MediaConchLib::report_MediaTrace || tool_i == MediaConchLib::report_MicroMediaTrace)
                continue;
            report_kind = tool_i;
        }
    }
}

}

#endif
