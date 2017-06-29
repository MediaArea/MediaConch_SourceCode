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

int SQLLiteReport::current_report_version = 7;

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
    if (report_version == 0)
    {
        get_sql_query_for_create_report_table(query);

        const char* end = NULL;
        int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
        if (ret != SQLITE_OK || !stmt || (end && *end))
            return -1;
        ret = execute();
        if (ret < 0)
            return -1;
    }

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
    UPDATE_REPORT_TABLE_FOR_VERSION(4);
    UPDATE_REPORT_TABLE_FOR_VERSION(5);
    UPDATE_REPORT_TABLE_FOR_VERSION(6);

#undef UPDATE_REPORT_TABLE_FOR_VERSION

    return set_db_version(current_report_version);
}

//---------------------------------------------------------------------------
void SQLLiteReport::get_users_id(std::vector<long>& ids, std::string& err)
{
    reports.clear();
    query = "SELECT USER FROM MEDIACONCH_FILE;";

    if (prepare_v2(query, err) < 0)
        return;

    if (execute() < 0)
    {
        err = error;
        return;
    }

    for (size_t i = 0; i < reports.size(); ++i)
    {
        if (reports[i].find("USER") != reports[i].end())
            ids.push_back(std_string_to_int(reports[i]["USER"]));
    }
}

//---------------------------------------------------------------------------
int SQLLiteReport::get_file_ids(long user, std::vector<long>& ids, std::string& err)
{
    reports.clear();
    query = "SELECT ID FROM MEDIACONCH_FILE WHERE USER = ?;";

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = sqlite3_bind_int(stmt, 1, user);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    if (execute() < 0)
    {
        err = error;
        return -1;
    }

    for (size_t i = 0; i < reports.size(); ++i)
    {
        if (reports[i].find("ID") != reports[i].end())
            ids.push_back(std_string_to_int(reports[i]["ID"]));
    }

    return 0;
}

