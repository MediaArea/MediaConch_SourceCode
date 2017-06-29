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

//***************************************************************************
// send_request generator
//***************************************************************************

//---------------------------------------------------------------------------
#define SEND_REQUEST_GET(NAME, DEF)                                     \
int Http::send_request(RESTAPI::NAME& req, std::string& err)            \
{                                                                       \
    std::string query;                                                  \
    if (rest.serialize_##DEF##_req(req, query, err) < 0)                \
        return -1;                                                      \
                                                                        \
    std::stringstream uri;                                              \
    uri << "/" << RESTAPI::API_VERSION << "/"#DEF << query;             \
    std::string uri_str = uri.str();                                    \
                                                                        \
    return send_request_get(uri_str, err);                              \
}

//---------------------------------------------------------------------------
#define SEND_REQUEST_POST(NAME, DEF)                                    \
int Http::send_request(RESTAPI::NAME& req, std::string& err)            \
{                                                                       \
    std::string cmd;                                                    \
    if (rest.serialize_##DEF##_req(req, cmd, err) < 0)                  \
        return -1;                                                      \
                                                                        \
    std::stringstream uri;                                              \
    uri << "/" << RESTAPI::API_VERSION << "/"#DEF;                      \
    std::string uri_str = uri.str();                                    \
                                                                        \
    return send_request_post(uri_str, cmd, err);                        \
}

//---------------------------------------------------------------------------
#define SEND_REQUEST_PUT(NAME, DEF)                                     \
int Http::send_request(RESTAPI::NAME& req, std::string& err)            \
{                                                                       \
    std::string cmd;                                                    \
    if (rest.serialize_##DEF##_req(req, cmd, err) < 0)                  \
        return -1;                                                      \
                                                                        \
    std::stringstream uri;                                              \
    uri << "/" << RESTAPI::API_VERSION << "/"#DEF;                      \
    std::string uri_str = uri.str();                                    \
                                                                        \
    return send_request_put(uri_str, cmd, err);                         \
}

//---------------------------------------------------------------------------
#define SEND_REQUEST_DELETE(NAME, DEF)                                  \
int Http::send_request(RESTAPI::NAME& req, std::string& err)            \
{                                                                       \
    std::string query;                                                  \
    if (rest.serialize_##DEF##_req(req, query, err) < 0)                \
        return -1;                                                      \
                                                                        \
    std::stringstream uri;                                              \
    uri << "/" << RESTAPI::API_VERSION << "/"#DEF << query;             \
    std::string uri_str = uri.str();                                    \
                                                                        \
    return send_request_delete(uri_str, err);                           \
}

//***************************************************************************
// MediaConch
//***************************************************************************
SEND_REQUEST_GET(MediaConch_Get_Plugins_Req, mediaconch_get_plugins);
SEND_REQUEST_POST(MediaConch_Watch_Folder_Req, mediaconch_watch_folder);
SEND_REQUEST_GET(MediaConch_List_Watch_Folders_Req, mediaconch_list_watch_folders);
SEND_REQUEST_POST(MediaConch_Edit_Watch_Folder_Req, mediaconch_edit_watch_folder);
SEND_REQUEST_POST(MediaConch_Remove_Watch_Folder_Req, mediaconch_remove_watch_folder);

//***************************************************************************
// Checker
//***************************************************************************
SEND_REQUEST_POST(Checker_Analyze_Req, checker_analyze);
SEND_REQUEST_GET(Checker_Status_Req, checker_status);
SEND_REQUEST_POST(Checker_Report_Req, checker_report);
SEND_REQUEST_DELETE(Checker_Clear_Req, checker_clear);
SEND_REQUEST_POST(Checker_Stop_Req, checker_stop);
SEND_REQUEST_GET(Checker_List_Req, checker_list);
SEND_REQUEST_POST(Checker_Validate_Req, checker_validate);
SEND_REQUEST_POST(Checker_File_From_Id_Req, checker_file_from_id);
SEND_REQUEST_POST(Checker_Id_From_Filename_Req, checker_id_from_filename);
SEND_REQUEST_POST(Checker_File_Information_Req, checker_file_information);
SEND_REQUEST_GET(Default_Values_For_Type_Req, default_values_for_type);

//***************************************************************************
// Policy
//***************************************************************************
SEND_REQUEST_GET(XSLT_Policy_Create_Req, xslt_policy_create);
SEND_REQUEST_POST(Policy_Import_Req, policy_import);
SEND_REQUEST_GET(Policy_Remove_Req, policy_remove);
SEND_REQUEST_GET(Policy_Dump_Req, policy_dump);
SEND_REQUEST_GET(Policy_Save_Req, policy_save);
SEND_REQUEST_GET(Policy_Duplicate_Req, policy_duplicate);
SEND_REQUEST_GET(Policy_Move_Req, policy_move);
SEND_REQUEST_POST(Policy_Change_Info_Req, policy_change_info);
SEND_REQUEST_POST(Policy_Change_Type_Req, policy_change_type);
SEND_REQUEST_POST(Policy_Change_Is_Public_Req, policy_change_is_public);
SEND_REQUEST_GET(Policy_Get_Req, policy_get);
SEND_REQUEST_GET(Policy_Get_Name_Req, policy_get_name);
SEND_REQUEST_GET(Policy_Get_Policies_Count_Req, policy_get_policies_count);
SEND_REQUEST_GET(Policy_Clear_Policies_Req, policy_clear_policies);
SEND_REQUEST_GET(Policy_Get_Policies_Req, policy_get_policies);
SEND_REQUEST_GET(Policy_Get_Public_Policies_Req, policy_get_public_policies);
SEND_REQUEST_GET(Policy_Get_Policies_Names_List_Req, policy_get_policies_names_list);
SEND_REQUEST_GET(XSLT_Policy_Create_From_File_Req, xslt_policy_create_from_file);
SEND_REQUEST_GET(XSLT_Policy_Rule_Create_Req, xslt_policy_rule_create);
SEND_REQUEST_GET(XSLT_Policy_Rule_Get_Req, xslt_policy_rule_get);
SEND_REQUEST_POST(XSLT_Policy_Rule_Edit_Req, xslt_policy_rule_edit);
SEND_REQUEST_GET(XSLT_Policy_Rule_Duplicate_Req, xslt_policy_rule_duplicate);
SEND_REQUEST_GET(XSLT_Policy_Rule_Move_Req, xslt_policy_rule_move);
SEND_REQUEST_GET(XSLT_Policy_Rule_Delete_Req, xslt_policy_rule_delete);

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
