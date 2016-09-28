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
#include "Common/MediaConchLib.h"
#include <sstream>
#include <stdlib.h>

#ifdef _WIN32
#include <process.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif
#include <event2/keyvalq_struct.h>

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Httpd
//***************************************************************************
int LibEventHttpd::pid = -1;
//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
LibEventHttpd::LibEventHttpd(void* arg) : Httpd(arg), base(NULL), http(NULL), handle(NULL)
{
    #ifdef _WIN32
    pid = _getpid();
    #else
    pid = getpid();
    #endif
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
int LibEventHttpd::bind()
{
    handle = evhttp_bind_socket_with_handle(http, address.c_str(), port);
    if (!handle)
    {
        std::stringstream ss;
        ss << "couldn't bind to address " << address << " and port " << port;
        error = ss.str();
        return -1;
    }
    return 0;
}

//---------------------------------------------------------------------------
int LibEventHttpd::start()
{
    return event_base_dispatch(base);
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
int LibEventHttpd::send_result(int ret_code, const std::string& ret_msg, void *arg)
{
    struct evhttp_request *req = (struct evhttp_request *)arg;
    struct evbuffer *evOutBuf = evbuffer_new();

    if (evOutBuf)
    {
        struct evkeyvalq *evOutHeaders;
        evOutHeaders = evhttp_request_get_output_headers(req);
        evhttp_add_header(evOutHeaders, "Host", address.c_str());
        std::stringstream ss;
        ss << pid;
        evhttp_add_header(evOutHeaders, "X-App-MediaConch-Instance-ID", ss.str().c_str());

        ss.str("");
        if (error.length())
        {
            ss << error.length();
            evbuffer_add_printf(evOutBuf, "%s\n", error.c_str());
        }
        else if (result.length())
        {
            ss << result.length();
            evhttp_add_header(evOutHeaders, "Content-Type", "application/json");
            evbuffer_add_printf(evOutBuf, "%s\n", result.c_str());
        }
        else
            ss << 0;
        evhttp_add_header(evOutHeaders, "Content-Length", ss.str().c_str());
    }
    evhttp_send_reply(req, ret_code, ret_msg.c_str(), evOutBuf);
    if (evOutBuf)
        evbuffer_free(evOutBuf);

    // clean error
    error.clear();
    return 0;
}

//---------------------------------------------------------------------------
void LibEventHttpd::request_get_coming(struct evhttp_request *req)
{
    int code = HTTP_OK;
    std::string ret_msg("OK");

    const evhttp_uri *uri = evhttp_request_get_evhttp_uri(req);
    std::string uri_path(evhttp_uri_get_path(uri));

    if (uri_api_version_is_valid(uri_path, req) < 0)
        return;
    const char* query_str = evhttp_uri_get_query(uri);
    if (query_str && !std::string("/checker_status").compare(uri_path))
    {
        std::string query(query_str);
        RESTAPI::Checker_Status_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::Checker_Status_Res res;
        if (commands.status_cb && commands.status_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_status_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (!std::string("/checker_list").compare(uri_path))
    {
        std::string query;
        RESTAPI::Checker_List_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::Checker_List_Res res;
        if (commands.list_cb && commands.list_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_list_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (query_str && !std::string("/default_values_for_type").compare(uri_path))
    {
        std::string query(query_str);
        RESTAPI::Default_Values_For_Type_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::Default_Values_For_Type_Res res;
        if (commands.default_values_for_type_cb && commands.default_values_for_type_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_default_values_for_type_res(res, result) < 0)
            error = rest.get_error();
    }

    else if (!std::string("/xslt_policy_create").compare(uri_path))
    {
        std::string query;
        if (query_str)
            query = std::string(query_str);

        RESTAPI::XSLT_Policy_Create_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::XSLT_Policy_Create_Res res;
        if (commands.xslt_policy_create_cb && commands.xslt_policy_create_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_xslt_policy_create_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (query_str && !std::string("/policy_remove").compare(uri_path))
    {
        std::string query(query_str);
        RESTAPI::Policy_Remove_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::Policy_Remove_Res res;
        if (commands.policy_remove_cb && commands.policy_remove_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_policy_remove_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (query_str && !std::string("/policy_dump").compare(uri_path))
    {
        std::string query(query_str);
        RESTAPI::Policy_Dump_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::Policy_Dump_Res res;
        if (commands.policy_dump_cb && commands.policy_dump_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_policy_dump_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (query_str && !std::string("/policy_save").compare(uri_path))
    {
        std::string query(query_str);
        RESTAPI::Policy_Save_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::Policy_Save_Res res;
        if (commands.policy_save_cb && commands.policy_save_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_policy_save_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (query_str && !std::string("/policy_duplicate").compare(uri_path))
    {
        std::string query(query_str);
        RESTAPI::Policy_Duplicate_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::Policy_Duplicate_Res res;
        if (commands.policy_duplicate_cb && commands.policy_duplicate_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_policy_duplicate_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (query_str && !std::string("/policy_move").compare(uri_path))
    {
        std::string query(query_str);
        RESTAPI::Policy_Move_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::Policy_Move_Res res;
        if (commands.policy_move_cb && commands.policy_move_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_policy_move_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (query_str && !std::string("/policy_get").compare(uri_path))
    {
        std::string query(query_str);
        RESTAPI::Policy_Get_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::Policy_Get_Res res;
        if (commands.policy_get_cb && commands.policy_get_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_policy_get_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (query_str && !std::string("/policy_get_name").compare(uri_path))
    {
        std::string query(query_str);
        RESTAPI::Policy_Get_Name_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::Policy_Get_Name_Res res;
        if (commands.policy_get_name_cb && commands.policy_get_name_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_policy_get_name_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (!std::string("/policy_get_policies_count").compare(uri_path))
    {
        std::string query;
        if (query_str)
            query = std::string(query_str);

        RESTAPI::Policy_Get_Policies_Count_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::Policy_Get_Policies_Count_Res res;
        if (commands.policy_get_policies_count_cb && commands.policy_get_policies_count_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_policy_get_policies_count_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (!std::string("/policy_clear_policies").compare(uri_path))
    {
        std::string query;
        if (query_str)
            query = std::string(query_str);

        RESTAPI::Policy_Clear_Policies_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::Policy_Clear_Policies_Res res;
        if (commands.policy_clear_policies_cb && commands.policy_clear_policies_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_policy_clear_policies_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (!std::string("/policy_get_policies").compare(uri_path))
    {
        std::string query;
        if (query_str)
            query = std::string(query_str);

        RESTAPI::Policy_Get_Policies_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::Policy_Get_Policies_Res res;
        if (commands.policy_get_policies_cb && commands.policy_get_policies_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_policy_get_policies_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (!std::string("/policy_get_policies_names_list").compare(uri_path))
    {
        std::string query;
        if (query_str)
            query = std::string(query_str);

        RESTAPI::Policy_Get_Policies_Names_List_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::Policy_Get_Policies_Names_List_Res res;
        if (commands.policy_get_policies_names_list_cb && commands.policy_get_policies_names_list_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_policy_get_policies_names_list_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (query_str && !std::string("/xslt_policy_create_from_file").compare(uri_path))
    {
        std::string query(query_str);
        RESTAPI::XSLT_Policy_Create_From_File_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::XSLT_Policy_Create_From_File_Res res;
        if (commands.xslt_policy_create_from_file_cb && commands.xslt_policy_create_from_file_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_xslt_policy_create_from_file_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (query_str && !std::string("/xslt_policy_rule_create").compare(uri_path))
    {
        std::string query(query_str);
        RESTAPI::XSLT_Policy_Rule_Create_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::XSLT_Policy_Rule_Create_Res res;
        if (commands.xslt_policy_rule_create_cb && commands.xslt_policy_rule_create_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_xslt_policy_rule_create_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (query_str && !std::string("/xslt_policy_rule_get").compare(uri_path))
    {
        std::string query(query_str);
        RESTAPI::XSLT_Policy_Rule_Get_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::XSLT_Policy_Rule_Get_Res res;
        if (commands.xslt_policy_rule_get_cb && commands.xslt_policy_rule_get_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_xslt_policy_rule_get_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (query_str && !std::string("/xslt_policy_rule_duplicate").compare(uri_path))
    {
        std::string query(query_str);
        RESTAPI::XSLT_Policy_Rule_Duplicate_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::XSLT_Policy_Rule_Duplicate_Res res;
        if (commands.xslt_policy_rule_duplicate_cb && commands.xslt_policy_rule_duplicate_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_xslt_policy_rule_duplicate_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (query_str && !std::string("/xslt_policy_rule_move").compare(uri_path))
    {
        std::string query(query_str);
        RESTAPI::XSLT_Policy_Rule_Move_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::XSLT_Policy_Rule_Move_Res res;
        if (commands.xslt_policy_rule_move_cb && commands.xslt_policy_rule_move_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_xslt_policy_rule_move_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (query_str && !std::string("/xslt_policy_rule_delete").compare(uri_path))
    {
        std::string query(query_str);
        RESTAPI::XSLT_Policy_Rule_Delete_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::XSLT_Policy_Rule_Delete_Res res;
        if (commands.xslt_policy_rule_delete_cb && commands.xslt_policy_rule_delete_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_xslt_policy_rule_delete_res(res, result) < 0)
            error = rest.get_error();
    }

    else
    {
        code = HTTP_NOTFOUND;
        ret_msg = std::string("NOTFOUND");
        error = std::string("invalid URI");
    }
send:
    send_result(code, ret_msg, req);
}

//---------------------------------------------------------------------------
void LibEventHttpd::request_post_coming(struct evhttp_request *req)
{
    std::string json;
    int code = HTTP_OK;
    std::string ret_msg("OK");

    if (get_body(req, json, ret_msg) < 0)
    {
        send_result(HTTP_NOTFOUND, ret_msg, req);
        return;
    }

    const evhttp_uri *uri = evhttp_request_get_evhttp_uri(req);
    std::string uri_path(evhttp_uri_get_path(uri));

    if (uri_api_version_is_valid(uri_path, req) < 0)
        return;
    if (!std::string("/checker_analyze").compare(uri_path))
    {
        RESTAPI::Checker_Analyze_Req *r = NULL;
        get_request(json, &r);
        if (!r)
        {
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        RESTAPI::Checker_Analyze_Res res;
        if (commands.analyze_cb && commands.analyze_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

            delete r;
        if (rest.serialize_analyze_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (!std::string("/checker_report").compare(uri_path))
    {
        RESTAPI::Checker_Report_Req *r = NULL;
        get_request(json, &r);
        if (!r)
        {
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        RESTAPI::Checker_Report_Res res;
        if (commands.report_cb && commands.report_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_report_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (!std::string("/checker_validate").compare(uri_path))
    {
        RESTAPI::Checker_Validate_Req *r = NULL;
        get_request(json, &r);
        if (!r)
        {
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        RESTAPI::Checker_Validate_Res res;
        if (commands.validate_cb && commands.validate_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_validate_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (!std::string("/checker_file_from_id").compare(uri_path))
    {
        RESTAPI::Checker_File_From_Id_Req *r = NULL;
        get_request(json, &r);
        if (!r)
        {
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        RESTAPI::Checker_File_From_Id_Res res;
        if (commands.file_from_id_cb && commands.file_from_id_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_file_from_id_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (!std::string("/checker_id_from_filename").compare(uri_path))
    {
        RESTAPI::Checker_Id_From_Filename_Req *r = NULL;
        get_request(json, &r);
        if (!r)
        {
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        RESTAPI::Checker_Id_From_Filename_Res res;
        if (commands.id_from_filename_cb && commands.id_from_filename_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_id_from_filename_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (!std::string("/checker_file_information").compare(uri_path))
    {
        RESTAPI::Checker_File_Information_Req *r = NULL;
        get_request(json, &r);
        if (!r)
        {
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        RESTAPI::Checker_File_Information_Res res;
        if (commands.file_information_cb && commands.file_information_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_file_information_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (!std::string("/policy_import").compare(uri_path))
    {
        RESTAPI::Policy_Import_Req *r = NULL;
        get_request(json, &r);
        if (!r)
        {
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        RESTAPI::Policy_Import_Res res;
        if (commands.policy_import_cb && commands.policy_import_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_policy_import_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (!std::string("/policy_change_info").compare(uri_path))
    {
        RESTAPI::Policy_Change_Info_Req *r = NULL;
        get_request(json, &r);
        if (!r)
        {
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        RESTAPI::Policy_Change_Info_Res res;
        if (commands.policy_change_info_cb && commands.policy_change_info_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_policy_change_info_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (!std::string("/policy_change_type").compare(uri_path))
    {
        RESTAPI::Policy_Change_Type_Req *r = NULL;
        get_request(json, &r);
        if (!r)
        {
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        RESTAPI::Policy_Change_Type_Res res;
        if (commands.policy_change_type_cb && commands.policy_change_type_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_policy_change_type_res(res, result) < 0)
            error = rest.get_error();
    }
    else if (!std::string("/xslt_policy_rule_edit").compare(uri_path))
    {
        RESTAPI::XSLT_Policy_Rule_Edit_Req *r = NULL;
        get_request(json, &r);
        if (!r)
        {
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        RESTAPI::XSLT_Policy_Rule_Edit_Res res;
        if (commands.xslt_policy_rule_edit_cb && commands.xslt_policy_rule_edit_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_xslt_policy_rule_edit_res(res, result) < 0)
            error = rest.get_error();
    }
    else
    {
        code = HTTP_NOTFOUND;
        ret_msg = std::string("NOTFOUND");
        error = std::string("invalid URI");
    }
send:
    send_result(code, ret_msg, req);
}

//---------------------------------------------------------------------------
void LibEventHttpd::request_put_coming(struct evhttp_request *req)
{
    std::string json;
    int code = HTTP_OK;
    std::string ret_msg("OK");

    if (get_body(req, json, ret_msg))
    {
        send_result(HTTP_NOTFOUND, ret_msg, req);
        return;
    }

    const evhttp_uri *uri = evhttp_request_get_evhttp_uri(req);
    std::string uri_path(evhttp_uri_get_path(uri));

    if (uri_api_version_is_valid(uri_path, req) < 0)
        return;
    if (!std::string("/checker_retry").compare(uri_path))
    {
        RESTAPI::Checker_Retry_Req *r = NULL;
        get_request(json, &r);
        if (!r)
        {
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        RESTAPI::Checker_Retry_Res res;
        if (commands.retry_cb && commands.retry_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_retry_res(res, result) < 0)
            error = rest.get_error();
    }
    else
    {
        code = HTTP_NOTFOUND;
        ret_msg = std::string("NOTFOUND");
        error = std::string("invalid URI");
    }
send:
    send_result(code, ret_msg, req);
}

//---------------------------------------------------------------------------
void LibEventHttpd::request_delete_coming(struct evhttp_request *req)
{
    int code = HTTP_OK;
    std::string ret_msg("OK");

    const evhttp_uri *uri = evhttp_request_get_evhttp_uri(req);
    std::string uri_path(evhttp_uri_get_path(uri));

    if (uri_api_version_is_valid(uri_path, req) < 0)
        return;

    const char* query_str = evhttp_uri_get_query(uri);
    if (query_str && !std::string("/checker_clear").compare(uri_path))
    {
        std::string query(query_str);
        RESTAPI::Checker_Clear_Req *r = NULL;
        get_uri_request(query, &r);

        RESTAPI::Checker_Clear_Res res;
        if (commands.clear_cb && commands.clear_cb(r, res, parent) < 0)
        {
            delete r;
            ret_msg = "NOVALIDCONTENT";
            code = HTTP_BADREQUEST;
            goto send;
        }

        delete r;
        if (rest.serialize_clear_res(res, result) < 0)
            error = rest.get_error();
    }
    else
    {
        code = HTTP_NOTFOUND;
        ret_msg = std::string("NOTFOUND");
        error = std::string("invalid URI");
    }
send:
    send_result(code, ret_msg, req);
}

//---------------------------------------------------------------------------
void LibEventHttpd::request_coming(struct evhttp_request *req, void *arg)
{
    LibEventHttpd    *evHttp = (LibEventHttpd*)arg;
    std::string       ret_msg("OK");

    evHttp->error.clear();
    evHttp->result.clear();

    struct evkeyvalq *kv = evhttp_request_get_input_headers(req);
    if (evHttp->get_mediaconch_instance(kv) < 0)
    {
        evHttp->error = std::string("HTTP header X-App-MediaConch-Instance-ID not corresponding");
        evHttp->send_result(410, "X-App-MediaConch-Instance-ID-INVALID", req);
        return;
    }

    switch (evhttp_request_get_command(req))
    {
        case EVHTTP_REQ_GET:
            evHttp->request_get_coming(req);
            break;
        case EVHTTP_REQ_POST:
            evHttp->request_post_coming(req);
            break;
        case EVHTTP_REQ_PUT:
            evHttp->request_put_coming(req);
            break;
        case EVHTTP_REQ_DELETE:
            evHttp->request_delete_coming(req);
            break;
        default:
            evHttp->error = std::string("HTTP Request command not supported");
            evHttp->send_result(HTTP_BADREQUEST, ret_msg, req);
            return;
    }
}

int LibEventHttpd::get_mediaconch_instance(const struct evkeyvalq *headers)
{
    if (!headers)
        return 0;

    struct evkeyval *header = headers->tqh_first;
    while (header)
    {
        if (header->key && !evutil_ascii_strcasecmp(header->key, "X-App-MediaConch-Instance-ID"))
        {
            if (!header->value)
                return 0;

            int value = strtol(header->value, NULL, 10);
            if (value == -1)
                return 0;
            else if (pid != value)
                return -1;
        }
        header = header->next.tqe_next;
    }

    return 0;
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
    char* tmpBuf = new char[len + 1];
    int n = evbuffer_remove(evBuf, tmpBuf, len);
    if (n <= 0)
    {
        delete[] tmpBuf;
        error = std::string("body of the request should contain the command");
        ret_msg = "NOVALIDCONTENT";
        send_result(HTTP_BADREQUEST, ret_msg, req);
        return -1;
    }
    tmpBuf[n] = '\0';

    json = std::string(tmpBuf);
    delete[] tmpBuf;

    return 0;
}

int LibEventHttpd::uri_api_version_is_valid(std::string& uri, struct evhttp_request *req)
{
    std::string search("/" + RESTAPI::API_VERSION);
    size_t pos = uri.find(search);
    if (pos != 0)
    {
        error = std::string("The API version is not valid");
        std::string ret_msg = "WRONG_API_VERSION";
        send_result(HTTP_BADREQUEST, ret_msg, req);
        return -1;
    }
    uri = uri.substr(search.length());
    return 0;
}

}

#endif // !HAVE_LIBEVENT
