/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// NoDatabase functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef NODATABASEH
#define NODATABASEH

//---------------------------------------------------------------------------
#include "Database.h"

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Class NoDatabase
//***************************************************************************

class NoDatabase : public Database
{
public:
    //Constructor/Destructor
    NoDatabase();
    virtual ~NoDatabase();

    virtual int init();
    virtual int create_report_table();
    virtual int save_report(Core::report reportKind, Core::format format, const std::string& filename, time_t file_last_modification, const std::string& report);
    virtual std::string get_report(Core::report reportKind, Core::format format, const std::string& filename, time_t file_last_modification);
    virtual bool file_is_registered(Core::report reportKind, Core::format format, const std::string& filename, time_t file_last_modification);

protected:
    virtual int execute();

private:
    NoDatabase (const NoDatabase&);
    NoDatabase& operator=(const NoDatabase&);

    struct Report
    {
        Core::report reportKind;
        Core::format format;
        time_t file_last_modification;
        std::string report;
    };

    std::map<std::string, std::vector<Report*> > reports_saved;
};

}

#endif /* !NODATABASEH */
