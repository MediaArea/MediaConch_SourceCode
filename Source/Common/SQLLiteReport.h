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
    int          get_file_ids(long user, std::vector<long>& ids, std::string& err);

    // File
    virtual long add_file(int user, const std::string& filename, const std::string& file_last_modification,
                          const std::string& options, std::string& err,
                          const std::vector<long>& generated_id, long source_id=-1, size_t generated_time=(size_t)-1,
                          const std::string& generated_log="", const std::string& generated_error_log="");
    virtual long update_file(int user, long file_id, const std::string& file_last_modification,
                             const std::string& options, std::string& err,
                             const std::vector<long>& generated_id, long source_id=-1, size_t generated_time=(size_t)-1,
                             const std::string& generated_log="", const std::string& generated_error_log="");
    virtual long get_file_id(int user, const std::string& file, const std::string& file_last_modification,
                             const std::string& options, std::string& err);
    virtual int  get_file_name_from_id(int user, long id, std::string& file, std::string& err);
    virtual int  get_file_information_from_id(int user, long id, std::string& filename, std::string& file_last_modification,
                                              std::vector<long>& generated_id, long& source_id, size_t& generated_time,
                                              std::string& generated_log, std::string& generated_error_log,
                                              std::string& options,
                                              bool& analyzed, bool& has_error, std::string& error_log,
                                              std::string& err);
    virtual bool file_is_analyzed(int user, long id, std::string& err);
    virtual int  reset_file(int user, long file, std::string& err);
    virtual int  remove_file(int user, long file, std::string& err);
    virtual int  remove_all_files(int user, std::string& err);
    virtual int  add_file_generated_id(int user, long source_id, long generated_id, std::string& err);
    virtual int  update_file_analyzed(int user, long id, std::string& err, bool analyzed=true);
    virtual int  update_file_error(int user, long id, std::string& err, bool has_error=true, const std::string& error_log="");

    // Report
    virtual int  save_report(int user, long file_id, MediaConchLib::report reportKind, MediaConchLib::format format,
                             const std::string& options,
                             const std::string& report, MediaConchLib::compression,
                             int mil_version, std::string& err);
    virtual int  update_report(int user, long file_id, MediaConchLib::report reportKind, MediaConchLib::format format,
                               const std::string& options,
                               const std::string& report, MediaConchLib::compression,
                               int mil_version, std::string& err);
    virtual int  remove_report(int user, long filename, std::string& err);
    virtual int  remove_all_reports(int user, std::string& err);
    virtual int  get_report(int user, long file_id, MediaConchLib::report reportKind, MediaConchLib::format format,
                            const std::string& options,
                            std::string& report, MediaConchLib::compression&, std::string& err);
    virtual int  report_is_registered(int user, long file_id, MediaConchLib::report reportKind,
                                      MediaConchLib::format format, const std::string& options,
                                      bool& registered, std::string& err);
    virtual int  version_registered(int user, long file_id, std::string& err);
    virtual int  get_elements(int user, std::vector<std::string>& vec, std::string& err);
    virtual int  get_elements(int user, std::vector<long>& vec, std::string& err);
    virtual int  get_element_report_kind(int user, long file_id, MediaConchLib::report& report_kind, std::string& err);

protected:
    virtual int init();
    virtual int init_report();

private:
    int           report_version;
    static int    current_report_version;

    bool          file_id_match_user(int user, long file_id, std::string& err);
    int           get_generated_id(int user, long id, std::vector<long>& generated_id, std::string& err);

    SQLLiteReport (const SQLLiteReport&);
    SQLLiteReport& operator=(const SQLLiteReport&);
};

}

#endif /* !SQLLiteReportH */

#endif /* !HAVE_SQLITE */
