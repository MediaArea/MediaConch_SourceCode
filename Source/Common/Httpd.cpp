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
#include <stdio.h>
#include "Common/MediaConchLib.h"
#include "Httpd.h"

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Httpd
//***************************************************************************

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Httpd::Httpd(void *p) : port(80), address("0.0.0.0"), parent(p)
{
}

//---------------------------------------------------------------------------
Httpd::~Httpd()
{
}

//---------------------------------------------------------------------------
void Httpd::set_port(int port)
{
    this->port = port;
}

//---------------------------------------------------------------------------
int Httpd::get_port() const
{
    return this->port;
}

//---------------------------------------------------------------------------
void Httpd::set_address(std::string& address)
{
    this->address = address;
}

//---------------------------------------------------------------------------
const std::string& Httpd::get_address() const
{
    return this->address;
}

//---------------------------------------------------------------------------
int Httpd::send_result()
{
    //TODO
    return -1;
}

//---------------------------------------------------------------------------
#define MAKE_REQ_FUNC(name, type)                                         \
    void Httpd::get_request(std::string& json, RESTAPI::type##_Req** req) \
    {                                                                     \
        std::string err;                                                  \
        *req = rest.parse_##name##_req(json, err);                        \
        if (!*req)                                                        \
            error = err;                                                  \
    }

    MAKE_REQ_FUNC(mediaconch_watch_folder, MediaConch_Watch_Folder)
    MAKE_REQ_FUNC(mediaconch_edit_watch_folder, MediaConch_Edit_Watch_Folder)
    MAKE_REQ_FUNC(mediaconch_remove_watch_folder, MediaConch_Remove_Watch_Folder)

    MAKE_REQ_FUNC(checker_analyze, Checker_Analyze)
    MAKE_REQ_FUNC(checker_stop, Checker_Stop)
    MAKE_REQ_FUNC(checker_report, Checker_Report)
    MAKE_REQ_FUNC(checker_validate, Checker_Validate)
    MAKE_REQ_FUNC(checker_file_from_id, Checker_File_From_Id)
    MAKE_REQ_FUNC(checker_id_from_filename, Checker_Id_From_Filename)
    MAKE_REQ_FUNC(checker_file_information, Checker_File_Information)

    MAKE_REQ_FUNC(policy_import, Policy_Import);
    MAKE_REQ_FUNC(policy_change_info, Policy_Change_Info);
    MAKE_REQ_FUNC(policy_change_type, Policy_Change_Type);
    MAKE_REQ_FUNC(policy_change_is_public, Policy_Change_Is_Public);
    MAKE_REQ_FUNC(xslt_policy_rule_edit, XSLT_Policy_Rule_Edit);

#undef MAKE_REQ_FUNC

//---------------------------------------------------------------------------
#define MAKE_URI_REQ_FUNC(name, type)                                      \
    void Httpd::get_uri_request(std::string& uri_query,                    \
                                RESTAPI::type##_Req** req)                 \
    {                                                                      \
        std::string err;                                                   \
        *req = rest.parse_uri_##name##_req(uri_query, err);                \
        if (!*req)                                                         \
            error = err;                                                   \
    }

    MAKE_URI_REQ_FUNC(mediaconch_get_plugins, MediaConch_Get_Plugins)
    MAKE_URI_REQ_FUNC(mediaconch_list_watch_folders, MediaConch_List_Watch_Folders)

    MAKE_URI_REQ_FUNC(checker_status, Checker_Status)
    MAKE_URI_REQ_FUNC(checker_clear, Checker_Clear)
    MAKE_URI_REQ_FUNC(checker_list, Checker_List)
    MAKE_URI_REQ_FUNC(default_values_for_type, Default_Values_For_Type)

    MAKE_URI_REQ_FUNC(xslt_policy_create, XSLT_Policy_Create)
    MAKE_URI_REQ_FUNC(policy_remove, Policy_Remove)
    MAKE_URI_REQ_FUNC(policy_dump, Policy_Dump)
    MAKE_URI_REQ_FUNC(policy_save, Policy_Save)
    MAKE_URI_REQ_FUNC(policy_duplicate, Policy_Duplicate)
    MAKE_URI_REQ_FUNC(policy_move, Policy_Move)
    MAKE_URI_REQ_FUNC(policy_get, Policy_Get)
    MAKE_URI_REQ_FUNC(policy_get_name, Policy_Get_Name)
    MAKE_URI_REQ_FUNC(policy_get_policies_count, Policy_Get_Policies_Count)
    MAKE_URI_REQ_FUNC(policy_clear_policies, Policy_Clear_Policies)
    MAKE_URI_REQ_FUNC(policy_get_policies, Policy_Get_Policies)
    MAKE_URI_REQ_FUNC(policy_get_public_policies, Policy_Get_Public_Policies)
    MAKE_URI_REQ_FUNC(policy_get_policies_names_list, Policy_Get_Policies_Names_List)
    MAKE_URI_REQ_FUNC(xslt_policy_create_from_file, XSLT_Policy_Create_From_File)
    MAKE_URI_REQ_FUNC(xslt_policy_rule_create, XSLT_Policy_Rule_Create)
    MAKE_URI_REQ_FUNC(xslt_policy_rule_get, XSLT_Policy_Rule_Get)
    MAKE_URI_REQ_FUNC(xslt_policy_rule_duplicate, XSLT_Policy_Rule_Duplicate)
    MAKE_URI_REQ_FUNC(xslt_policy_rule_move, XSLT_Policy_Rule_Move)
    MAKE_URI_REQ_FUNC(xslt_policy_rule_delete, XSLT_Policy_Rule_Delete)

#undef MAKE_URI_REQ_FUNC

//---------------------------------------------------------------------------
std::string Httpd::get_error() const
{
    return error;
}

//---------------------------------------------------------------------------
std::string Httpd::get_result() const
{
    return result;
}

}
