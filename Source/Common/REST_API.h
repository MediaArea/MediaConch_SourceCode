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

    enum Reason
    {
        NO_REASON,
        FILE_NOT_EXISTING,
        ID_NOT_EXISTING,
        NOT_READY,
    };

    enum Report
    {
        NO_REPORT,
        POLICY,
        IMPLEMENTATION,
        MI_XML,
        MEDIATRACE,
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
            ReportString(MI_XML);
            ReportString(MEDIATRACE);
            default:
                break;
        }
#undef ReportString
        return std::string();
    }

    Report string_to_Report(std::string str)
    {
#define ReportString(report)       \
        if (!str.compare(#report)) \
            return report;

        ReportString(POLICY);
        ReportString(IMPLEMENTATION);
        ReportString(MI_XML);
        ReportString(MEDIATRACE);

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
            ReasonString(FILE_NOT_EXISTING);
            ReasonString(ID_NOT_EXISTING);
            ReasonString(NOT_READY);
            default:
                break;
        }
#undef ReasonString
        return std::string();
    }

    Reason string_to_Reason(std::string str)
    {
#define ReasonString(reason)       \
        if (!str.compare(#reason)) \
            return reason;

        ReasonString(FILE_NOT_EXISTING);
        ReasonString(ID_NOT_EXISTING);
        ReasonString(NOT_READY);

#undef ReasonString

        return NO_REASON;
    }

    // Check
    struct Check_Arg
    {
        std::string            file;
        int                    id;
    };

    struct Check_Req
    {
        std::vector<Check_Arg> args;
        std::string            schematron;
    };

    struct Check_Ok
    {
        int                    inId;
        int                    outId;
    };

    struct Check_Nok
    {
        int                    id;
        Reason                 error;
    };

    struct Check_Res
    {
        std::vector<Check_Ok>  ok;
        std::vector<Check_Nok> nok;
    };

    // Status
    struct Status_Req
    {
        std::vector<int>        ids;
    };

    struct Status_Ok
    {
        Status_Ok() : has_validity(false) {}
        int                     id;
        bool                    finished;
        bool                    has_validity;
        bool                    validity;
    };

    struct Status_Nok
    {
        int                     id;
        Reason                  error;
    };

    struct Status_Res
    {
        std::vector<Status_Ok>  ok;
        std::vector<Status_Nok> nok;
    };

    // Report
    struct Report_Arg
    {
        Report_Arg() : has_offset(false), has_length(false) {}
        Report                 kind;
        int                    offset;
        int                    length;
        bool                   has_offset;
        bool                   has_length;
    };

    struct Report_Req
    {
        std::vector<int>        ids;
        std::vector<Report_Arg> reports;
    };

    struct Report_Ok
    {
        int                          id;
        std::string                  policy;
        std::string                  implementation;
        std::string                  mi_xml;
        std::string                  mediatrace;
    };

    struct Report_Nok
    {
        int                     id;
        Reason                  error;
    };

    struct Report_Res
    {
        std::vector<Report_Ok>  ok;
        std::vector<Report_Nok> nok;
    };

    // Retry
    struct Retry_Req
    {
        std::vector<int>        ids;
    };

    struct Retry_Nok
    {
        int                     id;
        Reason                  error;
    };

    struct Retry_Res
    {
        std::vector<int>       ok;
        std::vector<Retry_Nok> nok;
    };

    // Clear
    struct Clear_Req
    {
        std::vector<int>        ids;
    };

    struct Clear_Nok
    {
        int                     id;
        Reason                  error;
    };

    struct Clear_Res
    {
        std::vector<int>       ok;
        std::vector<Clear_Nok> nok;
    };

public:
    //Constructor/Destructor
    RESTAPI();
    virtual ~RESTAPI();

    // Serialize: Request
    std::string serialize_check_req(Check_Req& req);
    std::string serialize_status_req(Status_Req& req);
    std::string serialize_report_req(Report_Req& req);
    std::string serialize_retry_req(Retry_Req& req);
    std::string serialize_clear_req(Clear_Req& req);

    // Serialize: Result
    std::string serialize_check_res(Check_Res& res);
    std::string serialize_status_res(Status_Res& res);
    std::string serialize_report_res(Report_Res& res);
    std::string serialize_retry_res(Retry_Res& res);
    std::string serialize_clear_res(Clear_Res& res);

    // Parse: Request
    Check_Req *parse_check_req(std::string data);
    Status_Req *parse_status_req(std::string data);
    Report_Req *parse_report_req(std::string data);
    Retry_Req *parse_retry_req(std::string data);
    Clear_Req *parse_clear_req(std::string data);

    // Parse: Request
    Check_Res *parse_check_res(std::string data);
    Status_Res *parse_status_res(std::string data);
    Report_Res *parse_report_res(std::string data);
    Retry_Res *parse_retry_res(std::string data);
    Clear_Res *parse_clear_res(std::string data);

private:
    Container *model;

    //Helper
    Container::Value serialize_check_args(std::vector<Check_Arg>& args);
    Container::Value serialize_ids(std::vector<int>& ids);
    Container::Value serialize_report_reports(std::vector<Report_Arg>& args);
    Container::Value serialize_generic_nok(int id, Reason error);
    Container::Value serialize_check_oks(std::vector<Check_Ok>& array);
    Container::Value serialize_status_oks(std::vector<Status_Ok>& array);
    Container::Value serialize_report_oks(std::vector<Report_Ok>& array);

    int parse_check_arg(Container::Value *v, std::vector<Check_Arg>& args);
    int parse_report_arg(Container::Value *v, std::vector<Report_Arg>& args);
    int parse_generic_nok(Container::Value *v, int& id, Reason& error);
    int parse_check_ok(Container::Value *v, std::vector<Check_Ok>& ok);
    int parse_status_ok(Container::Value *v, std::vector<Status_Ok>& ok);
    int parse_report_ok(Container::Value *v, std::vector<Report_Ok>& ok);

    RESTAPI (const RESTAPI&);
    RESTAPI& operator=(const RESTAPI&);
};

}

#endif
