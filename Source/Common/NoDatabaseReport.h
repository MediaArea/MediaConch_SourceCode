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
#ifndef NODATABASEREPORTH
#define NODATABASEREPORTH

//---------------------------------------------------------------------------
#include "DatabaseReport.h"

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Class NoDatabaseReport
//***************************************************************************

class NoDatabaseReport : public DatabaseReport
{
public:
    //Constructor/Destructor
    NoDatabaseReport();
    virtual ~NoDatabaseReport();

    virtual int init();
    virtual int init_report();

    // Report
    virtual int create_report_table();
    virtual int update_report_table();
    virtual int save_report(MediaConchLib::report reportKind, MediaConchLib::format format, const std::string& filename, const std::string& file_last_modification, const std::string& report, MediaConchLib::compression, bool);
    virtual int remove_report(const std::string& filename);
    virtual void get_report(MediaConchLib::report reportKind, MediaConchLib::format format,
                            const std::string& filename, const std::string& file_last_modification,
                            std::string& report, MediaConchLib::compression&);
    virtual bool file_is_registered(MediaConchLib::report reportKind, MediaConchLib::format format, const std::string& filename, const std::string& file_last_modification);
    virtual bool file_is_registered(MediaConchLib::report reportKind, MediaConchLib::format format, const std::string& filename);
    virtual bool has_version_registered(const std::string& file);
    virtual void get_elements(std::vector<std::string>& vec);
    virtual void get_element_report_kind(const std::string& file, MediaConchLib::report& report_kind);

protected:
    virtual int execute();

private:
    NoDatabaseReport (const NoDatabaseReport&);
    NoDatabaseReport& operator=(const NoDatabaseReport&);

    struct Report
    {
        MediaConchLib::report reportKind;
        MediaConchLib::format format;
        MediaConchLib::compression compression;
        std::string file_last_modification;
        std::string report;
        bool        has_mil_version;
    };

    std::map<std::string, std::vector<Report*> > reports_saved;
};

}

#endif /* !NODATABASEREPORT */
