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
#include "Http.h"
#include <sstream>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Http
//***************************************************************************

int Http::current_daemon_id = -1;

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Http::Http() : address("0.0.0.0"), port(80)
{
}

//---------------------------------------------------------------------------
Http::~Http()
{
}

// MediaConch
//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::MediaConch_Get_Plugins_Req& req)
{
    std::string query;
    rest.serialize_mediaconch_get_plugins_req(req, query);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/mediaconch_get_plugins" << query;
    std::string uri_str = uri.str();
    return send_request_get(uri_str);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::MediaConch_Watch_Folder_Req& req)
{
    std::string cmd;
    rest.serialize_mediaconch_watch_folder_req(req, cmd);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/mediaconch_watch_folder";
    std::string uri_str = uri.str();
    return send_request_post(uri_str, cmd);
}

// Checker
//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Checker_Analyze_Req& req)
{
    std::string cmd;
    rest.serialize_analyze_req(req, cmd);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/checker_analyze";
    std::string uri_str = uri.str();
    return send_request_post(uri_str, cmd);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Checker_Status_Req& req)
{
    std::string query;
    rest.serialize_status_req(req, query);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/checker_status" << query;

    std::string uri_str = uri.str();
    return send_request_get(uri_str);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Checker_Report_Req& req)
{
    std::string cmd;
    rest.serialize_report_req(req, cmd);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/checker_report";
    std::string uri_str = uri.str();
    return send_request_post(uri_str, cmd);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Checker_Retry_Req& req)
{
    std::string cmd;
    rest.serialize_retry_req(req, cmd);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/checker_retry";
    std::string uri_str = uri.str();
    return send_request_put(uri_str, cmd);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Checker_Clear_Req& req)
{
    std::string query;
    rest.serialize_clear_req(req, query);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/checker_clear" << query;

    std::string uri_str = uri.str();
    return send_request_delete(uri_str);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Checker_List_Req&)
{
    std::string uri("/");
    uri +=  RESTAPI::API_VERSION + "/checker_list";
    return send_request_get(uri);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Checker_Validate_Req& req)
{
    std::string cmd;
    rest.serialize_validate_req(req, cmd);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/checker_validate";
    std::string uri_str = uri.str();
    return send_request_post(uri_str, cmd);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Checker_File_From_Id_Req& req)
{
    std::string cmd;
    rest.serialize_file_from_id_req(req, cmd);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/checker_file_from_id";
    std::string uri_str = uri.str();
    return send_request_post(uri_str, cmd);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Checker_Id_From_Filename_Req& req)
{
    std::string cmd;
    rest.serialize_id_from_filename_req(req, cmd);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/checker_id_from_filename";
    std::string uri_str = uri.str();
    return send_request_post(uri_str, cmd);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Checker_File_Information_Req& req)
{
    std::string cmd;
    rest.serialize_file_information_req(req, cmd);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/checker_file_information";
    std::string uri_str = uri.str();
    return send_request_post(uri_str, cmd);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Default_Values_For_Type_Req& req)
{
    std::string query;
    rest.serialize_default_values_for_type_req(req, query);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/default_values_for_type" << query;

    std::string uri_str = uri.str();
    return send_request_get(uri_str);
}

