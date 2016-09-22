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

    //Init
    virtual int create_report_table();
    virtual int update_report_table();

    //File
    virtual long add_file(const std::string& filename, const std::string& file_last_modification,
                          std::string& err,
                          long source_id=-1,
                          const std::string& generated_file="", const std::string& log="");
    virtual long get_file_id(const std::string& file, const std::string& file_last_modification);
    virtual void get_file_from_id(long id, std::string& file);

    // Report
    virtual int  save_report(long file_id, MediaConchLib::report reportKind, MediaConchLib::format format,
                             const std::string& report, MediaConchLib::compression, int);
    virtual int  remove_report(long file_id);
    virtual void get_report(long file_id, MediaConchLib::report reportKind, MediaConchLib::format format,
                            std::string& report, MediaConchLib::compression&);
    virtual bool report_is_registered(long file_id, MediaConchLib::report reportKind, MediaConchLib::format format);
    virtual int  version_registered(long file_id);
    virtual void get_elements(std::vector<std::string>& vec);
    virtual void get_element_report_kind(long file_id, MediaConchLib::report& report_kind);

protected:
    virtual int  execute();

private:
    NoDatabaseReport (const NoDatabaseReport&);
    NoDatabaseReport& operator=(const NoDatabaseReport&);

    struct MC_File
    {
        std::string filename;
        std::string file_last_modification;
        long        source_id;
        std::string generated_file;
        std::string log;
    };

    struct MC_Report
    {
        MediaConchLib::report      reportKind;
        MediaConchLib::format      format;
        MediaConchLib::compression compression;
        std::string                report;
        int                        mil_version;
    };

    std::vector<MC_File*> files_saved;
    std::map<long, std::vector<MC_Report*> > reports_saved;
};

}

#endif /* !NODATABASEREPORT */
