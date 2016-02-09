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
    virtual int update_report_table();
    virtual int save_report(MediaConchLib::report reportKind, MediaConchLib::format format, const std::string& filename, const std::string& file_last_modification, const std::string& report, MediaConchLib::compression);
    virtual int remove_report(const std::string& filename);
    virtual void get_report(MediaConchLib::report reportKind, MediaConchLib::format format,
                            const std::string& filename, const std::string& file_last_modification,
                            std::string& report, MediaConchLib::compression&);
    virtual bool file_is_registered(MediaConchLib::report reportKind, MediaConchLib::format format, const std::string& filename, const std::string& file_last_modification);
    virtual void get_elements(std::vector<std::string>& vec);

protected:
    virtual int execute();

private:
    NoDatabase (const NoDatabase&);
    NoDatabase& operator=(const NoDatabase&);

    struct Report
    {
        MediaConchLib::report reportKind;
        MediaConchLib::format format;
        std::string file_last_modification;
        std::string report;
    };

    std::map<std::string, std::vector<Report*> > reports_saved;
};

}

#endif /* !NODATABASEH */
