/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// LibEventHttpd functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifdef HAVE_LIBEVENT

//---------------------------------------------------------------------------
#ifndef LibEventHttpdH
#define LibEventHttpdH

#include "Httpd.h"
#include <string>

#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>

//---------------------------------------------------------------------------

namespace MediaConch
{

class MediaConchLib;

//***************************************************************************
// Class LibEventHttpd
//***************************************************************************

class LibEventHttpd : public Httpd
{
public:
    //Constructor/Destructor
    LibEventHttpd(void* arg);
    virtual ~LibEventHttpd();

    int init(std::string& err);
    int bind(std::string& err);
    int start(std::string& err);
    int finish();

    virtual int send_result(int ret_code, const std::string& ret_msg, void *arg);

private:
    struct event_base          *base;
    struct evhttp              *http;
    struct evhttp_bound_socket *handle;

    static int                  pid;

    static void request_coming(struct evhttp_request *req, void *arg);
    int get_body(struct evhttp_request *req, std::string& json, std::string& ret_msg);
    void request_get_coming(struct evhttp_request *req, std::string& err);
    void request_post_coming(struct evhttp_request *req, std::string& err);
    void request_put_coming(struct evhttp_request *req, std::string& err);
    void request_delete_coming(struct evhttp_request *req, std::string& err);
    int  uri_api_version_is_valid(std::string& uri, struct evhttp_request *req);
    int  get_mediaconch_instance(const struct evkeyvalq *headers);

    LibEventHttpd (const LibEventHttpd&);
    LibEventHttpd& operator=(const LibEventHttpd&);
};

}

#endif // !HAVE_LIBEVENT

#endif // !LibEventHttpdH
