/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Httpd functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef HttpdH
#define HttpdH
//---------------------------------------------------------------------------
#include "REST_API.h"
#include <string>
//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// Class Httpd
//***************************************************************************

class Httpd
{

public:
    //Constructor/Destructor
    Httpd(void *mcl);
    virtual ~Httpd();

    virtual int init() = 0;
    virtual int start() = 0;
    virtual int finish() = 0;

    virtual int send_result(int ret_code, std::string& ret_msg, void *arg) = 0;

#define REQ_FUNC(type) \
    void get_request(std::string& json, RESTAPI::type##_Req** req);

    REQ_FUNC(Analyze);
    REQ_FUNC(Report);
    REQ_FUNC(Retry);
#undef REQ_FUNC

#define URI_REQ_FUNC(type) \
    void get_uri_request(std::string& uri, RESTAPI::type##_Req** req);

    URI_REQ_FUNC(Status);
    URI_REQ_FUNC(Clear);
    URI_REQ_FUNC(List);
#undef URI_REQ_FUNC

    int send_result();

    void set_port(int port);
    void set_address(std::string& address);

    std::string get_error() const;
    std::string get_result() const;

    // On command received and parsed
    typedef int (*on_analyze_command)(const RESTAPI::Analyze_Req* req,
                                      RESTAPI::Analyze_Res& res, void* arg);
    typedef int (*on_status_command)(const RESTAPI::Status_Req* req,
                                     RESTAPI::Status_Res& res, void* arg);
    typedef int (*on_report_command)(const RESTAPI::Report_Req* req,
                                     RESTAPI::Report_Res& res, void* arg);
    typedef int (*on_retry_command)(const RESTAPI::Retry_Req* req,
                                    RESTAPI::Retry_Res& res, void* arg);
    typedef int (*on_clear_command)(const RESTAPI::Clear_Req* req,
                                    RESTAPI::Clear_Res& res, void* arg);
    typedef int (*on_list_command)(const RESTAPI::List_Req* req,
                                   RESTAPI::List_Res& res, void* arg);

    struct Commands
    {
        Commands() : analyze_cb(NULL), status_cb(NULL), report_cb(NULL),
                     retry_cb(NULL), clear_cb(NULL)
            {
            }
        on_analyze_command analyze_cb;
        on_status_command  status_cb;
        on_report_command  report_cb;
        on_retry_command   retry_cb;
        on_clear_command   clear_cb;
        on_list_command    list_cb;
    };

    Commands commands;

protected:
    int            port;
    std::string    address;
    RESTAPI        rest;
    void          *parent;

    std::string error;
    std::string result;

private:
    Httpd (const Httpd&);
    Httpd& operator=(const Httpd&);
};

}

#endif
