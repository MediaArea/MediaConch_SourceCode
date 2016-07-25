/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// RESTAPI functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef RESTAPIH
#define RESTAPIH
//---------------------------------------------------------------------------

#include <string>
#include <vector>
#include "Container.h"

//---------------------------------------------------------------------------

namespace MediaConch {

//---------------------------------------------------------------------------

//***************************************************************************
// Class RESTAPI
//***************************************************************************

class RESTAPI
{
public:

    static const std::string API_VERSION;

    enum Reason
    {
        NO_REASON = 0,
        FILE_NOT_EXISTING,
        ID_NOT_EXISTING,
        NOT_READY,
    };

    enum Report
    {
        NO_REPORT = 0,
        POLICY,
        IMPLEMENTATION,
        MEDIAINFO,
        MEDIATRACE,
        VERAPDF,
        DPFMANAGER,
    };

    std::string get_Report_string(Report r)
    {
#define ReportString(report) \
        case report:         \
            return #report;

        switch (r)
        {
            ReportString(POLICY);
            ReportString(IMPLEMENTATION);
            ReportString(MEDIAINFO);
            ReportString(MEDIATRACE);
            ReportString(VERAPDF);
            ReportString(DPFMANAGER);
            default:
                break;
        }
#undef ReportString
        return std::string();
    }

