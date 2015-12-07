/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// LibEventHttp functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifdef HAVE_LIBEVENT

//---------------------------------------------------------------------------
#ifndef LibEventHttpH
#define LibEventHttpH

#include "Http.h"
#include <string>

#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/keyvalq_struct.h>

//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// Class Httpd
//***************************************************************************

class LibEventHttp : public Http
{
public:
    //Constructor/Destructor
    LibEventHttp();
    virtual ~LibEventHttp();

    int init();
    int start();
    int stop();
    int finish();

private:
    struct event_base          *base;
    struct evhttp_connection   *connection;    

    static void result_coming(struct evhttp_request *req, void *arg);

    virtual int send_request_get(std::string& uri);
    virtual int send_request_post(std::string& uri, std::string& str);
    virtual int send_request_put(std::string& uri, std::string& str);
    virtual int send_request_delete(std::string& uri);
    int send_request(std::string& uri, std::string& str, enum evhttp_cmd_type type);

    LibEventHttp (const LibEventHttp&);
    LibEventHttp& operator=(const LibEventHttp&);
};

}

#endif // !LibEventHttpH

#endif // !HAVE_LIBEVENT
