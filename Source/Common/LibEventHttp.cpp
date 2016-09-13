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
#include "MediaConchLib.h"
#include "LibEventHttp.h"
#include <sstream>
#include <stdlib.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Httpd
//***************************************************************************

int LibEventHttp::current_daemon_id = -1;

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
        error = MediaConchLib::errorHttp_INIT;
        return MediaConchLib::errorHttp_INIT;
    }
    return MediaConchLib::errorHttp_NONE;
}

//---------------------------------------------------------------------------
int LibEventHttp::start()
{
    connection = evhttp_connection_base_new(base, NULL, address.c_str(), port);
    if (!connection)
    {
        error = MediaConchLib::errorHttp_CONNECT;
        return MediaConchLib::errorHttp_CONNECT;
    }
    return MediaConchLib::errorHttp_NONE;
}

//---------------------------------------------------------------------------
int LibEventHttp::stop()
{
    if (connection)
    {
        evhttp_connection_free(connection);
        connection = NULL;
    }
    return MediaConchLib::errorHttp_NONE;
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
    return MediaConchLib::errorHttp_NONE;
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
    error = MediaConchLib::errorHttp_NONE;
    result.clear();

    struct evhttp_request *req = evhttp_request_new(result_coming, this);
    if (req == NULL)
    {
        error = MediaConchLib::errorHttp_CONNECT;
        return MediaConchLib::errorHttp_CONNECT;
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
            error = MediaConchLib::errorHttp_CONNECT;
            return MediaConchLib::errorHttp_MAX;
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
        error = MediaConchLib::errorHttp_INVALID_DATA;
        return MediaConchLib::errorHttp_INVALID_DATA;
    }

    if (event_base_dispatch(base) < 0)
        return MediaConchLib::errorHttp_MAX;
    return MediaConchLib::errorHttp_NONE;
}

//---------------------------------------------------------------------------
void LibEventHttp::result_coming(struct evhttp_request *req, void *arg)
{
    LibEventHttp *evHttp = (LibEventHttp*)arg;

    if (!req)
    {
        evHttp->error = MediaConchLib::errorHttp_CONNECT;
        return;
    }

    int code = evhttp_request_get_response_code(req);
    if (code != HTTP_OK)
    {
        if (code == 410)
        {
            evHttp->error = MediaConchLib::errorHttp_DAEMON_RESTART;
            current_daemon_id = -1;
        }
        else if (code >= 400 && code < 500)
            evHttp->error = MediaConchLib::errorHttp_INVALID_DATA;
        else
            evHttp->error = MediaConchLib::errorHttp_CONNECT;
        event_base_loopexit(evHttp->base, 0);
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
