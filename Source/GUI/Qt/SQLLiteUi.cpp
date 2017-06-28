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
#include "SQLLiteUi.h"
#include "Common/FileRegistered.h"
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// SQLLiteUi
//***************************************************************************

int SQLLiteUi::ui_current_version          = 11;

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
SQLLiteUi::SQLLiteUi() : DatabaseUi(), SQLLite()
{
}

//---------------------------------------------------------------------------
SQLLiteUi::~SQLLiteUi()
{
}

//---------------------------------------------------------------------------
int SQLLiteUi::init()
{
    return SQLLite::init(db_dirname, db_filename);
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_create_table()
{
    get_sql_query_for_create_ui_table(query);

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;
    ret = execute();
    if (ret < 0)
        return -1;

    return ui_update_table();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_update_table()
{
    const char* end = NULL;
    int ret = 0;

#define UPDATE_UI_TABLE_FOR_VERSION(version)                            \
    do                                                                  \
    {                                                                   \
        if (ui_version > version)                                       \
            continue;                                                   \
                                                                        \
        get_sql_query_for_update_ui_table_v##version(query);            \
        if (!query.length())                                            \
            continue;                                                   \
                                                                        \
        const char* q = query.c_str();                                  \
        end = NULL;                                                     \
        while (1)                                                       \
        {                                                               \
            ret = sqlite3_prepare_v2(db, q, -1, &stmt, &end);           \
            if (version != 0 && (ret != SQLITE_OK || !stmt))            \
                return -1;                                              \
            ret = execute();                                            \
                                                                        \
            if (version != 0 && ret < 0)                                \
                return ret;                                             \
            if (!end || !*end)                                          \
                break;                                                  \
                                                                        \
            q = end;                                                    \
        }                                                               \
    } while(0);

    UPDATE_UI_TABLE_FOR_VERSION(0);
    // UPDATE_UI_TABLE_FOR_VERSION(1); nothing to do
    // UPDATE_UI_TABLE_FOR_VERSION(2); nothing to do
    // UPDATE_UI_TABLE_FOR_VERSION(3); nothing to do
    // UPDATE_UI_TABLE_FOR_VERSION(4); nothing to do
    // UPDATE_UI_TABLE_FOR_VERSION(5); nothing to do
    // UPDATE_UI_TABLE_FOR_VERSION(6); nothing to do
    // UPDATE_UI_TABLE_FOR_VERSION(7); nothing to do
    // UPDATE_UI_TABLE_FOR_VERSION(8); nothing to do
    // UPDATE_UI_TABLE_FOR_VERSION(9); nothing to do
    UPDATE_UI_TABLE_FOR_VERSION(10);

#undef UPDATE_UI_TABLE_FOR_VERSION

    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::init_ui()
{
    if (init() < 0)
        return -1;
    if (get_db_version(ui_version) < 0)
        return -1;
    if (ui_settings_create_table() < 0)
        return -1;
    if (ui_create_table() < 0)
        return -1;
    set_db_version(ui_current_version);
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_add_file(const FileRegistered* file)
{
    std::stringstream create;

    reports.clear();
    create << "INSERT INTO UI ";
    create << "(FILENAME, FILEPATH, POLICY, DISPLAY, ANALYZED, IMPLEMENTATION_VALID, POLICY_VALID, VERBOSITY, OPTIONS)";
    create << " VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
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

    std::string options;
    options_vec_to_string(file->options, options);
    ret = sqlite3_bind_blob(stmt, 9, options.c_str(), options.length(), SQLITE_TRANSIENT);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_add_files(const std::vector<FileRegistered*>& files)
{
    std::vector<FileRegistered*> create_files, update_files;
    int ret = 0;

    for (size_t i = 0; i < files.size(); ++i)
    {
        if (ui_file_exists(files[i]))
            update_files.push_back(files[i]);
        else
            create_files.push_back(files[i]);
    }

    if (ui_create_files(create_files) < 0)
        ret = -1;

    if (ui_update_files(update_files) < 0)
        ret = -1;

    return ret;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_create_files(const std::vector<FileRegistered*>& files)
{
    std::stringstream create;

    reports.clear();
    create << "INSERT INTO UI ";
    create << "(FILENAME, FILEPATH, POLICY, DISPLAY, ANALYZED, IMPLEMENTATION_VALID, POLICY_VALID, VERBOSITY, OPTIONS) VALUES";
    for (size_t i = 0; i < files.size(); ++i)
    {
        if (i)
            create << ",";
        create << " (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    }

    create << ";";
    query = create.str();

    std::string err;

    if (prepare_v2(query, err) < 0)
        return -1;

    int nb_column = 9;
    int ret = SQLITE_OK;
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

        std::string options;
        options_vec_to_string(file->options, options);

        ret = sqlite3_bind_blob(stmt, i * nb_column + 9, options.c_str(), options.length(), SQLITE_TRANSIENT);
        if (ret != SQLITE_OK)
            return -1;
    }

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_update_file(const FileRegistered* file)
{
    std::stringstream create;

    reports.clear();
    create << "UPDATE UI ";
    create << "SET POLICY           = ?, ";
    create << "DISPLAY              = ?, ";
    create << "ANALYZED             = ?, ";
    create << "IMPLEMENTATION_VALID = ?, ";
    create << "POLICY_VALID         = ?, ";
    create << "VERBOSITY            = ?, ";
    create << "OPTIONS              = ?  ";
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

    std::string options;
    options_vec_to_string(file->options, options);
    ret = sqlite3_bind_blob(stmt, 7, options.c_str(), options.length(), SQLITE_TRANSIENT);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 8, file->filename.c_str(), file->filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 9, file->filepath.c_str(), file->filepath.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_update_files(const std::vector<FileRegistered*>& files)
{
    std::stringstream create;

    reports.clear();
    create << "WITH Tmp(FILENAME, FILEPATH, POLICY, DISPLAY, ANALYZED, IMPLEMENTATION_VALID, POLICY_VALID, VERBOSITY, OPTIONS)";
    create << "AS (VALUES";
    for (size_t i = 0; i < files.size(); ++i)
    {
        if (i)
            create << ", ";
        create << "(?, ?, ?, ?, ?, ?, ?, ?, ?)";
    }
    create << ") ";

    create << "UPDATE UI SET ";
    create << "POLICY = (SELECT POLICY FROM Tmp WHERE UI.FILENAME = Tmp.FILENAME AND UI.FILEPATH = Tmp.FILEPATH), ";
    create << "DISPLAY = (SELECT DISPLAY FROM Tmp WHERE UI.FILENAME = Tmp.FILENAME AND UI.FILEPATH = Tmp.FILEPATH), ";
    create << "ANALYZED = (SELECT ANALYZED FROM Tmp WHERE UI.FILENAME = Tmp.FILENAME AND UI.FILEPATH = Tmp.FILEPATH), ";
    create << "IMPLEMENTATION_VALID = (SELECT IMPLEMENTATION_VALID FROM Tmp WHERE UI.FILENAME = Tmp.FILENAME AND UI.FILEPATH = Tmp.FILEPATH), ";
    create << "POLICY_VALID = (SELECT POLICY_VALID FROM Tmp WHERE UI.FILENAME = Tmp.FILENAME AND UI.FILEPATH = Tmp.FILEPATH), ";
    create << "VERBOSITY = (SELECT VERBOSITY FROM Tmp WHERE UI.FILENAME = Tmp.FILENAME AND UI.FILEPATH = Tmp.FILEPATH) ";
    create << "OPTIONS = (SELECT OPTIONS FROM Tmp WHERE UI.FILENAME = Tmp.FILENAME AND UI.FILEPATH = Tmp.FILEPATH) ";

    create << " WHERE FILENAME IN (SELECT FILENAME FROM Tmp) AND  FILEPATH IN (SELECT FILEPATH FROM Tmp)";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);

    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    int nb_column = 9;
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

        std::string options;
        options_vec_to_string(file->options, options);
        ret = sqlite3_bind_blob(stmt, i * nb_column + 9, options.c_str(), options.length(), SQLITE_TRANSIENT);
        if (ret != SQLITE_OK)
            return -1;
    }

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_get_file(FileRegistered* file)
{
    std::stringstream create;

    std::string p("POLICY");
    std::string d("DISPLAY");
    std::string a("ANALYZED");
    std::string i_v("IMPLEMENTATION_VALID");
    std::string p_v("POLICY_VALID");
    std::string v("VERBOSITY");
    std::string o("OPTIONS");

    reports.clear();
    create << "SELECT " << p << ", " << d << ", " << a << ", " << i_v << ", " << p_v << ", " << v << ", " << o << " FROM UI ";
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
    if (r.find(o) != r.end())
        options_string_to_vec(r[o], file->options);

    return 0;
}

//---------------------------------------------------------------------------
bool SQLLiteUi::ui_file_exists(FileRegistered* file)
{
    std::stringstream create;

    reports.clear();
    create << "SELECT COUNT(FILENAME) FROM UI ";
    create << "WHERE FILENAME = ? AND FILEPATH = ? AND OPTIONS = ?;";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return false;

    ret = sqlite3_bind_blob(stmt, 1, file->filename.c_str(), file->filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return false;

    ret = sqlite3_bind_blob(stmt, 2, file->filepath.c_str(), file->filepath.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return false;

    std::string options;
    options_vec_to_string(file->options, options);
    ret = sqlite3_bind_blob(stmt, 3, options.c_str(), options.length(), SQLITE_TRANSIENT);
    if (ret != SQLITE_OK)
        return false;

    if (execute())
        return false;

    if (!reports.size() || reports[0]["COUNT(FILENAME)"] == "0")
        return false;

    return true;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_remove_file(const FileRegistered* file)
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
int SQLLiteUi::ui_remove_files(const std::vector<FileRegistered*>& files)
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
int SQLLiteUi::ui_remove_all_files()
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
void SQLLiteUi::ui_get_elements(std::vector<FileRegistered*>& vec)
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
    std::string o("OPTIONS");

    reports.clear();
    create << "SELECT " << name << ", " << path << ", " << p << ", " << d;
    create << ", " << a << ", " << i_v << ", " << p_v << ", " << v << ", " << o;
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
            if (reports[i].find(o) != reports[i].end())
                options_string_to_vec(reports[i][o], file->options);
            if (file->analyzed)
                file->need_update = false;
            vec.push_back(file);
        }
    }
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_reset_file_for_reload(FileRegistered* file)
{
    std::stringstream create;

    reports.clear();
    create << "UPDATE UI ";
    create << "SET ANALYZED         = ?, ";
    create << "IMPLEMENTATION_VALID = ?, ";
    create << "POLICY_VALID         = ?, ";
    create << "WHERE FILENAME       = ?  ";
    create << "AND   FILEPATH       = ? ;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, false);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, false);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 3, false);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 4, file->filename.c_str(), file->filename.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_blob(stmt, 5, file->filepath.c_str(), file->filepath.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_create_table()
{
    get_sql_query_for_create_ui_settings_table(query);

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;
    ret = execute();
    if (ret < 0)
        return -1;

    return ui_settings_update_table();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_update_table()
{
    const char* end = NULL;
    int ret = 0;

#define UPDATE_UI_SETTINGS_TABLE_FOR_VERSION(version)                   \
    do                                                                  \
    {                                                                   \
        if (ui_version > version)                                       \
            continue;                                                   \
        get_sql_query_for_update_ui_settings_table_v##version(query);   \
        if (!query.length())                                            \
            continue;                                                   \
                                                                        \
        end = NULL;                                                     \
        const char* q = query.c_str();                                  \
        while (1)                                                       \
        {                                                               \
            ret = sqlite3_prepare_v2(db, q, -1, &stmt, &end);           \
            if (version != 0 && (ret != SQLITE_OK || !stmt))            \
                return -1;                                              \
            ret = execute();                                            \
                                                                        \
            if (version != 0 && ret < 0)                                \
                return ret;                                             \
                                                                        \
            if (!*end)                                                  \
                break;                                                  \
            q = end;                                                    \
        }                                                               \
    } while(0);

    // UPDATE_UI_SETTINGS_TABLE_FOR_VERSION(0); nothing to do
    UPDATE_UI_SETTINGS_TABLE_FOR_VERSION(1);
    UPDATE_UI_SETTINGS_TABLE_FOR_VERSION(2);
    UPDATE_UI_SETTINGS_TABLE_FOR_VERSION(3);
    UPDATE_UI_SETTINGS_TABLE_FOR_VERSION(4);
    UPDATE_UI_SETTINGS_TABLE_FOR_VERSION(5);
    UPDATE_UI_SETTINGS_TABLE_FOR_VERSION(6);
    UPDATE_UI_SETTINGS_TABLE_FOR_VERSION(7);
    UPDATE_UI_SETTINGS_TABLE_FOR_VERSION(8);
    UPDATE_UI_SETTINGS_TABLE_FOR_VERSION(9);
    // UPDATE_UI_SETTINGS_TABLE_FOR_VERSION(10); nothing to do

#undef UPDATE_UI_SETTINGS_TABLE_FOR_VERSION

    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_add_default_values_for_user(int user_id)
{

    reports.clear();
    std::stringstream create;
    create << "INSERT INTO UI_SETTINGS ";
    create << "(USER_ID, DEFAULT_POLICY, DEFAULT_DISPLAY, DEFAULT_VERBOSITY) VALUES";
    create << " (?, 'last', 'last', -1);";
    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, user_id);
    if (ret != SQLITE_OK)
        return -1;

    ret = execute();
    return ret;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_has_user_id(int user_id, bool& has_row)
{
    reports.clear();
    query = "SELECT USER_ID FROM UI_SETTINGS WHERE USER_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, user_id);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() < 0)
        return -1;

    if (reports.size() != 1 || reports[0].find("USER_ID") == reports[0].end())
        has_row = false;
    else
        has_row = true;
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_check_user_id(int user_id)
{
    bool has_row = false;
    if (ui_settings_has_user_id(user_id, has_row))
        return -1;
    if (!has_row)
        ui_settings_add_default_values_for_user(user_id);
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_save_default_policy(const std::string& policy, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    std::stringstream create;

    reports.clear();
    create << "UPDATE UI_SETTINGS ";
    create << "SET   DEFAULT_POLICY = ? ";
    create << "WHERE USER_ID        = ?;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, policy.c_str(), policy.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, user_id);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_get_default_policy(std::string& policy, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    reports.clear();
    query = "SELECT DEFAULT_POLICY FROM UI_SETTINGS WHERE USER_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, user_id);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || reports.size() != 1)
        return -1;

    if (reports[0].find("DEFAULT_POLICY") != reports[0].end())
        policy = reports[0]["DEFAULT_POLICY"];
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_save_default_display(const std::string& display, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    std::stringstream create;

    reports.clear();
    create << "UPDATE UI_SETTINGS ";
    create << "SET   DEFAULT_DISPLAY = ? ";
    create << "WHERE USER_ID         = ?;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, display.c_str(), display.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, user_id);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_get_default_display(std::string& display, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    reports.clear();
    query = "SELECT DEFAULT_DISPLAY FROM UI_SETTINGS WHERE USER_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, user_id);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || reports.size() != 1)
        return -1;

    if (reports[0].find("DEFAULT_DISPLAY") != reports[0].end())
        display = reports[0]["DEFAULT_DISPLAY"];
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_save_default_verbosity(int verbosity, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    std::stringstream create;

    reports.clear();
    create << "UPDATE UI_SETTINGS ";
    create << "SET   DEFAULT_VERBOSITY = ? ";
    create << "WHERE USER_ID           = ?;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, verbosity);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, user_id);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_get_default_verbosity(int& verbosity, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    reports.clear();
    query = "SELECT DEFAULT_VERBOSITY FROM UI_SETTINGS WHERE USER_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, user_id);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || reports.size() != 1)
        return -1;

    if (reports[0].find("DEFAULT_VERBOSITY") != reports[0].end())
        verbosity = std_string_to_int(reports[0]["DEFAULT_VERBOSITY"]);
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_save_last_policy(const std::string& policy, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    std::stringstream create;

    reports.clear();
    create << "UPDATE UI_SETTINGS ";
    create << "SET   LAST_POLICY    = ? ";
    create << "WHERE USER_ID        = ?;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, policy.c_str(), policy.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, user_id);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_get_last_policy(std::string& policy, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    reports.clear();
    query = "SELECT LAST_POLICY FROM UI_SETTINGS WHERE USER_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, user_id);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || reports.size() != 1)
        return -1;

    if (reports[0].find("LAST_POLICY") != reports[0].end())
        policy = reports[0]["LAST_POLICY"];
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_save_last_display(const std::string& display, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    std::stringstream create;

    reports.clear();
    create << "UPDATE UI_SETTINGS ";
    create << "SET   LAST_DISPLAY = ? ";
    create << "WHERE USER_ID         = ?;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, display.c_str(), display.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, user_id);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_get_last_display(std::string& display, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    reports.clear();
    query = "SELECT LAST_DISPLAY FROM UI_SETTINGS WHERE USER_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, user_id);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || reports.size() != 1)
        return -1;

    if (reports[0].find("LAST_DISPLAY") != reports[0].end())
        display = reports[0]["LAST_DISPLAY"];
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_save_last_verbosity(int verbosity, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    std::stringstream create;

    reports.clear();
    create << "UPDATE UI_SETTINGS ";
    create << "SET   LAST_VERBOSITY = ? ";
    create << "WHERE USER_ID           = ?;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, verbosity);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, user_id);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_get_last_verbosity(int& verbosity, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    reports.clear();
    query = "SELECT LAST_VERBOSITY FROM UI_SETTINGS WHERE USER_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, user_id);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || reports.size() != 1)
        return -1;

    if (reports[0].find("LAST_VERBOSITY") != reports[0].end())
        verbosity = std_string_to_int(reports[0]["LAST_VERBOSITY"]);
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_save_default_load_files_path(const std::string& load_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    std::stringstream create;

    reports.clear();
    create << "UPDATE UI_SETTINGS ";
    create << "SET    DEFAULT_LOAD_FILES_PATH = ? ";
    create << "WHERE USER_ID        = ?;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, load_path.c_str(), load_path.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, user_id);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_get_default_load_files_path(std::string& load_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    reports.clear();
    query = "SELECT DEFAULT_LOAD_FILES_PATH FROM UI_SETTINGS WHERE USER_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, user_id);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || reports.size() != 1)
        return -1;

    if (reports[0].find("DEFAULT_LOAD_FILES_PATH") != reports[0].end())
        load_path = reports[0]["DEFAULT_LOAD_FILES_PATH"];
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_save_last_load_files_path(const std::string& load_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    std::stringstream create;

    reports.clear();
    create << "UPDATE UI_SETTINGS ";
    create << "SET    LAST_LOAD_FILES_PATH = ? ";
    create << "WHERE USER_ID        = ?;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, load_path.c_str(), load_path.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, user_id);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_get_last_load_files_path(std::string& load_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    reports.clear();
    query = "SELECT LAST_LOAD_FILES_PATH FROM UI_SETTINGS WHERE USER_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, user_id);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || reports.size() != 1)
        return -1;

    if (reports[0].find("LAST_LOAD_FILES_PATH") != reports[0].end())
        load_path = reports[0]["LAST_LOAD_FILES_PATH"];
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_save_default_load_policy_path(const std::string& load_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    std::stringstream create;

    reports.clear();
    create << "UPDATE UI_SETTINGS ";
    create << "SET    DEFAULT_LOAD_POLICY_PATH = ? ";
    create << "WHERE USER_ID        = ?;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, load_path.c_str(), load_path.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, user_id);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_get_default_load_policy_path(std::string& load_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    reports.clear();
    query = "SELECT DEFAULT_LOAD_POLICY_PATH FROM UI_SETTINGS WHERE USER_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, user_id);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || reports.size() != 1)
        return -1;

    if (reports[0].find("DEFAULT_LOAD_POLICY_PATH") != reports[0].end())
        load_path = reports[0]["DEFAULT_LOAD_POLICY_PATH"];
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_save_last_load_policy_path(const std::string& load_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    std::stringstream create;

    reports.clear();
    create << "UPDATE UI_SETTINGS ";
    create << "SET    LAST_LOAD_POLICY_PATH = ? ";
    create << "WHERE USER_ID        = ?;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, load_path.c_str(), load_path.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, user_id);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_get_last_load_policy_path(std::string& load_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    reports.clear();
    query = "SELECT LAST_LOAD_POLICY_PATH FROM UI_SETTINGS WHERE USER_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, user_id);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || reports.size() != 1)
        return -1;

    if (reports[0].find("LAST_LOAD_POLICY_PATH") != reports[0].end())
        load_path = reports[0]["LAST_LOAD_POLICY_PATH"];
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_save_default_load_display_path(const std::string& load_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    std::stringstream create;

    reports.clear();
    create << "UPDATE UI_SETTINGS ";
    create << "SET    DEFAULT_LOAD_DISPLAY_PATH = ? ";
    create << "WHERE USER_ID        = ?;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, load_path.c_str(), load_path.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, user_id);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_get_default_load_display_path(std::string& load_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    reports.clear();
    query = "SELECT DEFAULT_LOAD_DISPLAY_PATH FROM UI_SETTINGS WHERE USER_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, user_id);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || reports.size() != 1)
        return -1;

    if (reports[0].find("DEFAULT_LOAD_DISPLAY_PATH") != reports[0].end())
        load_path = reports[0]["DEFAULT_LOAD_DISPLAY_PATH"];
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_save_last_load_display_path(const std::string& load_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    std::stringstream create;

    reports.clear();
    create << "UPDATE UI_SETTINGS ";
    create << "SET    LAST_LOAD_DISPLAY_PATH = ? ";
    create << "WHERE USER_ID        = ?;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, load_path.c_str(), load_path.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, user_id);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_get_last_load_display_path(std::string& load_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    reports.clear();
    query = "SELECT LAST_LOAD_DISPLAY_PATH FROM UI_SETTINGS WHERE USER_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, user_id);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || reports.size() != 1)
        return -1;

    if (reports[0].find("LAST_LOAD_DISPLAY_PATH") != reports[0].end())
        load_path = reports[0]["LAST_LOAD_DISPLAY_PATH"];
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_save_default_save_report_path(const std::string& save_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    std::stringstream create;

    reports.clear();
    create << "UPDATE UI_SETTINGS ";
    create << "SET    DEFAULT_SAVE_REPORT_PATH = ? ";
    create << "WHERE USER_ID        = ?;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, save_path.c_str(), save_path.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, user_id);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_get_default_save_report_path(std::string& save_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    reports.clear();
    query = "SELECT DEFAULT_SAVE_REPORT_PATH FROM UI_SETTINGS WHERE USER_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, user_id);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || reports.size() != 1)
        return -1;

    if (reports[0].find("DEFAULT_SAVE_REPORT_PATH") != reports[0].end())
        save_path = reports[0]["DEFAULT_SAVE_REPORT_PATH"];
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_save_last_save_report_path(const std::string& save_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    std::stringstream create;

    reports.clear();
    create << "UPDATE UI_SETTINGS ";
    create << "SET    LAST_SAVE_REPORT_PATH = ? ";
    create << "WHERE USER_ID        = ?;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, save_path.c_str(), save_path.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, user_id);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_get_last_save_report_path(std::string& save_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    reports.clear();
    query = "SELECT LAST_SAVE_REPORT_PATH FROM UI_SETTINGS WHERE USER_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, user_id);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || reports.size() != 1)
        return -1;

    if (reports[0].find("LAST_SAVE_REPORT_PATH") != reports[0].end())
        save_path = reports[0]["LAST_SAVE_REPORT_PATH"];
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_save_default_save_policy_path(const std::string& save_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    std::stringstream create;

    reports.clear();
    create << "UPDATE UI_SETTINGS ";
    create << "SET    DEFAULT_SAVE_POLICY_PATH = ? ";
    create << "WHERE USER_ID        = ?;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, save_path.c_str(), save_path.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, user_id);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_get_default_save_policy_path(std::string& save_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    reports.clear();
    query = "SELECT DEFAULT_SAVE_POLICY_PATH FROM UI_SETTINGS WHERE USER_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, user_id);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || reports.size() != 1)
        return -1;

    if (reports[0].find("DEFAULT_SAVE_POLICY_PATH") != reports[0].end())
        save_path = reports[0]["DEFAULT_SAVE_POLICY_PATH"];
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_save_last_save_policy_path(const std::string& save_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    std::stringstream create;

    reports.clear();
    create << "UPDATE UI_SETTINGS ";
    create << "SET    LAST_SAVE_POLICY_PATH = ? ";
    create << "WHERE USER_ID        = ?;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, save_path.c_str(), save_path.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, user_id);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_get_last_save_policy_path(std::string& save_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    reports.clear();
    query = "SELECT LAST_SAVE_POLICY_PATH FROM UI_SETTINGS WHERE USER_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, user_id);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || reports.size() != 1)
        return -1;

    if (reports[0].find("LAST_SAVE_POLICY_PATH") != reports[0].end())
        save_path = reports[0]["LAST_SAVE_POLICY_PATH"];
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_save_default_save_display_path(const std::string& save_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    std::stringstream create;

    reports.clear();
    create << "UPDATE UI_SETTINGS ";
    create << "SET    DEFAULT_SAVE_DISPLAY_PATH = ? ";
    create << "WHERE USER_ID        = ?;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, save_path.c_str(), save_path.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, user_id);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_get_default_save_display_path(std::string& save_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    reports.clear();
    query = "SELECT DEFAULT_SAVE_DISPLAY_PATH FROM UI_SETTINGS WHERE USER_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, user_id);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || reports.size() != 1)
        return -1;

    if (reports[0].find("DEFAULT_SAVE_DISPLAY_PATH") != reports[0].end())
        save_path = reports[0]["DEFAULT_SAVE_DISPLAY_PATH"];
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_save_last_save_display_path(const std::string& save_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    std::stringstream create;

    reports.clear();
    create << "UPDATE UI_SETTINGS ";
    create << "SET    LAST_SAVE_DISPLAY_PATH = ? ";
    create << "WHERE USER_ID        = ?;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, save_path.c_str(), save_path.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, user_id);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_get_last_save_display_path(std::string& save_path, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    reports.clear();
    query = "SELECT LAST_SAVE_DISPLAY_PATH FROM UI_SETTINGS WHERE USER_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, user_id);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || reports.size() != 1)
        return -1;

    if (reports[0].find("LAST_SAVE_DISPLAY_PATH") != reports[0].end())
        save_path = reports[0]["LAST_SAVE_DISPLAY_PATH"];
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_save_mco_token(const std::string& token, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    std::stringstream create;

    reports.clear();
    create << "UPDATE UI_SETTINGS ";
    create << "SET   MCO_TOKEN    = ? ";
    create << "WHERE USER_ID        = ?;";

    query = create.str();

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_blob(stmt, 1, token.c_str(), token.length(), SQLITE_STATIC);
    if (ret != SQLITE_OK)
        return -1;

    ret = sqlite3_bind_int(stmt, 2, user_id);
    if (ret != SQLITE_OK)
        return -1;

    return execute();
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_settings_get_mco_token(std::string& token, int user_id)
{
    if (ui_settings_check_user_id(user_id))
        return -1;

    reports.clear();
    query = "SELECT MCO_TOKEN FROM UI_SETTINGS WHERE USER_ID = ?;";

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;

    ret = sqlite3_bind_int(stmt, 1, user_id);
    if (ret != SQLITE_OK)
        return -1;

    if (execute() || reports.size() != 1)
        return -1;

    if (reports[0].find("MCO_TOKEN") != reports[0].end())
        token = reports[0]["MCO_TOKEN"];
    return 0;
}

//---------------------------------------------------------------------------
void SQLLiteUi::options_vec_to_string(const std::vector<std::string>& vec, std::string& str)
{
    std::stringstream options;
    for (size_t x = 0; x < vec.size(); ++x)
        options << vec[x].size() << "," << vec[x].c_str();
    str = options.str();
}

//---------------------------------------------------------------------------
void SQLLiteUi::options_string_to_vec(const std::string& str, std::vector<std::string>& vec)
{
    size_t pos = 0;
    size_t start;

    while (pos != std::string::npos)
    {
        start = pos;
        pos = str.find(",", start);
        if (pos == std::string::npos)
            continue;

        std::string size = str.substr(start, pos - start);
        pos += 1;
        char *end = NULL;
        long len = strtol(size.c_str(), &end, 10);
        if (len < 0 || pos + len > str.size())
        {
            pos = std::string::npos;
            continue;
        }

        vec.push_back(str.substr(pos, len));
        pos += len;
    }
}

}

#endif
