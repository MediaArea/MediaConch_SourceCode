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

    //Init
    virtual int create_report_table();
    virtual int update_report_table();

    //User
    virtual void get_users_id(std::vector<long>& ids, std::string& err);

    // File
    virtual long add_file(int user, const std::string& filename, const std::string& file_last_modification,
                          std::string& err,
                          long generated_id=-1, long source_id=-1, size_t generated_time=(size_t)-1,
                          const std::string& generated_log="", const std::string& generated_error_log="");
    virtual long update_file(int user, long file_id, const std::string& file_last_modification,
                             std::string& err,
                             long generated_id=-1, long source_id=-1, size_t generated_time=(size_t)-1,
                             const std::string& generated_log="", const std::string& generated_error_log="");
    virtual long get_file_id(int user, const std::string& file, const std::string& file_last_modification);
    virtual void get_file_name_from_id(int user, long id, std::string& file);
    virtual void get_file_information_from_id(int user, long id, std::string& filename, std::string& file_last_modification,
                                              long& generated_id, long& source_id, size_t& generated_time,
                                              std::string& generated_log, std::string& generated_error_log,
                                              bool& analyzed, bool& has_error, std::string& error_log);
    virtual bool file_is_analyzed(int user, long id);
    virtual int  update_file_generated_id(int user, long source_id, long generated_id);
    virtual int  update_file_analyzed(int user, long id, bool analyzed=true);
    virtual int  update_file_error(int user, long id, bool has_error=true, const std::string& error_log="");

    // Report
    virtual int save_report(int user, long file_id, MediaConchLib::report reportKind, MediaConchLib::format format,
                            const std::string& report, MediaConchLib::compression,
                            int mil_version);
    virtual int update_report(int user, long file_id, MediaConchLib::report reportKind, MediaConchLib::format format,
                              const std::string& report, MediaConchLib::compression,
                              int mil_version);
    virtual int remove_report(int user, long filename);
    virtual void get_report(int user, long file_id, MediaConchLib::report reportKind, MediaConchLib::format format,
                            std::string& report, MediaConchLib::compression&);
    virtual bool report_is_registered(int user, long file_id, MediaConchLib::report reportKind, MediaConchLib::format format);
    virtual int  version_registered(int user, long file_id);
    virtual void get_elements(int user, std::vector<std::string>& vec);
    virtual void get_element_report_kind(int user, long file_id, MediaConchLib::report& report_kind);

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
