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
#ifndef DatabaseReportH
#define DatabaseReportH
//---------------------------------------------------------------------------

#include <map>
#include <vector>
#include "MediaConchLib.h"
#include "Database.h"
//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// Class Database
//***************************************************************************

class DatabaseReport : public Database
{
public:
    //Constructor/Destructor
    DatabaseReport();
    virtual ~DatabaseReport();

    void        set_database_directory(const std::string& dirname);
    void        set_database_filename(const std::string& name);

    // Init
    virtual int create_report_table() = 0;
    virtual int update_report_table() = 0;

    // File
    virtual long add_file(int user, const std::string& filename, const std::string& file_last_modification,
                          std::string& err,
                          long generated_id = -1,
                          long source_id=-1, size_t generated_time=(size_t)-1,
                          const std::string& generated_log="", const std::string& generated_error_log="") = 0;
    virtual long update_file(int user, long file_id, const std::string& file_last_modification,
                             std::string& err,
                             long generated_id = -1, long source_id=-1, size_t generated_time=(size_t)-1,
                             const std::string& generated_log="", const std::string& generated_error_log="") = 0;
    virtual long get_file_id(int user, const std::string& file, const std::string& file_last_modification) = 0;
    virtual void get_file_name_from_id(int user, long id, std::string& file) = 0;
    virtual void get_file_information_from_id(int user, long id, std::string& filename, std::string& file_last_modification,
                                              long& generated_id, long& source_id, size_t& generated_time,
                                              std::string& generated_log, std::string& generated_error_log,
                                              bool& analyzed, bool& has_error, std::string& error_log) = 0;
    virtual bool file_is_analyzed(int user, long id) = 0;
    virtual int  update_file_generated_id(int user, long source_id, long generated_id) = 0;
    virtual int  update_file_analyzed(int user, long id, bool analyzed=true) = 0;
    virtual int  update_file_error(int user, long id, bool has_error=true, const std::string& error_log="") = 0;

    // Report
    virtual int save_report(int user, long file_id, MediaConchLib::report reportKind, MediaConchLib::format format,
                            const std::string& report, MediaConchLib::compression,
                            int mil_version) = 0;
    virtual int remove_report(int user, long file_id) = 0;
    virtual void get_report(int user, long file_id, MediaConchLib::report reportKind, MediaConchLib::format format,
                            std::string& report, MediaConchLib::compression&) = 0;
    virtual bool report_is_registered(int user, long file_id, MediaConchLib::report reportKind, MediaConchLib::format format) = 0;
    virtual int  version_registered(int user, long file_id) = 0;
    virtual void get_elements(int user, std::vector<std::string>& vec) = 0;
    virtual void get_element_report_kind(int user, long file_id, MediaConchLib::report& report_kind) = 0;

    virtual int init_report() = 0;

protected:
    //Database dependant
    void        get_sql_query_for_create_report_table(std::string& q);
    void        get_sql_query_for_update_report_table_v0(std::string& q);
    void        get_sql_query_for_update_report_table_v1(std::string& q);
    void        get_sql_query_for_update_report_table_v2(std::string& q);
    void        get_sql_query_for_update_report_table_v3(std::string& q);

private:
    DatabaseReport (const DatabaseReport&);
    DatabaseReport& operator=(const DatabaseReport&);
};

}

#endif
