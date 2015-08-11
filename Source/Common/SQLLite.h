/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
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

#include "Database.h"
#include <sqlite3.h>
//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// Class SQLLite
//***************************************************************************

class SQLLite : public Database
{
public:
    //Constructor/Destructor
    SQLLite();
    virtual ~SQLLite();

protected:
    void        add_report(std::string key, std::string report);
    virtual int execute();
    virtual int init();

private:
    sqlite3 *db;

    static int callback(void* data, int , char**, char**);

    SQLLite (const SQLLite&);
    SQLLite& operator=(const SQLLite&);
};

}

#endif /* !SQLLiteH */

#endif /* !HAVE_SQLITE */
