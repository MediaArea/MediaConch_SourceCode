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
#include <stdio.h>
#include "Common/MediaConchLib.h"
#include "Httpd.h"

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Httpd
//***************************************************************************

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Httpd::Httpd(void *p) : port(80), address("0.0.0.0"), parent(p)
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
#define MAKE_REQ_FUNC(name, type)                                         \
    void Httpd::get_request(std::string& json, RESTAPI::type##_Req** req) \
    {                                                                     \
        *req = rest.parse_##name##_req(json);                             \
        if (!*req)                                                        \
            error = rest.get_error();                                     \
    }

    MAKE_REQ_FUNC(analyze, Analyze)
    MAKE_REQ_FUNC(report, Report)
    MAKE_REQ_FUNC(validate, Validate)
    MAKE_REQ_FUNC(retry, Retry)

#undef MAKE_REQ_FUNC

//---------------------------------------------------------------------------
#define MAKE_URI_REQ_FUNC(name, type)                                      \
    void Httpd::get_uri_request(std::string& uri_query,                    \
                                RESTAPI::type##_Req** req)                 \
    {                                                                      \
        *req = rest.parse_uri_##name##_req(uri_query);                     \
        if (!*req)                                                         \
            error = rest.get_error();                                      \
    }

    MAKE_URI_REQ_FUNC(status, Status)
    MAKE_URI_REQ_FUNC(clear, Clear)
    MAKE_URI_REQ_FUNC(list, List)

#undef MAKE_URI_REQ_FUNC

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
