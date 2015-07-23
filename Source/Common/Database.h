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

const std::string databaseName = "MediaConch.db";

class Database
{
public:
    //Constructor/Destructor
    Database();
    virtual ~Database();

    int         create_report_table();
    int         save_report(Core::report reportKind, Core::format format, const std::string& filename, time_t file_last_modification, const std::string& report);
    std::string get_report(Core::report reportKind, Core::format format, const std::string& filename, time_t file_last_modification);
    bool        file_is_registered(const std::string& file, time_t file_last_modification);

protected:
    std::string                        query;
    std::vector<std::string>           errors;
    std::map<std::string, std::string> reports;

    //Database dependant
    virtual int execute() = 0;
    virtual int init() = 0;

private:
    Database (const Database&);
    Database& operator=(const Database&);
};

}

#endif
