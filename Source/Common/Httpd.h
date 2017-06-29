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
#ifndef HttpdH
#define HttpdH
//---------------------------------------------------------------------------
#include "REST_API.h"
#include <string>
//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// Class Httpd
//***************************************************************************

class Httpd
{

public:
    //Constructor/Destructor
    Httpd(void *mcl);
    virtual ~Httpd();

    virtual int init(std::string& err) = 0;
    virtual int bind(std::string& err) = 0;
    virtual int start(std::string& err) = 0;
    virtual int finish() = 0;

    virtual int send_result(int ret_code, const std::string& ret_msg, void *arg) = 0;

#define REQ_FUNC(type) \
    void get_request(std::string& json, RESTAPI::type##_Req** req);

    REQ_FUNC(MediaConch_Watch_Folder);
    REQ_FUNC(MediaConch_Edit_Watch_Folder);
    REQ_FUNC(MediaConch_Remove_Watch_Folder);

    REQ_FUNC(Checker_Analyze);
    REQ_FUNC(Checker_Stop);
    REQ_FUNC(Checker_Report);
    REQ_FUNC(Checker_Validate);
    REQ_FUNC(Checker_File_From_Id);
    REQ_FUNC(Checker_Id_From_Filename);
    REQ_FUNC(Checker_File_Information);

    REQ_FUNC(Policy_Import);
    REQ_FUNC(Policy_Change_Info);
    REQ_FUNC(Policy_Change_Type);
    REQ_FUNC(Policy_Change_Is_Public);
    REQ_FUNC(XSLT_Policy_Rule_Edit);
#undef REQ_FUNC

#define URI_REQ_FUNC(type) \
    void get_uri_request(std::string& uri, RESTAPI::type##_Req** req);

    URI_REQ_FUNC(MediaConch_Get_Plugins);
    URI_REQ_FUNC(MediaConch_List_Watch_Folders);

    URI_REQ_FUNC(Checker_Status);
    URI_REQ_FUNC(Checker_Clear);
    URI_REQ_FUNC(Checker_List);
    URI_REQ_FUNC(Default_Values_For_Type);

    URI_REQ_FUNC(XSLT_Policy_Create);
    URI_REQ_FUNC(Policy_Remove);
    URI_REQ_FUNC(Policy_Dump);
    URI_REQ_FUNC(Policy_Save);
    URI_REQ_FUNC(Policy_Duplicate);
    URI_REQ_FUNC(Policy_Move);
    URI_REQ_FUNC(Policy_Get);
    URI_REQ_FUNC(Policy_Get_Name);
    URI_REQ_FUNC(Policy_Get_Policies_Count);
    URI_REQ_FUNC(Policy_Clear_Policies);
    URI_REQ_FUNC(Policy_Get_Policies);
    URI_REQ_FUNC(Policy_Get_Public_Policies);
    URI_REQ_FUNC(Policy_Get_Policies_Names_List);
    URI_REQ_FUNC(XSLT_Policy_Create_From_File);
    URI_REQ_FUNC(XSLT_Policy_Rule_Create);
    URI_REQ_FUNC(XSLT_Policy_Rule_Get);
    URI_REQ_FUNC(XSLT_Policy_Rule_Duplicate);
    URI_REQ_FUNC(XSLT_Policy_Rule_Move);
    URI_REQ_FUNC(XSLT_Policy_Rule_Delete);
#undef URI_REQ_FUNC

    int send_result();

    void set_port(int port);
    int  get_port() const;
    void set_address(std::string& address);
    const std::string& get_address() const;

    std::string get_error() const;
    std::string get_result() const;

    // On command received and parsed
    typedef int (*on_mediaconch_get_plugins_command)(const RESTAPI::MediaConch_Get_Plugins_Req* req,
                                                     RESTAPI::MediaConch_Get_Plugins_Res& res, void* arg);
    typedef int (*on_mediaconch_watch_folder_command)(const RESTAPI::MediaConch_Watch_Folder_Req* req,
                                                      RESTAPI::MediaConch_Watch_Folder_Res& res, void* arg);
    typedef int (*on_mediaconch_list_watch_folders_command)(const RESTAPI::MediaConch_List_Watch_Folders_Req* req,
                                                            RESTAPI::MediaConch_List_Watch_Folders_Res& res, void* arg);
    typedef int (*on_mediaconch_edit_watch_folder_command)(const RESTAPI::MediaConch_Edit_Watch_Folder_Req* req,
                                                           RESTAPI::MediaConch_Edit_Watch_Folder_Res& res, void* arg);
    typedef int (*on_mediaconch_remove_watch_folder_command)(const RESTAPI::MediaConch_Remove_Watch_Folder_Req* req,
                                                             RESTAPI::MediaConch_Remove_Watch_Folder_Res& res, void* arg);

    typedef int (*on_checker_analyze_command)(const RESTAPI::Checker_Analyze_Req* req,
                                              RESTAPI::Checker_Analyze_Res& res, void* arg);
    typedef int (*on_checker_status_command)(const RESTAPI::Checker_Status_Req* req,
                                             RESTAPI::Checker_Status_Res& res, void* arg);
    typedef int (*on_checker_report_command)(const RESTAPI::Checker_Report_Req* req,
                                             RESTAPI::Checker_Report_Res& res, void* arg);
    typedef int (*on_checker_clear_command)(const RESTAPI::Checker_Clear_Req* req,
                                            RESTAPI::Checker_Clear_Res& res, void* arg);
    typedef int (*on_checker_stop_command)(const RESTAPI::Checker_Stop_Req* req,
                                           RESTAPI::Checker_Stop_Res& res, void* arg);
    typedef int (*on_checker_list_command)(const RESTAPI::Checker_List_Req* req,
                                           RESTAPI::Checker_List_Res& res, void* arg);
    typedef int (*on_checker_validate_command)(const RESTAPI::Checker_Validate_Req* req,
                                               RESTAPI::Checker_Validate_Res& res, void* arg);
    typedef int (*on_checker_file_from_id_command)(const RESTAPI::Checker_File_From_Id_Req* req,
                                                   RESTAPI::Checker_File_From_Id_Res& res, void* arg);
    typedef int (*on_checker_id_from_filename_command)(const RESTAPI::Checker_Id_From_Filename_Req* req,
                                                       RESTAPI::Checker_Id_From_Filename_Res& res, void* arg);
    typedef int (*on_checker_file_information_command)(const RESTAPI::Checker_File_Information_Req* req,
                                                       RESTAPI::Checker_File_Information_Res& res, void* arg);
    typedef int (*on_default_values_for_type_command)(const RESTAPI::Default_Values_For_Type_Req* req,
                                                      RESTAPI::Default_Values_For_Type_Res& res, void* arg);

    typedef int (*on_xslt_policy_create_command)(const RESTAPI::XSLT_Policy_Create_Req* req,
                                                 RESTAPI::XSLT_Policy_Create_Res& res, void* arg);
    typedef int (*on_policy_import_command)(const RESTAPI::Policy_Import_Req* req,
                                            RESTAPI::Policy_Import_Res& res, void* arg);
    typedef int (*on_policy_remove_command)(const RESTAPI::Policy_Remove_Req* req,
                                            RESTAPI::Policy_Remove_Res& res, void* arg);
    typedef int (*on_policy_dump_command)(const RESTAPI::Policy_Dump_Req* req,
                                          RESTAPI::Policy_Dump_Res& res, void* arg);
    typedef int (*on_policy_save_command)(const RESTAPI::Policy_Save_Req* req,
                                          RESTAPI::Policy_Save_Res& res, void* arg);
    typedef int (*on_policy_duplicate_command)(const RESTAPI::Policy_Duplicate_Req* req,
                                               RESTAPI::Policy_Duplicate_Res& res, void* arg);
    typedef int (*on_policy_move_command)(const RESTAPI::Policy_Move_Req* req,
                                          RESTAPI::Policy_Move_Res& res, void* arg);
    typedef int (*on_policy_change_info_command)(const RESTAPI::Policy_Change_Info_Req* req,
                                                 RESTAPI::Policy_Change_Info_Res& res, void* arg);
    typedef int (*on_policy_change_type_command)(const RESTAPI::Policy_Change_Type_Req* req,
                                                 RESTAPI::Policy_Change_Type_Res& res, void* arg);
    typedef int (*on_policy_change_is_public_command)(const RESTAPI::Policy_Change_Is_Public_Req* req,
                                                      RESTAPI::Policy_Change_Is_Public_Res& res, void* arg);
    typedef int (*on_policy_get_command)(const RESTAPI::Policy_Get_Req* req,
                                         RESTAPI::Policy_Get_Res& res, void* arg);
    typedef int (*on_policy_get_name_command)(const RESTAPI::Policy_Get_Name_Req* req,
                                              RESTAPI::Policy_Get_Name_Res& res, void* arg);
    typedef int (*on_policy_get_policies_count_command)(const RESTAPI::Policy_Get_Policies_Count_Req* req,
                                                        RESTAPI::Policy_Get_Policies_Count_Res& res, void* arg);
    typedef int (*on_policy_clear_policies_command)(const RESTAPI::Policy_Clear_Policies_Req* req,
                                                    RESTAPI::Policy_Clear_Policies_Res& res, void* arg);
    typedef int (*on_policy_get_policies_command)(const RESTAPI::Policy_Get_Policies_Req* req,
                                                  RESTAPI::Policy_Get_Policies_Res& res, void* arg);
    typedef int (*on_policy_get_public_policies_command)(const RESTAPI::Policy_Get_Public_Policies_Req* req,
                                                         RESTAPI::Policy_Get_Public_Policies_Res& res, void* arg);
    typedef int (*on_policy_get_policies_names_list_command)(const RESTAPI::Policy_Get_Policies_Names_List_Req* req,
                                                             RESTAPI::Policy_Get_Policies_Names_List_Res& res, void* arg);
    typedef int (*on_xslt_policy_create_from_file_command)(const RESTAPI::XSLT_Policy_Create_From_File_Req* req,
                                                           RESTAPI::XSLT_Policy_Create_From_File_Res& res, void* arg);
    typedef int (*on_xslt_policy_rule_create_command)(const RESTAPI::XSLT_Policy_Rule_Create_Req* req,
                                                      RESTAPI::XSLT_Policy_Rule_Create_Res& res, void* arg);
    typedef int (*on_xslt_policy_rule_get_command)(const RESTAPI::XSLT_Policy_Rule_Get_Req* req,
                                                   RESTAPI::XSLT_Policy_Rule_Get_Res& res, void* arg);
    typedef int (*on_xslt_policy_rule_edit_command)(const RESTAPI::XSLT_Policy_Rule_Edit_Req* req,
                                                    RESTAPI::XSLT_Policy_Rule_Edit_Res& res, void* arg);
    typedef int (*on_xslt_policy_rule_duplicate_command)(const RESTAPI::XSLT_Policy_Rule_Duplicate_Req* req,
                                                         RESTAPI::XSLT_Policy_Rule_Duplicate_Res& res, void* arg);
    typedef int (*on_xslt_policy_rule_move_command)(const RESTAPI::XSLT_Policy_Rule_Move_Req* req,
                                                    RESTAPI::XSLT_Policy_Rule_Move_Res& res, void* arg);
    typedef int (*on_xslt_policy_rule_delete_command)(const RESTAPI::XSLT_Policy_Rule_Delete_Req* req,
                                                      RESTAPI::XSLT_Policy_Rule_Delete_Res& res, void* arg);

    struct Commands
    {
        Commands() : mediaconch_get_plugins_cb(NULL), mediaconch_watch_folder_cb(NULL),
                     mediaconch_list_watch_folders_cb(NULL),
                     mediaconch_edit_watch_folder_cb(NULL), mediaconch_remove_watch_folder_cb(NULL),
                     checker_analyze_cb(NULL), checker_status_cb(NULL), checker_report_cb(NULL),
                     checker_clear_cb(NULL), checker_stop_cb(NULL), checker_list_cb(NULL),
                     checker_validate_cb(NULL), checker_file_from_id_cb(NULL),
                     checker_id_from_filename_cb(NULL), checker_file_information_cb(NULL),
                     default_values_for_type_cb(NULL),
                     xslt_policy_create_cb(NULL),
                     policy_import_cb(NULL),
                     policy_remove_cb(NULL),
                     policy_dump_cb(NULL),
                     policy_save_cb(NULL),
                     policy_duplicate_cb(NULL),
                     policy_move_cb(NULL),
                     policy_change_info_cb(NULL),
                     policy_change_type_cb(NULL),
                     policy_change_is_public_cb(NULL),
                     policy_get_cb(NULL),
                     policy_get_name_cb(NULL),
                     policy_get_policies_count_cb(NULL),
                     policy_clear_policies_cb(NULL),
                     policy_get_policies_cb(NULL),
                     policy_get_public_policies_cb(NULL),
                     policy_get_policies_names_list_cb(NULL),
                     xslt_policy_create_from_file_cb(NULL),
                     xslt_policy_rule_create_cb(NULL),
                     xslt_policy_rule_get_cb(NULL),
                     xslt_policy_rule_edit_cb(NULL),
                     xslt_policy_rule_duplicate_cb(NULL),
                     xslt_policy_rule_move_cb(NULL),
                     xslt_policy_rule_delete_cb(NULL)
            {
            }
        //mediaconch
        on_mediaconch_get_plugins_command         mediaconch_get_plugins_cb;
        on_mediaconch_watch_folder_command        mediaconch_watch_folder_cb;
        on_mediaconch_list_watch_folders_command  mediaconch_list_watch_folders_cb;
        on_mediaconch_edit_watch_folder_command   mediaconch_edit_watch_folder_cb;
        on_mediaconch_remove_watch_folder_command mediaconch_remove_watch_folder_cb;

        //checker
        on_checker_analyze_command                checker_analyze_cb;
        on_checker_status_command                 checker_status_cb;
        on_checker_report_command                 checker_report_cb;
        on_checker_clear_command                  checker_clear_cb;
        on_checker_stop_command                   checker_stop_cb;
        on_checker_list_command                   checker_list_cb;
        on_checker_validate_command               checker_validate_cb;
        on_checker_file_from_id_command           checker_file_from_id_cb;
        on_checker_id_from_filename_command       checker_id_from_filename_cb;
        on_checker_file_information_command       checker_file_information_cb;
        on_default_values_for_type_command        default_values_for_type_cb;

        // policy
        on_xslt_policy_create_command             xslt_policy_create_cb;
        on_policy_import_command                  policy_import_cb;
        on_policy_remove_command                  policy_remove_cb;
        on_policy_dump_command                    policy_dump_cb;
        on_policy_save_command                    policy_save_cb;
        on_policy_duplicate_command               policy_duplicate_cb;
        on_policy_move_command                    policy_move_cb;
        on_policy_change_info_command             policy_change_info_cb;
        on_policy_change_type_command             policy_change_type_cb;
        on_policy_change_is_public_command        policy_change_is_public_cb;
        on_policy_get_command                     policy_get_cb;
        on_policy_get_name_command                policy_get_name_cb;
        on_policy_get_policies_count_command      policy_get_policies_count_cb;
        on_policy_clear_policies_command          policy_clear_policies_cb;
        on_policy_get_policies_command            policy_get_policies_cb;
        on_policy_get_public_policies_command     policy_get_public_policies_cb;
        on_policy_get_policies_names_list_command policy_get_policies_names_list_cb;
        on_xslt_policy_create_from_file_command   xslt_policy_create_from_file_cb;
        on_xslt_policy_rule_create_command        xslt_policy_rule_create_cb;
        on_xslt_policy_rule_get_command           xslt_policy_rule_get_cb;
        on_xslt_policy_rule_edit_command          xslt_policy_rule_edit_cb;
        on_xslt_policy_rule_duplicate_command     xslt_policy_rule_duplicate_cb;
        on_xslt_policy_rule_move_command          xslt_policy_rule_move_cb;
        on_xslt_policy_rule_delete_command        xslt_policy_rule_delete_cb;
    };

    Commands commands;

protected:
    int            port;
    std::string    address;
    RESTAPI        rest;
    void          *parent;

    std::string error;
    std::string result;

private:
    Httpd (const Httpd&);
    Httpd& operator=(const Httpd&);
};

}

#endif