// Checker
//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::XSLT_Policy_Create_Req& req)
{
    std::string query;
    rest.serialize_xslt_policy_create_req(req, query);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/xslt_policy_create" << query;

    std::string uri_str = uri.str();
    return send_request_get(uri_str);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Policy_Import_Req& req)
{
    std::string cmd;
    rest.serialize_policy_import_req(req, cmd);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/policy_import";
    std::string uri_str = uri.str();
    return send_request_post(uri_str, cmd);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Policy_Remove_Req& req)
{
    std::string query;
    rest.serialize_policy_remove_req(req, query);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/policy_remove" << query;

    std::string uri_str = uri.str();
    return send_request_get(uri_str);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Policy_Dump_Req& req)
{
    std::string query;
    rest.serialize_policy_dump_req(req, query);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/policy_dump" << query;

    std::string uri_str = uri.str();
    return send_request_get(uri_str);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Policy_Save_Req& req)
{
    std::string query;
    rest.serialize_policy_save_req(req, query);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/policy_save" << query;

    std::string uri_str = uri.str();
    return send_request_get(uri_str);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Policy_Duplicate_Req& req)
{
    std::string query;
    rest.serialize_policy_duplicate_req(req, query);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/policy_duplicate" << query;

    std::string uri_str = uri.str();
    return send_request_get(uri_str);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Policy_Move_Req& req)
{
    std::string query;
    rest.serialize_policy_move_req(req, query);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/policy_move" << query;

    std::string uri_str = uri.str();
    return send_request_get(uri_str);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Policy_Change_Info_Req& req)
{
    std::string cmd;
    rest.serialize_policy_change_info_req(req, cmd);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/policy_change_info";
    std::string uri_str = uri.str();
    return send_request_post(uri_str, cmd);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Policy_Change_Type_Req& req)
{
    std::string cmd;
    rest.serialize_policy_change_type_req(req, cmd);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/policy_change_type";
    std::string uri_str = uri.str();
    return send_request_post(uri_str, cmd);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Policy_Get_Req& req)
{
    std::string query;
    rest.serialize_policy_get_req(req, query);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/policy_get" << query;

    std::string uri_str = uri.str();
    return send_request_get(uri_str);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Policy_Get_Name_Req& req)
{
    std::string query;
    rest.serialize_policy_get_name_req(req, query);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/policy_get_name" << query;

    std::string uri_str = uri.str();
    return send_request_get(uri_str);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Policy_Get_Policies_Count_Req&)
{
    std::string uri = "/" + RESTAPI::API_VERSION + "/policy_get_policies_count";
    return send_request_get(uri);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Policy_Clear_Policies_Req&)
{
    std::string uri = "/" + RESTAPI::API_VERSION + "/policy_clear_policies";
    return send_request_get(uri);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Policy_Get_Policies_Req& req)
{
    std::string query;
    rest.serialize_policy_get_policies_req(req, query);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/policy_get_policies" << query;

    std::string uri_str = uri.str();
    return send_request_get(uri_str);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::Policy_Get_Policies_Names_List_Req&)
{
    std::string uri = "/" + RESTAPI::API_VERSION + "/policy_get_policies_names_list";
    return send_request_get(uri);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::XSLT_Policy_Create_From_File_Req& req)
{
    std::string query;
    rest.serialize_xslt_policy_create_from_file_req(req, query);

    std::stringstream uri;
    uri << "/" << RESTAPI::API_VERSION << "/xslt_policy_create_from_file" << query;

    std::string uri_str = uri.str();
    return send_request_get(uri_str);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::XSLT_Policy_Rule_Create_Req& req)
{
    std::string query;
    rest.serialize_xslt_policy_rule_create_req(req, query);

    std::stringstream uri;
    uri << "/" << RESTAPI::API_VERSION << "/xslt_policy_rule_create" << query;

    std::string uri_str = uri.str();
    return send_request_get(uri_str);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::XSLT_Policy_Rule_Get_Req& req)
{
    std::string query;
    rest.serialize_xslt_policy_rule_get_req(req, query);

    std::stringstream uri;
    uri << "/" << RESTAPI::API_VERSION << "/xslt_policy_rule_get" << query;

    std::string uri_str = uri.str();
    return send_request_get(uri_str);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::XSLT_Policy_Rule_Edit_Req& req)
{
    std::string cmd;
    rest.serialize_xslt_policy_rule_edit_req(req, cmd);
    std::stringstream uri;

    uri << "/" << RESTAPI::API_VERSION << "/xslt_policy_rule_edit";
    std::string uri_str = uri.str();
    return send_request_post(uri_str, cmd);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::XSLT_Policy_Rule_Duplicate_Req& req)
{
    std::string query;
    rest.serialize_xslt_policy_rule_duplicate_req(req, query);

    std::stringstream uri;
    uri << "/" << RESTAPI::API_VERSION << "/xslt_policy_rule_duplicate" << query;

    std::string uri_str = uri.str();
    return send_request_get(uri_str);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::XSLT_Policy_Rule_Move_Req& req)
{
    std::string query;
    rest.serialize_xslt_policy_rule_move_req(req, query);

    std::stringstream uri;
    uri << "/" << RESTAPI::API_VERSION << "/xslt_policy_rule_move" << query;

    std::string uri_str = uri.str();
    return send_request_get(uri_str);
}

//---------------------------------------------------------------------------
int Http::send_request(RESTAPI::XSLT_Policy_Rule_Delete_Req& req)
{
    std::string query;
    rest.serialize_xslt_policy_rule_delete_req(req, query);

    std::stringstream uri;
    uri << "/" << RESTAPI::API_VERSION << "/xslt_policy_rule_delete" << query;

    std::string uri_str = uri.str();
    return send_request_get(uri_str);
}

//---------------------------------------------------------------------------
void Http::set_port(int port)
{
    this->port = port;
}

//---------------------------------------------------------------------------
void Http::set_address(std::string& address)
{
    this->address = address;
}

}
