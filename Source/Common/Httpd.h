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

    virtual int init() = 0;
    virtual int bind() = 0;
    virtual int start() = 0;
    virtual int finish() = 0;

    virtual int send_result(int ret_code, std::string& ret_msg, void *arg) = 0;

#define REQ_FUNC(type) \
    void get_request(std::string& json, RESTAPI::type##_Req** req);

    REQ_FUNC(Checker_Analyze);
    REQ_FUNC(Checker_Report);
    REQ_FUNC(Checker_Validate);
    REQ_FUNC(Checker_File_From_Id);
    REQ_FUNC(Checker_Retry);
    REQ_FUNC(Policy_Import);
    REQ_FUNC(Policy_Change_Name);
    REQ_FUNC(XSLT_Policy_Rule_Edit);
#undef REQ_FUNC

#define URI_REQ_FUNC(type) \
    void get_uri_request(std::string& uri, RESTAPI::type##_Req** req);

    URI_REQ_FUNC(Checker_Status);
    URI_REQ_FUNC(Checker_Clear);
    URI_REQ_FUNC(Checker_List);
    URI_REQ_FUNC(Default_Values_For_Type);
    URI_REQ_FUNC(XSLT_Policy_Create);
    URI_REQ_FUNC(Policy_Remove);
    URI_REQ_FUNC(Policy_Dump);
    URI_REQ_FUNC(Policy_Save);
    URI_REQ_FUNC(Policy_Duplicate);
    URI_REQ_FUNC(Policy_Get);
    URI_REQ_FUNC(Policy_Get_Name);
    URI_REQ_FUNC(Policy_Get_Policies_Count);
    URI_REQ_FUNC(Policy_Clear_Policies);
    URI_REQ_FUNC(Policy_Get_Policies);
    URI_REQ_FUNC(XSLT_Policy_Create_From_File);
    URI_REQ_FUNC(XSLT_Policy_Rule_Create);
    URI_REQ_FUNC(XSLT_Policy_Rule_Get);
    URI_REQ_FUNC(XSLT_Policy_Rule_Duplicate);
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
    typedef int (*on_analyze_command)(const RESTAPI::Checker_Analyze_Req* req,
                                      RESTAPI::Checker_Analyze_Res& res, void* arg);
    typedef int (*on_status_command)(const RESTAPI::Checker_Status_Req* req,
                                     RESTAPI::Checker_Status_Res& res, void* arg);
    typedef int (*on_report_command)(const RESTAPI::Checker_Report_Req* req,
                                     RESTAPI::Checker_Report_Res& res, void* arg);
    typedef int (*on_retry_command)(const RESTAPI::Checker_Retry_Req* req,
                                    RESTAPI::Checker_Retry_Res& res, void* arg);
    typedef int (*on_clear_command)(const RESTAPI::Checker_Clear_Req* req,
                                    RESTAPI::Checker_Clear_Res& res, void* arg);
    typedef int (*on_list_command)(const RESTAPI::Checker_List_Req* req,
                                   RESTAPI::Checker_List_Res& res, void* arg);
    typedef int (*on_validate_command)(const RESTAPI::Checker_Validate_Req* req,
                                       RESTAPI::Checker_Validate_Res& res, void* arg);
    typedef int (*on_file_from_id_command)(const RESTAPI::Checker_File_From_Id_Req* req,
                                           RESTAPI::Checker_File_From_Id_Res& res, void* arg);
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
    typedef int (*on_policy_change_name_command)(const RESTAPI::Policy_Change_Name_Req* req,
                                                 RESTAPI::Policy_Change_Name_Res& res, void* arg);
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
    typedef int (*on_xslt_policy_rule_delete_command)(const RESTAPI::XSLT_Policy_Rule_Delete_Req* req,
                                                      RESTAPI::XSLT_Policy_Rule_Delete_Res& res, void* arg);

    struct Commands
    {
        Commands() : analyze_cb(NULL), status_cb(NULL), report_cb(NULL),
                     retry_cb(NULL), clear_cb(NULL), list_cb(NULL),
                     validate_cb(NULL), file_from_id_cb(NULL),
                     default_values_for_type_cb(NULL),
                     xslt_policy_create_cb(NULL),
                     policy_import_cb(NULL),
                     policy_remove_cb(NULL),
                     policy_dump_cb(NULL),
                     policy_save_cb(NULL),
                     policy_duplicate_cb(NULL),
                     policy_change_name_cb(NULL),
                     policy_get_cb(NULL),
                     policy_get_name_cb(NULL),
                     policy_get_policies_count_cb(NULL),
                     policy_clear_policies_cb(NULL),
                     policy_get_policies_cb(NULL),
                     xslt_policy_create_from_file_cb(NULL),
                     xslt_policy_rule_create_cb(NULL),
                     xslt_policy_rule_get_cb(NULL),
                     xslt_policy_rule_edit_cb(NULL),
                     xslt_policy_rule_duplicate_cb(NULL),
                     xslt_policy_rule_delete_cb(NULL)
            {
            }
        //checker
        on_analyze_command                      analyze_cb;
        on_status_command                       status_cb;
        on_report_command                       report_cb;
        on_retry_command                        retry_cb;
        on_clear_command                        clear_cb;
        on_list_command                         list_cb;
        on_validate_command                     validate_cb;
        on_file_from_id_command                 file_from_id_cb;
        on_default_values_for_type_command      default_values_for_type_cb;

        on_xslt_policy_create_command xslt_policy_create_cb;
        on_policy_import_command policy_import_cb;
        on_policy_remove_command policy_remove_cb;
        on_policy_dump_command policy_dump_cb;
        on_policy_save_command policy_save_cb;
        on_policy_duplicate_command policy_duplicate_cb;
        on_policy_change_name_command policy_change_name_cb;
        on_policy_get_command policy_get_cb;
        on_policy_get_name_command policy_get_name_cb;
        on_policy_get_policies_count_command policy_get_policies_count_cb;
        on_policy_clear_policies_command policy_clear_policies_cb;
        on_policy_get_policies_command policy_get_policies_cb;
        on_xslt_policy_create_from_file_command xslt_policy_create_from_file_cb;
        on_xslt_policy_rule_create_command xslt_policy_rule_create_cb;
        on_xslt_policy_rule_get_command xslt_policy_rule_get_cb;
        on_xslt_policy_rule_edit_command xslt_policy_rule_edit_cb;
        on_xslt_policy_rule_duplicate_command xslt_policy_rule_duplicate_cb;
        on_xslt_policy_rule_delete_command xslt_policy_rule_delete_cb;
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
