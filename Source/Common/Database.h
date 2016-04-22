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
#include <vector>
#include "MediaConchLib.h"
//---------------------------------------------------------------------------

namespace MediaConch {

class FileRegistered;

//***************************************************************************
// Class Database
//***************************************************************************

class Database
{
public:
    //Constructor/Destructor
    Database();
    virtual ~Database();

    void        set_database_directory(const std::string& dirname);
    void        set_database_filename(const std::string& name);

    virtual int init() = 0;

    const std::vector<std::string>& get_errors() const { return errors; }

protected:
    std::vector<std::string>           errors;
    std::string                        db_filename;
    std::string                        db_dirname;

private:
    Database (const Database&);
    Database& operator=(const Database&);
};

}

#endif
