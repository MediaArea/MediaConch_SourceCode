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
#ifndef SQLLiteReportH
#define SQLLiteReportH
//---------------------------------------------------------------------------

#include "DatabaseReport.h"
#include "SQLLite.h"
#include <sqlite3.h>
//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// Class SQLLiteReport
//***************************************************************************

class SQLLiteReport : public DatabaseReport, public SQLLite
{
public:
    //Constructor/Destructor
    SQLLiteReport();
    virtual ~SQLLiteReport();

    // Report
    virtual int create_report_table();
    virtual int update_report_table();
    virtual int save_report(MediaConchLib::report reportKind, MediaConchLib::format format,
                            const std::string& filename, const std::string& file_last_modification,
                            const std::string& report, MediaConchLib::compression);
    virtual int update_report(MediaConchLib::report reportKind, MediaConchLib::format format,
                              const std::string& filename, const std::string& file_last_modification,
                              const std::string& report, MediaConchLib::compression);
    virtual int remove_report(const std::string& filename);
    virtual void get_report(MediaConchLib::report reportKind, MediaConchLib::format format,
                            const std::string& filename, const std::string& file_last_modification,
                            std::string& report, MediaConchLib::compression&);
    virtual bool file_is_registered(MediaConchLib::report reportKind, MediaConchLib::format format,
                                    const std::string& filename, const std::string& file_last_modification);
    virtual bool file_is_registered(MediaConchLib::report reportKind, MediaConchLib::format format,
                                    const std::string& filename);
    virtual void get_elements(std::vector<std::string>& vec);
    virtual void get_element_report_kind(const std::string& file, MediaConchLib::report& report_kind);

protected:
    virtual int init();
    virtual int init_report();

private:
    int           report_version;
    static int    current_report_version;

    SQLLiteReport (const SQLLiteReport&);
    SQLLiteReport& operator=(const SQLLiteReport&);
};

}

#endif /* !SQLLiteReportH */

#endif /* !HAVE_SQLITE */
