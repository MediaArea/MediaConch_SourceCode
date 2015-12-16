/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Http functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef HttpH
#define HttpH
//---------------------------------------------------------------------------
#include "REST_API.h"
#include <string>
//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// Class Http
//***************************************************************************

class Http
{
public:
    //Constructor/Destructor
    Http();
    virtual ~Http();

    virtual int init() = 0;
    virtual int start() = 0;
    virtual int finish() = 0;

    int send_request(RESTAPI::Check_Req& req);
    int send_request(RESTAPI::Status_Req& req);
    /* int send_request(RESTAPI::Status_Req& req); */
    int send_request(RESTAPI::Report_Req& req);
    int send_request(RESTAPI::Retry_Req& req);
    int send_request(RESTAPI::Clear_Req& req);

    void set_port(int port);
    void set_address(std::string& address);

    std::string get_error() const { return error; }

protected:
    RESTAPI     rest;
    std::string address;
    int         port;
    std::string result;
    std::string error;

    virtual int send_request_get(std::string& uri) = 0;
    virtual int send_request_post(std::string& uri, std::string& str) = 0;
    virtual int send_request_put(std::string& uri, std::string& str) = 0;
    virtual int send_request_delete(std::string& uri) = 0;

private:
    Http (const Http&);
    Http& operator=(const Http&);
};

}

#endif