long SQLLiteReport::add_file(int user, const std::string& filename, const std::string& file_last_modification,
                             const std::string& options, std::string& err,
                             const std::vector<long>& generated_id,
                             long source_id, size_t generated_time,
                             const std::string& generated_log, const std::string& generated_error_log)
{
    long id = get_file_id(user, filename, file_last_modification, options, err);
    if (id >= 0)
        return id;

    err.clear();

    std::stringstream create;

    reports.clear();
    create << "INSERT INTO MEDIACONCH_FILE";
    create << " (USER, FILENAME, FILE_LAST_MODIFICATION, GENERATED_ID, SOURCE_ID, GENERATED_TIME, GENERATED_LOG, GENERATED_ERROR_LOG, OPTIONS)";
    create << " VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
    query = create.str();

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = sqlite3_bind_int(stmt, 1, user);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_blob(stmt, 2, filename.c_str(), filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_blob(stmt, 3, file_last_modification.c_str(), file_last_modification.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    std::string g_id;
    longs_to_string(generated_id, g_id);
    ret = sqlite3_bind_blob(stmt, 4, g_id.c_str(), g_id.size(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 5, source_id);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 6, generated_time);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_blob(stmt, 7, generated_log.c_str(), generated_log.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_blob(stmt, 8, generated_error_log.c_str(), generated_error_log.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_text(stmt, 9, options.c_str(), options.size(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    if (execute() < 0)
    {
        err = error;
        return -1;
    }

    id = get_file_id(user, filename, file_last_modification, options, err);
    return id;
}

long SQLLiteReport::update_file(int user, long file_id, const std::string& file_last_modification,
                                const std::string& options, std::string& err,
                                const std::vector<long>& generated_id, long source_id, size_t generated_time,
                                const std::string& generated_log, const std::string& generated_error_log)
{
    std::stringstream create;

    reports.clear();
    create << "UPDATE MEDIACONCH_FILE";
    create << " SET FILE_LAST_MODIFICATION = ?, GENERATED_ID = ?, SOURCE_ID = ?,";
    create << " GENERATED_TIME = ?, GENERATED_LOG = ?, GENERATED_ERROR_LOG = ?,";
    create << " ANALYZED = 0";
    create << " WHERE ID = ? AND USER = ? AND OPTIONS = ?;";
    query = create.str();

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = sqlite3_bind_blob(stmt, 1, file_last_modification.c_str(), file_last_modification.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    std::string g_id;
    longs_to_string(generated_id, g_id);
    ret = sqlite3_bind_blob(stmt, 2, g_id.c_str(), g_id.size(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 3, source_id);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 4, generated_time);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_blob(stmt, 5, generated_log.c_str(), generated_log.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_blob(stmt, 6, generated_error_log.c_str(), generated_error_log.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 7, file_id);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 8, user);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_text(stmt, 9, options.c_str(), options.size(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    if (execute() < 0)
    {
        err = error;
        return -1;
    }

    return file_id;
}

long SQLLiteReport::get_file_id(int user, const std::string& filename, const std::string& file_last_modification,
                                const std::string& options, std::string& err)
{
    std::stringstream create;
    std::string key("ID");

    reports.clear();
    create << "SELECT " << key << " FROM MEDIACONCH_FILE";
    create << " WHERE FILENAME = ?";
    create << " AND USER = ?";
    create << " AND OPTIONS = ?";
    if (file_last_modification.size())
        create << " AND FILE_LAST_MODIFICATION = ?";
    create << ";";
    query = create.str();

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = sqlite3_bind_blob(stmt, 1, filename.c_str(), filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 2, user);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_text(stmt, 3, options.c_str(), options.size(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    if (file_last_modification.size())
    {
        ret = sqlite3_bind_blob(stmt, 4, file_last_modification.c_str(), file_last_modification.length(), SQLITE_STATIC);
        if (ret != SQLITE_OK)
        {
            err = get_sqlite_error(ret);
            return -1;
        }
    }

    if (execute() < 0)
    {
        err = error;
        return -1;
    }

    if (!reports.size() || reports[0].find(key) == reports[0].end())
    {
        err = "Cannot find file ID.";
        return -1;
    }

    return std_string_to_int(reports[0][key]);
}

//---------------------------------------------------------------------------
int SQLLiteReport::get_file_name_from_id(int user, long id, std::string& file, std::string& err)
{
    std::stringstream create;
    std::string key("FILENAME");

    reports.clear();
    create << "SELECT " << key << " FROM MEDIACONCH_FILE";
    create << " WHERE ID = ? AND USER = ?;";
    query = create.str();

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = sqlite3_bind_int(stmt, 1, id);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 2, user);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    if (execute() || !reports.size() || reports[0].find(key) == reports[0].end())
    {
        err = error;
        return -1;
    }

    file = reports[0][key];
    return 0;
}

int SQLLiteReport::get_file_information_from_id(int user, long id, std::string& filename, std::string& file_last_modification,
                                                std::vector<long>& generated_id, long& source_id, size_t& generated_time,
                                                std::string& generated_log, std::string& generated_error_log,
                                                std::string& options, bool& analyzed,
                                                bool& has_error, std::string& error_log, std::string& err)
{
    std::stringstream create;
    std::string name("FILENAME");
    std::string modif("FILE_LAST_MODIFICATION");
    std::string analyz("ANALYZED");
    std::string gen_id("GENERATED_ID");
    std::string src_id("SOURCE_ID");
    std::string gen_time("GENERATED_TIME");
    std::string gen_log("GENERATED_LOG");
    std::string gen_error_log("GENERATED_ERROR_LOG");
    std::string opt("OPTIONS");
    std::string has_err("HAS_ERROR");
    std::string err_log("ERROR_LOG");

    reports.clear();
    create << "SELECT " << name;
    create << ", " << modif;
    create << ", " << analyz;
    create << ", " << src_id;
    create << ", " << gen_id;
    create << ", " << gen_time;
    create << ", " << gen_log;
    create << ", " << gen_error_log;
    create << ", " << opt;
    create << ", " << has_err;
    create << ", " << err_log;
    create << " FROM MEDIACONCH_FILE";
    create << " WHERE ID = ? AND USER = ?;";
    query = create.str();

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = sqlite3_bind_int(stmt, 1, id);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 2, user);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    if (execute())
    {
        err = error;
        return -1;
    }

    if (reports.size() != 1)
        return -1;

    if (reports[0].find(name) != reports[0].end())
        filename = reports[0][name];

    if (reports[0].find(modif) != reports[0].end())
        file_last_modification = reports[0][modif];

    if (reports[0].find(analyz) != reports[0].end())
        analyzed = (bool)std_string_to_int(reports[0][analyz]);

    if (reports[0].find(gen_id) != reports[0].end())
    {
        std::string g_id;
        string_to_longs(reports[0][gen_id], generated_id);
    }

    if (reports[0].find(src_id) != reports[0].end())
        source_id = std_string_to_int(reports[0][src_id]);

    if (reports[0].find(gen_time) != reports[0].end())
        generated_time = std_string_to_uint(reports[0][gen_time]);

    if (reports[0].find(gen_log) != reports[0].end())
        generated_log = reports[0][gen_log];

    if (reports[0].find(gen_error_log) != reports[0].end())
        generated_error_log = reports[0][gen_error_log];

    if (reports[0].find(opt) != reports[0].end())
        options = reports[0][opt];

    if (reports[0].find(has_err) != reports[0].end())
        has_error = (bool)std_string_to_int(reports[0][has_err]);

    if (reports[0].find(err_log) != reports[0].end())
        error_log = reports[0][err_log];

    return 0;
}

int SQLLiteReport::get_generated_id(int user, long id, std::vector<long>& generated_id, std::string& err)
{
    std::stringstream create;
    std::string gen_id("GENERATED_ID");

    reports.clear();
    create << "SELECT " << gen_id;
    create << " FROM MEDIACONCH_FILE";
    create << " WHERE ID = ? AND USER = ?;";
    query = create.str();

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = sqlite3_bind_int(stmt, 1, id);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 2, user);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    if (execute())
    {
        err = error;
        return -1;
    }

    if (reports.size() != 1)
        return 0;

    if (reports[0].find(gen_id) != reports[0].end())
    {
        std::string g_id;
        string_to_longs(reports[0][gen_id], generated_id);
    }

    return 0;
}

int SQLLiteReport::add_file_generated_id(int user, long source_id, long generated_id, std::string& err)
{
    std::vector<long> ids;
    if (get_generated_id(user, source_id, ids, err))
        return -1;
    ids.push_back(generated_id);

    std::stringstream create;

    reports.clear();
    create << "UPDATE MEDIACONCH_FILE ";
    create << "SET GENERATED_ID = ? ";
    create << "WHERE ID = ? AND USER = ?;";
    query = create.str();

    if (prepare_v2(query, err) < 0)
        return -1;

    std::string g_id;
    longs_to_string(ids, g_id);
    int ret = sqlite3_bind_blob(stmt, 1, g_id.c_str(), g_id.size(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 2, source_id);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 3, user);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    return execute();
}

int SQLLiteReport::update_file_analyzed(int user, long id, std::string& err, bool analyzed)
{
    std::stringstream create;

    reports.clear();
    create << "UPDATE MEDIACONCH_FILE ";
    create << "SET ANALYZED = ? ";
    create << "WHERE ID = ? AND USER = ?;";
    query = create.str();

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = sqlite3_bind_int(stmt, 1, analyzed);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 2, id);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 3, user);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    return execute();
}

int SQLLiteReport::update_file_error(int user, long id, std::string& err, bool has_error, const std::string& error_log)
{
    std::stringstream create;

    reports.clear();
    create << "UPDATE MEDIACONCH_FILE ";
    create << "SET HAS_ERROR = ?, ERROR_LOG = ? ";
    create << "WHERE ID = ? AND USER = ?;";
    query = create.str();

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = sqlite3_bind_int(stmt, 1, has_error);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_blob(stmt, 2, error_log.c_str(), error_log.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 3, id);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 4, user);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    return execute();
}

bool SQLLiteReport::file_id_match_user(int user, long id, std::string& err)
{
    std::stringstream create;

    reports.clear();
    create << "SELECT ID FROM MEDIACONCH_FILE ";
    create << "WHERE ID = ? AND USER = ?;";
    query = create.str();

    if (prepare_v2(query, err) < 0)
        return false;

    int ret = sqlite3_bind_int(stmt, 1, id);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return false;
    }

    ret = sqlite3_bind_int(stmt, 2, user);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return false;
    }

    if (execute() < 0)
    {
        err = error;
        return false;
    }

    if (reports.size() <= 0)
        return false;

    return true;
}

bool SQLLiteReport::file_is_analyzed(int user, long id, std::string& err)
{
    std::stringstream create;

    reports.clear();
    create << "SELECT ANALYZED FROM MEDIACONCH_FILE ";
    create << "WHERE ID = ? AND USER = ?;";
    query = create.str();

    if (prepare_v2(query, err) < 0)
        return false;

    int ret = sqlite3_bind_int(stmt, 1, id);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return false;
    }

    ret = sqlite3_bind_int(stmt, 2, user);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return false;
    }

    if (execute() || reports.size() <= 0)
    {
        err = error;
        return false;
    }

    if (reports[0]["ANALYZED"] == "1")
        return true;

    return false;
}

int SQLLiteReport::remove_file(int user, long file_id, std::string& err)
{
    std::stringstream create;

    reports.clear();
    create << "UPDATE MEDIACONCH_FILE ";
    create << "SET ANALYZED = ? ";
    create << "WHERE ID = ? AND USER = ?;";
    query = create.str();

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = sqlite3_bind_int(stmt, 1, false);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 2, file_id);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 3, user);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    return execute();
}

