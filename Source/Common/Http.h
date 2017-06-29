/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Http functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef HttpH
#define HttpH
//---------------------------------------------------------------------------
#include "REST_API.h"
#include "MediaConchLib.h"
#include <string>
//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// Class Http
//***************************************************************************

class Http
{
public:
    //Constructor/Destructor
    Http();
    virtual ~Http();

    virtual int init(std::string& err) = 0;
    virtual int start(std::string& err) = 0;
    virtual int stop() = 0;
    virtual int finish() = 0;

    // MediaConch
    int send_request(RESTAPI::MediaConch_Get_Plugins_Req& req, std::string& err);
    int send_request(RESTAPI::MediaConch_Watch_Folder_Req& req, std::string& err);
    int send_request(RESTAPI::MediaConch_List_Watch_Folders_Req& req, std::string& err);
    int send_request(RESTAPI::MediaConch_Edit_Watch_Folder_Req& req, std::string& err);
    int send_request(RESTAPI::MediaConch_Remove_Watch_Folder_Req& req, std::string& err);

    // Checker
    int send_request(RESTAPI::Checker_Analyze_Req& req, std::string& err);
    int send_request(RESTAPI::Checker_Status_Req& req, std::string& err);
    int send_request(RESTAPI::Checker_Report_Req& req, std::string& err);
    int send_request(RESTAPI::Checker_Clear_Req& req, std::string& err);
    int send_request(RESTAPI::Checker_Stop_Req& req, std::string& err);
    int send_request(RESTAPI::Checker_List_Req& req, std::string& err);
    int send_request(RESTAPI::Checker_Validate_Req& req, std::string& err);
    int send_request(RESTAPI::Checker_File_From_Id_Req& req, std::string& err);
    int send_request(RESTAPI::Checker_Id_From_Filename_Req& req, std::string& err);
    int send_request(RESTAPI::Checker_File_Information_Req& req, std::string& err);
    int send_request(RESTAPI::Default_Values_For_Type_Req& req, std::string& err);

    // Policy
    int send_request(RESTAPI::XSLT_Policy_Create_Req& req, std::string& err);
    int send_request(RESTAPI::Policy_Import_Req& req, std::string& err);
    int send_request(RESTAPI::Policy_Remove_Req& req, std::string& err);
    int send_request(RESTAPI::Policy_Dump_Req& req, std::string& err);
    int send_request(RESTAPI::Policy_Save_Req& req, std::string& err);
    int send_request(RESTAPI::Policy_Duplicate_Req& req, std::string& err);
    int send_request(RESTAPI::Policy_Move_Req& req, std::string& err);
    int send_request(RESTAPI::Policy_Change_Info_Req& req, std::string& err);
    int send_request(RESTAPI::Policy_Change_Type_Req& req, std::string& err);
    int send_request(RESTAPI::Policy_Change_Is_Public_Req& req, std::string& err);
    int send_request(RESTAPI::Policy_Get_Req& req, std::string& err);
    int send_request(RESTAPI::Policy_Get_Name_Req& req, std::string& err);
    int send_request(RESTAPI::Policy_Get_Policies_Count_Req& req, std::string& err);
    int send_request(RESTAPI::Policy_Clear_Policies_Req& req, std::string& err);
    int send_request(RESTAPI::Policy_Get_Policies_Req& req, std::string& err);
    int send_request(RESTAPI::Policy_Get_Public_Policies_Req& req, std::string& err);
    int send_request(RESTAPI::Policy_Get_Policies_Names_List_Req& req, std::string& err);
    int send_request(RESTAPI::XSLT_Policy_Create_From_File_Req& req, std::string& err);
    int send_request(RESTAPI::XSLT_Policy_Rule_Create_Req& req, std::string& err);
    int send_request(RESTAPI::XSLT_Policy_Rule_Get_Req& req, std::string& err);
    int send_request(RESTAPI::XSLT_Policy_Rule_Edit_Req& req, std::string& err);
    int send_request(RESTAPI::XSLT_Policy_Rule_Duplicate_Req& req, std::string& err);
    int send_request(RESTAPI::XSLT_Policy_Rule_Move_Req& req, std::string& err);
    int send_request(RESTAPI::XSLT_Policy_Rule_Delete_Req& req, std::string& err);

    void set_port(int port);
    void set_address(std::string& address);

    void reset_daemon_id() { current_daemon_id = -1; }

    void get_result(std::string& res) { res = result; }
    int  get_http_code() { return http_code; }

protected:
    RESTAPI                  rest;
    std::string              address;
    int                      port;
    std::string              result;
    int                      http_code;

    static int               current_daemon_id;

    virtual int send_request_get(std::string& uri, std::string& err) = 0;
    virtual int send_request_post(std::string& uri, std::string& str, std::string& err) = 0;
    virtual int send_request_put(std::string& uri, std::string& str, std::string& err) = 0;
    virtual int send_request_delete(std::string& uri, std::string& err) = 0;

private:
    Http (const Http&);
    Http& operator=(const Http&);
};

}

#endif
