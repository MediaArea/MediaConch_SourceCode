/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Core functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef ReportH
#define ReportH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaConchLib.h"
#include <bitset>
#include <vector>
#include <map>

//---------------------------------------------------------------------------
namespace MediaConch {

class Core;
class Schema;

//***************************************************************************
// Struct Checker Report
//***************************************************************************

struct CheckerReport
{
    CheckerReport() : display_name(NULL), display_content(NULL), user(-1), format(MediaConchLib::format_Xml) {}

    std::bitset<MediaConchLib::report_Max>  report_set;
    std::vector<long>                       files;
    std::map<std::string, std::string>      options;
    std::vector<size_t>                     policies_ids;
    std::vector<std::string>                policies_contents;
    std::string                            *display_name;
    std::string                            *display_content;
    int                                     user;
    MediaConchLib::format                   format;
};

//***************************************************************************
// Class Report
//***************************************************************************

class Reports
{
public:
    // Constructor/Destructor
    Reports(Core *c);
    ~Reports();

    // Reports
    int   checker_get_report(CheckerReport& c_report, MediaConchLib::Checker_ReportRes* result, std::string& err);
    int   checker_validate(int user, MediaConchLib::report report,
                           const std::vector<long>& file,
                           const std::vector<size_t>& policies_ids,
                           const std::vector<std::string>& policies_contents,
                           const std::map<std::string, std::string>& options,
                           std::vector<MediaConchLib::Checker_ValidateRes*>& result, std::string& error);

    // Reports Helpers
    int   get_reports_output(int user, const std::vector<long>& files,
                             const std::map<std::string, std::string>& options,
                             MediaConchLib::format f,
                             std::bitset<MediaConchLib::report_Max> report_set,
                             MediaConchLib::Checker_ReportRes* result, std::string& err);
    int   get_reports_output_JStree(int user, const std::vector<long>& files,
                                    const std::bitset<MediaConchLib::report_Max>& report_set,
                                    std::string& report, std::string& err);
    int   get_implementation_reports(int user, const std::vector<long>& files,
                                     const std::map<std::string, std::string>& options,
                                     std::string& report, bool& valid, std::string& err);
    int   get_implementation_report(int user, long file, const std::map<std::string, std::string>& options,
                                    std::string& report, bool& valid, std::string& err);
    int   get_verapdf_report(int user, long file, std::string& report, bool& valid, std::string& err);
    int   get_dpfmanager_report(int user, long file, std::string& report, bool& valid, std::string& err);

    // Reports change
    int   create_report_mi_xml(int user, const std::vector<long>& filename, std::string& report, std::string& err);
    int   create_report_mt_xml(int user, const std::vector<long>& filename, std::string& report, std::string& err);
    int   create_report_mmt_xml(int user, const std::vector<long>& filename, std::string& report, std::string& err);
    int   create_report_ma_xml(int user, const std::vector<long>& files, const std::map<std::string, std::string>& options,
                               std::string& report, std::bitset<MediaConchLib::report_Max> reports,
                               std::string& err);
    void  get_content_of_media_in_xml(std::string& report);

    // Reports validity
    bool  implementation_is_valid(const std::string& report);
    bool  policy_is_valid(const std::string& report);
    bool  verapdf_report_is_valid(const std::string& report);
    bool  dpfmanager_report_is_valid(const std::string& report);

    // Policies
    int   check_policies(int user, const std::vector<long>& files,
                         const std::map<std::string, std::string>& options,
                         MediaConchLib::Checker_ReportRes *result, std::string& error,
                         const std::vector<size_t>* policies_ids = NULL,
                         const std::vector<std::string>* policies_contents = NULL);
    int   check_policies_xslts(int user, const std::vector<long>& files,
                               const std::map<std::string, std::string>& options,
                               const std::vector<std::string>& policies,
                               std::stringstream& Out, bool& valid, std::string& err);

    // Display
    int   transform_with_xslt_file(const std::string& report, const std::string& Xslt,
                                   const std::map<std::string, std::string>& opts, std::string& result);
    int   transform_with_xslt_memory(const std::string& report, const std::string& memory,
                                     const std::map<std::string, std::string>& opts, std::string& result);
    int   transform_with_xslt_text_memory(const std::string& report, std::string& result);
    int   transform_with_xslt_html_memory(const std::string& report, std::string& result);
    int   transform_with_xslt_simple_memory(const std::string& report, std::string& result);
    int   transform_with_xslt_csv_memory(const std::string& report, std::string& result);

    // Validation
    int   validate_xslt_from_memory(int user, const std::vector<long>& files, const std::map<std::string, std::string>& opts,
                                    const std::string& memory, bool is_implem, std::string& report, bool& valid, std::string& err);
    int   validation(int user, const std::vector<long>& files, Schema* S,
                     std::string& report, bool& valid, std::string& err);

    // Helper
    std::bitset<MediaConchLib::report_Max> get_bitset_with_mi_mt();
    std::bitset<MediaConchLib::report_Max> get_bitset_with_mi_mmt();
    void  unify_implementation_options(std::map<std::string, std::string>& opts);
    void  unify_policy_options(int user, std::map<std::string, std::string>& opts, std::string& err);

private:
    Core *core;

    void  xml_escape_attributes(std::string& xml);
};

}

#endif