int SQLLiteReport::reset_file(int user, long file_id, std::string& err)
{
    remove_report(user, file_id, err);

    reports.clear();
    query = "DELETE FROM MEDIACONCH_FILE WHERE ID = ? AND USER = ?;";

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = sqlite3_bind_int(stmt, 1, file_id);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 2, user);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    return execute();
}

int SQLLiteReport::remove_all_files(int user, std::string& err)
{
    reports.clear();
    query = "DELETE FROM MEDIACONCH_FILE WHERE USER = ?;";

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = sqlite3_bind_int(stmt, 1, user);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    return execute();
}

int SQLLiteReport::save_report(int user, long file_id, MediaConchLib::report report_kind, MediaConchLib::format format,
                               const std::string& options,
                               const std::string& report, MediaConchLib::compression compress,
                               int mil_version, std::string& err)
{
    std::stringstream create;

    if (!file_id_match_user(user, file_id, err))
    {
        err = "File ID is not matching the user.";
        return -1;
    }

    bool registered = false;
    if (report_is_registered(user, file_id, report_kind, format, options, registered, err) < 0)
        return -1;

    if (registered)
        return update_report(user, file_id, report_kind, format, options, report, compress, mil_version, err);

    reports.clear();
    create << "INSERT INTO MEDIACONCH_REPORT";
    create << " (FILE_ID, TOOL, FORMAT, OPTIONS, REPORT, COMPRESS, MIL_VERSION)";
    create << " VALUES (?, ?, ?, ?, ?, ?, ?);";
    query = create.str();

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = sqlite3_bind_int(stmt, 1, file_id);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 2, (int)report_kind);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 3, (int)format);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_text(stmt, 4, options.c_str(), options.size(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_blob(stmt, 5, report.c_str(), report.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 6, (int)compress);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 7, mil_version);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    return execute();
}

