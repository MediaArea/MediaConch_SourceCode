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
#include "SQLLite.h"
#include <sstream>
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
    if (init())
    {
        db = NULL;
        return;
    }
    create_report_table();
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
    int ret;
    std::string database(databaseName.begin(), databaseName.end());

    ret = sqlite3_open(database.c_str(), &db);
    if (ret)
    {
        fprintf(stderr, "Error to open the DB: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }
    return 0;
}

//---------------------------------------------------------------------------
int SQLLite::execute()
{
    if (!db)
        return -1;

    errors.clear();
    char *error = NULL;

    int ret = sqlite3_exec(db, query.c_str(), callback, this, &error);
    if (ret != SQLITE_OK)
    {
        errors.push_back(std::string(error));
        sqlite3_free(error); //copy
    }

    return ret;
}

//---------------------------------------------------------------------------
int SQLLite::callback(void* data, int columns, char **column_texts, char** column_name)
{
    SQLLite *ptr = (SQLLite*)data;

    for (int i = 0; i < columns; ++i)
        ptr->add_report(column_name[i], column_texts[i]);
    return 0;
}

//---------------------------------------------------------------------------
void SQLLite::add_report(std::string key, std::string report)
{
    reports[key] = report;
}

}
