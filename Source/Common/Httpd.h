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
    Httpd();
    virtual ~Httpd();

    virtual int init() = 0;
    virtual int start() = 0;
    virtual int finish() = 0;

    virtual int send_result(int ret_code, std::string& ret_msg, void *arg) = 0;

#define REQ_FUNC(type) \
    void get_request(std::string& uri, std::string& json, RESTAPI::type##_Req** req);

    REQ_FUNC(Check);
    REQ_FUNC(Status);
    REQ_FUNC(Report);
    REQ_FUNC(Retry);
    REQ_FUNC(Clear);
#undef REQ_FUNC
    int send_result();

    void set_port(int port);
    void set_address(std::string& address);

    std::string get_error() const;
    std::string get_result() const;

protected:
    int port;
    std::string address;
    RESTAPI rest;

    std::string error;
    std::string result;

private:
    Httpd (const Httpd&);
    Httpd& operator=(const Httpd&);
};

}

#endif
