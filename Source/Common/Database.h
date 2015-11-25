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

#include <map>
#include "MediaConchLib.h"
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

    virtual int create_report_table() = 0;
    virtual int save_report(MediaConchLib::report reportKind, MediaConchLib::format format, const std::string& filename, const std::string& file_last_modification, const std::string& report) = 0;
    virtual std::string get_report(MediaConchLib::report reportKind, MediaConchLib::format format, const std::string& filename, const std::string& file_last_modification) = 0;
    virtual bool file_is_registered(MediaConchLib::report reportKind, MediaConchLib::format format, const std::string& file, const std::string& file_last_modification) = 0;

    void        set_database_directory(const std::string& dirname);
    virtual int init() = 0;

protected:
    std::string                        query;
    std::vector<std::string>           errors;
    std::map<std::string, std::string> reports;
    std::string                        db_file;

    //Database dependant
    virtual int execute() = 0;

private:
    Database (const Database&);
    Database& operator=(const Database&);
};

}

#endif
