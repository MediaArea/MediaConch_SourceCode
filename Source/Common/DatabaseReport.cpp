/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "DatabaseReport.h"
#include <sstream>
#include <stdlib.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// DatabaseReport
//***************************************************************************

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
DatabaseReport::DatabaseReport() : Database()
{
}

//---------------------------------------------------------------------------
DatabaseReport::~DatabaseReport()
{
}

//---------------------------------------------------------------------------
void DatabaseReport::set_database_directory(const std::string& dirname)
{
    Database::set_database_directory(dirname);
}

//---------------------------------------------------------------------------
void DatabaseReport::set_database_filename(const std::string& filename)
{
    Database::set_database_filename(filename);
}

//---------------------------------------------------------------------------
void DatabaseReport::get_sql_query_for_create_report_table(std::string& q)
{
    std::stringstream create;
    create << "CREATE TABLE IF NOT EXISTS Report "; // Table name
    create << "(FILENAME              TEXT NOT NULL,";
    create << "FILE_LAST_MODIFICATION INT  NOT NULL,";
    create << "TOOL                   INT  NOT NULL,";
    create << "FORMAT                 INT  NOT NULL,";
    create << "REPORT                 TEXT         );";
    q = create.str();
}

//---------------------------------------------------------------------------
void DatabaseReport::get_sql_query_for_update_report_table_v0(std::string& q)
{
    std::stringstream create;
    create << "ALTER TABLE Report "; // Table name
    create << "ADD COMPRESS INT DEFAULT 0 NOT NULL;";
    q = create.str();
}

//---------------------------------------------------------------------------
void DatabaseReport::get_sql_query_for_update_report_table_v1(std::string& q)
{
    std::stringstream create;
    create << "ALTER TABLE Report "; // Table name
    create << "ADD HAS_MIL_VERSION INT DEFAULT 0 NOT NULL;";
    q = create.str();
}

//---------------------------------------------------------------------------
void DatabaseReport::get_sql_query_for_update_report_table_v2(std::string& q)
{
    std::stringstream create;
    create << "DELETE FROM Report;"; // Table name
    q = create.str();
}

//---------------------------------------------------------------------------
void DatabaseReport::get_sql_query_for_update_report_table_v3(std::string& q)
{
    std::stringstream create;
    create << "CREATE TABLE IF NOT EXISTS MEDIACONCH_FILE ";
    create << "(ID INTEGER PRIMARY KEY ASC,";
    create << " USER INT DEFAULT -1,";
    create << " FILENAME TEXT NOT NULL,";
    create << " FILE_LAST_MODIFICATION TEXT NOT NULL,";

    create << " ANALYZED INT DEFAULT 0,";

    create << " HAS_ERROR INT DEFAULT 0,";
    create << " ERROR_LOG TEXT,";

    create << " GENERATED_ID INT DEFAULT -1,";

    create << " SOURCE_ID INT DEFAULT -1,";
    create << " GENERATED_TIME INT DEFAULT -1,";
    create << " GENERATED_LOG TEXT,";
    create << " GENERATED_ERROR_LOG TEXT);";

    create << " CREATE TABLE IF NOT EXISTS MEDIACONCH_REPORT ";
    create << "(FILE_ID INT NOT NULL,";
    create << " TOOL INT NOT NULL,";
    create << " FORMAT INT NOT NULL,";
    create << " COMPRESS INT DEFAULT 0 NOT NULL,";
    create << " MIL_VERSION INT DEFAULT 0 NOT NULL,";
    create << " REPORT TEXT);";

    create << " DROP TABLE Report;";
    q = create.str();
}

//---------------------------------------------------------------------------
void DatabaseReport::get_sql_query_for_update_report_table_v4(std::string& q)
{
    std::stringstream create;
    create << "ALTER TABLE MEDIACONCH_FILE ";
    create << "ADD OPTIONS TEXT DEFAULT \"\" NOT NULL;";
    q = create.str();
}

//---------------------------------------------------------------------------
void DatabaseReport::get_sql_query_for_update_report_table_v5(std::string& q)
{
    std::stringstream create;
    create << "ALTER TABLE MEDIACONCH_FILE";
    create << " RENAME TO TMP;";

    create << "CREATE TABLE MEDIACONCH_FILE ";
    create << "(ID INTEGER PRIMARY KEY ASC,";
    create << " USER INT DEFAULT -1,";
    create << " FILENAME TEXT NOT NULL,";
    create << " FILE_LAST_MODIFICATION TEXT NOT NULL,";

    create << " ANALYZED INT DEFAULT 0,";

    create << " HAS_ERROR INT DEFAULT 0,";
    create << " ERROR_LOG TEXT,";

    create << " GENERATED_ID TEXT DEFAULT \"\",";

    create << " SOURCE_ID INT DEFAULT -1,";
    create << " GENERATED_TIME INT DEFAULT -1,";
    create << " GENERATED_LOG TEXT,";
    create << " GENERATED_ERROR_LOG TEXT,";
    create << " OPTIONS TEXT DEFAULT \"\" NOT NULL);";

    create << "INSERT INTO MEDIACONCH_FILE(USER, FILENAME, FILE_LAST_MODIFICATION, ANALYZED, HAS_ERROR, ERROR_LOG,";
    create << " GENERATED_ID, SOURCE_ID, GENERATED_TIME, GENERATED_LOG, GENERATED_ERROR_LOG, OPTIONS)";

    create << " SELECT USER, FILENAME, FILE_LAST_MODIFICATION, ANALYZED, HAS_ERROR, ERROR_LOG,";
    create << " GENERATED_ID, SOURCE_ID, GENERATED_TIME, GENERATED_LOG, GENERATED_ERROR_LOG, OPTIONS";
    create << " FROM TMP;";

    create << "DROP TABLE TMP;";

    q = create.str();
}

//---------------------------------------------------------------------------
void DatabaseReport::get_sql_query_for_update_report_table_v6(std::string& q)
{
    std::stringstream create;
    create << "ALTER TABLE MEDIACONCH_REPORT";
    create << " ADD OPTIONS TEXT DEFAULT \"\";";

    q = create.str();
}

void DatabaseReport::longs_to_string(const std::vector<long>& arr, std::string& str)
{
    std::stringstream ss;

    for (size_t i = 0; i < arr.size(); ++i)
    {
        if (i)
            ss << ",";
        ss << arr[i];
    }
    str = ss.str();
}

void DatabaseReport::string_to_longs(const std::string& str, std::vector<long>& arr)
{
    size_t start = 0;
    size_t pos = 0;
    while (pos != std::string::npos)
    {
        if (pos >= str.size())
        {
            pos = std::string::npos;
            continue;
        }

        start = pos;
        pos = str.find(",", start);

        std::string nb;
        if (pos == std::string::npos)
            nb = str.substr(start);
        else
        {
            nb = str.substr(start, pos - start);
            pos += 1;
        }

        if (!nb.size() || nb == "-1")
            continue;

        char *end = NULL;
        arr.push_back(strtol(nb.c_str(), &end, 10));
    }
}

}