    Report string_to_Report(const std::string& str)
    {
#define ReportString(report)       \
        if (!str.compare(#report)) \
            return report;

        ReportString(POLICY);
        ReportString(IMPLEMENTATION);
        ReportString(MEDIAINFO);
        ReportString(MEDIATRACE);
        ReportString(VERAPDF);
        ReportString(DPFMANAGER);

#undef ReportString

        return NO_REPORT;
    }

    std::string get_Reason_string(Reason r)
    {
#define ReasonString(reason) \
        case reason:         \
            return #reason;

        switch (r)
        {
            ReasonString(NO_REASON);
            ReasonString(FILE_NOT_EXISTING);
            ReasonString(ID_NOT_EXISTING);
            ReasonString(NOT_READY);
            default:
                break;
        }
#undef ReasonString
        return std::string();
    }

    Reason string_to_Reason(const std::string& str)
    {
#define ReasonString(reason)       \
        if (!str.compare(#reason)) \
            return reason;

        ReasonString(NO_REASON);
        ReasonString(FILE_NOT_EXISTING);
        ReasonString(ID_NOT_EXISTING);
        ReasonString(NOT_READY);

#undef ReasonString

        return NO_REASON;
    }

    // Analyze
    struct Checker_Analyze_Arg
    {
        Checker_Analyze_Arg() : has_force_analyze(false) {}
        std::string             to_str() const;
        std::string             file;
        int                     id;
        bool                    has_force_analyze;
        bool                    force_analyze;
    };

    struct Checker_Analyze_Req
    {
        std::vector<Checker_Analyze_Arg> args;
        std::string                      to_str() const;
    };

    struct Checker_Analyze_Ok
    {
        int                    inId;
        int                    outId;
        bool                   create;
        std::string            to_str() const;
    };

    struct Checker_Analyze_Nok
    {
        int                    id;
        Reason                 error;
        std::string            to_str() const;
    };

    struct Checker_Analyze_Res
    {
        ~Checker_Analyze_Res();
        std::vector<Checker_Analyze_Ok*>  ok;
        std::vector<Checker_Analyze_Nok*> nok;
        std::string                       to_str() const;
    };

    // Status
    struct Checker_Status_Req
    {
        std::vector<int>        ids;
        std::string             to_str() const;
    };

    struct Checker_Status_Ok
    {
        Checker_Status_Ok() : finished(false), has_percent(false), has_tool(false) {}
        int                     id;
        bool                    finished;
        bool                    has_percent;
        double                  done;
        bool                    has_tool;
        Report                  tool;
        std::string             to_str() const;
    };

    struct Checker_Status_Nok
    {
        int                     id;
        Reason                  error;
        std::string             to_str() const;
    };

    struct Checker_Status_Res
    {
        ~Checker_Status_Res();
        std::vector<Checker_Status_Ok*>  ok;
        std::vector<Checker_Status_Nok*> nok;
        std::string                      to_str() const;
    };

    // Report
    struct Checker_Report_Req
    {
        Checker_Report_Req() :   has_verbosity(false) {}
        std::vector<int>         ids;
        std::vector<Report>      reports;
        std::vector<size_t>      policies_ids;
        std::vector<std::string> policies_contents;
        std::string              display_name;
        std::string              display_content;
        bool                     has_verbosity;
        int                      verbosity;
        std::string              to_str() const;
    };

    struct Checker_Report_Ok
    {
        Checker_Report_Ok() :   has_valid(false) {}
        std::string             report;
        bool                    has_valid;
        bool                    valid;
        std::string             to_str() const;
    };

    struct Checker_Report_Nok
    {
        int                     id;
        Reason                  error;
        std::string             to_str() const;
    };

    struct Checker_Report_Res
    {
        ~Checker_Report_Res();
        Checker_Report_Ok                ok;
        std::vector<Checker_Report_Nok*> nok;
        std::string                      to_str() const;
    };

    // Retry
    struct Checker_Retry_Req
    {
        std::vector<int>        ids;
        std::string             to_str() const;
    };

    struct Checker_Retry_Nok
    {
        int                     id;
        Reason                  error;
        std::string             to_str() const;
    };

    struct Checker_Retry_Res
    {
        ~Checker_Retry_Res();
        std::vector<int>                ok;
        std::vector<Checker_Retry_Nok*> nok;
        std::string                     to_str() const;
    };

    // Clear
    struct Checker_Clear_Req
    {
        std::vector<int>        ids;
        std::string             to_str() const;
    };

    struct Checker_Clear_Nok
    {
        int                     id;
        Reason                  error;
        std::string             to_str() const;
    };

    struct Checker_Clear_Res
    {
        ~Checker_Clear_Res();
        std::vector<int>                ok;
        std::vector<Checker_Clear_Nok*> nok;
        std::string                     to_str() const;
    };

    // List
    struct Checker_List_Req
    {
    };

    struct Checker_List_File
    {
        std::string            file;
        int                    id;
    };

    struct Checker_List_Res
    {
        ~Checker_List_Res();
        std::vector<Checker_List_File*> files;
        std::string                     to_str() const;
    };

    // Validate
    struct Checker_Validate_Req
    {
        std::vector<int>           ids;
        Report                     report;
        std::vector<size_t>        policies_ids;
        std::vector<std::string>   policies_contents;
        std::string                to_str() const;
    };

    struct Checker_Validate_Ok
    {
        Checker_Validate_Ok() :    valid(false) {}
        int                        id;
        bool                       valid;
        std::string                to_str() const;
    };

    struct Checker_Validate_Nok
    {
        int                        id;
        Reason                     error;
        std::string                to_str() const;
    };

    struct Checker_Validate_Res
    {
        ~Checker_Validate_Res();
        std::vector<Checker_Validate_Ok*>  ok;
        std::vector<Checker_Validate_Nok*> nok;
        std::string                        to_str() const;
    };

    struct Checker_File_From_Id_Req
    {
        int          id;
        std::string  to_str() const;
    };

    struct Checker_File_From_Id_Res
    {
        std::string  file;
        std::string  to_str() const;
    };

    struct Default_Values_For_Type_Req
    {
        std::string  type;
        std::string  field;
        std::string  to_str() const;
    };

    struct Default_Values_For_Type_Res
    {
        std::vector<std::string> values;
        std::string              to_str() const;
    };

    struct XSLT_Policy_Create_From_File_Req
    {
        int                          id;
        std::string                  to_str() const;
    };

    struct XSLT_Policy_Create_From_File_Nok
    {
        int                          id;
        Reason                       error;
        std::string                  to_str() const;
    };

    struct XSLT_Policy_Create_From_File_Res
    {
        XSLT_Policy_Create_From_File_Res() :  nok(NULL) {}
        int                              policy_id;
        XSLT_Policy_Create_From_File_Nok     *nok;
        std::string                      to_str() const;
    };

public:
    //Constructor/Destructor
    RESTAPI();
    virtual ~RESTAPI();

    // Serialize: Request
    int serialize_analyze_req(Checker_Analyze_Req& req, std::string&);
    int serialize_status_req(Checker_Status_Req& req, std::string&);
    int serialize_report_req(Checker_Report_Req& req, std::string&);
    int serialize_retry_req(Checker_Retry_Req& req, std::string&);
    int serialize_clear_req(Checker_Clear_Req& req, std::string&);
    int serialize_list_req(Checker_List_Req& req, std::string&);
    int serialize_validate_req(Checker_Validate_Req& req, std::string&);
    int serialize_file_from_id_req(Checker_File_From_Id_Req& req, std::string&);
    int serialize_default_values_for_type_req(Default_Values_For_Type_Req& req, std::string&);
    int serialize_xslt_policy_create_from_file_req(XSLT_Policy_Create_From_File_Req& req, std::string&);

    // Serialize: Result
    int serialize_analyze_res(Checker_Analyze_Res& res, std::string&);
    int serialize_status_res(Checker_Status_Res& res, std::string&);
    int serialize_report_res(Checker_Report_Res& res, std::string&);
    int serialize_retry_res(Checker_Retry_Res& res, std::string&);
    int serialize_clear_res(Checker_Clear_Res& res, std::string&);
    int serialize_list_res(Checker_List_Res& res, std::string&);
    int serialize_validate_res(Checker_Validate_Res& res, std::string&);
    int serialize_file_from_id_res(Checker_File_From_Id_Res& res, std::string&);
    int serialize_default_values_for_type_res(Default_Values_For_Type_Res& res, std::string&);
    int serialize_xslt_policy_create_from_file_res(XSLT_Policy_Create_From_File_Res& res, std::string&);

    // Parse: Request
    Checker_Analyze_Req              *parse_analyze_req(const std::string& data);
    Checker_Status_Req               *parse_status_req(const std::string& data);
    Checker_Report_Req               *parse_report_req(const std::string& data);
    Checker_Retry_Req                *parse_retry_req(const std::string& data);
    Checker_Clear_Req                *parse_clear_req(const std::string& data);
    Checker_List_Req                 *parse_list_req(const std::string& data);
    Checker_Validate_Req             *parse_validate_req(const std::string& data);
    Checker_File_From_Id_Req         *parse_file_from_id_req(const std::string& data);
    Default_Values_For_Type_Req      *parse_default_values_for_type_req(const std::string& data);
    XSLT_Policy_Create_From_File_Req *parse_xslt_policy_create_from_file_req(const std::string& data);

    // Parse: URI Request
    Checker_Analyze_Req              *parse_uri_analyze_req(const std::string& uri);
    Checker_Status_Req               *parse_uri_status_req(const std::string& uri);
    Checker_Report_Req               *parse_uri_report_req(const std::string& uri);
    Checker_Retry_Req                *parse_uri_retry_req(const std::string& uri);
    Checker_Clear_Req                *parse_uri_clear_req(const std::string& uri);
    Checker_List_Req                 *parse_uri_list_req(const std::string& uri);
    Checker_Validate_Req             *parse_uri_validate_req(const std::string& uri);
    Checker_File_From_Id_Req         *parse_uri_file_from_id_req(const std::string& uri);
    Default_Values_For_Type_Req      *parse_uri_default_values_for_type_req(const std::string& uri);
    XSLT_Policy_Create_From_File_Req *parse_uri_xslt_policy_create_from_file_req(const std::string& uri);

    // Parse: Request
    Checker_Analyze_Res              *parse_analyze_res(const std::string& data);
    Checker_Status_Res               *parse_status_res(const std::string& data);
    Checker_Report_Res               *parse_report_res(const std::string& data);
    Checker_Retry_Res                *parse_retry_res(const std::string& data);
    Checker_Clear_Res                *parse_clear_res(const std::string& data);
    Checker_List_Res                 *parse_list_res(const std::string& data);
    Checker_Validate_Res             *parse_validate_res(const std::string& data);
    Checker_File_From_Id_Res         *parse_file_from_id_res(const std::string& data);
    Default_Values_For_Type_Res      *parse_default_values_for_type_res(const std::string& data);
    XSLT_Policy_Create_From_File_Res *parse_xslt_policy_create_from_file_res(const std::string& data);

    std::string get_error() const { return error; }

private:
    Container *model;

    std::string error;

    //Helper
    Container::Value serialize_analyze_args(std::vector<Checker_Analyze_Arg>& args);
    Container::Value serialize_ids(std::vector<int>& ids);
    Container::Value serialize_report_reports(std::vector<Report>& args);
    Container::Value serialize_report_string(const std::string& args);
    Container::Value serialize_report_int(int val);
    Container::Value serialize_report_arr_str(const std::vector<std::string>& reports);
    Container::Value serialize_report_arr_long_u(const std::vector<size_t>& reports);
    Container::Value serialize_generic_nok(int id, Reason error);
    Container::Value serialize_analyze_oks(std::vector<Checker_Analyze_Ok*>& array);
    Container::Value serialize_status_oks(std::vector<Checker_Status_Ok*>& array);
    Container::Value serialize_report_ok(Checker_Report_Ok& ok);
    Container::Value serialize_list_file(const std::string& file, int id);
    Container::Value serialize_validate_ok(Checker_Validate_Ok* ok);

    int parse_analyze_arg(Container::Value *v, std::vector<Checker_Analyze_Arg>& args);
    int parse_report_reports(Container::Value *v, std::vector<Report>& reports);
    int parse_generic_nok(Container::Value *v, int& id, Reason& error);
    int parse_analyze_ok(Container::Value *v, std::vector<Checker_Analyze_Ok*>& ok);
    int parse_status_ok(Container::Value *v, std::vector<Checker_Status_Ok*>& ok);
    int parse_report_ok(Container::Value *v, Checker_Report_Ok& ok);
    int parse_list_file(Container::Value *v, std::string& file, int& id);
    int parse_validate_ok(Container::Value *v, std::vector<Checker_Validate_Ok*>& oks);

    RESTAPI (const RESTAPI&);
    RESTAPI& operator=(const RESTAPI&);
};

}

#endif
