/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifdef HAVE_LIBEVENT

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//---------------------------------------------------------------------------
#include "MediaConchLib.h"
#include "LibEventHttp.h"
#include <sstream>
#include <stdlib.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

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
int LibEventHttp::init(std::string& err)
{
    base = event_base_new();
    if (!base)
    {
        err = "Cannot initialize LibEvent.";
        return -1;
    }
    return 0;
}

//---------------------------------------------------------------------------
int LibEventHttp::start(std::string& err)
{
    connection = evhttp_connection_base_new(base, NULL, address.c_str(), port);
    if (!connection)
    {
        std::stringstream ss;
        ss << "Cannot connect to " << address << ":" << port;
        err = ss.str();
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
int LibEventHttp::send_request_get(std::string& uri, std::string& err)
{
    std::string str;
    return send_request(uri, str, EVHTTP_REQ_GET, err);
}

//---------------------------------------------------------------------------
int LibEventHttp::send_request_post(std::string& uri, std::string& cmd, std::string& err)
{
    return send_request(uri, cmd, EVHTTP_REQ_POST, err);
}

//---------------------------------------------------------------------------
int LibEventHttp::send_request_put(std::string& uri, std::string& cmd, std::string& err)
{
    return send_request(uri, cmd, EVHTTP_REQ_PUT, err);
}

//---------------------------------------------------------------------------
int LibEventHttp::send_request_delete(std::string& uri, std::string& err)
{
    std::string str;
    return send_request(uri, str, EVHTTP_REQ_DELETE, err);
}

//---------------------------------------------------------------------------
int LibEventHttp::send_request(std::string& uri, std::string& str, enum evhttp_cmd_type type, std::string& err)
{
    // clean result
    result.clear();
    error.clear();
    http_code = 0;

    struct evhttp_request *req = evhttp_request_new(result_coming, this);
    if (!req)
    {
        err = "Cannot create HTTP (LibEvent) request.";
        return -1;
    }

    struct evkeyvalq *evOutHeaders;
    evOutHeaders = evhttp_request_get_output_headers(req);
    evhttp_add_header(evOutHeaders, "Host", address.c_str());

    std::stringstream ss;
    ss << current_daemon_id;
    evhttp_add_header(evOutHeaders, "X-App-MediaConch-Instance-ID", ss.str().c_str());

    if (str.length())
    {
        struct evbuffer *evOutBuf = evhttp_request_get_output_buffer(req);;
        if (!evOutBuf)
        {
            err = "Cannot create output HTTP (LibEvent) buffer.";
            return -1;
        }

        evbuffer_add(evOutBuf, str.c_str(), str.length());

        std::stringstream len_str;
        len_str << str.length();
        evhttp_add_header(evOutHeaders, "Content-Length", len_str.str().c_str());
        evhttp_add_header(evOutHeaders, "Content-Type", "application/json");
    }

    int r = evhttp_make_request(connection, req, type, uri.c_str());
    if (r)
    {
        err = "The HTTP (LibEvent) request created has invalid data.";
        return -1;
    }

    if (event_base_dispatch(base) < 0 || error.size())
    {
        err = error;
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
void LibEventHttp::result_coming(struct evhttp_request *req, void *arg)
{
    LibEventHttp *evHttp = (LibEventHttp*)arg;

    if (!req)
    {
        evHttp->error = "Invalid request";
        return;
    }

    int code = evhttp_request_get_response_code(req);
    if (code != HTTP_OK)
    {
        if (code == 410)
            evHttp->error = "Daemon restarted";
        else if (code >= 400 && code < 500)
            evHttp->error = "Invalid Data in request";
        else
        {
            evHttp->http_code = code;
            evHttp->error = "Internal error: HTTP (LibEvent) connection failed";
        }
        event_base_loopexit(evHttp->base, NULL);
        return;
    }

    struct evkeyvalq *headers = evhttp_request_get_input_headers(req);
    if (headers)
    {
        for (struct evkeyval *header = headers->tqh_first; header; header = header->next.tqe_next)
        {
            if (header->key && std::string(header->key) == "X-App-MediaConch-Instance-ID")
                current_daemon_id = header->value ? strtol(header->value, NULL, 10) : -1;
        }
    }

    std::string data;
    struct evbuffer *evInputBuf = evhttp_request_get_input_buffer(req);
    int len = evbuffer_get_length(evInputBuf);
    if (len > 0)
    {
        char* buff=new char[len + 1];
        int n = evbuffer_remove(evInputBuf, buff, len);

        buff[n >= 0 ? n : 0] = '\0';
        data = std::string(buff, n);

        delete[] buff;
    }

    evHttp->result = data;
    event_base_loopexit(evHttp->base, 0);
}

}

#endif // !HAVE_LIBEVENT