int SQLLiteReport::update_report(int, long file_id, MediaConchLib::report reportKind, MediaConchLib::format format,
                                 const std::string& options,
                                 const std::string& report, MediaConchLib::compression compress,
                                 int mil_version, std::string& err)
{
    std::stringstream create;

    reports.clear();
    create << "UPDATE MEDIACONCH_REPORT ";
    create << "SET REPORT = ?, COMPRESS = ?, MIL_VERSION = ? ";
    create << "WHERE FILE_ID = ? AND TOOL = ? AND FORMAT = ? ";
    create << "AND OPTIONS = ?;";
    query = create.str();

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = sqlite3_bind_blob(stmt, 1, report.c_str(), report.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 2, (int)compress);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 3, mil_version);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 4, file_id);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 5, (int)reportKind);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 6, (int)format);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_text(stmt, 7, options.c_str(), options.size(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    return execute();
}

int SQLLiteReport::remove_report(int user, long file_id, std::string& err)
{
    if (!file_id_match_user(user, file_id, err))
    {
        err = "File ID is not matching the user.";
        return -1;
    }

    std::stringstream delete_query;

    reports.clear();
    query = "DELETE FROM MEDIACONCH_REPORT WHERE FILE_ID = ?;";

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = sqlite3_bind_int(stmt, 1, file_id);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    return execute();
}

int SQLLiteReport::remove_all_reports(int user, std::string& err)
{
    std::vector<long> ids;
    get_file_ids(user, ids, err);
    if (!ids.size())
        return 0;

    std::stringstream delete_query;

    reports.clear();
    delete_query << "DELETE FROM MEDIACONCH_REPORT WHERE";
    for (size_t i = 0; i < ids.size(); ++i)
    {
        if (i)
            delete_query << " OR";
        delete_query << " FILE_ID = ?";
    }
    delete_query << ";";

    query = delete_query.str();

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = 0;

    for (size_t i = 0; i < ids.size(); ++i)
    {
        ret = sqlite3_bind_int(stmt, i + 1, ids[i]);
        if (ret != SQLITE_OK)
        {
            err = get_sqlite_error(ret);
            return -1;
        }
    }

    return execute();
}

int SQLLiteReport::get_report(int user, long file_id, MediaConchLib::report report_kind,
                              MediaConchLib::format format, const std::string& options,
                              std::string& report, MediaConchLib::compression& compress, std::string& err)
{
    if (!file_id_match_user(user, file_id, err))
    {
        err = "File ID is not matching the user.";
        return -1;
    }

    std::stringstream create;

    reports.clear();
    create << "SELECT REPORT, COMPRESS FROM MEDIACONCH_REPORT WHERE ";
    create << "FILE_ID = ? ";
    create << "AND TOOL = ? ";
    create << "AND FORMAT = ?";
    create << "AND OPTIONS = ?;";
    query = create.str();

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = sqlite3_bind_int(stmt, 1, file_id);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 2, (int)report_kind);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 3, (int)format);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_text(stmt, 4, options.c_str(), options.size(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    if (execute() < 0 || !reports.size())
    {
        err = error;
        return -1;
    }

    std::map<std::string, std::string> r = reports[0];

    if (r.find("REPORT") == r.end())
    {
        err = "No report found";
        return -1;
    }
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

    return 0;
}

int SQLLiteReport::report_is_registered(int user, long file_id, MediaConchLib::report report_kind,
                                        MediaConchLib::format format, const std::string& options,
                                        bool& registered, std::string& err)
{
    registered = false;

    if (!file_id_match_user(user, file_id, err))
    {
        err = "File ID is not matching the user.";
        return -1;
    }

    std::stringstream create;
    std::string key("COUNT(REPORT)");

    reports.clear();
    create << "SELECT " << key << " FROM MEDIACONCH_REPORT WHERE ";
    create << "FILE_ID = ? ";
    create << "AND TOOL = ? ";
    create << "AND FORMAT = ? ";
    create << "AND OPTIONS = ?;";
    query = create.str();

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = sqlite3_bind_int(stmt, 1, file_id);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 2, (int)report_kind);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_int(stmt, 3, (int)format);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    ret = sqlite3_bind_text(stmt, 4, options.c_str(), options.size(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    if (execute())
    {
        err = error;
        return -1;
    }

    if (!reports.size() || reports[0].find(key) == reports[0].end())
        return 0;

    if (reports[0][key] == "0")
        return 0;

    registered = true;
    return 0;
}

int SQLLiteReport::version_registered(int user, long file_id, std::string& err)
{
    if (!file_id_match_user(user, file_id, err))
    {
        err = "File ID is not matching the user.";
        return -1;
    }

    std::stringstream create;
    std::string key("MIL_VERSION");

    reports.clear();
    create << "SELECT " << key << " FROM MEDIACONCH_REPORT WHERE ";
    create << "FILE_ID = ?;";
    query = create.str();

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = sqlite3_bind_int(stmt, 1, file_id);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    if (execute())
    {
        err = error;
        return -1;
    }

    if (!reports.size() || reports[0].find(key) == reports[0].end())
    {
        return -1;
    }

    return std_string_to_int(reports[0][key]);
}

int SQLLiteReport::get_elements(int user, std::vector<std::string>& vec, std::string& err)
{
    std::stringstream create;
    std::string key("FILENAME");

    reports.clear();
    create << "SELECT " << key << " FROM MEDIACONCH_FILE WHERE USER = ?;";
    query = create.str();

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = sqlite3_bind_int(stmt, 1, user);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    if (execute())
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    for (size_t i = 0; i < reports.size(); ++i)
    {
        if (reports[i].find(key) != reports[i].end())
            vec.push_back(reports[i][key]);
    }

    return 0;
}

int SQLLiteReport::get_elements(int user, std::vector<long>& vec, std::string& err)
{
    std::stringstream create;
    std::string key("ID");

    reports.clear();
    create << "SELECT " << key << " FROM MEDIACONCH_FILE WHERE USER = ?;";
    query = create.str();

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = sqlite3_bind_int(stmt, 1, user);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    if (execute())
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    for (size_t i = 0; i < reports.size(); ++i)
    {
        if (reports[i].find(key) != reports[i].end())
            vec.push_back((long)std_string_to_int(reports[i][key]));
    }

    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteReport::get_element_report_kind(int user, long file_id, MediaConchLib::report& report_kind, std::string& err)
{
    if (!file_id_match_user(user, file_id, err))
    {
        err = "File ID is not matching the user.";
        return -1;
    }

    report_kind = MediaConchLib::report_MediaConch;
    reports.clear();
    query = "SELECT TOOL FROM MEDIACONCH_REPORT WHERE FILE_ID = ?;";

    if (prepare_v2(query, err) < 0)
        return -1;

    int ret = sqlite3_bind_int(stmt, 1, file_id);
    if (ret != SQLITE_OK)
    {
        err = get_sqlite_error(ret);
        return -1;
    }

    if (execute())
    {
        err = error;
        return -1;
    }

    if (!reports.size())
        return 0;

    for (size_t i = 0; i < reports.size(); ++i)
    {
        if (reports[i].find("TOOL") != reports[i].end())
        {
            MediaConchLib::report tool_i = (MediaConchLib::report)std_string_to_int(reports[i]["TOOL"]);

            if (tool_i == MediaConchLib::report_MediaConch || tool_i == MediaConchLib::report_MediaInfo || tool_i == MediaConchLib::report_MediaTrace || tool_i == MediaConchLib::report_MicroMediaTrace)
                continue;
            report_kind = tool_i;
        }
    }

    return 0;
}

}

#endif
