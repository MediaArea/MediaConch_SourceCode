/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "Http.h"
#include <sstream>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Http
//***************************************************************************

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Http::Http() : address("0.0.0.0"), port(80)
{
}

//---------------------------------------------------------------------------
Http::~Http()
{
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Analyze_Req& req)
{
    std::string cmd = rest.serialize_analyze_req(req);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/"<< "analyze";
    std::string uri_str = uri.str();
    return send_request_post(uri_str, cmd);
}

// //---------------------------------------------------------------------------
// int Http::send_request(RESTAPI::List_Req& req)
// {
//     std::string uri("/list");
//     return send_request_get(uri);
// }

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Status_Req& req)
{
    std::string query = rest.serialize_status_req(req);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/"<< "status" << query;

    std::string uri_str = uri.str();
    return send_request_get(uri_str);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Report_Req& req)
{
    std::string cmd = rest.serialize_report_req(req);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/"<< "report";
    std::string uri_str = uri.str();
    return send_request_post(uri_str, cmd);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Retry_Req& req)
{
    std::string cmd = rest.serialize_retry_req(req);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/"<< "retry";
    std::string uri_str = uri.str();
    return send_request_put(uri_str, cmd);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Clear_Req& req)
{
    std::string query = rest.serialize_clear_req(req);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/" << "clear" << query;

    std::string uri_str = uri.str();
    return send_request_delete(uri_str);
}

//---------------------------------------------------------------------------
void Http::set_port(int port)
{
    this->port = port;
}

//---------------------------------------------------------------------------
void Http::set_address(std::string& address)
{
    this->address = address;
}

}
