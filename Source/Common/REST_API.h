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
#include "MediaConchLib.h"
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

//***************************************************************************
// Checker
//***************************************************************************

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
        Checker_Report_Req() :   user(-1), has_verbosity(false) {}
        int                      user;
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
        Checker_Validate_Req() : user(-1) {}
        int                        user;
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

//***************************************************************************
// Policy
//***************************************************************************

    // Generic Policy error
    struct Policy_Nok
    {
        std::string  error;
        std::string  to_str() const;
    };

    struct XSLT_Policy_Create_Req
    {
        XSLT_Policy_Create_Req() : user(-1), parent_id(-1) {}
        int         user;
        int         parent_id;
        std::string type;
        std::string to_str() const;
    };

    struct XSLT_Policy_Create_Res
    {
        XSLT_Policy_Create_Res() : id(-1), nok(NULL) {}
        ~XSLT_Policy_Create_Res();
        int          id;
        Policy_Nok  *nok;
        std::string  to_str() const;
    };

    struct Policy_Import_Req
    {
        Policy_Import_Req() : user(-1) {}
        int         user;
        std::string xml;
        std::string to_str() const;
    };

    struct Policy_Import_Res
    {
        Policy_Import_Res() : id(-1), nok(NULL) {}
        ~Policy_Import_Res();
        int          id;
        Policy_Nok  *nok;
        std::string to_str() const;
    };

    struct Policy_Remove_Req
    {
        Policy_Remove_Req() : user(-1), id(-1) {}
        int         user;
        int         id;
        std::string to_str() const;
    };

    struct Policy_Remove_Res
    {
        Policy_Remove_Res() : nok(NULL) {}
        ~Policy_Remove_Res();
        Policy_Nok  *nok;
        std::string  to_str() const;
    };

    struct Policy_Dump_Req
    {
        Policy_Dump_Req() : user(-1), id(-1) {}
        int         user;
        int         id;
        std::string to_str() const;
    };

    struct Policy_Dump_Res
    {
        Policy_Dump_Res() : nok(NULL) {}
        ~Policy_Dump_Res();
        std::string  xml;
        Policy_Nok  *nok;
        std::string  to_str() const;
    };

    struct Policy_Save_Req
    {
        Policy_Save_Req() : user(-1), id(-1) {}
        int         user;
        int         id;
        std::string to_str() const;
    };

    struct Policy_Save_Res
    {
        Policy_Save_Res() : nok(NULL) {}
        ~Policy_Save_Res();
        Policy_Nok  *nok;
        std::string  to_str() const;
    };

    struct Policy_Duplicate_Req
    {
        Policy_Duplicate_Req() : user(-1), id(-1) {}
        int         user;
        int         id;
        std::string to_str() const;
    };

    struct Policy_Duplicate_Res
    {
        Policy_Duplicate_Res() : id(-1), nok(NULL) {}
        ~Policy_Duplicate_Res();
        int         id;
        Policy_Nok *nok;
        std::string to_str() const;
    };

    struct Policy_Change_Name_Req
    {
        Policy_Change_Name_Req() : user(-1), id(-1) {}
        int          user;
        int          id;
        std::string  name;
        std::string  description;
        std::string  to_str() const;
    };

    struct Policy_Change_Name_Res
    {
        Policy_Change_Name_Res() : nok(NULL) {}
        ~Policy_Change_Name_Res();
        Policy_Nok  *nok;
        std::string  to_str() const;
    };

    struct Policy_Get_Req
    {
        Policy_Get_Req() : user(-1), id(-1) {}
        int         user;
        int         id;
        std::string to_str() const;
    };

    struct Policy_Get_Res
    {
        Policy_Get_Res() : nok(NULL) {}
        ~Policy_Get_Res();
        //TODO
        Policy_Nok  *nok;
        std::string  to_str() const;
    };

    struct Policy_Get_Name_Req
    {
        Policy_Get_Name_Req() : user(-1), id(-1) {}
        int          user;
        int          id;
        std::string  to_str() const;
    };

    struct Policy_Get_Name_Res
    {
        Policy_Get_Name_Res() : nok(NULL) {}
        ~Policy_Get_Name_Res();
        std::string  name;
        Policy_Nok  *nok;
        std::string  to_str() const;
    };

    struct Policy_Get_Policies_Count_Req
    {
        Policy_Get_Policies_Count_Req() : user(-1) {}
        int         user;
        std::string to_str() const;
    };

    struct Policy_Get_Policies_Count_Res
    {
        Policy_Get_Policies_Count_Res() : size(0), nok(NULL) {}
        ~Policy_Get_Policies_Count_Res();
        int          size;
        Policy_Nok  *nok;
        std::string  to_str() const;
    };

    struct Policy_Clear_Policies_Req
    {
        Policy_Clear_Policies_Req() : user(-1) {}
        int         user;
        std::string to_str() const;
    };

    struct Policy_Clear_Policies_Res
    {
        Policy_Clear_Policies_Res() : nok(NULL) {}
        ~Policy_Clear_Policies_Res();
        Policy_Nok  *nok;
        std::string  to_str() const;
    };

    struct Policy_Get_Policies_Req
    {
        Policy_Get_Policies_Req() : user(-1) {}
        int         user;
        std::string to_str() const;
    };

    struct Policy_Get_Policies_Res
    {
        Policy_Get_Policies_Res() : nok(NULL) {}
        ~Policy_Get_Policies_Res();
        std::vector<MediaConchLib::Policy_Policy*>  policies;
        Policy_Nok                                 *nok;
        std::string                                 to_str() const;
    };

    struct XSLT_Policy_Create_From_File_Req
    {
        XSLT_Policy_Create_From_File_Req() : user(-1), id(-1) {}
        int         user;
        int         id;
        std::string to_str() const;
    };

    struct XSLT_Policy_Create_From_File_Nok
    {
        int         id;
        Reason      error;
        std::string to_str() const;
    };

    struct XSLT_Policy_Create_From_File_Res
    {
        XSLT_Policy_Create_From_File_Res() : policy_id(-1), nok(NULL) {}
        ~XSLT_Policy_Create_From_File_Res();
        int                               policy_id;
        XSLT_Policy_Create_From_File_Nok *nok;
        std::string                       to_str() const;
    };

    // XSLT Rule
    struct XSLT_Policy_Rule
    {
        int          id;
        std::string  name;
        std::string  tracktype;
        std::string  field;
        int          occurrence;
        std::string  ope;
        std::string  value;
        std::string  to_str() const;
    };

    struct XSLT_Policy_Rule_Create_Req
    {
        XSLT_Policy_Rule_Create_Req() : user(-1), policy_id(-1) {}
        int         user;
        int         policy_id;
        std::string to_str() const;
    };

    struct XSLT_Policy_Rule_Create_Res
    {
        XSLT_Policy_Rule_Create_Res() : nok(NULL) {}
        ~XSLT_Policy_Rule_Create_Res();
        int          id;
        Policy_Nok  *nok;
        std::string  to_str() const;
    };

    struct XSLT_Policy_Rule_Edit_Req
    {
        XSLT_Policy_Rule_Edit_Req() : user(-1), policy_id(-1) {}
        int              user;
        int              policy_id;
        XSLT_Policy_Rule rule;
        std::string to_str() const;
    };

    struct XSLT_Policy_Rule_Edit_Res
    {
        XSLT_Policy_Rule_Edit_Res() : nok(NULL) {}
        ~XSLT_Policy_Rule_Edit_Res();
        Policy_Nok  *nok;
        std::string  to_str() const;
    };

    struct XSLT_Policy_Rule_Duplicate_Req
    {
        XSLT_Policy_Rule_Duplicate_Req() : user(-1), policy_id(-1), id(-1) {}
        int         user;
        int         policy_id;
        int         id;
        std::string to_str() const;
    };

    struct XSLT_Policy_Rule_Duplicate_Res
    {
        XSLT_Policy_Rule_Duplicate_Res() : id(-1), nok(NULL) {}
        ~XSLT_Policy_Rule_Duplicate_Res();
        int          id;
        Policy_Nok  *nok;
        std::string  to_str() const;
    };

    struct XSLT_Policy_Rule_Delete_Req
    {
        XSLT_Policy_Rule_Delete_Req() : user(-1), policy_id(-1), id(-1) {}
        int         user;
        int         policy_id;
        int         id;
        std::string to_str() const;
    };

    struct XSLT_Policy_Rule_Delete_Res
    {
        XSLT_Policy_Rule_Delete_Res() : nok(NULL) {}
        ~XSLT_Policy_Rule_Delete_Res();
        Policy_Nok  *nok;
        std::string  to_str() const;
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

    int serialize_xslt_policy_create_req(XSLT_Policy_Create_Req& req, std::string&);
    int serialize_policy_import_req(Policy_Import_Req& req, std::string&);
    int serialize_policy_remove_req(Policy_Remove_Req& req, std::string&);
    int serialize_policy_dump_req(Policy_Dump_Req& req, std::string&);
    int serialize_policy_save_req(Policy_Save_Req& req, std::string&);
    int serialize_policy_duplicate_req(Policy_Duplicate_Req& req, std::string&);
    int serialize_policy_change_name_req(Policy_Change_Name_Req& req, std::string&);
    int serialize_policy_get_req(Policy_Get_Req& req, std::string&);
    int serialize_policy_get_name_req(Policy_Get_Name_Req& req, std::string&);
    int serialize_policy_get_policies_count_req(Policy_Get_Policies_Count_Req& req, std::string&);
    int serialize_policy_clear_policies_req(Policy_Clear_Policies_Req& req, std::string&);
    int serialize_policy_get_policies_req(Policy_Get_Policies_Req& req, std::string&);
    int serialize_xslt_policy_create_from_file_req(XSLT_Policy_Create_From_File_Req& req, std::string&);
    int serialize_xslt_policy_rule_create_req(XSLT_Policy_Rule_Create_Req& req, std::string&);
    int serialize_xslt_policy_rule(XSLT_Policy_Rule& rule, Container::Value&);
    int serialize_xslt_policy_rule_edit_req(XSLT_Policy_Rule_Edit_Req& req, std::string&);
    int serialize_xslt_policy_rule_duplicate_req(XSLT_Policy_Rule_Duplicate_Req& req, std::string&);
    int serialize_xslt_policy_rule_delete_req(XSLT_Policy_Rule_Delete_Req& req, std::string&);

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

    int serialize_xslt_policy_create_res(XSLT_Policy_Create_Res& res, std::string&);
    int serialize_policy_import_res(Policy_Import_Res& res, std::string&);
    int serialize_policy_remove_res(Policy_Remove_Res& res, std::string&);
    int serialize_policy_dump_res(Policy_Dump_Res& res, std::string&);
    int serialize_policy_save_res(Policy_Save_Res& res, std::string&);
    int serialize_policy_duplicate_res(Policy_Duplicate_Res& res, std::string&);
    int serialize_policy_change_name_res(Policy_Change_Name_Res& res, std::string&);
    int serialize_policy_get_res(Policy_Get_Res& res, std::string&);
    int serialize_policy_get_name_res(Policy_Get_Name_Res& res, std::string&);
    int serialize_policy_get_policies_count_res(Policy_Get_Policies_Count_Res& res, std::string&);
    int serialize_policy_clear_policies_res(Policy_Clear_Policies_Res& res, std::string&);
    int serialize_policy_get_policies_res(Policy_Get_Policies_Res& res, std::string&);
    int serialize_xslt_policy_create_from_file_res(XSLT_Policy_Create_From_File_Res& res, std::string&);
    int serialize_xslt_policy_rule_create_res(XSLT_Policy_Rule_Create_Res& res, std::string&);
    int serialize_xslt_policy_rule_edit_res(XSLT_Policy_Rule_Edit_Res& res, std::string&);
    int serialize_xslt_policy_rule_duplicate_res(XSLT_Policy_Rule_Duplicate_Res& res, std::string&);
    int serialize_xslt_policy_rule_delete_res(XSLT_Policy_Rule_Delete_Res& res, std::string&);

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

    XSLT_Policy_Create_Req           *parse_xslt_policy_create_req(const std::string&);
    Policy_Import_Req                *parse_policy_import_req(const std::string&);
    Policy_Remove_Req                *parse_policy_remove_req(const std::string&);
    Policy_Dump_Req                  *parse_policy_dump_req(const std::string&);
    Policy_Save_Req                  *parse_policy_save_req(const std::string&);
    Policy_Duplicate_Req             *parse_policy_duplicate_req(const std::string&);
    Policy_Change_Name_Req           *parse_policy_change_name_req(const std::string&);
    Policy_Get_Req                   *parse_policy_get_req(const std::string&);
    Policy_Get_Name_Req              *parse_policy_get_name_req(const std::string&);
    Policy_Get_Policies_Count_Req    *parse_policy_get_policies_count_req(const std::string&);
    Policy_Clear_Policies_Req        *parse_policy_clear_policies_req(const std::string&);
    Policy_Get_Policies_Req          *parse_policy_get_policies_req(const std::string&);
    XSLT_Policy_Create_From_File_Req *parse_xslt_policy_create_from_file_req(const std::string&);
    XSLT_Policy_Rule_Create_Req      *parse_xslt_policy_rule_create_req(const std::string&);
    int                               parse_xslt_policy_rule(Container::Value *val, XSLT_Policy_Rule *);
    XSLT_Policy_Rule_Edit_Req        *parse_xslt_policy_rule_edit_req(const std::string&);
    XSLT_Policy_Rule_Duplicate_Req   *parse_xslt_policy_rule_duplicate_req(const std::string&);
    XSLT_Policy_Rule_Delete_Req      *parse_xslt_policy_rule_delete_req(const std::string&);

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

    XSLT_Policy_Create_Req           *parse_uri_xslt_policy_create_req(const std::string&);
    Policy_Import_Req                *parse_uri_policy_import_req(const std::string&);
    Policy_Remove_Req                *parse_uri_policy_remove_req(const std::string&);
    Policy_Dump_Req                  *parse_uri_policy_dump_req(const std::string&);
    Policy_Save_Req                  *parse_uri_policy_save_req(const std::string&);
    Policy_Duplicate_Req             *parse_uri_policy_duplicate_req(const std::string&);
    Policy_Change_Name_Req           *parse_uri_policy_change_name_req(const std::string&);
    Policy_Get_Req                   *parse_uri_policy_get_req(const std::string&);
    Policy_Get_Name_Req              *parse_uri_policy_get_name_req(const std::string&);
    Policy_Get_Policies_Count_Req    *parse_uri_policy_get_policies_count_req(const std::string&);
    Policy_Clear_Policies_Req        *parse_uri_policy_clear_policies_req(const std::string&);
    Policy_Get_Policies_Req          *parse_uri_policy_get_policies_req(const std::string&);
    XSLT_Policy_Create_From_File_Req *parse_uri_xslt_policy_create_from_file_req(const std::string&);
    XSLT_Policy_Rule_Create_Req      *parse_uri_xslt_policy_rule_create_req(const std::string&);
    XSLT_Policy_Rule_Edit_Req        *parse_uri_xslt_policy_rule_edit_req(const std::string&);
    XSLT_Policy_Rule_Duplicate_Req   *parse_uri_xslt_policy_rule_duplicate_req(const std::string&);
    XSLT_Policy_Rule_Delete_Req      *parse_uri_xslt_policy_rule_delete_req(const std::string&);

    // Parse: Result
    Checker_Analyze_Res              *parse_analyze_res(const std::string& data);
    Checker_Status_Res               *parse_status_res(const std::string& data);
    Checker_Report_Res               *parse_report_res(const std::string& data);
    Checker_Retry_Res                *parse_retry_res(const std::string& data);
    Checker_Clear_Res                *parse_clear_res(const std::string& data);
    Checker_List_Res                 *parse_list_res(const std::string& data);
    Checker_Validate_Res             *parse_validate_res(const std::string& data);
    Checker_File_From_Id_Res         *parse_file_from_id_res(const std::string& data);
    Default_Values_For_Type_Res      *parse_default_values_for_type_res(const std::string& data);

    XSLT_Policy_Create_Res           *parse_xslt_policy_create_res(const std::string&);
    Policy_Import_Res                *parse_policy_import_res(const std::string&);
    Policy_Remove_Res                *parse_policy_remove_res(const std::string&);
    Policy_Dump_Res                  *parse_policy_dump_res(const std::string&);
    Policy_Save_Res                  *parse_policy_save_res(const std::string&);
    Policy_Duplicate_Res             *parse_policy_duplicate_res(const std::string&);
    Policy_Change_Name_Res           *parse_policy_change_name_res(const std::string&);
    Policy_Get_Res                   *parse_policy_get_res(const std::string&);
    Policy_Get_Name_Res              *parse_policy_get_name_res(const std::string&);
    Policy_Get_Policies_Count_Res    *parse_policy_get_policies_count_res(const std::string&);
    Policy_Clear_Policies_Res        *parse_policy_clear_policies_res(const std::string&);
    Policy_Get_Policies_Res          *parse_policy_get_policies_res(const std::string&);
    XSLT_Policy_Create_From_File_Res *parse_xslt_policy_create_from_file_res(const std::string&);
    XSLT_Policy_Rule_Create_Res      *parse_xslt_policy_rule_create_res(const std::string&);
    XSLT_Policy_Rule_Edit_Res        *parse_xslt_policy_rule_edit_res(const std::string&);
    XSLT_Policy_Rule_Duplicate_Res   *parse_xslt_policy_rule_duplicate_res(const std::string&);
    XSLT_Policy_Rule_Delete_Res      *parse_xslt_policy_rule_delete_res(const std::string&);

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
    Container::Value serialize_policy_nok(Policy_Nok* nok);
    void serialize_policies_get_policies(std::vector<MediaConchLib::Policy_Policy*>, Container::Value& policies);

    int parse_analyze_arg(Container::Value *v, std::vector<Checker_Analyze_Arg>& args);
    int parse_report_reports(Container::Value *v, std::vector<Report>& reports);
    int parse_generic_nok(Container::Value *v, int& id, Reason& error);
    int parse_analyze_ok(Container::Value *v, std::vector<Checker_Analyze_Ok*>& ok);
    int parse_status_ok(Container::Value *v, std::vector<Checker_Status_Ok*>& ok);
    int parse_report_ok(Container::Value *v, Checker_Report_Ok& ok);
    int parse_list_file(Container::Value *v, std::string& file, int& id);
    int parse_validate_ok(Container::Value *v, std::vector<Checker_Validate_Ok*>& oks);
    int parse_policy_nok(Container::Value *v, Policy_Nok** n);
    int parse_policies_get_policies(Container::Value* policies, std::vector<MediaConchLib::Policy_Policy*>);

    RESTAPI (const RESTAPI&);
    RESTAPI& operator=(const RESTAPI&);
};

}

#endif
