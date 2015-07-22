/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Database functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef DatabaseH
#define DatabaseH
//---------------------------------------------------------------------------

#include "Core.h"
//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// Class Database
//***************************************************************************

const String databaseName = __T("MediaConch.db");

class Database
{
public:
    //Constructor/Destructor
    Database();
    virtual ~Database();

    int    create_report_table();
    int    save_report(Core::format format, Core::report reportKind, String& filename, time_t file_last_modification, String& report);
    String report_exists(Core::format format, Core::report reportKind, String& filename, time_t file_last_modification);

protected:
    String query;
    vector<String> errors;
    vector<String> reports;

    //Database dependant
    virtual int execute() = 0;
    virtual int init() = 0;

private:
    Database (const Database&);
    Database& operator=(const Database&);
};

}

#endif
