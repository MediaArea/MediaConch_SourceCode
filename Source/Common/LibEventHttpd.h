/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Httpd functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef LibEventHttpdH
#define LibEventHttpdH
//---------------------------------------------------------------------------

#include "Httpd.h"
#include <string>

#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>

//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// Class Httpd
//***************************************************************************

class LibEventHttpd : public Httpd
{
public:
    //Constructor/Destructor
    LibEventHttpd();
    virtual ~LibEventHttpd();

    int init();
    int start();
    int finish();

    virtual int send_result(int ret_code, std::string& ret_msg, void *arg);

private:
    struct event_base          *base;
    struct evhttp              *http;
    struct evhttp_bound_socket *handle;

    static void request_coming(struct evhttp_request *req, void *arg);
    int get_body(struct evhttp_request *req, std::string& json, std::string& ret_msg);

    LibEventHttpd (const LibEventHttpd&);
    LibEventHttpd& operator=(const LibEventHttpd&);
};

}

#endif
