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
#include "Httpd.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Httpd
//***************************************************************************

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Httpd::Httpd() : port(80), address("0.0.0.0")
{
}

//---------------------------------------------------------------------------
Httpd::~Httpd()
{
}

//---------------------------------------------------------------------------
void Httpd::set_port(int port)
{
    this->port = port;
}

//---------------------------------------------------------------------------
void Httpd::set_address(std::string& address)
{
    this->address = address;
}

//---------------------------------------------------------------------------
int Httpd::send_result()
{
    //TODO
    return -1;
}

//---------------------------------------------------------------------------
#define MAKE_REQ_FUNC(name, type)                                       \
    void Httpd::get_request(std::string& uri_query, std::string& json,  \
                            RESTAPI::type##_Req** req)                  \
    {                                                                   \
        *req = rest.parse_##name##_req(json);                           \
        if (!*req)                                                      \
            error = rest.get_error();                                   \
    }

    MAKE_REQ_FUNC(check, Check)
    MAKE_REQ_FUNC(status, Status)
    MAKE_REQ_FUNC(report, Report)
    MAKE_REQ_FUNC(retry, Retry)
    MAKE_REQ_FUNC(clear, Clear)
#undef MAKE_REQ_FUNC

//---------------------------------------------------------------------------
std::string Httpd::get_error() const
{
    return error;
}

//---------------------------------------------------------------------------
std::string Httpd::get_result() const
{
    return result;
}

}
