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

int SQLLiteUi::current_ui_version     = 1;

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
int SQLLiteUi::create_ui_table()
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
int SQLLiteUi::update_ui_table()
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
int SQLLiteUi::init_ui()
{
    if (init() < 0)
        return -1;
    if (get_db_version(ui_version) < 0)
        return -1;
    create_ui_settings_table();
    create_ui_table();
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_add_file(const FileRegistered* file)
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
int SQLLiteUi::ui_add_files(const std::vector<FileRegistered*>& files)
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
int SQLLiteUi::ui_update_files(const std::vector<FileRegistered*>& files)
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
int SQLLiteUi::ui_get_file(FileRegistered* file)
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
int SQLLiteUi::create_ui_settings_table()
{
    get_sql_query_for_create_ui_settings_table(query);

    const char* end = NULL;
    int ret = sqlite3_prepare_v2(db, query.c_str(), query.length() + 1, &stmt, &end);
    if (ret != SQLITE_OK || !stmt || (end && *end))
        return -1;
    ret = execute();
    if (ret < 0)
        return -1;

    return ret;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_save_default_policy(const std::string&)
{
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_get_default_policy(std::string&)
{
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_save_default_display(const std::string&)
{
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_get_default_display(std::string&)
{
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_save_default_verbosity(int)
{
    return 0;
}

//---------------------------------------------------------------------------
int SQLLiteUi::ui_get_default_verbosity(int&)
{
    return 0;
}

}

#endif
