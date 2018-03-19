/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// SQLLite functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef HAVE_SQLITE

//---------------------------------------------------------------------------
#ifndef SQLLiteH
#define SQLLiteH
//---------------------------------------------------------------------------

#include <sqlite3.h>
#include <vector>
#include <map>
#include <string>
//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// Class SQLLite
//***************************************************************************

class SQLLite
{
public:
    //Constructor/Destructor
    SQLLite();
    virtual ~SQLLite();

protected:
    virtual int execute();
    virtual int init(const std::string& db_dirname, const std::string& db_filename);

    std::string                                      query;
    std::vector<std::map<std::string, std::string> > reports;
    std::string                                      error;
    sqlite3                                         *db;
    sqlite3_stmt                                    *stmt; // Statement handler

    // Helper
    long                std_string_to_int(const std::string& str);
    size_t              std_string_to_uint(const std::string& str);
    int                 get_db_version(int& version);
    int                 set_db_version(int version);
    const std::string&  get_error() const;
    std::string         get_sqlite_error(int err);
    int                 prepare_v2(std::string& query, std::string& err);

    SQLLite (const SQLLite&);
    SQLLite& operator=(const SQLLite&);
};

}

#endif /* !SQLLiteH */

#endif /* !HAVE_SQLITE */
