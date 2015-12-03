/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifdef HAVE_LIBEVENT

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//---------------------------------------------------------------------------
#include "LibEventHttpd.h"
#include <sstream>
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
LibEventHttpd::LibEventHttpd() : Httpd(), base(NULL), http(NULL), handle(NULL)
{
}

//---------------------------------------------------------------------------
LibEventHttpd::~LibEventHttpd()
{
    finish();
}

//---------------------------------------------------------------------------
int LibEventHttpd::init()
{
    base = event_base_new();
    if (!base)
    {
        error = std::string("cannot create an event-base");
        return -1;
    }

    http = evhttp_new(base);
    if (!http)
    {
        error = std::string("cannot create event-http");
        return -1;
    }

    evhttp_set_gencb(http, request_coming, this);
    return 0;
}

//---------------------------------------------------------------------------
int LibEventHttpd::start()
{
    handle = evhttp_bind_socket_with_handle(http, address.c_str(), port);
    if (!handle)
    {
        std::stringstream ss;
        ss << "couldn't bind to address " << address << " and port " << port;
        error = ss.str();
        return 1;
    }


    event_base_dispatch(base);
    return 0;
}

//---------------------------------------------------------------------------
int LibEventHttpd::finish()
{
    if (handle)
    {
        evhttp_del_accept_socket(http, handle);
        handle = NULL;
    }
    if (http)
    {
        evhttp_free(http);
        http = NULL;
    }
    if (base)
    {
        event_base_free(base);
        base = NULL;
    }
    return 0;
}

//---------------------------------------------------------------------------
int LibEventHttpd::send_result(int ret_code, std::string& ret_msg, void *arg)
{
    struct evhttp_request *req = (struct evhttp_request *)arg;
    struct evbuffer *evOutBuf = evbuffer_new();

    if (evOutBuf)
    {
        if (error.length())
            evbuffer_add_printf(evOutBuf, "%s\n", error.c_str());
        else if (result.length())
            evbuffer_add_printf(evOutBuf, "%s\n", result.c_str());
    }
    evhttp_send_reply(req, ret_code, ret_msg.c_str(), evOutBuf);
    if (evOutBuf)
        evbuffer_free(evOutBuf);

    // clean error
    error.clear();
    return 0;
}

//---------------------------------------------------------------------------
void LibEventHttpd::request_coming(struct evhttp_request *req, void *arg)
{
    LibEventHttpd    *evHttp = (LibEventHttpd*)arg;
    const evhttp_uri *uri = evhttp_request_get_evhttp_uri(req);
    std::string       ret_msg("OK");
    std::string       json;

    switch (evhttp_request_get_command(req))
    {
        case EVHTTP_REQ_GET:
            break;
        case EVHTTP_REQ_POST:
            if (evHttp->get_body(req, json, ret_msg))
                return;
            break;
        case EVHTTP_REQ_PUT:
            if (evHttp->get_body(req, json, ret_msg))
                return;
            break;
        case EVHTTP_REQ_DELETE:
            break;
        default:
            evHttp->error = std::string("HTTP Request command not supported");
            evHttp->send_result(HTTP_BADREQUEST, ret_msg, req);
            return;
    }

    int code = HTTP_OK;
    if (0);
#define DO_COMMAND(name, type)                                          \
    else if (!std::string("/"#name).compare(evhttp_uri_get_path(uri)))  \
    {                                                                   \
        RESTAPI::type##_Req *r = NULL;                                  \
        std::string uri_query = std::string(evhttp_uri_get_query(uri)); \
        evHttp->get_request(uri_query, json, &r);                       \
        if (!r)                                                         \
        {                                                               \
            ret_msg = "NOVALIDCONTENT";                                 \
            code = HTTP_BADREQUEST;                                     \
            goto send;                                                  \
        }                                                               \
                                                                        \
        RESTAPI::type##_Res res;                                        \
        /* TODO: do the job by the Core */                              \
        evHttp->result = evHttp->rest.serialize_##name##_res(res);      \
        if (!evHttp->result.length())                                   \
            evHttp->error = evHttp->rest.get_error();                   \
    }

    DO_COMMAND(check, Check)
    DO_COMMAND(status, Status)
    DO_COMMAND(report, Report)
    DO_COMMAND(retry, Retry)
    DO_COMMAND(clear, Clear)
#undef DO_COMMAND

    else
    {
        code = HTTP_NOTFOUND;
        ret_msg = std::string("NOTFOUND");
        evHttp->error = std::string("invalid URI");
    }

send:
    evHttp->send_result(code, ret_msg, req);
}

//---------------------------------------------------------------------------
int LibEventHttpd::get_body(struct evhttp_request *req, std::string& json, std::string& ret_msg)
{
    struct evbuffer *evBuf = evhttp_request_get_input_buffer(req);
    if (!evBuf)
    {
        error = std::string("body of the request should contain the command");
        ret_msg = "NOCONTENT";
        send_result(HTTP_BADREQUEST, ret_msg, req);
        return -1;
    }

    int len = evbuffer_get_length(evBuf);
    char tmpBuf[len + 1];
    int n = evbuffer_remove(evBuf, tmpBuf, len);
    if (n <= 0)
    {
        error = std::string("body of the request should contain the command");
        ret_msg = "NOVALIDCONTENT";
        send_result(HTTP_BADREQUEST, ret_msg, req);
        return -1;
    }
    tmpBuf[n] = '\0';

    json = std::string(tmpBuf);
    return 0;
}

}

#endif // !HAVE_LIBEVENT
