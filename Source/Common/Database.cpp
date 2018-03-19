/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "Database.h"
#include <sstream>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Database
//***************************************************************************

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Database::Database()
{
}

//---------------------------------------------------------------------------
Database::~Database()
{
}

//---------------------------------------------------------------------------
void Database::set_database_directory(const std::string& dirname)
{
    db_dirname = dirname;
    if (db_dirname.length() && db_dirname[db_dirname.length() - 1] != '/')
        db_dirname += "/";
}

//---------------------------------------------------------------------------
void Database::set_database_filename(const std::string& filename)
{
    db_filename = filename;
}

}
