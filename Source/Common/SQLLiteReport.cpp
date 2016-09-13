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

int SQLLiteReport::current_report_version = 3;

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
    create_report_table();
    return 0;
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

#define UPDATE_REPORT_TABLE_FOR_VERSION(version)                                      \
    do                                                                                \
    {                                                                                 \
        end = NULL;                                                                   \
        if (report_version > version)                                                 \
            continue;                                                                 \
        get_sql_query_for_update_report_table_v##version(query);                      \
                                                                                      \
        ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end); \
        if (version > 1 && (ret != SQLITE_OK || !stmt || (end && *end)))              \
            return -1;                                                                \
        ret = execute();                                                              \
                                                                                      \
        if (version > 1 && ret < 0)                                                   \
            return ret;                                                               \
    } while(0);

    UPDATE_REPORT_TABLE_FOR_VERSION(0);
    UPDATE_REPORT_TABLE_FOR_VERSION(1);
    UPDATE_REPORT_TABLE_FOR_VERSION(2);

#undef UPDATE_REPORT_TABLE_FOR_VERSION

    return set_db_version(current_report_version);
}

int SQLLiteReport::save_report(MediaConchLib::report reportKind, MediaConchLib::format format,
                               const std::string& filename, const std::string& file_last_modification,
                               const std::string& report, MediaConchLib::compression compress,
                               bool has_mil_version)
{
    std::stringstream create;

    if (file_is_registered(reportKind, format, filename))
        return update_report(reportKind, format, filename, file_last_modification, report, compress, has_mil_version);

    reports.clear();
    create << "INSERT INTO Report";
    create << " (FILENAME, FILE_LAST_MODIFICATION, TOOL, FORMAT, REPORT, COMPRESS, HAS_MIL_VERSION)";
    create << " VALUES (?, ?, ?, ?, ?, ?, ?);";
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

    ret = sqlite3_bind_int(stmt, 7, (int)has_mil_version);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

int SQLLiteReport::update_report(MediaConchLib::report reportKind, MediaConchLib::format format,
                                 const std::string& filename, const std::string& file_last_modification,
                                 const std::string& report, MediaConchLib::compression compress,
                                 bool has_mil_version)
{
    std::stringstream create;

    reports.clear();
    create << "UPDATE Report ";
    create << "SET FILE_LAST_MODIFICATION = ?, REPORT = ?, COMPRESS = ?, HAS_MIL_VERSION = ? ";
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

    ret = sqlite3_bind_int(stmt, 4, (int)has_mil_version);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 5, filename.c_str(), filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 6, (int)reportKind);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 7, (int)format);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

int SQLLiteReport::remove_report(const std::string& filename)
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

void SQLLiteReport::get_report(MediaConchLib::report reportKind, MediaConchLib::format format,
                               const std::string& filename, const std::string& file_last_modification,
                               std::string& report, MediaConchLib::compression& compress)
{
    std::stringstream create;

    reports.clear();
    create << "SELECT REPORT, COMPRESS FROM " << "Report" << " WHERE ";
    create << "FILENAME = ? ";
    create << "AND TOOL = ? ";
    create << "AND FORMAT = ? ";
    if (file_last_modification.length())
        create << "AND FILE_LAST_MODIFICATION = ?;";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return;

    ret = sqlite3_bind_blob(stmt, 1, filename.c_str(), filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return;

    ret = sqlite3_bind_int(stmt, 2, (int)reportKind);
    if (ret != SQLITE_OK)
        return;

    ret = sqlite3_bind_int(stmt, 3, (int)format);
    if (ret != SQLITE_OK)
        return;

    if (file_last_modification.length())
    {
        ret = sqlite3_bind_blob(stmt, 4, file_last_modification.c_str(), file_last_modification.length(), SQLITE_STATIC);
        if (ret != SQLITE_OK)
            return;
    }

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

bool SQLLiteReport::file_is_registered(MediaConchLib::report reportKind, MediaConchLib::format format,
                                       const std::string& filename, const std::string& file_last_modification)
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

bool SQLLiteReport::file_is_registered(MediaConchLib::report reportKind, MediaConchLib::format format,
                                       const std::string& filename)
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

bool SQLLiteReport::has_version_registered(const std::string& filename)
{
    std::stringstream create;
    std::string key("HAS_MIL_VERSION");

    reports.clear();
    create << "SELECT " << key << " FROM Report WHERE ";
    create << "FILENAME = ?;";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return false;

    ret = sqlite3_bind_blob(stmt, 1, filename.c_str(), filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return false;

    if (execute() || !reports.size() || reports[0].find(key) == reports[0].end())
        return false;

    if (reports[0][key] == "0")
        return false;
    return true;
}

void SQLLiteReport::get_elements(std::vector<std::string>& vec)
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
void SQLLiteReport::get_element_report_kind(const std::string& file, MediaConchLib::report& report_kind)
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

            if (tool_i == MediaConchLib::report_MediaInfo || tool_i == MediaConchLib::report_MediaTrace || tool_i == MediaConchLib::report_MicroMediaTrace)
                continue;
            report_kind = tool_i;
        }
    }
}

}

#endif
