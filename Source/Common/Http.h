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

    virtual int init() = 0;
    virtual int start() = 0;
    virtual int stop() = 0;
    virtual int finish() = 0;

    // Checker
    int send_request(RESTAPI::Checker_Analyze_Req& req);
    int send_request(RESTAPI::Checker_Status_Req& req);
    int send_request(RESTAPI::Checker_Report_Req& req);
    int send_request(RESTAPI::Checker_Retry_Req& req);
    int send_request(RESTAPI::Checker_Clear_Req& req);
    int send_request(RESTAPI::Checker_List_Req& req);
    int send_request(RESTAPI::Checker_Validate_Req& req);
    int send_request(RESTAPI::Checker_File_From_Id_Req& req);
    int send_request(RESTAPI::Default_Values_For_Type_Req& req);

    // Policy
    int send_request(RESTAPI::XSLT_Policy_Create_Req& req);
    int send_request(RESTAPI::Policy_Import_Req& req);
    int send_request(RESTAPI::Policy_Remove_Req& req);
    int send_request(RESTAPI::Policy_Dump_Req& req);
    int send_request(RESTAPI::Policy_Save_Req& req);
    int send_request(RESTAPI::Policy_Duplicate_Req& req);
    int send_request(RESTAPI::Policy_Change_Name_Req& req);
    int send_request(RESTAPI::Policy_Get_Req& req);
    int send_request(RESTAPI::Policy_Get_Name_Req& req);
    int send_request(RESTAPI::Policy_Get_Policies_Count_Req& req);
    int send_request(RESTAPI::Policy_Clear_Policies_Req& req);
    int send_request(RESTAPI::Policy_Get_Policies_Req& req);
    int send_request(RESTAPI::Policy_Get_Policies_Names_List_Req& req);
    int send_request(RESTAPI::XSLT_Policy_Create_From_File_Req& req);
    int send_request(RESTAPI::XSLT_Policy_Rule_Create_Req& req);
    int send_request(RESTAPI::XSLT_Policy_Rule_Get_Req& req);
    int send_request(RESTAPI::XSLT_Policy_Rule_Edit_Req& req);
    int send_request(RESTAPI::XSLT_Policy_Rule_Duplicate_Req& req);
    int send_request(RESTAPI::XSLT_Policy_Rule_Delete_Req& req);

    void set_port(int port);
    void set_address(std::string& address);

    MediaConchLib::errorHttp get_error() const { return error; }
    std::string              get_result() const { return result; }

protected:
    RESTAPI                  rest;
    std::string              address;
    int                      port;
    std::string              result;
    MediaConchLib::errorHttp error;

    virtual int send_request_get(std::string& uri) = 0;
    virtual int send_request_post(std::string& uri, std::string& str) = 0;
    virtual int send_request_put(std::string& uri, std::string& str) = 0;
    virtual int send_request_delete(std::string& uri) = 0;

private:
    Http (const Http&);
    Http& operator=(const Http&);
};

}

#endif
