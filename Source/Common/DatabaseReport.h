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

    virtual int create_report_table() = 0;
    virtual int update_report_table() = 0;
    virtual int save_report(MediaConchLib::report reportKind, MediaConchLib::format format,
                            const std::string& filename, const std::string& file_last_modification,
                            const std::string& report, MediaConchLib::compression,
                            bool has_mil_version) = 0;
    virtual int remove_report(const std::string& filename) = 0;
    virtual void get_report(MediaConchLib::report reportKind, MediaConchLib::format format,
                            const std::string& filename, const std::string& file_last_modification,
                            std::string& report, MediaConchLib::compression&) = 0;
    virtual bool file_is_registered(MediaConchLib::report reportKind, MediaConchLib::format format,
                                    const std::string& file, const std::string& file_last_modification) = 0;
    virtual bool file_is_registered(MediaConchLib::report reportKind, MediaConchLib::format format,
                                    const std::string& file) = 0;
    virtual bool has_version_registered(const std::string& file) = 0;
    virtual void get_elements(std::vector<std::string>& vec) = 0;
    virtual void get_element_report_kind(const std::string& file, MediaConchLib::report& report_kind) = 0;

    virtual int init_report() = 0;

protected:
    //Database dependant
    void        get_sql_query_for_create_report_table(std::string& q);
    void        get_sql_query_for_update_report_table_v0(std::string& q);
    void        get_sql_query_for_update_report_table_v1(std::string& q);

private:
    DatabaseReport (const DatabaseReport&);
    DatabaseReport& operator=(const DatabaseReport&);
};

}

#endif
