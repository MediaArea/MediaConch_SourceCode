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
#include "LibEventHttp.h"
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
LibEventHttp::LibEventHttp() : Http(), base(NULL), connection(NULL)
{
}

//---------------------------------------------------------------------------
LibEventHttp::~LibEventHttp()
{
    finish();
}

//---------------------------------------------------------------------------
int LibEventHttp::init()
{
    base = event_base_new();
    if (!base)
    {
        error = std::string("cannot create an event-base");
        return -1;
    }
    return 0;
}

//---------------------------------------------------------------------------
int LibEventHttp::start()
{
    connection = evhttp_connection_base_new(base, NULL, address.c_str(), port);
    if (!connection)
    {
        error = std::string("cannot make an event-http connection");
        return -1;
    }
    return 0;
}

//---------------------------------------------------------------------------
int LibEventHttp::stop()
{
    if (connection)
    {
        evhttp_connection_free(connection);
        connection = NULL;
    }
    return 0;
}

//---------------------------------------------------------------------------
int LibEventHttp::finish()
{
    stop();
    if (base)
    {
        event_base_free(base);
        base = NULL;
    }
    return 0;
}

//---------------------------------------------------------------------------
int LibEventHttp::send_request_get(std::string& uri)
{
    std::string str;
    return send_request(uri, str, EVHTTP_REQ_GET);
}

//---------------------------------------------------------------------------
int LibEventHttp::send_request_post(std::string& uri, std::string& cmd)
{
    return send_request(uri, cmd, EVHTTP_REQ_POST);
}

//---------------------------------------------------------------------------
int LibEventHttp::send_request_put(std::string& uri, std::string& cmd)
{
    return send_request(uri, cmd, EVHTTP_REQ_PUT);
}

//---------------------------------------------------------------------------
int LibEventHttp::send_request_delete(std::string& uri)
{
    std::string str;
    return send_request(uri, str, EVHTTP_REQ_DELETE);
}

//---------------------------------------------------------------------------
int LibEventHttp::send_request(std::string& uri, std::string& str, enum evhttp_cmd_type type)
{
    // clean error
    error.clear();
    result.clear();

    struct evhttp_request *req = evhttp_request_new(result_coming, this);
    if (req == NULL)
    {
        error = std::string("cannot create event-http request failed");
        return -1;
    }

    struct evkeyvalq *evOutHeaders;
    evOutHeaders = evhttp_request_get_output_headers(req);
    evhttp_add_header(evOutHeaders, "Host", address.c_str());

    if (str.length())
    {
        struct evbuffer *evOutBuf = evhttp_request_get_output_buffer(req);;
        if (!evOutBuf)
        {
            error = std::string("cannot get the output event-http buffer");
            return -1;
        }

        evbuffer_add(evOutBuf, str.c_str(), str.length());

        std::stringstream len_str;
        len_str << str.length();
        evhttp_add_header(evOutHeaders, "Content-Length", len_str.str().c_str());
    }

    int r = evhttp_make_request(connection, req, type, uri.c_str());
    if (r)
    {
        error = std::string("cannot make the request");
        return -1;
    }

    event_base_dispatch(base);
    return 0;
}

//---------------------------------------------------------------------------
void LibEventHttp::result_coming(struct evhttp_request *req, void *arg)
{
    LibEventHttp *evHttp = (LibEventHttp*)arg;

    if (!req)
    {
        //TODO
        return;
    }

    std::string data;
    struct evbuffer *evInputBuf = evhttp_request_get_input_buffer(req);
    int len = evbuffer_get_length(evInputBuf);
    if (len > 0)
    {
        char buff[len + 1];
        int n = evbuffer_remove(evInputBuf, buff, len);

        buff[n >= 0 ? n : 0] = '\0';
        data = std::string(buff, n);
    }
    if (evhttp_request_get_response_code(req) != HTTP_OK)
    {
        //TODO
        evHttp->error = data;
    }
    else
        evHttp->result = data;
    event_base_loopexit(evHttp->base, 0);
}

}

#endif // !HAVE_LIBEVENT
