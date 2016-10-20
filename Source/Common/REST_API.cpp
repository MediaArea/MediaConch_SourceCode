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
#include <sstream>
#include "REST_API.h"
#include "Container.h"
#include "NoContainer.h"
#include "Json.h"
#include "stdlib.h"

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// RESTAPI
//***************************************************************************

const std::string RESTAPI::API_VERSION = "1.11";

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
RESTAPI::RESTAPI()
{
#ifdef HAVE_JANSSON
    model = new Json;
#else
    model = new NoContainer;
#endif
}

//---------------------------------------------------------------------------
RESTAPI::~RESTAPI()
{
    if (model)
    {
        delete model;
        model = NULL;
    }
}

//***************************************************************************
// Destructors
//***************************************************************************

//---------------------------------------------------------------------------
RESTAPI::MediaConch_Watch_Folder_Req::~MediaConch_Watch_Folder_Req()
{
    if (user)
    {
        delete user;
        user = NULL;
    }
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Duplicate_Req::~Policy_Duplicate_Req()
{
    if (dst_user)
    {
        delete dst_user;
        dst_user = NULL;
    }
}

//---------------------------------------------------------------------------
RESTAPI::MediaConch_Watch_Folder_Res::~MediaConch_Watch_Folder_Res()
{
    if (nok)
        delete nok;
}

//---------------------------------------------------------------------------
RESTAPI::MediaConch_List_Watch_Folders_Res::~MediaConch_List_Watch_Folders_Res()
{
    if (nok)
        delete nok;
}

//---------------------------------------------------------------------------
RESTAPI::MediaConch_Edit_Watch_Folder_Res::~MediaConch_Edit_Watch_Folder_Res()
{
    if (nok)
        delete nok;
}

//---------------------------------------------------------------------------
RESTAPI::MediaConch_Remove_Watch_Folder_Res::~MediaConch_Remove_Watch_Folder_Res()
{
    if (nok)
        delete nok;
}

//---------------------------------------------------------------------------
RESTAPI::MediaConch_Get_Plugins_Res::~MediaConch_Get_Plugins_Res()
{
    plugins.clear();
    if (nok)
        delete nok;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Analyze_Res::~Checker_Analyze_Res()
{
    for (size_t i = 0; i < ok.size(); ++i)
        delete ok[i];
    ok.clear();

    for (size_t i = 0; i < nok.size(); ++i)
        delete nok[i];
    nok.clear();
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Status_Ok::~Checker_Status_Ok()
{
    if (tool)
    {
        delete tool;
        tool = NULL;
    }

    if (percent)
    {
        delete percent;
        percent = NULL;
    }
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Status_Res::~Checker_Status_Res()
{
    for (size_t i = 0; i < ok.size(); ++i)
        delete ok[i];
    ok.clear();

    for (size_t i = 0; i < nok.size(); ++i)
        delete nok[i];
    nok.clear();
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Report_Res::~Checker_Report_Res()
{
    for (size_t i = 0; i < nok.size(); ++i)
        delete nok[i];
    nok.clear();
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Retry_Res::~Checker_Retry_Res()
{
    for (size_t i = 0; i < nok.size(); ++i)
        delete nok[i];
    nok.clear();
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Clear_Res::~Checker_Clear_Res()
{
    for (size_t i = 0; i < nok.size(); ++i)
        delete nok[i];
    nok.clear();
}

//---------------------------------------------------------------------------
RESTAPI::Checker_List_Res::~Checker_List_Res()
{
    for (size_t i = 0; i < files.size(); ++i)
        delete files[i];
    files.clear();
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Validate_Res::~Checker_Validate_Res()
{
    for (size_t i = 0; i < ok.size(); ++i)
        delete ok[i];
    ok.clear();
    for (size_t i = 0; i < nok.size(); ++i)
        delete nok[i];
    nok.clear();
}

//---------------------------------------------------------------------------
#define DESTRUCTOR_POLICY(NAME) \
    RESTAPI::NAME::~NAME()   \
    {                           \
        if (nok)                \
        {                       \
            delete nok;         \
            nok = NULL;         \
        }                       \
    }

DESTRUCTOR_POLICY(XSLT_Policy_Create_Res)
DESTRUCTOR_POLICY(Policy_Import_Res)
DESTRUCTOR_POLICY(Policy_Remove_Res)
DESTRUCTOR_POLICY(Policy_Dump_Res)
DESTRUCTOR_POLICY(Policy_Save_Res)
DESTRUCTOR_POLICY(Policy_Duplicate_Res)
DESTRUCTOR_POLICY(Policy_Move_Res)
DESTRUCTOR_POLICY(Policy_Change_Info_Res)
DESTRUCTOR_POLICY(Policy_Change_Type_Res)
DESTRUCTOR_POLICY(Policy_Change_Is_Public_Res)
DESTRUCTOR_POLICY(Policy_Get_Name_Res)
DESTRUCTOR_POLICY(Policy_Get_Policies_Count_Res)
DESTRUCTOR_POLICY(Policy_Get_Policies_Names_List_Res)
DESTRUCTOR_POLICY(Policy_Clear_Policies_Res)
DESTRUCTOR_POLICY(XSLT_Policy_Create_From_File_Res)
DESTRUCTOR_POLICY(XSLT_Policy_Rule_Create_Res)
DESTRUCTOR_POLICY(XSLT_Policy_Rule_Get_Res)
DESTRUCTOR_POLICY(XSLT_Policy_Rule_Edit_Res)
DESTRUCTOR_POLICY(XSLT_Policy_Rule_Duplicate_Res)
DESTRUCTOR_POLICY(XSLT_Policy_Rule_Move_Res)
DESTRUCTOR_POLICY(XSLT_Policy_Rule_Delete_Res)

#undef DESTRUCTOR_POLICY

//---------------------------------------------------------------------------
RESTAPI::Policy_Get_Policies_Res::~Policy_Get_Policies_Res()
{
    for (size_t i = 0; i < policies.size(); ++i)
        delete policies[i];
    policies.clear();

    if (nok)
    {
        delete nok;
        nok = NULL;
    }
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Get_Public_Policies_Res::~Policy_Get_Public_Policies_Res()
{
    for (size_t i = 0; i < policies.size(); ++i)
        delete policies[i];
    policies.clear();

    if (nok)
    {
        delete nok;
        nok = NULL;
    }
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Get_Res::~Policy_Get_Res()
{
    if (policy)
    {
        delete policy;
        policy = NULL;
    }

    if (nok)
    {
        delete nok;
        nok = NULL;
    }
}

//***************************************************************************
// Request: to_str()
//***************************************************************************

//---------------------------------------------------------------------------
std::string RESTAPI::MediaConch_Watch_Folder_Req::to_str() const
{
    std::stringstream out;

    out << "{\"folder\":\"" << folder << "\"";
    out << ",\"folder_report\":\"" << folder_reports << "\"";
    out << ",\"plugins\":[";
    for (size_t i = 0; i < plugins.size(); ++i)
    {
        if (i)
            out << ",";
        out << "\"" << plugins[i] << "\"";
    }
    out << "],\"policies\":[";
    for (size_t i = 0; i < policies.size(); ++i)
    {
        if (i)
            out << ",";
        out << "\"" << policies[i].size() << "\"";
    }
    if (user)
        out << "],\"user\":" << user;
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::MediaConch_List_Watch_Folders_Req::to_str() const
{
    std::stringstream out;

    out << "{}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::MediaConch_Edit_Watch_Folder_Req::to_str() const
{
    std::stringstream out;

    out << "{\"folder\":\"" << folder << "\"";
    out << ",\"folder_report\":\"" << folder_reports << "\"";
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::MediaConch_Remove_Watch_Folder_Req::to_str() const
{
    std::stringstream out;

    out << "{\"folder\":\"" << folder << "\"";
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::MediaConch_Get_Plugins_Req::to_str() const
{
    std::stringstream out;

    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Analyze_Arg::to_str() const
{
    std::stringstream out;

    out << "[file:\"" << file << "\",user:" << user << ",id:" << id << ", force:";
    if (!has_force_analyze || !force_analyze)
        out << "false";
    else
        out << "true";
    out << ",plugins:[";
    for (size_t i = 0; i < plugins.size(); ++i)
    {
        if (i)
            out << ",";
        out << plugins[i];
    }
    out << "]]";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Analyze_Req::to_str() const
{
    std::stringstream out;

    for (size_t i = 0; i < args.size(); ++i)
    {
        if (i)
            out << ", ";
        out << args[i].to_str();
    }
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Status_Req::to_str() const
{
    std::stringstream out;

    out << "[\"user\":" << user;
    out << ",\"ids\":[";
    for (size_t i = 0; i < ids.size(); ++i)
    {
        if (i)
            out << ",";
        out << ids[i];
    }
    out << "]]";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Report_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\":" << user;
    out << ",\"ids\":[";
    for (size_t i = 0; i < ids.size(); ++i)
    {
        if (i)
            out << ",";
        out << ids[i];
    }
    out << "],\"reports\":[";
    for (size_t i = 0; i < reports.size(); ++i)
    {
        if (i)
            out << ",";
        RESTAPI api;
        out << api.get_Report_string(reports[i]);
    }
    out << "],\"policies_ids_size\":" << policies_ids.size();
    out << ",\"policies_contents_size\":" << policies_contents.size();
    out << ",\"display_name\":" << display_name;
    out << ",\"display_content_length\":" << display_content.size();
    out << ",\"options\":[";

    std::map<std::string, std::string>::const_iterator it = options.begin();
    for (; it != options.end(); ++it)
    {
        if (it != options.begin())
            out << ",";
        out << "{\"" << it->first << "\":\"" << it->second << "\"}";
    }
    out << "]}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Retry_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\":" << user;
    out << ",\"ids\":[";
    for (size_t i = 0; i < ids.size(); ++i)
    {
        if (i)
            out << ",";
        out << ids[i];
    }
    out << "]}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Clear_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\":" << user;
    out << ",\"ids\":[";
    for (size_t i = 0; i < ids.size(); ++i)
    {
        if (i)
            out << ",";
        out << ids[i];
    }
    out << "]}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_List_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\":" << user << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Validate_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\":" << user;
    out << ",\"ids\":[";
    for (size_t i = 0; i < ids.size(); ++i)
    {
        if (i)
            out << ",";
        out << ids[i];
    }
    RESTAPI api;
    out << "],\"report\":" << api.get_Report_string(report);
    out << ",\"policies_ids_size\":" << policies_ids.size();
    out << ",\"policies_contents_size\":" << policies_contents.size();
    out << ",\"options\":[";

    std::map<std::string, std::string>::const_iterator it = options.begin();
    for (; it != options.end(); ++it)
    {
        if (it != options.begin())
            out << ",";
        out << "{\"" << it->first << "\":\"" << it->second << "\"}";
    }
    out << "]}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_File_From_Id_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\":" << user << ",\"id\":" << id << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Id_From_Filename_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\":" << user << ",\"filename\":\"" << filename << "\"}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_File_Information_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\":" << user << ",\"id\":" << id << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Default_Values_For_Type_Req::to_str() const
{
    std::stringstream out;

    out << "{\"type\":\"" << type << "\"";
    out << ",\"field\":\"" << field << "\"}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::XSLT_Policy_Create_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\":" << user << ",\"type\":\"" << type << "\",\"parent_id\":" << parent_id << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Import_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\": " << user << ",size(\"xml\"): " << xml.size() << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Remove_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\": " << user << ",\"id\": " << id << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Dump_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\": " << user << ",\"id\": " << id;
    out << ",\"must_be_public\":" << std::boolalpha << must_be_public << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Save_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\": " << user << ",\"id\": " << id << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Duplicate_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\":" << user;
    if (dst_user)
        out << ",\"dst_user\":" << dst_user;
    out << ",\"must_be_public\":" << std::boolalpha << must_be_public;
    out << ",\"id\": " << id << ",\"dst_policy_id\": " << dst_policy_id << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Move_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\": " << user << ",\"id\": " << id << ",\"dst_policy_id\": " << dst_policy_id << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Change_Info_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\": " << user;
    out << ",\"id\": " << id;
    out << ",\"name\":\"" << name;
    out << "\",\"description\":\"" << description;
    out << "\",\"license\":\"" << license;
    out << "\"}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Change_Type_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\": " << user;
    out << ",\"id\": " << id;
    out << ",\"type\":\"" << type;
    out << "\"}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Change_Is_Public_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\":" << user;
    out << ",\"id\":" << id;
    out << ",\"is_public\":" << std::boolalpha << is_public;
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Get_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\":" << user << ",\"id\":" << id << ",\"format\":\"" << format;
    out <<",\"must_be_public\":" << std::boolalpha << must_be_public << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Get_Name_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\": " << user << ",\"id\": " << id << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Get_Policies_Count_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\": " << user << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Clear_Policies_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\": " << user << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Get_Policies_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\":" << user;
    out << ",\"ids\":[";
    for (size_t i = 0; i < ids.size(); ++i)
    {
        if (i)
            out << ",";
        out << ids[i];
    }
    out << "],\"format\":\"" << format;
    out << "\"}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Get_Public_Policies_Req::to_str() const
{
    return std::string();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Get_Policies_Names_List_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\":" << user << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::XSLT_Policy_Create_From_File_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\": " << user << ",\"id\": " << id << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::XSLT_Policy_Rule_Create_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\":" << user << ",\"policy_id\":" << policy_id << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::XSLT_Policy_Rule_Get_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\":" << user << ",\"policy_id\":" << policy_id << ",\"id\":" << id << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::XSLT_Policy_Rule_Edit_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\": " << user << ",\"policy_id\": " << policy_id;
    out << ", " << rule.to_str();
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::XSLT_Policy_Rule_Duplicate_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\": " << user;
    out << ",\"policy_id\": " << policy_id;
    out << ", \"id\":" << id;
    out << ",\"dst_policy_id\": " << dst_policy_id;
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::XSLT_Policy_Rule_Move_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\": " << user;
    out << ",\"policy_id\": " << policy_id;
    out << ", \"id\":" << id;
    out << ",\"dst_policy_id\": " << dst_policy_id;
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::XSLT_Policy_Rule_Delete_Req::to_str() const
{
    std::stringstream out;

    out << "{\"user\": " << user << ",\"policy_id\": " << policy_id;
    out << ", \"id\":" << id;
    out << "}";
    return out.str();
}

//***************************************************************************
// Result: to_str()
//***************************************************************************

//  MediaConch
//---------------------------------------------------------------------------
std::string RESTAPI::MediaConch_Nok::to_str() const
{
    std::stringstream out;

    out << "{\"error\":\"" << error << "\"}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::MediaConch_Get_Plugins_Res::to_str() const
{
    std::stringstream out;

    out << "{";
    if (nok)
        out << "\"nok\":" << nok->to_str();
    else
    {
        out << "\"plugins\":[";
        for (size_t i = 0; i < plugins.size(); ++i)
        {
            if (i)
                out << ",";
            out << plugins[i];
        }
        out << "]";
    }
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::MediaConch_Watch_Folder_Res::to_str() const
{
    std::stringstream out;

    out << "{";
    if (nok)
        out << "\"nok\":" << nok->to_str();
    else
        out << "\"user\":" << user;
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::MediaConch_List_Watch_Folders_Res::to_str() const
{
    std::stringstream out;

    out << "{";
    if (nok)
        out << "\"nok\":" << nok->to_str();
    else
    {
        out << "\"folders\":[";
        for (size_t i = 0; i < folders.size(); ++i)
        {
            if (i)
                out << ",";
            out << "\"" << folders[i] << "\"";
        }
        out << "]";
    }
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::MediaConch_Edit_Watch_Folder_Res::to_str() const
{
    std::stringstream out;

    out << "{";
    if (nok)
        out << "\"nok\":" << nok->to_str();
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::MediaConch_Remove_Watch_Folder_Res::to_str() const
{
    std::stringstream out;

    out << "{";
    if (nok)
        out << "\"nok\":" << nok->to_str();
    out << "}";
    return out.str();
}

// Checker
//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Analyze_Ok::to_str() const
{
    std::stringstream out;

    out << "{inId: " << inId;
    out << ", outId: " << outId;
    out << ", create: " << std::boolalpha << create << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Analyze_Nok::to_str() const
{
    std::stringstream out;

    out << "{id: " << id;
    RESTAPI api;
    out << ", reason: " << api.get_Reason_string(error) << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Analyze_Res::to_str() const
{
    std::stringstream out;

    out << "[ok: [";
    for (size_t i = 0; i < ok.size(); ++i)
    {
        if (i)
            out << ", ";
        out << ok[i]->to_str();
    }
    out << "], nok: [";
    for (size_t i = 0; i < nok.size(); ++i)
    {
        if (i)
            out << ", ";
        out << nok[i]->to_str();
    }
    out << "] ]";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Status_Ok::to_str() const
{
    std::stringstream out;

    out << "{\"id\": " << id;
    out << ", \"finished\": " << std::boolalpha << finished;
    out << ", \"has_error\": " << std::boolalpha << has_error;

    if (finished && has_error)
    out << ", \"error_log\": " << error_log;

    if (!finished && percent)
        out << ", \"done\": " << *percent;

    if (finished && tool)
    {
        RESTAPI api;
        out << ", \"tool\": " << api.get_Report_string(*tool);
    }

    if (generated_id != -1)
        out << ", \"generated_id\": " << generated_id;

    if (source_id != -1)
        out << ", \"source_id\": " << source_id;

    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Status_Nok::to_str() const
{
    std::stringstream out;

    out << "{id: " << id;
    RESTAPI api;
    out << ", reason: " << api.get_Reason_string(error) << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Status_Res::to_str() const
{
    std::stringstream out;

    out << "[ok: [";
    for (size_t i = 0; i < ok.size(); ++i)
    {
        if (i)
            out << ", ";
        out << ok[i]->to_str();
    }
    out << "], nok: [";
    for (size_t i = 0; i < nok.size(); ++i)
    {
        if (i)
            out << ", ";
        out << nok[i]->to_str();
    }
    out << "] ]";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Report_Ok::to_str() const
{
    std::stringstream out;

    out << "{report_length: " << report.length();
    if (has_valid)
        out << ", valid: " << std::boolalpha << valid;
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Report_Nok::to_str() const
{
    std::stringstream out;

    out << "{id: " << id;
    RESTAPI api;
    out << ", reason: " << api.get_Reason_string(error) << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Report_Res::to_str() const
{
    std::stringstream out;

    out << "[ok: " << ok.to_str() << ", nok: [";
    for (size_t i = 0; i < nok.size(); ++i)
    {
        if (i)
            out << ", ";
        out << nok[i]->to_str();
    }
    out << "] ]";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Retry_Nok::to_str() const
{
    std::stringstream out;

    out << "{id: " << id;
    RESTAPI api;
    out << ", reason: " << api.get_Reason_string(error) << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Retry_Res::to_str() const
{
    std::stringstream out;

    out << "[ok: [";
    for (size_t i = 0; i < ok.size(); ++i)
    {
        if (i)
            out << ", ";
        out << ok[i];
    }
    out << "], nok: [";
    for (size_t i = 0; i < nok.size(); ++i)
    {
        if (i)
            out << ", ";
        out << nok[i]->to_str();
    }
    out << "] ]";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Clear_Nok::to_str() const
{
    std::stringstream out;

    out << "{id: " << id;
    RESTAPI api;
    out << ", reason: " << api.get_Reason_string(error) << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Clear_Res::to_str() const
{
    std::stringstream out;

    out << "[ok: [";
    for (size_t i = 0; i < ok.size(); ++i)
    {
        if (i)
            out << ", ";
        out << ok[i];
    }
    out << "], nok: [";
    for (size_t i = 0; i < nok.size(); ++i)
    {
        if (i)
            out << ", ";
        out << nok[i]->to_str();
    }
    out << "] ]";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_List_Res::to_str() const
{
    std::stringstream out;

    out << "[files: [";
    for (size_t i = 0; i < files.size(); ++i)
    {
        if (i)
            out << ", ";
        out << "{file:" << files[i]->file;
        out << ", id:" << files[i]->id << "}";
    }
    out << "] ]";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Validate_Ok::to_str() const
{
    std::stringstream out;

    out << "{id: " << id << ", valid: " << std::boolalpha << valid << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Validate_Nok::to_str() const
{
    std::stringstream out;

    out << "{id: " << id;
    RESTAPI api;
    out << ", reason: " << api.get_Reason_string(error) << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Validate_Res::to_str() const
{
    std::stringstream out;

    out << "[ok: [";
    for (size_t i = 0; i < ok.size(); ++i)
    {
        if (i)
            out << ", ";
        out << ok[i]->to_str();
    }
    out <<  "], nok: [";
    for (size_t i = 0; i < nok.size(); ++i)
    {
        if (i)
            out << ", ";
        out << nok[i]->to_str();
    }
    out << "] ]";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_File_From_Id_Res::to_str() const
{
    std::stringstream out;

    out << "[file: '" << file << "']";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_Id_From_Filename_Res::to_str() const
{
    std::stringstream out;

    out << "[id: " << id << "]";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Checker_File_Information_Res::to_str() const
{
    std::stringstream out;

    out << "[\"filename\":\"" << filename << "\"";
    out << ",\"file_last_modification\":\"" << file_last_modification << "\"";
    out << ",\"generated_id\":" << generated_id;
    out << ",\"source_id\":" << source_id;
    out << ",\"generated_time\":" << generated_time;
    out << ",\"generated_log\":\"" << generated_log << "\"";
    out << ",\"generated_error_log\":\"" << generated_error_log << "\"";
    out << ",\"analyzed\":" << analyzed;
    out << ",\"has_error\":" << has_error;
    out << ",\"error_log\":\"" << error_log << "\"";
    out << "]";

    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Default_Values_For_Type_Res::to_str() const
{
    std::stringstream out;

    out << "[values: '";
    for (size_t i = 0; i < values.size(); ++i)
    {
        if (i)
            out << ",";
        out << values[i];
    }
    out  << "']";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::XSLT_Policy_Create_From_File_Nok::to_str() const
{
    std::stringstream out;

    out << "{id: " << id;
    RESTAPI api;
    out << ", reason: " << api.get_Reason_string(error) << "}";
    return out.str();
}

//  Policy
//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Nok::to_str() const
{
    std::stringstream out;

    out << "{\"error\":\"" << error << "\"}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::XSLT_Policy_Create_Res::to_str() const
{
    std::stringstream out;

    out << "{\"id\": " << id;
    if (nok)
        out << "," << nok->to_str();
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Import_Res::to_str() const
{
    std::stringstream out;

    out << "{\"id\": " << id;
    if (nok)
        out << "," << nok->to_str();
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Remove_Res::to_str() const
{
    std::stringstream out;

    out << "{";
    if (nok)
        out << nok->to_str();
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Dump_Res::to_str() const
{
    std::stringstream out;

    out << "{size(\"xml\"):" << xml.size();
    if (nok)
        out << "," << nok->to_str();
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Save_Res::to_str() const
{
    std::stringstream out;

    out << "{";
    if (nok)
        out << nok->to_str();
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Duplicate_Res::to_str() const
{
    std::stringstream out;

    out << "{";
    if (nok)
        out << "\"error\":\"" << nok->to_str() << "\"";
    else
        out << "\"id\": " << id;
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Move_Res::to_str() const
{
    std::stringstream out;

    out << "{";
    if (nok)
        out << "\"error\":\"" << nok->to_str() << "\"";
    else
        out << "\"id\": " << id;
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Change_Info_Res::to_str() const
{
    std::stringstream out;

    out << "{";
    if (nok)
        out << nok->to_str();
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Change_Type_Res::to_str() const
{
    std::stringstream out;

    out << "{";
    if (nok)
        out << nok->to_str();
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Change_Is_Public_Res::to_str() const
{
    std::stringstream out;

    out << "{";
    if (nok)
        out << nok->to_str();
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Get_Res::to_str() const
{
    std::stringstream out;

    out << "{";
    if (nok)
        out << nok->to_str();
    else if (policy)
        out << "\"policy\":" << policy->to_str();
    else if (policyTree.size())
        out << "\"policyTree\":\"" << policyTree << "\"";
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Get_Name_Res::to_str() const
{
    std::stringstream out;

    out << "{\"name\": \"" << name;
    out << "\"";
    if (nok)
        out << "," << nok->to_str();
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Get_Policies_Count_Res::to_str() const
{
    std::stringstream out;

    out << "{\"size\": " << size;
    if (nok)
        out << "," << nok->to_str();
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Clear_Policies_Res::to_str() const
{
    std::stringstream out;

    out << "{";
    if (nok)
        out << nok->to_str();
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Get_Policies_Res::to_str() const
{
    std::stringstream out;

    if (policies.size())
    {
        out << "{\"policies\":[";
        for (size_t i = 0; i < policies.size(); ++i)
        {
            if (!policies[i])
                continue;

            if (i)
                out << ",";
            out << policies[i]->to_str();
        }
        out << "]}";
    }
    else if (policiesTree.size())
        out << policiesTree;
    else if (nok)
        out << "{" << nok->to_str() << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Public_Policy::to_str() const
{
    std::stringstream out;

    out << "{\"id\":" << id;
    out << ",\"user\":" << user;
    out << ",\"name\":\"" << name << "\"";
    out << ",\"description\":\"" << description << "\"";
    out << ",\"license\":\"" << license << "\"}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Get_Public_Policies_Res::to_str() const
{
    std::stringstream out;

    if (policies.size())
    {
        out << "{\"policies\":[";
        for (size_t i = 0; i < policies.size(); ++i)
        {
            if (!policies[i])
                continue;

            if (i)
                out << ",";
            out << policies[i]->to_str();
        }
        out << "]}";
    }
    else if (nok)
        out << "{" << nok->to_str() << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::Policy_Get_Policies_Names_List_Res::to_str() const
{
    std::stringstream out;

    if (nok)
        out << "{" << nok->to_str() << "}";
    else
    {
        out << "{[";
        for (size_t i = 0; i < policies.size(); ++i)
        {
            if (i)
                out << ",";
            out << "{\"id\":" << policies[i].first << ", \"name\":\"" << policies[i].second << "\"}";
        }

        out << "]}";
    }
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::XSLT_Policy_Create_From_File_Res::to_str() const
{
    std::stringstream out;

    if (policy_id < 0)
        out << "{\"policy\": " << policy_id << "}";
    else if (nok)
        out << "{\"nok\": " << nok->to_str() << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::XSLT_Policy_Rule_Create_Res::to_str() const
{
    std::stringstream out;

    if (nok)
        out << "{\"nok\":" << nok->to_str();
    else
        out << "{\"id\":" << id;
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::XSLT_Policy_Rule_Get_Res::to_str() const
{
    std::stringstream out;

    if (nok)
        out << "{\"nok\":" << nok->to_str();
    else
        out << "{\"rule\":" << rule.to_str();
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::XSLT_Policy_Rule_Edit_Res::to_str() const
{
    std::stringstream out;

    out << "{";
    if (nok)
        out << nok->to_str();
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::XSLT_Policy_Rule_Duplicate_Res::to_str() const
{
    std::stringstream out;

    out << "{";
    if (nok)
        out << "\"error\":\"" << nok->to_str() << "\"";
    else
        out << "\"id\":" << id;
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::XSLT_Policy_Rule_Move_Res::to_str() const
{
    std::stringstream out;

    out << "{";
    if (nok)
        out << "\"error\":\"" << nok->to_str() << "\"";
    else
        out << "\"id\":" << id;
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::XSLT_Policy_Rule_Delete_Res::to_str() const
{
    std::stringstream out;

    out << "{";
    if (nok)
        out << nok->to_str();
    out << "}";
    return out.str();
}

//***************************************************************************
// Serialize: Request
//***************************************************************************

//---------------------------------------------------------------------------
int RESTAPI::serialize_mediaconch_get_plugins_req(MediaConch_Get_Plugins_Req&, std::string& data)
{
    //URI
    std::stringstream ss;

    data = ss.str();

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_mediaconch_watch_folder_req(MediaConch_Watch_Folder_Req& req, std::string& data)
{
    Container::Value v, child, folder, folder_reports, plugins, policies, user;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    folder.type = Container::Value::CONTAINER_TYPE_STRING;
    folder.s = req.folder;
    child.obj["folder"] = folder;

    folder_reports.type = Container::Value::CONTAINER_TYPE_STRING;
    folder_reports.s = req.folder_reports;
    child.obj["folder_reports"] = folder_reports;

    plugins.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < req.plugins.size(); ++i)
    {
        Container::Value plugin;
        plugin.type = Container::Value::CONTAINER_TYPE_STRING;
        plugin.s = req.plugins[i];
        plugins.array.push_back(plugin);
    }
    child.obj["plugins"] = plugins;

    policies.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < req.policies.size(); ++i)
    {
        Container::Value policy;
        policy.type = Container::Value::CONTAINER_TYPE_STRING;
        policy.s = req.policies[i];
        policies.array.push_back(policy);
    }
    child.obj["policies"] = policies;

    if (req.user)
    {
        user.type = Container::Value::CONTAINER_TYPE_INTEGER;
        user.l = *req.user;
        child.obj["user"] = user;
    }

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["MEDIACONCH_WATCH_FOLDER"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_mediaconch_list_watch_folders_req(MediaConch_List_Watch_Folders_Req&, std::string& data)
{
    //URI
    std::stringstream ss;

    data = ss.str();

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_mediaconch_edit_watch_folder_req(MediaConch_Edit_Watch_Folder_Req& req, std::string& data)
{
    Container::Value v, child, folder, folder_reports;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    folder.type = Container::Value::CONTAINER_TYPE_STRING;
    folder.s = req.folder;
    child.obj["folder"] = folder;

    folder_reports.type = Container::Value::CONTAINER_TYPE_STRING;
    folder_reports.s = req.folder_reports;
    child.obj["folder_reports"] = folder_reports;

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["MEDIACONCH_EDIT_WATCH_FOLDER"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_mediaconch_remove_watch_folder_req(MediaConch_Remove_Watch_Folder_Req& req, std::string& data)
{
    Container::Value v, child, folder;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    folder.type = Container::Value::CONTAINER_TYPE_STRING;
    folder.s = req.folder;
    child.obj["folder"] = folder;

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["MEDIACONCH_REMOVE_WATCH_FOLDER"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_analyze_req(Checker_Analyze_Req& req, std::string& data)
{
    Container::Value v, child;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj["args"] = serialize_analyze_args(req.args);

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["CHECKER_ANALYZE"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_status_req(Checker_Status_Req& req, std::string& data)
{
    //URI
    std::stringstream ss;

    ss << "?user=" << req.user;
    for (size_t i = 0; i < req.ids.size(); ++i)
        ss << "&id=" << req.ids[i];
    data = ss.str();

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_report_req(Checker_Report_Req& req, std::string& data)
{
    Container::Value v, child, user;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj["ids"] = serialize_ids(req.ids);
    child.obj["reports"] = serialize_report_reports(req.reports);

    user.type = Container::Value::CONTAINER_TYPE_INTEGER;
    user.l = req.user;
    child.obj["user"] = user;

    if (req.policies_ids.size())
        child.obj["policies_ids"] = serialize_report_arr_long_u(req.policies_ids);
    if (req.policies_contents.size())
        child.obj["policies_contents"] = serialize_report_arr_str(req.policies_contents);

    if (req.display_name.length())
        child.obj["display_name"] = serialize_report_string(req.display_name);
    if (req.display_content.length())
        child.obj["display_content"] = serialize_report_string(req.display_content);

    if (req.options.size())
    {
        Container::Value options;
        options.type = Container::Value::CONTAINER_TYPE_OBJECT;

        std::map<std::string, std::string>::iterator it = req.options.begin();
        for (; it != req.options.end(); ++it)
        {
            if (!it->first.size())
                continue;

            Container::Value value;
            value.type = Container::Value::CONTAINER_TYPE_STRING;
            value.s = it->second;

            options.obj[it->first.c_str()] = value;
        }

        child.obj["options"] = options;
    }

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["CHECKER_REPORT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_retry_req(Checker_Retry_Req& req, std::string& data)
{
    Container::Value v, child, user;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    user.type = Container::Value::CONTAINER_TYPE_INTEGER;
    user.l = req.user;
    child.obj["user"] = user;

    child.obj["ids"] = serialize_ids(req.ids);

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["CHECKER_RETRY"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_clear_req(Checker_Clear_Req& req, std::string& data)
{
    //URI
    std::stringstream ss;

    ss << "?user=" << req.user;
    for (size_t i = 0; i < req.ids.size(); ++i)
        ss << "&id=" << req.ids[i];
    data = ss.str();

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_list_req(Checker_List_Req& req, std::string& data)
{
    //URI
    std::stringstream ss;

    ss << "?user=" << req.user;
    data = ss.str();
    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_validate_req(Checker_Validate_Req& req, std::string& data)
{
    Container::Value v, child, report, user;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj["ids"] = serialize_ids(req.ids);

    report.type = Container::Value::CONTAINER_TYPE_STRING;
    report.s = get_Report_string(req.report);
    child.obj["report"] = report;

    user.type = Container::Value::CONTAINER_TYPE_INTEGER;
    user.l = req.user;
    child.obj["user"] = user;

    if (req.policies_ids.size())
        child.obj["policies_ids"] = serialize_report_arr_long_u(req.policies_ids);
    if (req.policies_contents.size())
        child.obj["policies_contents"] = serialize_report_arr_str(req.policies_contents);

    if (req.options.size())
    {
        Container::Value options;
        options.type = Container::Value::CONTAINER_TYPE_OBJECT;

        std::map<std::string, std::string>::iterator it = req.options.begin();
        for (; it != req.options.end(); ++it)
        {
            if (!it->first.size())
                continue;

            Container::Value value;
            value.type = Container::Value::CONTAINER_TYPE_STRING;
            value.s = it->second;

            options.obj[it->first.c_str()] = value;
        }

        child.obj["options"] = options;
    }

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["CHECKER_VALIDATE"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_file_from_id_req(Checker_File_From_Id_Req& req, std::string& data)
{
    Container::Value v, child, id, user;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    user.type = Container::Value::CONTAINER_TYPE_INTEGER;
    user.l = req.user;
    child.obj["user"] = user;

    id.type = Container::Value::CONTAINER_TYPE_INTEGER;
    id.l = req.id;
    child.obj["id"] = id;

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["CHECKER_FILE_FROM_ID"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_id_from_filename_req(Checker_Id_From_Filename_Req& req, std::string& data)
{
    Container::Value v, child, filename, user;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    user.type = Container::Value::CONTAINER_TYPE_INTEGER;
    user.l = req.user;
    child.obj["user"] = user;

    filename.type = Container::Value::CONTAINER_TYPE_STRING;
    filename.s = req.filename;
    child.obj["filename"] = filename;

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["CHECKER_ID_FROM_FILENAME"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_file_information_req(Checker_File_Information_Req& req, std::string& data)
{
    Container::Value v, child, id, user;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    user.type = Container::Value::CONTAINER_TYPE_INTEGER;
    user.l = req.user;
    child.obj["user"] = user;

    id.type = Container::Value::CONTAINER_TYPE_INTEGER;
    id.l = req.id;
    child.obj["id"] = id;

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["CHECKER_FILE_INFORMATION"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_default_values_for_type_req(Default_Values_For_Type_Req& req, std::string& data)
{
    Container::Value v, child, type, field;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    type.type = Container::Value::CONTAINER_TYPE_STRING;
    type.s = req.type;

    child.obj["type"] = type;

    field.type = Container::Value::CONTAINER_TYPE_STRING;
    field.s = req.field;

    child.obj["field"] = field;

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["DEFAULT_VALUES_FOR_TYPE"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_xslt_policy_create_req(XSLT_Policy_Create_Req& req, std::string& data)
{
    //URI
    std::stringstream ss;

    ss << "?parent_id=" << req.parent_id;
    ss << "&type=" << req.type;
    ss << "&user=" << req.user;
    data = ss.str();

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_import_req(Policy_Import_Req& req, std::string& data)
{
    Container::Value v, child, xml, user;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    xml.type = Container::Value::CONTAINER_TYPE_STRING;
    xml.s = req.xml;
    child.obj["xml"] = xml;

    user.type = Container::Value::CONTAINER_TYPE_INTEGER;
    user.l = req.user;
    child.obj["user"] = user;

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["POLICY_IMPORT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_remove_req(Policy_Remove_Req& req, std::string& data)
{
    //URI
    std::stringstream ss;

    ss << "?id=" << req.id;
    ss << "&user=" << req.user;
    data = ss.str();

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_dump_req(Policy_Dump_Req& req, std::string& data)
{
    //URI
    std::stringstream ss;

    ss << "?id=" << req.id;
    ss << "&user=" << req.user;
    if (req.must_be_public)
        ss << "&must_be_public=" << req.must_be_public;
    data = ss.str();

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_save_req(Policy_Save_Req& req, std::string& data)
{
    //URI
    std::stringstream ss;

    ss << "?id=" << req.id;
    ss << "&user=" << req.user;
    data = ss.str();

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_duplicate_req(Policy_Duplicate_Req& req, std::string& data)
{
    //URI
    std::stringstream ss;

    ss << "?id=" << req.id;
    ss << "&dst_policy_id=" << req.dst_policy_id;
    ss << "&user=" << req.user;
    if (req.dst_user)
        ss << "&dst_user=" << *req.dst_user;
    if (req.must_be_public)
        ss << "&must_be_public=" << std::boolalpha << req.must_be_public;
    data = ss.str();

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_move_req(Policy_Move_Req& req, std::string& data)
{
    //URI
    std::stringstream ss;

    ss << "?id=" << req.id;
    ss << "&dst_policy_id=" << req.dst_policy_id;
    ss << "&user=" << req.user;
    data = ss.str();

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_change_info_req(Policy_Change_Info_Req& req, std::string& data)
{
    Container::Value v, child, name, description, license, user, id;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    name.type = Container::Value::CONTAINER_TYPE_STRING;
    name.s = req.name;
    child.obj["name"] = name;

    description.type = Container::Value::CONTAINER_TYPE_STRING;
    description.s = req.description;
    child.obj["description"] = description;

    if (req.license.size())
    {
        license.type = Container::Value::CONTAINER_TYPE_STRING;
        license.s = req.license;
        child.obj["license"] = license;
    }

    user.type = Container::Value::CONTAINER_TYPE_INTEGER;
    user.l = req.user;
    child.obj["user"] = user;

    id.type = Container::Value::CONTAINER_TYPE_INTEGER;
    id.l = req.id;
    child.obj["id"] = id;

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["POLICY_CHANGE_INFO"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_change_type_req(Policy_Change_Type_Req& req, std::string& data)
{
    Container::Value v, child, id, type, user;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    id.type = Container::Value::CONTAINER_TYPE_INTEGER;
    id.l = req.id;
    child.obj["id"] = id;

    type.type = Container::Value::CONTAINER_TYPE_STRING;
    type.s = req.type;
    child.obj["type"] = type;

    user.type = Container::Value::CONTAINER_TYPE_INTEGER;
    user.l = req.user;
    child.obj["user"] = user;

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["POLICY_CHANGE_TYPE"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_change_is_public_req(Policy_Change_Is_Public_Req& req, std::string& data)
{
    Container::Value v, child, id, is_public, user;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    id.type = Container::Value::CONTAINER_TYPE_INTEGER;
    id.l = req.id;
    child.obj["id"] = id;

    is_public.type = Container::Value::CONTAINER_TYPE_BOOL;
    is_public.b = req.is_public;
    child.obj["is_public"] = is_public;

    user.type = Container::Value::CONTAINER_TYPE_INTEGER;
    user.l = req.user;
    child.obj["user"] = user;

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["POLICY_CHANGE_IS_PUBLIC"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_get_req(Policy_Get_Req& req, std::string& data)
{
    //URI
    std::stringstream ss;

    ss << "?id=" << req.id;
    ss << "&user=" << req.user;
    ss << "&format=" << req.format;
    ss << "&must_be_public=" << req.must_be_public;
    data = ss.str();

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_get_name_req(Policy_Get_Name_Req& req, std::string& data)
{
    //URI
    std::stringstream ss;

    ss << "?id=" << req.id;
    ss << "&user=" << req.user;
    data = ss.str();

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_get_policies_count_req(Policy_Get_Policies_Count_Req& req, std::string& data)
{
    //URI
    std::stringstream ss;

    ss << "?user=" << req.user;
    data = ss.str();

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_clear_policies_req(Policy_Clear_Policies_Req& req, std::string& data)
{
    //URI
    std::stringstream ss;

    ss << "?user=" << req.user;
    data = ss.str();

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_get_policies_req(Policy_Get_Policies_Req& req, std::string& data)
{
    //URI
    std::stringstream ss;

    ss << "?user=" << req.user;
    for (size_t i = 0; i < req.ids.size(); ++i)
        ss << "&id=" << req.ids[i];
    ss << "&format=" << req.format;
    data = ss.str();

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_get_public_policies_req(Policy_Get_Public_Policies_Req&, std::string&)
{
    //URI
    std::stringstream ss;

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_get_policies_names_list_req(Policy_Get_Policies_Names_List_Req& req, std::string& data)
{
    //URI
    std::stringstream ss;

    ss << "?user=" << req.user;
    data = ss.str();

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_xslt_policy_create_from_file_req(XSLT_Policy_Create_From_File_Req& req, std::string& data)
{
    //URI
    std::stringstream ss;

    ss << "?id=" << req.id;
    ss << "&user=" << req.user;
    data = ss.str();

    return 0;
}

// XSLT Rule
//---------------------------------------------------------------------------
int RESTAPI::serialize_xslt_policy_rule(MediaConchLib::XSLT_Policy_Rule& rule, Container::Value& val)
{
    Container::Value id, name, tracktype, field, scope, occurrence, ope, value;
    val.type = Container::Value::CONTAINER_TYPE_OBJECT;

    id.type = Container::Value::CONTAINER_TYPE_INTEGER;
    id.l = rule.id;
    val.obj["id"] = id;

    if (rule.name.size())
    {
        name.type = Container::Value::CONTAINER_TYPE_STRING;
        name.s = rule.name;
        val.obj["name"] = name;
    }

    if (rule.tracktype.size())
    {
        tracktype.type = Container::Value::CONTAINER_TYPE_STRING;
        tracktype.s = rule.tracktype;
        val.obj["tracktype"] = tracktype;
    }

    if (rule.field.size())
    {
        field.type = Container::Value::CONTAINER_TYPE_STRING;
        field.s = rule.field;
        val.obj["field"] = field;
    }

    if (rule.scope.size())
    {
        scope.type = Container::Value::CONTAINER_TYPE_STRING;
        scope.s = rule.scope;
        val.obj["scope"] = scope;
    }

    occurrence.type = Container::Value::CONTAINER_TYPE_INTEGER;
    occurrence.l = rule.occurrence;
    val.obj["occurrence"] = occurrence;

    if (rule.ope.size())
    {
        ope.type = Container::Value::CONTAINER_TYPE_STRING;
        ope.s = rule.ope;
        val.obj["ope"] = ope;
    }

    if (rule.value.size())
    {
        value.type = Container::Value::CONTAINER_TYPE_STRING;
        value.s = rule.value;
        val.obj["value"] = value;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_xslt_policy_rule_create_req(XSLT_Policy_Rule_Create_Req& req, std::string& data)
{
    //URI
    std::stringstream ss;

    ss << "?policy_id=" << req.policy_id;
    ss << "&user=" << req.user;
    data = ss.str();

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_xslt_policy_rule_get_req(XSLT_Policy_Rule_Get_Req& req, std::string& data)
{
    //URI
    std::stringstream ss;

    ss << "?policy_id=" << req.policy_id;
    ss << "&id=" << req.id;
    ss << "&user=" << req.user;
    data = ss.str();

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_xslt_policy_rule_edit_req(XSLT_Policy_Rule_Edit_Req& req, std::string& data)
{
    Container::Value v, child, policy_id, rule, user;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    policy_id.type = Container::Value::CONTAINER_TYPE_INTEGER;
    policy_id.l = req.policy_id;
    child.obj["policy_id"] = policy_id;

    user.type = Container::Value::CONTAINER_TYPE_INTEGER;
    user.l = req.user;
    child.obj["user"] = user;

    serialize_xslt_policy_rule(req.rule, rule);
    child.obj["rule"] = rule;

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["XSLT_POLICY_RULE_EDIT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_xslt_policy_rule_duplicate_req(XSLT_Policy_Rule_Duplicate_Req& req, std::string& data)
{
    //URI
    std::stringstream ss;

    ss << "?policy_id=" << req.policy_id << "&id=" << req.id;
    ss << "&dst_policy_id=" << req.dst_policy_id;
    ss << "&user=" << req.user;
    data = ss.str();

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_xslt_policy_rule_move_req(XSLT_Policy_Rule_Move_Req& req, std::string& data)
{
    //URI
    std::stringstream ss;

    ss << "?policy_id=" << req.policy_id << "&id=" << req.id;
    ss << "&dst_policy_id=" << req.dst_policy_id;
    ss << "&user=" << req.user;
    data = ss.str();

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_xslt_policy_rule_delete_req(XSLT_Policy_Rule_Delete_Req& req, std::string& data)
{
    //URI
    std::stringstream ss;

    ss << "?policy_id=" << req.policy_id << "&id=" << req.id;
    ss << "&user=" << req.user;
    data = ss.str();

    return 0;
}


//***************************************************************************
// Serialize: Result
//***************************************************************************

//---------------------------------------------------------------------------
int RESTAPI::serialize_mediaconch_get_plugins_res(MediaConch_Get_Plugins_Res& res, std::string& data)
{
    Container::Value v, child, nok, plugins;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.nok)
    {
        nok = serialize_mediaconch_nok(res.nok);
        child.obj["nok"] = nok;
    }
    else
    {
        plugins.type = Container::Value::CONTAINER_TYPE_ARRAY;
        for (size_t i = 0; i < res.plugins.size(); ++i)
        {
            Container::Value plugin;

            plugin.type = Container::Value::CONTAINER_TYPE_STRING;
            plugin.s = res.plugins[i];

            plugins.array.push_back(plugin);
        }
        child.obj["plugins"] = plugins;
    }

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["MEDIACONCH_GET_PLUGINS_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }
    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_mediaconch_watch_folder_res(MediaConch_Watch_Folder_Res& res, std::string& data)
{
    Container::Value v, child, nok, user;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.nok)
    {
        nok = serialize_mediaconch_nok(res.nok);
        child.obj["nok"] = nok;
    }
    else
    {
        user.type = Container::Value::CONTAINER_TYPE_INTEGER;
        user.l = res.user;
        child.obj["user"] = user;
    }

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["MEDIACONCH_WATCH_FOLDER_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }
    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_mediaconch_list_watch_folders_res(MediaConch_List_Watch_Folders_Res& res, std::string& data)
{
    Container::Value v, child, nok, folders;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.nok)
    {
        nok = serialize_mediaconch_nok(res.nok);
        child.obj["nok"] = nok;
    }
    else
    {
        folders.type = Container::Value::CONTAINER_TYPE_ARRAY;

        for (size_t i = 0; i < res.folders.size(); ++i)
        {
            Container::Value folder;
            folder.type = Container::Value::CONTAINER_TYPE_STRING;
            folder.s = res.folders[i];
            folders.array.push_back(folder);
        }
        child.obj["folders"] = folders;
    }

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["MEDIACONCH_LIST_WATCH_FOLDERS_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }
    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_mediaconch_edit_watch_folder_res(MediaConch_Edit_Watch_Folder_Res& res, std::string& data)
{
    Container::Value v, child, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.nok)
    {
        nok = serialize_mediaconch_nok(res.nok);
        child.obj["nok"] = nok;
    }

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["MEDIACONCH_EDIT_WATCH_FOLDER_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }
    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_mediaconch_remove_watch_folder_res(MediaConch_Remove_Watch_Folder_Res& res, std::string& data)
{
    Container::Value v, child, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.nok)
    {
        nok = serialize_mediaconch_nok(res.nok);
        child.obj["nok"] = nok;
    }

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["MEDIACONCH_REMOVE_WATCH_FOLDER_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }
    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_analyze_res(Checker_Analyze_Res& res, std::string& data)
{
    Container::Value v, child, nok;

    nok.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < res.nok.size(); ++i)
        nok.array.push_back(serialize_generic_nok(res.nok[i]->id, res.nok[i]->error));

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj["ok"] = serialize_analyze_oks(res.ok);
    child.obj["nok"] = nok;

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["CHECKER_ANALYZE_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }
    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_status_res(Checker_Status_Res& res, std::string& data)
{
    Container::Value v, child, nok;

    nok.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < res.nok.size(); ++i)
        nok.array.push_back(serialize_generic_nok(res.nok[i]->id, res.nok[i]->error));

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj["ok"] = serialize_status_oks(res.ok);
    child.obj["nok"] = nok;

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["CHECKER_STATUS_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_report_res(Checker_Report_Res& res, std::string& data)
{
    Container::Value v, child, nok;

    nok.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < res.nok.size(); ++i)
        nok.array.push_back(serialize_generic_nok(res.nok[i]->id, res.nok[i]->error));

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj["ok"] = serialize_report_ok(res.ok);
    child.obj["nok"] = nok;

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["CHECKER_REPORT_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_retry_res(Checker_Retry_Res& res, std::string& data)
{
    Container::Value v, child, nok;

    nok.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < res.nok.size(); ++i)
        nok.array.push_back(serialize_generic_nok(res.nok[i]->id, res.nok[i]->error));

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj["ok"] = serialize_ids(res.ok);
    child.obj["nok"] = nok;

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["CHECKER_RETRY_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_clear_res(Checker_Clear_Res& res, std::string& data)
{
    Container::Value v, child, nok;

    nok.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < res.nok.size(); ++i)
        nok.array.push_back(serialize_generic_nok(res.nok[i]->id, res.nok[i]->error));

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj["ok"] = serialize_ids(res.ok);
    child.obj["nok"] = nok;

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["CHECKER_CLEAR_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_list_res(Checker_List_Res& res, std::string& data)
{
    Container::Value v, child, files;

    files.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < res.files.size(); ++i)
        files.array.push_back(serialize_list_file(res.files[i]->file, res.files[i]->id));

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj["files"] = files;

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["CHECKER_LIST_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_validate_res(Checker_Validate_Res& res, std::string& data)
{
    Container::Value v, child, ok, nok;

    ok.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < res.ok.size(); ++i)
        ok.array.push_back(serialize_validate_ok(res.ok[i]));

    nok.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < res.nok.size(); ++i)
        nok.array.push_back(serialize_generic_nok(res.nok[i]->id, res.nok[i]->error));

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj["ok"] = ok;
    child.obj["nok"] = nok;

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["CHECKER_VALIDATE_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_file_from_id_res(Checker_File_From_Id_Res& res, std::string& data)
{
    Container::Value v, child, file;

    file.type = Container::Value::CONTAINER_TYPE_STRING;
    file.s = res.file;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj["file"] = file;

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["CHECKER_FILE_FROM_ID_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_id_from_filename_res(Checker_Id_From_Filename_Res& res, std::string& data)
{
    Container::Value v, child, id;

    id.type = Container::Value::CONTAINER_TYPE_INTEGER;
    id.l = res.id;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj["id"] = id;

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["CHECKER_ID_FROM_FILENAME_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_file_information_res(Checker_File_Information_Res& res, std::string& data)
{
    Container::Value v, child, filename, file_last_modification, generated_id, source_id, generated_time,
        generated_log, generated_error_log, analyzed, has_error, error_log;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    filename.type = Container::Value::CONTAINER_TYPE_STRING;
    filename.s = res.filename;
    child.obj["filename"] = filename;

    file_last_modification.type = Container::Value::CONTAINER_TYPE_STRING;
    file_last_modification.s = res.file_last_modification;
    child.obj["file_last_modification"] = file_last_modification;

    generated_id.type = Container::Value::CONTAINER_TYPE_INTEGER;
    generated_id.l = res.generated_id;
    child.obj["generated_id"] = generated_id;

    source_id.type = Container::Value::CONTAINER_TYPE_INTEGER;
    source_id.l = res.source_id;
    child.obj["source_id"] = source_id;

    generated_time.type = Container::Value::CONTAINER_TYPE_INTEGER;
    generated_time.l = res.generated_time;
    child.obj["generated_time"] = generated_time;

    generated_log.type = Container::Value::CONTAINER_TYPE_STRING;
    generated_log.s = res.generated_log;
    child.obj["generated_log"] = generated_log;

    generated_error_log.type = Container::Value::CONTAINER_TYPE_STRING;
    generated_error_log.s = res.generated_error_log;
    child.obj["generated_error_log"] = generated_error_log;

    analyzed.type = Container::Value::CONTAINER_TYPE_BOOL;
    analyzed.b = res.analyzed;
    child.obj["analyzed"] = analyzed;

    has_error.type = Container::Value::CONTAINER_TYPE_BOOL;
    has_error.b = res.has_error;
    child.obj["has_error"] = has_error;

    if (res.has_error)
    {
        error_log.type = Container::Value::CONTAINER_TYPE_STRING;
        error_log.s = res.error_log;
        child.obj["error_log"] = error_log;
    }

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["CHECKER_FILE_INFORMATION_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_default_values_for_type_res(Default_Values_For_Type_Res& res, std::string& data)
{
    Container::Value v, child, values;

    values.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < res.values.size(); ++i)
    {
        Container::Value value;
        value.type = Container::Value::CONTAINER_TYPE_STRING;
        value.s = res.values[i];
        values.array.push_back(value);
    }

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj["values"] = values;

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["DEFAULT_VALUES_FOR_TYPE_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_xslt_policy_create_res(XSLT_Policy_Create_Res& res, std::string& data)
{
    Container::Value v, child, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (!res.nok)
    {
        Container::Value id;
        id.type = Container::Value::CONTAINER_TYPE_INTEGER;
        id.l = res.id;
        child.obj["id"] = id;
    }
    else
        child.obj["nok"] = serialize_policy_nok(res.nok);

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["XSLT_POLICY_CREATE_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_import_res(Policy_Import_Res& res, std::string& data)
{
    Container::Value v, child, id, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.id >= 0)
    {
        id.type = Container::Value::CONTAINER_TYPE_INTEGER;
        id.l = res.id;
        child.obj["id"] = id;
    }
    else if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["POLICY_IMPORT_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_remove_res(Policy_Remove_Res& res, std::string& data)
{
    Container::Value v, child, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["POLICY_REMOVE_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_dump_res(Policy_Dump_Res& res, std::string& data)
{
    Container::Value v, child, xml, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.xml.size())
    {
        xml.type = Container::Value::CONTAINER_TYPE_STRING;
        xml.s = res.xml;
        child.obj["xml"] = xml;
    }
    else if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["POLICY_DUMP_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_save_res(Policy_Save_Res& res, std::string& data)
{
    Container::Value v, child, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["POLICY_SAVE_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_duplicate_res(Policy_Duplicate_Res& res, std::string& data)
{
    Container::Value v, child, id, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.id >= 0)
    {
        id.type = Container::Value::CONTAINER_TYPE_INTEGER;
        id.l = res.id;
        child.obj["id"] = id;
    }
    else if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["POLICY_DUPLICATE_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_move_res(Policy_Move_Res& res, std::string& data)
{
    Container::Value v, child, id, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.id >= 0)
    {
        id.type = Container::Value::CONTAINER_TYPE_INTEGER;
        id.l = res.id;
        child.obj["id"] = id;
    }
    else if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["POLICY_MOVE_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_change_info_res(Policy_Change_Info_Res& res, std::string& data)
{
    Container::Value v, child, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["POLICY_CHANGE_INFO_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_change_type_res(Policy_Change_Type_Res& res, std::string& data)
{
    Container::Value v, child, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["POLICY_CHANGE_TYPE_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_change_is_public_res(Policy_Change_Is_Public_Res& res, std::string& data)
{
    Container::Value v, child, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["POLICY_CHANGE_IS_PUBLIC_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_get_res(Policy_Get_Res& res, std::string& data)
{
    Container::Value v, child, policy, policyTree, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);
    else if (res.policy)
    {
        serialize_a_policy(res.policy, policy);
        child.obj["policy"] = policy;
    }
    else if (res.policyTree.size())
    {
        child.type = Container::Value::CONTAINER_TYPE_STRING;
        child.s = res.policyTree;
    }

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["POLICY_GET_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_get_name_res(Policy_Get_Name_Res& res, std::string& data)
{
    Container::Value v, child, name, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    name.type = Container::Value::CONTAINER_TYPE_STRING;
    name.s = res.name;
    child.obj["name"] = name;

    if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["POLICY_GET_NAME_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_get_policies_count_res(Policy_Get_Policies_Count_Res& res, std::string& data)
{
    Container::Value v, child, size, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    size.type = Container::Value::CONTAINER_TYPE_INTEGER;
    size.l = res.size;
    child.obj["size"] = size;

    if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["POLICY_GET_POLICIES_COUNT_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_clear_policies_res(Policy_Clear_Policies_Res& res, std::string& data)
{
    Container::Value v, child, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["POLICY_CLEAR_POLICIES_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_get_policies_res(Policy_Get_Policies_Res& res, std::string& data)
{
    Container::Value v, child;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);
    else if (res.policies.size())
    {
        Container::Value policies;
        serialize_policies_get_policies(res.policies, policies);
        child.obj["policies"] = policies;
    }
    else if (res.policiesTree.size())
    {
        child.type = Container::Value::CONTAINER_TYPE_STRING;
        child.s = res.policiesTree;
    }

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["POLICY_GET_POLICIES_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_get_public_policies_res(Policy_Get_Public_Policies_Res& res, std::string& data)
{
    Container::Value v, child;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);
    else if (res.policies.size())
    {
        Container::Value policies;
        serialize_policy_public_policy(res.policies, policies);
        child.obj["policies"] = policies;
    }


    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["POLICY_GET_PUBLIC_POLICIES_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_policy_get_policies_names_list_res(Policy_Get_Policies_Names_List_Res& res, std::string& data)
{
    Container::Value v, child, policies, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);
    else
    {
        serialize_policies_get_policies_names(res.policies, policies);
        child.obj["policies"] = policies;
    }

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["POLICY_GET_POLICIES_NAMES_LIST_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_xslt_policy_create_from_file_res(XSLT_Policy_Create_From_File_Res& res, std::string& data)
{
    Container::Value v, child, policy_id, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.policy_id >= 0)
    {
        policy_id.type = Container::Value::CONTAINER_TYPE_INTEGER;
        policy_id.l = res.policy_id;
        child.obj["policy_id"] = policy_id;
    }
    else if (res.nok)
        child.obj["nok"] = serialize_generic_nok(res.nok->id, res.nok->error);

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["XSLT_POLICY_CREATE_FROM_FILE_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_xslt_policy_rule_create_res(XSLT_Policy_Rule_Create_Res& res, std::string& data)
{
    Container::Value v, child, id, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.id >= 0)
    {
        id.type = Container::Value::CONTAINER_TYPE_INTEGER;
        id.l = res.id;
        child.obj["id"] = id;
    }
    else if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["XSLT_POLICY_RULE_CREATE_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_xslt_policy_rule_get_res(XSLT_Policy_Rule_Get_Res& res, std::string& data)
{
    Container::Value v, child, rule, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);
    else
    {
        serialize_xslt_policy_rule(res.rule, rule);
        child.obj["rule"] = rule;
    }

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["XSLT_POLICY_RULE_GET_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_xslt_policy_rule_edit_res(XSLT_Policy_Rule_Edit_Res& res, std::string& data)
{
    Container::Value v, child, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["XSLT_POLICY_RULE_EDIT_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_xslt_policy_rule_duplicate_res(XSLT_Policy_Rule_Duplicate_Res& res, std::string& data)
{
    Container::Value v, child, id, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.id >= 0)
    {
        id.type = Container::Value::CONTAINER_TYPE_INTEGER;
        id.l = res.id;
        child.obj["id"] = id;
    }
    else if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["XSLT_POLICY_RULE_DUPLICATE_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_xslt_policy_rule_move_res(XSLT_Policy_Rule_Move_Res& res, std::string& data)
{
    Container::Value v, child, id, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.id >= 0)
    {
        id.type = Container::Value::CONTAINER_TYPE_INTEGER;
        id.l = res.id;
        child.obj["id"] = id;
    }
    else if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["XSLT_POLICY_RULE_MOVE_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::serialize_xslt_policy_rule_delete_res(XSLT_Policy_Rule_Delete_Res& res, std::string& data)
{
    Container::Value v, child, nok;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (res.nok)
        child.obj["nok"] = serialize_policy_nok(res.nok);

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj["XSLT_POLICY_RULE_DELETE_RESULT"] = child;

    if (model->serialize(v, data) < 0)
    {
        error = model->get_error();
        return -1;
    }

    return 0;
}

//***************************************************************************
// Parse: Request
//***************************************************************************

//---------------------------------------------------------------------------
RESTAPI::MediaConch_Get_Plugins_Req *RESTAPI::parse_mediaconch_get_plugins_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "MEDIACONCH_GET_PLUGINS");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;
    MediaConch_Get_Plugins_Req *req = new MediaConch_Get_Plugins_Req;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::MediaConch_Watch_Folder_Req *RESTAPI::parse_mediaconch_watch_folder_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "MEDIACONCH_WATCH_FOLDER");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *folder = model->get_value_by_key(*child, "folder");
    if (!folder || folder->type != Container::Value::CONTAINER_TYPE_STRING)
        return NULL;

    MediaConch_Watch_Folder_Req *req = new MediaConch_Watch_Folder_Req;
    req->folder = folder->s;

    Container::Value *folder_reports = model->get_value_by_key(*child, "folder_reports");
    if (folder_reports && folder_reports->type == Container::Value::CONTAINER_TYPE_STRING)
        req->folder_reports = folder_reports->s;

    Container::Value *plugins = model->get_value_by_key(*child, "plugins");
    if (plugins && plugins->type == Container::Value::CONTAINER_TYPE_ARRAY)
    {
        for (size_t i = 0; i < plugins->array.size(); ++i)
            if (plugins->array[i].type == Container::Value::CONTAINER_TYPE_STRING)
                req->plugins.push_back(plugins->array[i].s);
    }

    Container::Value *policies = model->get_value_by_key(*child, "policies");
    if (policies && policies->type == Container::Value::CONTAINER_TYPE_ARRAY)
    {
        for (size_t i = 0; i < policies->array.size(); ++i)
            if (policies->array[i].type == Container::Value::CONTAINER_TYPE_STRING)
                req->policies.push_back(policies->array[i].s);
    }

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
    {
        req->user = new long;
        *req->user = user->l;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::MediaConch_List_Watch_Folders_Req *RESTAPI::parse_mediaconch_list_watch_folders_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "MEDIACONCH_LIST_WATCH_FOLDERS");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    MediaConch_List_Watch_Folders_Req *req = new MediaConch_List_Watch_Folders_Req;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::MediaConch_Edit_Watch_Folder_Req *RESTAPI::parse_mediaconch_edit_watch_folder_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "MEDIACONCH_EDIT_WATCH_FOLDER");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *folder = model->get_value_by_key(*child, "folder");
    if (!folder || folder->type != Container::Value::CONTAINER_TYPE_STRING)
        return NULL;

    MediaConch_Edit_Watch_Folder_Req *req = new MediaConch_Edit_Watch_Folder_Req;
    req->folder = folder->s;

    Container::Value *folder_reports = model->get_value_by_key(*child, "folder_reports");
    if (folder_reports && folder_reports->type == Container::Value::CONTAINER_TYPE_STRING)
        req->folder_reports = folder_reports->s;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::MediaConch_Remove_Watch_Folder_Req *RESTAPI::parse_mediaconch_remove_watch_folder_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "MEDIACONCH_REMOVE_WATCH_FOLDER");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *folder = model->get_value_by_key(*child, "folder");
    if (!folder || folder->type != Container::Value::CONTAINER_TYPE_STRING)
        return NULL;

    MediaConch_Remove_Watch_Folder_Req *req = new MediaConch_Remove_Watch_Folder_Req;
    req->folder = folder->s;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Analyze_Req *RESTAPI::parse_analyze_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECKER_ANALYZE");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;
    Checker_Analyze_Req *req = new Checker_Analyze_Req;

    Container::Value *args;
    args = model->get_value_by_key(*child, "args");

    if (!args || parse_analyze_arg(args, req->args) < 0)
    {
        delete req;
        return NULL;
    }
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Status_Req *RESTAPI::parse_status_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECKER_STATUS");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;


    Container::Value *ids = model->get_value_by_key(*child, "ids");
    if (!ids || ids->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return NULL;

    Checker_Status_Req *req = new Checker_Status_Req;
    for (size_t i = 0; i < ids->array.size(); ++i)
    {
        Container::Value *id = &ids->array[i];

        if (id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        {
            delete req;
            return NULL;
        }
        req->ids.push_back(id->l);
    }

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Report_Req *RESTAPI::parse_report_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECKER_REPORT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *ids, *user, *reports, *policies_ids, *policies_contents, *display_name, *display_content, *options;
    ids = model->get_value_by_key(*child, "ids");
    user = model->get_value_by_key(*child, "user");
    reports = model->get_value_by_key(*child, "reports");
    policies_ids = model->get_value_by_key(*child, "policies_ids");
    policies_contents = model->get_value_by_key(*child, "policies_contents");
    display_name = model->get_value_by_key(*child, "display_name");
    display_content = model->get_value_by_key(*child, "display_content");
    options = model->get_value_by_key(*child, "options");

    if (!ids || !reports || ids->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return NULL;

    Checker_Report_Req *req = new Checker_Report_Req;
    for (size_t i = 0; i < ids->array.size(); ++i)
    {
        Container::Value *id = &ids->array[i];

        if (id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        {
            delete req;
            return NULL;
        }
        req->ids.push_back(id->l);
    }
    if (parse_report_reports(reports, req->reports))
    {
        delete req;
        return NULL;
    }

    if (policies_ids && policies_ids->type == Container::Value::CONTAINER_TYPE_ARRAY)
    {
        for (size_t i = 0; i < policies_ids->array.size(); ++i)
            if (policies_ids->array[i].type == Container::Value::CONTAINER_TYPE_INTEGER)
                req->policies_ids.push_back(policies_ids->array[i].l);
    }
    if (policies_contents && policies_contents->type == Container::Value::CONTAINER_TYPE_ARRAY)
    {
        for (size_t i = 0; i < policies_contents->array.size(); ++i)
            if (policies_contents->array[i].type == Container::Value::CONTAINER_TYPE_STRING)
                req->policies_contents.push_back(policies_contents->array[i].s);
    }
    if (display_name && display_name->type == Container::Value::CONTAINER_TYPE_STRING)
        req->display_name = display_name->s;
    if (display_content && display_content->type == Container::Value::CONTAINER_TYPE_STRING)
        req->display_content = display_content->s;

    if (options && options->type == Container::Value::CONTAINER_TYPE_OBJECT)
    {
        std::map<std::string, Container::Value>::iterator it = options->obj.begin();

        for (; it != options->obj.end(); ++it)
        {
            Container::Value& object = it->second;
            if (it->first.size() && object.type != Container::Value::CONTAINER_TYPE_STRING)
                continue;

            req->options[it->first.c_str()] = object.s;
        }
    }

    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;
    else
        req->user = -1;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Retry_Req *RESTAPI::parse_retry_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECKER_RETRY");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *ids;
    ids = model->get_value_by_key(*child, "ids");

    if (!ids || ids->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return NULL;

    Checker_Retry_Req *req = new Checker_Retry_Req;
    for (size_t i = 0; i < ids->array.size(); ++i)
    {
        Container::Value *id = &ids->array[i];

        if (id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        {
            delete req;
            return NULL;
        }
        req->ids.push_back(id->l);
    }

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Clear_Req *RESTAPI::parse_clear_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECKER_CLEAR");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *ids;
    ids = model->get_value_by_key(*child, "ids");

    if (!ids || ids->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return NULL;

    Checker_Clear_Req *req = new Checker_Clear_Req;
    for (size_t i = 0; i < ids->array.size(); ++i)
    {
        Container::Value *id = &ids->array[i];

        if (id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        {
            delete req;
            return NULL;
        }
        req->ids.push_back(id->l);
    }

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_List_Req *RESTAPI::parse_list_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECKER_LIST");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Checker_List_Req *req = new Checker_List_Req;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Validate_Req *RESTAPI::parse_validate_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECKER_VALIDATE");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *ids, *report, *user, *policies_ids, *policies_contents, *options;
    ids = model->get_value_by_key(*child, "ids");
    user = model->get_value_by_key(*child, "user");
    report = model->get_value_by_key(*child, "report");
    policies_ids = model->get_value_by_key(*child, "policies_ids");
    policies_contents = model->get_value_by_key(*child, "policies_contents");
    options = model->get_value_by_key(*child, "options");

    if (!ids || !report || ids->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return NULL;

    Checker_Validate_Req *req = new Checker_Validate_Req;
    for (size_t i = 0; i < ids->array.size(); ++i)
    {
        Container::Value *id = &ids->array[i];

        if (id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        {
            delete req;
            return NULL;
        }
        req->ids.push_back(id->l);
    }

    if (report->type != Container::Value::CONTAINER_TYPE_STRING)
    {
        delete req;
        return NULL;
    }
    req->report = string_to_Report(report->s);

    if (policies_ids && policies_ids->type == Container::Value::CONTAINER_TYPE_ARRAY)
    {
        for (size_t i = 0; i < policies_ids->array.size(); ++i)
            if (policies_ids->array[i].type == Container::Value::CONTAINER_TYPE_INTEGER)
                req->policies_ids.push_back(policies_ids->array[i].l);
    }
    if (policies_contents && policies_contents->type == Container::Value::CONTAINER_TYPE_ARRAY)
    {
        for (size_t i = 0; i < policies_contents->array.size(); ++i)
            if (policies_contents->array[i].type == Container::Value::CONTAINER_TYPE_STRING)
                req->policies_contents.push_back(policies_contents->array[i].s);
    }

    if (options && options->type == Container::Value::CONTAINER_TYPE_OBJECT)
    {
        std::map<std::string, Container::Value>::iterator it = options->obj.begin();

        for (; it != options->obj.end(); ++it)
        {
            Container::Value& object = it->second;
            if (it->first.size() && object.type != Container::Value::CONTAINER_TYPE_STRING)
                continue;

            req->options[it->first.c_str()] = object.s;
        }
    }

    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;
    else
        req->user = -1;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_File_From_Id_Req *RESTAPI::parse_file_from_id_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECKER_FILE_FROM_ID");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id;
    id = model->get_value_by_key(*child, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Checker_File_From_Id_Req *req = new Checker_File_From_Id_Req;
    req->id = id->l;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Id_From_Filename_Req *RESTAPI::parse_id_from_filename_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECKER_ID_FROM_FILENAME");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *filename;
    filename = model->get_value_by_key(*child, "filename");
    if (!filename || filename->type != Container::Value::CONTAINER_TYPE_STRING)
        return NULL;

    Checker_Id_From_Filename_Req *req = new Checker_Id_From_Filename_Req;
    req->filename = filename->s;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_File_Information_Req *RESTAPI::parse_file_information_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECKER_FILE_INFORMATION");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id;
    id = model->get_value_by_key(*child, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Checker_File_Information_Req *req = new Checker_File_Information_Req;
    req->id = id->l;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Default_Values_For_Type_Req *RESTAPI::parse_default_values_for_type_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "DEFAULT_VALUES_FOR_TYPE");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *type = model->get_value_by_key(*child, "type");
    if (!type || type->type != Container::Value::CONTAINER_TYPE_STRING)
        return NULL;

    Container::Value *field = model->get_value_by_key(*child, "field");
    if (!field || field->type != Container::Value::CONTAINER_TYPE_STRING)
        return NULL;

    Default_Values_For_Type_Req *req = new Default_Values_For_Type_Req;
    req->type = type->s;
    req->field = field->s;
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Create_Req *RESTAPI::parse_xslt_policy_create_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "XSLT_POLICY_CREATE");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    XSLT_Policy_Create_Req *req = new XSLT_Policy_Create_Req;

    Container::Value *type = model->get_value_by_key(*child, "type");
    if (type && type->type == Container::Value::CONTAINER_TYPE_STRING)
        req->type = type->s;

    Container::Value *parent_id = model->get_value_by_key(*child, "parent_id");
    if (parent_id && parent_id->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->parent_id = parent_id->l;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Import_Req *RESTAPI::parse_policy_import_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_IMPORT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *xml = model->get_value_by_key(*child, "xml");
    if (!xml || xml->type != Container::Value::CONTAINER_TYPE_STRING)
        return NULL;

    Policy_Import_Req *req = new Policy_Import_Req;
    req->xml = xml->s;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Remove_Req *RESTAPI::parse_policy_remove_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_REMOVE");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id = model->get_value_by_key(*child, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Policy_Remove_Req *req = new Policy_Remove_Req;
    req->id = id->l;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Dump_Req *RESTAPI::parse_policy_dump_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_DUMP");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id = model->get_value_by_key(*child, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Policy_Dump_Req *req = new Policy_Dump_Req;
    req->id = id->l;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    Container::Value *must_be_public = model->get_value_by_key(*child, "must_be_public");
    if (must_be_public && must_be_public->type == Container::Value::CONTAINER_TYPE_BOOL)
        req->must_be_public = must_be_public->b;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Save_Req *RESTAPI::parse_policy_save_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_SAVE");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id = model->get_value_by_key(*child, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Policy_Save_Req *req = new Policy_Save_Req;
    req->id = id->l;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Duplicate_Req *RESTAPI::parse_policy_duplicate_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_DUPLICATE");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id = model->get_value_by_key(*child, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Policy_Duplicate_Req *req = new Policy_Duplicate_Req;
    req->id = id->l;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    Container::Value *dst_policy_id = model->get_value_by_key(*child, "dst_policy_id");
    if (dst_policy_id && dst_policy_id->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->dst_policy_id = dst_policy_id->l;

    Container::Value *dst_user = model->get_value_by_key(*child, "dst_user");
    if (dst_user && dst_user->type == Container::Value::CONTAINER_TYPE_INTEGER)
    {
        req->dst_user = new int;
        *req->dst_user = dst_user->l;
    }

    Container::Value *must_be_public = model->get_value_by_key(*child, "must_be_public");
    if (must_be_public && must_be_public->type == Container::Value::CONTAINER_TYPE_BOOL)
        req->must_be_public = must_be_public->b;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Move_Req *RESTAPI::parse_policy_move_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_MOVE");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id = model->get_value_by_key(*child, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Policy_Move_Req *req = new Policy_Move_Req;
    req->id = id->l;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    Container::Value *dst_policy_id = model->get_value_by_key(*child, "dst_policy_id");
    if (dst_policy_id && dst_policy_id->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->dst_policy_id = dst_policy_id->l;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Change_Info_Req *RESTAPI::parse_policy_change_info_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_CHANGE_INFO");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id = model->get_value_by_key(*child, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Container::Value *name = model->get_value_by_key(*child, "name");
    if (!name || name->type != Container::Value::CONTAINER_TYPE_STRING)
        return NULL;

    Container::Value *desc = model->get_value_by_key(*child, "description");
    if (!desc || desc->type != Container::Value::CONTAINER_TYPE_STRING)
        return NULL;

    Policy_Change_Info_Req *req = new Policy_Change_Info_Req;
    req->id = id->l;
    req->name = name->s;
    req->description = desc->s;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    Container::Value *license = model->get_value_by_key(*child, "license");
    if (license && license->type == Container::Value::CONTAINER_TYPE_STRING)
        req->license = license->s;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Change_Type_Req *RESTAPI::parse_policy_change_type_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_CHANGE_TYPE");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id = model->get_value_by_key(*child, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Container::Value *type = model->get_value_by_key(*child, "type");
    if (!type || type->type != Container::Value::CONTAINER_TYPE_STRING)
        return NULL;

    Policy_Change_Type_Req *req = new Policy_Change_Type_Req;
    req->id = id->l;
    req->type = type->s;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Change_Is_Public_Req *RESTAPI::parse_policy_change_is_public_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_CHANGE_IS_PUBLIC");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id = model->get_value_by_key(*child, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Container::Value *is_public = model->get_value_by_key(*child, "is_public");
    if (!is_public || is_public->type != Container::Value::CONTAINER_TYPE_BOOL)
        return NULL;

    Policy_Change_Is_Public_Req *req = new Policy_Change_Is_Public_Req;
    req->id = id->l;
    req->is_public = is_public->b;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Get_Req *RESTAPI::parse_policy_get_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_GET");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id = model->get_value_by_key(*child, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Policy_Get_Req *req = new Policy_Get_Req;
    req->id = id->l;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    Container::Value *format = model->get_value_by_key(*child, "format");
    if (format && format->type == Container::Value::CONTAINER_TYPE_STRING)
        req->format = format->s;

    Container::Value *be_public = model->get_value_by_key(*child, "must_be_public");
    if (be_public && be_public->type == Container::Value::CONTAINER_TYPE_BOOL)
        req->must_be_public = be_public->b;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Get_Name_Req *RESTAPI::parse_policy_get_name_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_GET_NAME");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id;
    id = model->get_value_by_key(*child, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Policy_Get_Name_Req *req = new Policy_Get_Name_Req;
    req->id = id->l;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Get_Policies_Count_Req *RESTAPI::parse_policy_get_policies_count_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_GET_POLICIES_COUNT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Policy_Get_Policies_Count_Req *req = new Policy_Get_Policies_Count_Req;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Clear_Policies_Req *RESTAPI::parse_policy_clear_policies_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_CLEAR_POLICIES");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Policy_Clear_Policies_Req *req = new Policy_Clear_Policies_Req;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Get_Policies_Req *RESTAPI::parse_policy_get_policies_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_GET_POLICIES");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Policy_Get_Policies_Req *req = new Policy_Get_Policies_Req;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    Container::Value *ids = model->get_value_by_key(*child, "ids");
    if (ids && ids->type != Container::Value::CONTAINER_TYPE_ARRAY)
    {
        for (size_t i = 0; i < ids->array.size(); ++i)
            if (ids->array[i].type == Container::Value::CONTAINER_TYPE_INTEGER)
                req->ids.push_back(ids->array[i].l);
    }

    Container::Value *format = model->get_value_by_key(*child, "format");
    if (format && format->type == Container::Value::CONTAINER_TYPE_STRING)
        req->format = format->s;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Get_Public_Policies_Req *RESTAPI::parse_policy_get_public_policies_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_GET_PUBLIC_POLICIES");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Policy_Get_Public_Policies_Req *req = new Policy_Get_Public_Policies_Req;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Get_Policies_Names_List_Req *RESTAPI::parse_policy_get_policies_names_list_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_GET_POLICIES_NAMES_LIST");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Policy_Get_Policies_Names_List_Req *req = new Policy_Get_Policies_Names_List_Req;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Create_From_File_Req *RESTAPI::parse_xslt_policy_create_from_file_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "XSLT_POLICY_CREATE_FROM_FILE");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id = model->get_value_by_key(*child, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    XSLT_Policy_Create_From_File_Req *req = new XSLT_Policy_Create_From_File_Req;
    req->id = id->l;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    return req;
}

//---------------------------------------------------------------------------
int RESTAPI::parse_xslt_policy_rule(Container::Value *val, MediaConchLib::XSLT_Policy_Rule *rule)
{
    if (!val || val->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return -1;

    Container::Value *id = model->get_value_by_key(*val, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return -1;
    rule->id = id->l;

    Container::Value *name = model->get_value_by_key(*val, "name");
    if (name && name->type == Container::Value::CONTAINER_TYPE_STRING)
        rule->name = name->s;

    Container::Value *tracktype = model->get_value_by_key(*val, "tracktype");
    if (tracktype && tracktype->type == Container::Value::CONTAINER_TYPE_STRING)
        rule->tracktype = tracktype->s;

    Container::Value *field = model->get_value_by_key(*val, "field");
    if (field && field->type == Container::Value::CONTAINER_TYPE_STRING)
        rule->field = field->s;

    Container::Value *scope = model->get_value_by_key(*val, "scope");
    if (scope && scope->type == Container::Value::CONTAINER_TYPE_STRING)
        rule->scope = scope->s;

    Container::Value *occurrence = model->get_value_by_key(*val, "occurrence");
    if (occurrence && occurrence->type == Container::Value::CONTAINER_TYPE_INTEGER)
        rule->occurrence = occurrence->l;

    Container::Value *ope = model->get_value_by_key(*val, "ope");
    if (ope && ope->type == Container::Value::CONTAINER_TYPE_STRING)
        rule->ope = ope->s;

    Container::Value *value = model->get_value_by_key(*val, "value");
    if (value && value->type == Container::Value::CONTAINER_TYPE_STRING)
        rule->value = value->s;

    return 0;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Rule_Create_Req *RESTAPI::parse_xslt_policy_rule_create_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "XSLT_POLICY_RULE_CREATE");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *policy_id = model->get_value_by_key(*child, "policy_id");
    if (!policy_id || policy_id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    XSLT_Policy_Rule_Create_Req *req = new XSLT_Policy_Rule_Create_Req;
    req->policy_id = policy_id->l;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Rule_Get_Req *RESTAPI::parse_xslt_policy_rule_get_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "XSLT_POLICY_RULE_GET");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *policy_id = model->get_value_by_key(*child, "policy_id");
    if (!policy_id || policy_id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Container::Value *id = model->get_value_by_key(*child, "id");
    if (!policy_id || policy_id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    XSLT_Policy_Rule_Get_Req *req = new XSLT_Policy_Rule_Get_Req;
    req->policy_id = policy_id->l;
    req->id = id->l;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Rule_Edit_Req *RESTAPI::parse_xslt_policy_rule_edit_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "XSLT_POLICY_RULE_EDIT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *policy_id = model->get_value_by_key(*child, "policy_id");
    if (!policy_id || policy_id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Container::Value *rule = model->get_value_by_key(*child, "rule");
    if (!rule || rule->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    XSLT_Policy_Rule_Edit_Req *req = new XSLT_Policy_Rule_Edit_Req;
    req->policy_id = policy_id->l;
    if (parse_xslt_policy_rule(rule, &req->rule) < 0)
    {
        delete req;
        return NULL;
    }

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Rule_Duplicate_Req *RESTAPI::parse_xslt_policy_rule_duplicate_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "XSLT_POLICY_RULE_DUPLICATE");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *policy_id = model->get_value_by_key(*child, "policy_id");
    if (!policy_id || policy_id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Container::Value *id = model->get_value_by_key(*child, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    XSLT_Policy_Rule_Duplicate_Req *req = new XSLT_Policy_Rule_Duplicate_Req;
    req->policy_id = policy_id->l;
    req->id = id->l;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    Container::Value *dst_policy_id = model->get_value_by_key(*child, "dst_policy_id");
    if (dst_policy_id && dst_policy_id->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->dst_policy_id = dst_policy_id->l;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Rule_Move_Req *RESTAPI::parse_xslt_policy_rule_move_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "XSLT_POLICY_RULE_MOVE");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *policy_id = model->get_value_by_key(*child, "policy_id");
    if (!policy_id || policy_id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Container::Value *id = model->get_value_by_key(*child, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    XSLT_Policy_Rule_Move_Req *req = new XSLT_Policy_Rule_Move_Req;
    req->policy_id = policy_id->l;
    req->id = id->l;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    Container::Value *dst_policy_id = model->get_value_by_key(*child, "dst_policy_id");
    if (dst_policy_id && dst_policy_id->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->dst_policy_id = dst_policy_id->l;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Rule_Delete_Req *RESTAPI::parse_xslt_policy_rule_delete_req(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "XSLT_POLICY_RULE_DELETE");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *policy_id = model->get_value_by_key(*child, "policy_id");
    if (!policy_id || policy_id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Container::Value *id = model->get_value_by_key(*child, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    XSLT_Policy_Rule_Delete_Req *req = new XSLT_Policy_Rule_Delete_Req;
    req->policy_id = policy_id->l;
    req->id = id->l;

    Container::Value *user = model->get_value_by_key(*child, "user");
    if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        req->user = user->l;

    return req;
}

//***************************************************************************
// Parse: Request URI
//***************************************************************************

//---------------------------------------------------------------------------
RESTAPI::MediaConch_Get_Plugins_Req *RESTAPI::parse_uri_mediaconch_get_plugins_req(const std::string&)
{
    MediaConch_Get_Plugins_Req *req = new MediaConch_Get_Plugins_Req;
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::MediaConch_Watch_Folder_Req *RESTAPI::parse_uri_mediaconch_watch_folder_req(const std::string&)
{
    MediaConch_Watch_Folder_Req *req = new MediaConch_Watch_Folder_Req;
    //TODO
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::MediaConch_List_Watch_Folders_Req *RESTAPI::parse_uri_mediaconch_list_watch_folders_req(const std::string&)
{
    MediaConch_List_Watch_Folders_Req *req = new MediaConch_List_Watch_Folders_Req;
    //TODO
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::MediaConch_Edit_Watch_Folder_Req *RESTAPI::parse_uri_mediaconch_edit_watch_folder_req(const std::string&)
{
    MediaConch_Edit_Watch_Folder_Req *req = new MediaConch_Edit_Watch_Folder_Req;
    //TODO
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::MediaConch_Remove_Watch_Folder_Req *RESTAPI::parse_uri_mediaconch_remove_watch_folder_req(const std::string&)
{
    MediaConch_Remove_Watch_Folder_Req *req = new MediaConch_Remove_Watch_Folder_Req;
    //TODO
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Analyze_Req *RESTAPI::parse_uri_analyze_req(const std::string&)
{
    Checker_Analyze_Req *req = new Checker_Analyze_Req;
    //TODO
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Status_Req *RESTAPI::parse_uri_status_req(const std::string& uri)
{
    Checker_Status_Req *req = new Checker_Status_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "id")
        {
            if (!val.length())
                continue;

            req->ids.push_back(strtoll(val.c_str(), NULL, 10));
        }
        else if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Report_Req *RESTAPI::parse_uri_report_req(const std::string&)
{
    Checker_Report_Req *req = new Checker_Report_Req;
    //TODO
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Retry_Req *RESTAPI::parse_uri_retry_req(const std::string&)
{
    Checker_Retry_Req *req = new Checker_Retry_Req;
    //TODO
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Clear_Req *RESTAPI::parse_uri_clear_req(const std::string& uri)
{
    Checker_Clear_Req *req = new Checker_Clear_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "id")
        {
            if (!val.length())
                continue;

            req->ids.push_back(strtoll(val.c_str(), NULL, 10));
        }
        else if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_List_Req *RESTAPI::parse_uri_list_req(const std::string& uri)
{
    Checker_List_Req *req = new Checker_List_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else
            start = std::string::npos;
    }
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Validate_Req *RESTAPI::parse_uri_validate_req(const std::string&)
{
    Checker_Validate_Req *req = new Checker_Validate_Req;
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_File_From_Id_Req *RESTAPI::parse_uri_file_from_id_req(const std::string&)
{
    Checker_File_From_Id_Req *req = new Checker_File_From_Id_Req;
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Id_From_Filename_Req *RESTAPI::parse_uri_id_from_filename_req(const std::string&)
{
    Checker_Id_From_Filename_Req *req = new Checker_Id_From_Filename_Req;
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_File_Information_Req *RESTAPI::parse_uri_file_information_req(const std::string&)
{
    Checker_File_Information_Req *req = new Checker_File_Information_Req;
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Default_Values_For_Type_Req *RESTAPI::parse_uri_default_values_for_type_req(const std::string& uri)
{
    Default_Values_For_Type_Req *req = new Default_Values_For_Type_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "type")
        {
            if (!val.length())
                continue;

            req->type = val;
        }
        else if (substr == "field")
        {
            if (!val.length())
                continue;

            req->field = val;
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Create_Req *RESTAPI::parse_uri_xslt_policy_create_req(const std::string& uri)
{
    XSLT_Policy_Create_Req *req = new XSLT_Policy_Create_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "parent_id")
        {
            if (!val.length())
                continue;

            req->parent_id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "type")
        {
            if (!val.length())
                req->type = "and";
            else
                req->type = val;
        }
        else if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Import_Req *RESTAPI::parse_uri_policy_import_req(const std::string&)
{
    Policy_Import_Req *req = new Policy_Import_Req;
    //todo
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Remove_Req *RESTAPI::parse_uri_policy_remove_req(const std::string& uri)
{
    Policy_Remove_Req *req = new Policy_Remove_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "id")
        {
            if (!val.length())
                continue;

            req->id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Dump_Req *RESTAPI::parse_uri_policy_dump_req(const std::string& uri)
{
    Policy_Dump_Req *req = new Policy_Dump_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "id")
        {
            if (!val.length())
                continue;

            req->id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "must_be_public")
        {
            if (!val.length())
                continue;

            if (val == "true")
                req->must_be_public = true;
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Save_Req *RESTAPI::parse_uri_policy_save_req(const std::string& uri)
{
    Policy_Save_Req *req = new Policy_Save_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "id")
        {
            if (!val.length())
                continue;

            req->id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Duplicate_Req *RESTAPI::parse_uri_policy_duplicate_req(const std::string& uri)
{
    Policy_Duplicate_Req *req = new Policy_Duplicate_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "id")
        {
            if (!val.length())
                continue;

            req->id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "dst_policy_id")
        {
            if (!val.length())
                continue;

            req->dst_policy_id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "dst_user")
        {
            if (!val.length())
                continue;

            req->dst_user = new int;
            *req->dst_user = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "must_be_public")
        {
            if (!val.length())
                continue;

            if (val == "true")
                req->must_be_public = true;
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Move_Req *RESTAPI::parse_uri_policy_move_req(const std::string& uri)
{
    Policy_Move_Req *req = new Policy_Move_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "id")
        {
            if (!val.length())
                continue;

            req->id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "dst_policy_id")
        {
            if (!val.length())
                continue;

            req->dst_policy_id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Change_Info_Req *RESTAPI::parse_uri_policy_change_info_req(const std::string& uri)
{
    Policy_Change_Info_Req *req = new Policy_Change_Info_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "id")
        {
            if (!val.length())
                continue;

            req->id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "name")
        {
            if (!val.length())
                continue;

            req->name = val;
        }
        else if (substr == "description")
        {
            if (!val.length())
                continue;

            req->description = val;
        }
        else if (substr == "license")
        {
            if (!val.length())
                continue;

            req->license = val;
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Change_Type_Req *RESTAPI::parse_uri_policy_change_type_req(const std::string& uri)
{
    Policy_Change_Type_Req *req = new Policy_Change_Type_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "id")
        {
            if (!val.length())
                continue;

            req->id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "type")
        {
            if (!val.length())
                continue;

            req->type = val;
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Change_Is_Public_Req *RESTAPI::parse_uri_policy_change_is_public_req(const std::string& uri)
{
    Policy_Change_Is_Public_Req *req = new Policy_Change_Is_Public_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "id")
        {
            if (!val.length())
                continue;

            req->id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "is_public")
        {
            if (!val.length())
                continue;

            if (val == "true")
                req->is_public = true;
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Get_Req *RESTAPI::parse_uri_policy_get_req(const std::string& uri)
{
    Policy_Get_Req *req = new Policy_Get_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "id")
        {
            if (!val.length())
                continue;

            req->id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "format")
        {
            if (!val.length())
                continue;

            req->format = val;
        }
        else if (substr == "must_be_public")
        {
            if (!val.length())
                continue;

            if (val == "true")
                req->must_be_public = true;
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Get_Name_Req *RESTAPI::parse_uri_policy_get_name_req(const std::string& uri)
{
    Policy_Get_Name_Req *req = new Policy_Get_Name_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "id")
        {
            if (!val.length())
                continue;

            req->id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Get_Policies_Count_Req *RESTAPI::parse_uri_policy_get_policies_count_req(const std::string& uri)
{
    Policy_Get_Policies_Count_Req *req = new Policy_Get_Policies_Count_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Clear_Policies_Req *RESTAPI::parse_uri_policy_clear_policies_req(const std::string& uri)
{
    Policy_Clear_Policies_Req *req = new Policy_Clear_Policies_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Get_Policies_Req *RESTAPI::parse_uri_policy_get_policies_req(const std::string& uri)
{
    Policy_Get_Policies_Req *req = new Policy_Get_Policies_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "id")
        {
            if (!val.length())
                continue;

            req->ids.push_back(strtoll(val.c_str(), NULL, 10));
        }
        else if (substr == "format")
        {
            if (!val.length())
                continue;

            req->format = val;
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Get_Public_Policies_Req *RESTAPI::parse_uri_policy_get_public_policies_req(const std::string&)
{
    Policy_Get_Public_Policies_Req *req = new Policy_Get_Public_Policies_Req;

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Get_Policies_Names_List_Req *RESTAPI::parse_uri_policy_get_policies_names_list_req(const std::string& uri)
{
    Policy_Get_Policies_Names_List_Req *req = new Policy_Get_Policies_Names_List_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Create_From_File_Req *RESTAPI::parse_uri_xslt_policy_create_from_file_req(const std::string& uri)
{
    XSLT_Policy_Create_From_File_Req *req = new XSLT_Policy_Create_From_File_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "id")
        {
            if (!val.length())
                continue;

            req->id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Rule_Create_Req *RESTAPI::parse_uri_xslt_policy_rule_create_req(const std::string& uri)
{
    XSLT_Policy_Rule_Create_Req *req = new XSLT_Policy_Rule_Create_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "policy_id")
        {
            if (!val.length())
                continue;

            req->policy_id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Rule_Get_Req *RESTAPI::parse_uri_xslt_policy_rule_get_req(const std::string& uri)
{
    XSLT_Policy_Rule_Get_Req *req = new XSLT_Policy_Rule_Get_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "policy_id")
        {
            if (!val.length())
                continue;

            req->policy_id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "id")
        {
            if (!val.length())
                continue;

            req->id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Rule_Edit_Req *RESTAPI::parse_uri_xslt_policy_rule_edit_req(const std::string&)
{
    XSLT_Policy_Rule_Edit_Req *req = new XSLT_Policy_Rule_Edit_Req;
    //TODO

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Rule_Duplicate_Req *RESTAPI::parse_uri_xslt_policy_rule_duplicate_req(const std::string& uri)
{
    XSLT_Policy_Rule_Duplicate_Req *req = new XSLT_Policy_Rule_Duplicate_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            ++start;

        if (substr == "id")
        {
            if (!val.length())
                continue;

            req->id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "policy_id")
        {
            if (!val.length())
                continue;

            req->policy_id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "dst_policy_id")
        {
            if (!val.length())
                continue;

            req->dst_policy_id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Rule_Move_Req *RESTAPI::parse_uri_xslt_policy_rule_move_req(const std::string& uri)
{
    XSLT_Policy_Rule_Move_Req *req = new XSLT_Policy_Rule_Move_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            ++start;

        if (substr == "id")
        {
            if (!val.length())
                continue;

            req->id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "policy_id")
        {
            if (!val.length())
                continue;

            req->policy_id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "dst_policy_id")
        {
            if (!val.length())
                continue;

            req->dst_policy_id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else
            start = std::string::npos;
    }

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Rule_Delete_Req *RESTAPI::parse_uri_xslt_policy_rule_delete_req(const std::string& uri)
{
    XSLT_Policy_Rule_Delete_Req *req = new XSLT_Policy_Rule_Delete_Req;

    size_t start = 0;
    size_t and_pos = 0;
    while (start != std::string::npos)
    {
        size_t key_start = start;
        start = uri.find("=", start);
        if (start == std::string::npos)
            continue;

        std::string substr = uri.substr(key_start, start - key_start);
        ++start;
        and_pos = uri.find("&", start);
        std::string val = uri.substr(start, and_pos - start);

        start = and_pos;
        if (start != std::string::npos)
            start += 1;

        if (substr == "id")
        {
            if (!val.length())
                continue;

            req->id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "policy_id")
        {
            if (!val.length())
                continue;

            req->policy_id = strtoll(val.c_str(), NULL, 10);
        }
        else if (substr == "user")
        {
            if (!val.length())
                continue;

            req->user = strtoll(val.c_str(), NULL, 10);
        }
        else
            start = std::string::npos;
    }

    return req;
}

//***************************************************************************
// Parse: Result
//***************************************************************************

//---------------------------------------------------------------------------
RESTAPI::MediaConch_Get_Plugins_Res *RESTAPI::parse_mediaconch_get_plugins_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "MEDIACONCH_GET_PLUGINS_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *plugins = model->get_value_by_key(*child, "plugins");
    Container::Value *nok = model->get_value_by_key(*child, "nok");

    MediaConch_Get_Plugins_Res *res = new MediaConch_Get_Plugins_Res;

    if (nok)
    {
        if (nok->type != Container::Value::CONTAINER_TYPE_OBJECT)
        {
            delete res;
            return NULL;
        }

        parse_mediaconch_nok(nok, &res->nok);
    }
    else if (plugins && plugins->type == Container::Value::CONTAINER_TYPE_ARRAY)
    {
        for (size_t i = 0; i < plugins->array.size(); ++i)
        {
            if (plugins->array[i].type != Container::Value::CONTAINER_TYPE_STRING)
            {
                delete res;
                return NULL;
            }

            res->plugins.push_back(plugins->array[i].s);
        }
    }
    else
    {
        delete res;
        return NULL;
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::MediaConch_Watch_Folder_Res *RESTAPI::parse_mediaconch_watch_folder_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "MEDIACONCH_WATCH_FOLDER_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *user = model->get_value_by_key(*child, "user");
    Container::Value *nok = model->get_value_by_key(*child, "nok");

    MediaConch_Watch_Folder_Res *res = new MediaConch_Watch_Folder_Res;

    if (nok)
    {
        if (nok->type != Container::Value::CONTAINER_TYPE_OBJECT)
        {
            delete res;
            return NULL;
        }

        parse_mediaconch_nok(nok, &res->nok);
    }
    else if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
        res->user = user->l;
    else
    {
        delete res;
        return NULL;
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::MediaConch_List_Watch_Folders_Res *RESTAPI::parse_mediaconch_list_watch_folders_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "MEDIACONCH_LIST_WATCH_FOLDERS_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *folders = model->get_value_by_key(*child, "folders");
    Container::Value *nok = model->get_value_by_key(*child, "nok");

    MediaConch_List_Watch_Folders_Res *res = new MediaConch_List_Watch_Folders_Res;

    if (nok)
    {
        if (nok->type != Container::Value::CONTAINER_TYPE_OBJECT)
        {
            delete res;
            return NULL;
        }

        parse_mediaconch_nok(nok, &res->nok);
    }
    else if (folders && folders->type == Container::Value::CONTAINER_TYPE_ARRAY)
    {
        for (size_t i = 0; i < folders->array.size(); ++i)
        {
            if (folders->array[i].type == Container::Value::CONTAINER_TYPE_STRING)
                res->folders.push_back(folders->array[i].s);
        }
    }
    else
    {
        delete res;
        return NULL;
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::MediaConch_Edit_Watch_Folder_Res *RESTAPI::parse_mediaconch_edit_watch_folder_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "MEDIACONCH_EDIT_WATCH_FOLDER_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    MediaConch_Edit_Watch_Folder_Res *res = new MediaConch_Edit_Watch_Folder_Res;

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (nok)
    {
        if (nok->type != Container::Value::CONTAINER_TYPE_OBJECT)
        {
            delete res;
            return NULL;
        }

        parse_mediaconch_nok(nok, &res->nok);
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::MediaConch_Remove_Watch_Folder_Res *RESTAPI::parse_mediaconch_remove_watch_folder_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "MEDIACONCH_REMOVE_WATCH_FOLDER_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    MediaConch_Remove_Watch_Folder_Res *res = new MediaConch_Remove_Watch_Folder_Res;

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (nok)
    {
        if (nok->type != Container::Value::CONTAINER_TYPE_OBJECT)
        {
            delete res;
            return NULL;
        }

        parse_mediaconch_nok(nok, &res->nok);
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Analyze_Res *RESTAPI::parse_analyze_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECKER_ANALYZE_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *ok, *nok;
    ok = model->get_value_by_key(*child, "ok");
    nok = model->get_value_by_key(*child, "nok");

    Checker_Analyze_Res *res = new Checker_Analyze_Res;

    if (ok && parse_analyze_ok(ok, res->ok))
    {
        delete res;
        return NULL;
    }

    if (!nok)
        return res;

    if (nok->type != Container::Value::CONTAINER_TYPE_ARRAY)
    {
        delete res;
        return NULL;
    }

    for (size_t i = 0; i < nok->array.size(); ++i)
    {
        Checker_Analyze_Nok *tmp = new Checker_Analyze_Nok;

        if (parse_generic_nok(&nok->array[i], tmp->id, tmp->error))
        {
            delete res;
            return NULL;
        }
        res->nok.push_back(tmp);
    }
    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Status_Res *RESTAPI::parse_status_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECKER_STATUS_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *ok, *nok;
    ok = model->get_value_by_key(*child, "ok");
    nok = model->get_value_by_key(*child, "nok");

    Checker_Status_Res *res = new Checker_Status_Res;
    if (ok && parse_status_ok(ok, res->ok))
    {
        delete res;
        return NULL;
    }

    if (!nok)
        return res;

    if (nok->type != Container::Value::CONTAINER_TYPE_ARRAY)
    {
        delete res;
        return NULL;
    }

    for (size_t i = 0; i < nok->array.size(); ++i)
    {
        Checker_Status_Nok *tmp = new Checker_Status_Nok;

        if (parse_generic_nok(&nok->array[i], tmp->id, tmp->error))
        {
            delete res;
            return NULL;
        }
        res->nok.push_back(tmp);
    }
    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Report_Res *RESTAPI::parse_report_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECKER_REPORT_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *ok, *nok;
    ok = model->get_value_by_key(*child, "ok");
    nok = model->get_value_by_key(*child, "nok");

    Checker_Report_Res *res = new Checker_Report_Res;
    if (ok && parse_report_ok(ok, res->ok))
    {
        delete res;
        return NULL;
    }

    if (!nok)
        return res;

    if (nok->type != Container::Value::CONTAINER_TYPE_ARRAY)
    {
        delete res;
        return NULL;
    }

    for (size_t i = 0; i < nok->array.size(); ++i)
    {
        Checker_Report_Nok *tmp = new Checker_Report_Nok;

        if (parse_generic_nok(&nok->array[i], tmp->id, tmp->error))
        {
            delete res;
            return NULL;
        }
        res->nok.push_back(tmp);
    }
    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Retry_Res *RESTAPI::parse_retry_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECKER_RETRY_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *ok, *nok;
    ok = model->get_value_by_key(*child, "ok");
    nok = model->get_value_by_key(*child, "nok");

    Checker_Retry_Res *res = new Checker_Retry_Res;
    if (ok)
    {
        if (ok->type != Container::Value::CONTAINER_TYPE_ARRAY)
        {
            delete res;
            return NULL;
        }
        for (size_t i = 0; i < ok->array.size(); ++i)
        {
            Container::Value *tmp = &ok->array[i];

            if (tmp->type != Container::Value::CONTAINER_TYPE_INTEGER)
            {
                delete res;
                return NULL;
            }
            res->ok.push_back(tmp->l);
        }
    }

    if (!nok)
        return res;

    if (nok->type != Container::Value::CONTAINER_TYPE_ARRAY)
    {
        delete res;
        return NULL;
    }

    for (size_t i = 0; i < nok->array.size(); ++i)
    {
        Checker_Retry_Nok *tmp = new Checker_Retry_Nok;

        if (parse_generic_nok(&nok->array[i], tmp->id, tmp->error))
        {
            delete res;
            return NULL;
        }
        res->nok.push_back(tmp);
    }
    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Clear_Res *RESTAPI::parse_clear_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECKER_CLEAR_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *ok, *nok;
    ok = model->get_value_by_key(*child, "ok");
    nok = model->get_value_by_key(*child, "nok");

    Checker_Clear_Res *res = new Checker_Clear_Res;
    if (ok)
    {
        if (ok->type != Container::Value::CONTAINER_TYPE_ARRAY)
        {
            delete res;
            return NULL;
        }
        for (size_t i = 0; i < ok->array.size(); ++i)
        {
            Container::Value *tmp = &ok->array[i];

            if (tmp->type != Container::Value::CONTAINER_TYPE_INTEGER)
            {
                delete res;
                return NULL;
            }
            res->ok.push_back(tmp->l);
        }
    }

    if (!nok)
        return res;

    if (nok->type != Container::Value::CONTAINER_TYPE_ARRAY)
    {
        delete res;
        return NULL;
    }

    for (size_t i = 0; i < nok->array.size(); ++i)
    {
        Checker_Clear_Nok *tmp = new Checker_Clear_Nok;

        if (parse_generic_nok(&nok->array[i], tmp->id, tmp->error))
        {
            delete res;
            return NULL;
        }
        res->nok.push_back(tmp);
    }
    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_List_Res *RESTAPI::parse_list_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECKER_LIST_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *files;
    files = model->get_value_by_key(*child, "files");

    Checker_List_Res *res = new Checker_List_Res;
    if (files->type != Container::Value::CONTAINER_TYPE_ARRAY)
    {
        delete res;
        return NULL;
    }

    for (size_t i = 0; i < files->array.size(); ++i)
    {
        Checker_List_File *tmp = new Checker_List_File;

        if (parse_list_file(&files->array[i], tmp->file, tmp->id))
        {
            delete res;
            return NULL;
        }
        res->files.push_back(tmp);
    }
    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Validate_Res *RESTAPI::parse_validate_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECKER_VALIDATE_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *ok, *nok;
    ok = model->get_value_by_key(*child, "ok");
    nok = model->get_value_by_key(*child, "nok");

    Checker_Validate_Res *res = new Checker_Validate_Res;
    if (ok && parse_validate_ok(ok, res->ok))
    {
        delete res;
        return NULL;
    }

    if (!nok)
        return res;

    if (nok->type != Container::Value::CONTAINER_TYPE_ARRAY)
    {
        delete res;
        return NULL;
    }

    for (size_t i = 0; i < nok->array.size(); ++i)
    {
        Checker_Validate_Nok *tmp = new Checker_Validate_Nok;

        if (parse_generic_nok(&nok->array[i], tmp->id, tmp->error))
        {
            delete res;
            return NULL;
        }
        res->nok.push_back(tmp);
    }
    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_File_From_Id_Res *RESTAPI::parse_file_from_id_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECKER_FILE_FROM_ID_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *file;
    file = model->get_value_by_key(*child, "file");

    if (!file || file->type != Container::Value::CONTAINER_TYPE_STRING)
        return NULL;

    Checker_File_From_Id_Res *res = new Checker_File_From_Id_Res;
    res->file = file->s;

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_Id_From_Filename_Res *RESTAPI::parse_id_from_filename_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECKER_ID_FROM_FILENAME_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id;
    id = model->get_value_by_key(*child, "id");

    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Checker_Id_From_Filename_Res *res = new Checker_Id_From_Filename_Res;
    res->id = id->l;

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Checker_File_Information_Res *RESTAPI::parse_file_information_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECKER_FILE_INFORMATION_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *filename = model->get_value_by_key(*child, "filename");
    if (!filename || filename->type != Container::Value::CONTAINER_TYPE_STRING)
        return NULL;

    Checker_File_Information_Res *res = new Checker_File_Information_Res;
    res->filename = filename->s;

    Container::Value *file_last_modification = model->get_value_by_key(*child, "file_last_modification");
    if (file_last_modification && file_last_modification->type == Container::Value::CONTAINER_TYPE_STRING)
        res->file_last_modification = file_last_modification->s;

    Container::Value *generated_id = model->get_value_by_key(*child, "generated_id");
    if (generated_id && generated_id->type == Container::Value::CONTAINER_TYPE_INTEGER)
        res->generated_id = generated_id->l;

    Container::Value *source_id = model->get_value_by_key(*child, "source_id");
    if (source_id && source_id->type == Container::Value::CONTAINER_TYPE_INTEGER)
        res->source_id = source_id->l;

    Container::Value *generated_time = model->get_value_by_key(*child, "generated_time");
    if (generated_time && generated_time->type == Container::Value::CONTAINER_TYPE_INTEGER)
        res->generated_time = generated_time->l;

    Container::Value *generated_log = model->get_value_by_key(*child, "generated_log");
    if (generated_log && generated_log->type == Container::Value::CONTAINER_TYPE_STRING)
        res->generated_log = generated_log->s;

    Container::Value *generated_error_log = model->get_value_by_key(*child, "generated_error_log");
    if (generated_error_log && generated_error_log->type == Container::Value::CONTAINER_TYPE_STRING)
        res->generated_error_log = generated_error_log->s;

    Container::Value *analyzed = model->get_value_by_key(*child, "analyzed");
    if (analyzed && analyzed->type == Container::Value::CONTAINER_TYPE_BOOL)
        res->analyzed = analyzed->b;

    Container::Value *has_error = model->get_value_by_key(*child, "has_error");
    if (has_error && has_error->type == Container::Value::CONTAINER_TYPE_BOOL)
        res->has_error = has_error->b;

    if (res->has_error)
    {
        Container::Value *error_log = model->get_value_by_key(*child, "error_log");
        if (error_log && error_log->type == Container::Value::CONTAINER_TYPE_STRING)
            res->error_log = error_log->s;
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Default_Values_For_Type_Res *RESTAPI::parse_default_values_for_type_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "DEFAULT_VALUES_FOR_TYPE_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *values;
    values = model->get_value_by_key(*child, "values");

    if (!values || values->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return NULL;

    Default_Values_For_Type_Res *res = new Default_Values_For_Type_Res;
    for (size_t i = 0; i < values->array.size(); ++i)
    {
        if (values->array[i].type != Container::Value::CONTAINER_TYPE_STRING)
            continue;

        res->values.push_back(values->array[i].s);
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Create_Res *RESTAPI::parse_xslt_policy_create_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "XSLT_POLICY_CREATE_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id = model->get_value_by_key(*child, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    XSLT_Policy_Create_Res *res = new XSLT_Policy_Create_Res;
    res->id = id->l;

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (parse_policy_nok(nok, &res->nok) < 0)
    {
        delete res;
        return NULL;
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Import_Res *RESTAPI::parse_policy_import_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_IMPORT_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id = model->get_value_by_key(*child, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Policy_Import_Res *res = new Policy_Import_Res;
    res->id = id->l;

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (parse_policy_nok(nok, &res->nok) < 0)
    {
        delete res;
        return NULL;
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Remove_Res *RESTAPI::parse_policy_remove_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_REMOVE_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Policy_Remove_Res *res = new Policy_Remove_Res;

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (parse_policy_nok(nok, &res->nok) < 0)
    {
        delete res;
        return NULL;
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Dump_Res *RESTAPI::parse_policy_dump_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_DUMP_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *xml = model->get_value_by_key(*child, "xml");
    if (!xml || xml->type != Container::Value::CONTAINER_TYPE_STRING)
        return NULL;

    Policy_Dump_Res *res = new Policy_Dump_Res;
    res->xml = xml->s;

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (parse_policy_nok(nok, &res->nok) < 0)
    {
        delete res;
        return NULL;
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Save_Res *RESTAPI::parse_policy_save_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_SAVE_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Policy_Save_Res *res = new Policy_Save_Res;

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (parse_policy_nok(nok, &res->nok) < 0)
    {
        delete res;
        return NULL;
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Duplicate_Res *RESTAPI::parse_policy_duplicate_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_DUPLICATE_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id = model->get_value_by_key(*child, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Policy_Duplicate_Res *res = new Policy_Duplicate_Res;
    res->id = id->l;

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (parse_policy_nok(nok, &res->nok) < 0)
    {
        delete res;
        return NULL;
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Move_Res *RESTAPI::parse_policy_move_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_MOVE_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id = model->get_value_by_key(*child, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Policy_Move_Res *res = new Policy_Move_Res;
    res->id = id->l;

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (parse_policy_nok(nok, &res->nok) < 0)
    {
        delete res;
        return NULL;
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Change_Info_Res *RESTAPI::parse_policy_change_info_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_CHANGE_INFO_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Policy_Change_Info_Res *res = new Policy_Change_Info_Res;

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (parse_policy_nok(nok, &res->nok) < 0)
    {
        delete res;
        return NULL;
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Change_Type_Res *RESTAPI::parse_policy_change_type_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_CHANGE_TYPE_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Policy_Change_Type_Res *res = new Policy_Change_Type_Res;

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (parse_policy_nok(nok, &res->nok) < 0)
    {
        delete res;
        return NULL;
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Change_Is_Public_Res *RESTAPI::parse_policy_change_is_public_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_CHANGE_IS_PUBLIC_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Policy_Change_Is_Public_Res *res = new Policy_Change_Is_Public_Res;

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (parse_policy_nok(nok, &res->nok) < 0)
    {
        delete res;
        return NULL;
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Get_Res *RESTAPI::parse_policy_get_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_GET_RESULT");
    if (!child)
        return NULL;

    Policy_Get_Res *res = NULL;

    if (child->type == Container::Value::CONTAINER_TYPE_OBJECT)
    {
        Container::Value *nok = model->get_value_by_key(*child, "nok");
        Container::Value *policy = model->get_value_by_key(*child, "policy");

        res = new Policy_Get_Res;

        if (nok && parse_policy_nok(nok, &res->nok) < 0)
        {
            delete res;
            return NULL;
        }
        else if (policy && (res->policy = parse_a_policy(policy)) == NULL)
        {
            delete res;
            return NULL;
        }
        else
        {
            delete res;
            return NULL;
        }
    }
    else if (child->type == Container::Value::CONTAINER_TYPE_STRING)
    {
        res = new Policy_Get_Res;
        res->policyTree = child->s;
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Get_Name_Res *RESTAPI::parse_policy_get_name_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_GET_NAME_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *name = model->get_value_by_key(*child, "name");
    if (!name || name->type != Container::Value::CONTAINER_TYPE_STRING)
        return NULL;

    Policy_Get_Name_Res *res = new Policy_Get_Name_Res;
    res->name = name->s;

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (parse_policy_nok(nok, &res->nok) < 0)
    {
        delete res;
        return NULL;
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Get_Policies_Count_Res *RESTAPI::parse_policy_get_policies_count_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_GET_POLICIES_COUNT_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *size = model->get_value_by_key(*child, "size");
    if (!size || size->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    Policy_Get_Policies_Count_Res *res = new Policy_Get_Policies_Count_Res;
    res->size = size->l;

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (parse_policy_nok(nok, &res->nok) < 0)
    {
        delete res;
        return NULL;
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Clear_Policies_Res *RESTAPI::parse_policy_clear_policies_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_CLEAR_POLICIES_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Policy_Clear_Policies_Res *res = new Policy_Clear_Policies_Res;

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (parse_policy_nok(nok, &res->nok) < 0)
    {
        delete res;
        return NULL;
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Get_Policies_Res *RESTAPI::parse_policy_get_policies_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_GET_POLICIES_RESULT");
    if (!child)
        return NULL;

    Policy_Get_Policies_Res *res = NULL;
    if (child->type == Container::Value::CONTAINER_TYPE_OBJECT)
    {
        Container::Value *policies = model->get_value_by_key(*child, "policies");
        Container::Value *nok = model->get_value_by_key(*child, "nok");

        res = new Policy_Get_Policies_Res;
        if (nok && parse_policy_nok(nok, &res->nok) < 0)
        {
            delete res;
            return NULL;
        }
        else if (policies && policies->type == Container::Value::CONTAINER_TYPE_ARRAY)
        {
            if (parse_policies_get_policies(policies, res->policies) < 0)
            {
                delete res;
                return NULL;
            }
        }
        else
        {
            delete res;
            return NULL;
        }
    }
    else if (child->type == Container::Value::CONTAINER_TYPE_STRING)
    {
        res = new Policy_Get_Policies_Res;
        res->policiesTree = child->s;
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Get_Public_Policies_Res *RESTAPI::parse_policy_get_public_policies_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_GET_PUBLIC_POLICIES_RESULT");
    if (!child)
        return NULL;

    Policy_Get_Public_Policies_Res *res = NULL;
    if (child->type == Container::Value::CONTAINER_TYPE_OBJECT)
    {
        Container::Value *policies = model->get_value_by_key(*child, "policies");
        Container::Value *nok = model->get_value_by_key(*child, "nok");

        res = new Policy_Get_Public_Policies_Res;
        if (nok && parse_policy_nok(nok, &res->nok) < 0)
        {
            delete res;
            return NULL;
        }
        else if (policies && policies->type == Container::Value::CONTAINER_TYPE_ARRAY)
        {
            if (parse_policy_public_policy(policies, res->policies) < 0)
            {
                delete res;
                return NULL;
            }
        }
        else
        {
            delete res;
            return NULL;
        }
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::Policy_Get_Policies_Names_List_Res *RESTAPI::parse_policy_get_policies_names_list_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "POLICY_GET_POLICIES_NAMES_LIST_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *policies = model->get_value_by_key(*child, "policies");
    if (!policies || policies->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return NULL;

    Policy_Get_Policies_Names_List_Res *res = new Policy_Get_Policies_Names_List_Res;
    if (parse_policies_get_policies_names(policies, res->policies) < 0)
    {
        delete res;
        return NULL;;
    }

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (parse_policy_nok(nok, &res->nok) < 0)
    {
        delete res;
        return NULL;
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Create_From_File_Res *RESTAPI::parse_xslt_policy_create_from_file_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "XSLT_POLICY_CREATE_FROM_FILE_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *policy_id = model->get_value_by_key(*child, "policy_id");

    if (!policy_id || policy_id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    XSLT_Policy_Create_From_File_Res *res = new XSLT_Policy_Create_From_File_Res;
    res->policy_id = policy_id->l;

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Rule_Create_Res *RESTAPI::parse_xslt_policy_rule_create_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "XSLT_POLICY_RULE_CREATE_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    XSLT_Policy_Rule_Create_Res *res = new XSLT_Policy_Rule_Create_Res;

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (parse_policy_nok(nok, &res->nok) < 0)
    {
        delete res;
        return NULL;
    }

    if (!nok)
    {
        Container::Value *id = model->get_value_by_key(*child, "id");
        if (id && id->type == Container::Value::CONTAINER_TYPE_INTEGER)
            res->id = id->l;
    }
    return res;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Rule_Get_Res *RESTAPI::parse_xslt_policy_rule_get_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "XSLT_POLICY_RULE_GET_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    XSLT_Policy_Rule_Get_Res *res = new XSLT_Policy_Rule_Get_Res;

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (parse_policy_nok(nok, &res->nok) < 0)
    {
        delete res;
        return NULL;
    }
    else
    {
        Container::Value *rule = model->get_value_by_key(*child, "rule");
        if (!rule)
        {
            delete res;
            return NULL;
        }

        if (parse_xslt_policy_rule(rule, &res->rule) < 0)
        {
            delete res;
            return NULL;
        }
    }

    return res;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Rule_Edit_Res *RESTAPI::parse_xslt_policy_rule_edit_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "XSLT_POLICY_RULE_EDIT_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    XSLT_Policy_Rule_Edit_Res *res = new XSLT_Policy_Rule_Edit_Res;

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (parse_policy_nok(nok, &res->nok) < 0)
    {
        delete res;
        return NULL;
    }
    return res;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Rule_Duplicate_Res *RESTAPI::parse_xslt_policy_rule_duplicate_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "XSLT_POLICY_RULE_DUPLICATE_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id = model->get_value_by_key(*child, "id");

    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    XSLT_Policy_Rule_Duplicate_Res *res = new XSLT_Policy_Rule_Duplicate_Res;
    res->id = id->l;

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (parse_policy_nok(nok, &res->nok) < 0)
    {
        delete res;
        return NULL;
    }
    return res;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Rule_Move_Res *RESTAPI::parse_xslt_policy_rule_move_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "XSLT_POLICY_RULE_MOVE_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id = model->get_value_by_key(*child, "id");

    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    XSLT_Policy_Rule_Move_Res *res = new XSLT_Policy_Rule_Move_Res;
    res->id = id->l;

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (parse_policy_nok(nok, &res->nok) < 0)
    {
        delete res;
        return NULL;
    }
    return res;
}

//---------------------------------------------------------------------------
RESTAPI::XSLT_Policy_Rule_Delete_Res *RESTAPI::parse_xslt_policy_rule_delete_res(const std::string& data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "XSLT_POLICY_RULE_DELETE_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    XSLT_Policy_Rule_Delete_Res *res = new XSLT_Policy_Rule_Delete_Res;

    Container::Value *nok = model->get_value_by_key(*child, "nok");
    if (parse_policy_nok(nok, &res->nok) < 0)
    {
        delete res;
        return NULL;
    }
    return res;
}

//***************************************************************************
// HELPER
//***************************************************************************

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_mediaconch_nok(MediaConch_Nok* nok)
{
    Container::Value nok_v, error_v;

    nok_v.type = Container::Value::CONTAINER_TYPE_OBJECT;

    error_v.type = Container::Value::CONTAINER_TYPE_STRING;
    error_v.s = nok->error;
    nok_v.obj["error"] = error_v;

    return nok_v;
}

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_analyze_args(std::vector<Checker_Analyze_Arg>& args)
{
    Container::Value args_val;

    args_val.type = Container::Value::CONTAINER_TYPE_ARRAY;

    for (size_t i = 0; i < args.size(); ++i)
    {
        Container::Value arg, file, user, id, force, plugins;
        arg.type = Container::Value::CONTAINER_TYPE_OBJECT;

        file.type = Container::Value::CONTAINER_TYPE_STRING;
        file.s = args[i].file;
        arg.obj["file"] = file;

        user.type = Container::Value::CONTAINER_TYPE_INTEGER;
        user.l = args[i].user;
        arg.obj["user"] = user;

        id.type = Container::Value::CONTAINER_TYPE_INTEGER;
        id.l = args[i].id;
        arg.obj["id"] = id;

        if (args[i].has_force_analyze)
        {
            force.type = Container::Value::CONTAINER_TYPE_BOOL;
            force.b = args[i].force_analyze;
            arg.obj["force"] = force;
        }

        plugins.type = Container::Value::CONTAINER_TYPE_ARRAY;
        for (size_t j = 0; j < args[i].plugins.size(); ++j)
        {
            Container::Value plugin;
            plugin.type = Container::Value::CONTAINER_TYPE_STRING;
            plugin.s = args[i].plugins[j];
            plugins.array.push_back(plugin);
        }
        arg.obj["plugins"] = plugins;

        args_val.array.push_back(arg);
    }

    return args_val;
}

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_ids(std::vector<int>& ids)
{
    Container::Value array;

    array.type = Container::Value::CONTAINER_TYPE_ARRAY;

    for (size_t i = 0; i < ids.size(); ++i)
    {
        Container::Value arg, id;
        id.type = Container::Value::CONTAINER_TYPE_INTEGER;
        id.l = ids[i];
        array.array.push_back(id);
    }

    return array;
}

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_report_reports(std::vector<Report>& reports)
{
    Container::Value reports_val;

    reports_val.type = Container::Value::CONTAINER_TYPE_ARRAY;

    for (size_t i = 0; i < reports.size(); ++i)
    {
        Container::Value report;

        report.type = Container::Value::CONTAINER_TYPE_STRING;
        report.s = get_Report_string(reports[i]);

        reports_val.array.push_back(report);
    }

    return reports_val;
}

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_report_string(const std::string& reports)
{
    Container::Value str_val;

    str_val.type = Container::Value::CONTAINER_TYPE_STRING;
    str_val.s = reports;

    return str_val;
}

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_report_int(int val)
{
    Container::Value int_val;

    int_val.type = Container::Value::CONTAINER_TYPE_INTEGER;
    int_val.l = val;

    return int_val;
}

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_report_arr_str(const std::vector<std::string>& reports)
{
    Container::Value arr_val;

    arr_val.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < reports.size(); ++i)
    {
        Container::Value str_val;

        str_val.type = Container::Value::CONTAINER_TYPE_STRING;
        str_val.s = reports[i];
        arr_val.array.push_back(str_val);
    }

    return arr_val;
}

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_report_arr_long_u(const std::vector<size_t>& reports)
{
    Container::Value arr_val;

    arr_val.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < reports.size(); ++i)
    {
        Container::Value str_val;

        str_val.type = Container::Value::CONTAINER_TYPE_INTEGER;
        str_val.l = reports[i];
        arr_val.array.push_back(str_val);
    }

    return arr_val;
}

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_generic_nok(int id, Reason error)
{
    Container::Value nok, id_v, error_v;

    nok.type = Container::Value::CONTAINER_TYPE_OBJECT;

    id_v.type = Container::Value::CONTAINER_TYPE_INTEGER;
    id_v.l = id;
    nok.obj["id"] = id_v;

    error_v.type = Container::Value::CONTAINER_TYPE_STRING;
    error_v.s = get_Reason_string(error);
    nok.obj["error"] = error_v;
    return nok;
}

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_analyze_oks(std::vector<Checker_Analyze_Ok*>& array)
{
    Container::Value ok;

    ok.type = Container::Value::CONTAINER_TYPE_ARRAY;

    for (size_t i = 0; i < array.size(); ++i)
    {
        if (!array[i])
            continue;

        Container::Value v, in, out, create;

        v.type = Container::Value::CONTAINER_TYPE_OBJECT;

        in.type = Container::Value::CONTAINER_TYPE_INTEGER;
        in.l = array[i]->inId;
        v.obj["inId"] = in;

        out.type = Container::Value::CONTAINER_TYPE_INTEGER;
        out.l = array[i]->outId;
        v.obj["outId"] = out;

        create.type = Container::Value::CONTAINER_TYPE_BOOL;
        create.b = array[i]->create;
        v.obj["create"] = create;

        ok.array.push_back(v);
    }

    return ok;
}

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_status_oks(std::vector<Checker_Status_Ok*>& array)
{
    Container::Value ok;

    ok.type = Container::Value::CONTAINER_TYPE_ARRAY;

    for (size_t i = 0; i < array.size(); ++i)
    {
        if (!array[i])
            continue;

        Container::Value v, id, finished, has_error, error_log, done, tool, generated_id, source_id;

        v.type = Container::Value::CONTAINER_TYPE_OBJECT;

        id.type = Container::Value::CONTAINER_TYPE_INTEGER;
        id.l = array[i]->id;
        v.obj["id"] = id;

        finished.type = Container::Value::CONTAINER_TYPE_BOOL;
        finished.b = array[i]->finished;
        v.obj["finished"] = finished;

        if (array[i]->finished && array[i]->has_error)
        {
            has_error.type = Container::Value::CONTAINER_TYPE_BOOL;
            has_error.b = array[i]->has_error;
            v.obj["has_error"] = has_error;

            error_log.type = Container::Value::CONTAINER_TYPE_STRING;
            error_log.s = array[i]->error_log;
            v.obj["error_log"] = error_log;
        }

        if (!array[i]->finished && array[i]->percent)
        {
            done.type = Container::Value::CONTAINER_TYPE_REAL;
            done.d = *array[i]->percent;
            v.obj["done"] = done;
        }

        if (array[i]->finished && array[i]->tool)
        {
            tool.type = Container::Value::CONTAINER_TYPE_INTEGER;
            tool.l = *array[i]->tool;
            v.obj["tool"] = tool;
        }

        if (array[i]->generated_id != -1)
        {
            generated_id.type = Container::Value::CONTAINER_TYPE_INTEGER;
            generated_id.l = array[i]->generated_id;
            v.obj["generated_id"] = generated_id;
        }

        if (array[i]->source_id != -1)
        {
            source_id.type = Container::Value::CONTAINER_TYPE_INTEGER;
            source_id.l = array[i]->source_id;
            v.obj["source_id"] = source_id;
        }

        ok.array.push_back(v);
    }

    return ok;
}

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_report_ok(Checker_Report_Ok& obj)
{
    Container::Value ok;

    ok.type = Container::Value::CONTAINER_TYPE_OBJECT;
    Container::Value report, valid;

    if (obj.report.length() > 0)
    {
        report.type = Container::Value::CONTAINER_TYPE_STRING;
        report.s = obj.report;
        ok.obj["report"] = report;
    }

    if (obj.has_valid)
    {
        valid.type = Container::Value::CONTAINER_TYPE_BOOL;
        valid.b = obj.valid;
        ok.obj["valid"] = valid;
    }
    return ok;
}

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_list_file(const std::string& filename, int id)
{
    Container::Value file, id_v, filename_v;

    file.type = Container::Value::CONTAINER_TYPE_OBJECT;

    id_v.type = Container::Value::CONTAINER_TYPE_INTEGER;
    id_v.l = id;
    file.obj["id"] = id_v;

    filename_v.type = Container::Value::CONTAINER_TYPE_STRING;
    filename_v.s = filename;
    file.obj["file"] = filename_v;
    return file;
}

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_validate_ok(Checker_Validate_Ok* obj)
{
    Container::Value ok;
    ok.type = Container::Value::CONTAINER_TYPE_OBJECT;

    if (!obj)
        return ok;

    Container::Value id, valid;

    id.type = Container::Value::CONTAINER_TYPE_INTEGER;
    id.l = obj->id;
    ok.obj["id"] = id;

    valid.type = Container::Value::CONTAINER_TYPE_BOOL;
    valid.b = obj->valid;
    ok.obj["valid"] = valid;

    return ok;
}

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_policy_nok(Policy_Nok* nok)
{
    Container::Value nok_v, error_v;

    nok_v.type = Container::Value::CONTAINER_TYPE_OBJECT;

    error_v.type = Container::Value::CONTAINER_TYPE_STRING;
    error_v.s = nok->error;
    nok_v.obj["error"] = error_v;
    return nok_v;
}

//---------------------------------------------------------------------------
void RESTAPI::serialize_a_policy(MediaConchLib::Policy_Policy* policy, Container::Value &ok_v)
{
    Container::Value id, parent_id, is_system, is_public, kind, type, name, description, license, children;

    ok_v.type = Container::Value::CONTAINER_TYPE_OBJECT;

    id.type = Container::Value::CONTAINER_TYPE_INTEGER;
    id.l = policy->id;
    ok_v.obj["id"] = id;

    parent_id.type = Container::Value::CONTAINER_TYPE_INTEGER;
    parent_id.l = policy->parent_id;
    ok_v.obj["parent_id"] = parent_id;

    is_system.type = Container::Value::CONTAINER_TYPE_BOOL;
    is_system.b = policy->is_system;
    ok_v.obj["is_system"] = is_system;

    is_public.type = Container::Value::CONTAINER_TYPE_BOOL;
    is_public.b = policy->is_public;
    ok_v.obj["is_public"] = is_public;

    kind.type = Container::Value::CONTAINER_TYPE_STRING;
    kind.s = policy->kind;
    ok_v.obj["kind"] = kind;

    type.type = Container::Value::CONTAINER_TYPE_STRING;
    type.s = policy->type;
    ok_v.obj["type"] = type;

    name.type = Container::Value::CONTAINER_TYPE_STRING;
    name.s = policy->name;
    ok_v.obj["name"] = name;

    description.type = Container::Value::CONTAINER_TYPE_STRING;
    description.s = policy->description;
    ok_v.obj["description"] = description;

    if (policy->license.size())
    {
        license.type = Container::Value::CONTAINER_TYPE_STRING;
        license.s = policy->license;
        ok_v.obj["license"] = license;
    }

    children.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < policy->children.size(); ++i)
    {
        Container::Value v;
        if (policy->children[i].first == 0)
            serialize_a_policy(policy->children[i].second.policy, v);
        else if (policy->children[i].first == 1)
            serialize_a_xslt_policy_rule(policy->children[i].second.rule, v);

        children.array.push_back(v);
    }
    ok_v.obj["children"] = children;
}

//---------------------------------------------------------------------------
void RESTAPI::serialize_a_xslt_policy_rule(MediaConchLib::XSLT_Policy_Rule* rule, Container::Value &ok_v)
{
    Container::Value id, name, tracktype, field, scope, occurrence, ope, value;

    ok_v.type = Container::Value::CONTAINER_TYPE_OBJECT;

    id.type = Container::Value::CONTAINER_TYPE_INTEGER;
    id.l = rule->id;
    ok_v.obj["id"] = id;

    if (rule->name.size())
    {
        name.type = Container::Value::CONTAINER_TYPE_STRING;
        name.s = rule->name;
        ok_v.obj["name"] = name;
    }

    if (rule->tracktype.size())
    {
        tracktype.type = Container::Value::CONTAINER_TYPE_STRING;
        tracktype.s = rule->tracktype;
        ok_v.obj["tracktype"] = tracktype;
    }

    if (rule->field.size())
    {
        field.type = Container::Value::CONTAINER_TYPE_STRING;
        field.s = rule->field;
        ok_v.obj["field"] = field;
    }

    if (rule->scope.size())
    {
        scope.type = Container::Value::CONTAINER_TYPE_STRING;
        scope.s = rule->scope;
        ok_v.obj["scope"] = scope;
    }

    occurrence.type = Container::Value::CONTAINER_TYPE_INTEGER;
    occurrence.l = rule->occurrence;
    ok_v.obj["occurrence"] = occurrence;

    if (rule->ope.size())
    {
        ope.type = Container::Value::CONTAINER_TYPE_STRING;
        ope.s = rule->ope;
        ok_v.obj["ope"] = ope;
    }

    if (rule->value.size())
    {
        value.type = Container::Value::CONTAINER_TYPE_STRING;
        value.s = rule->value;
        ok_v.obj["value"] = value;
    }
}

//---------------------------------------------------------------------------
void RESTAPI::serialize_policies_get_policies(const std::vector<MediaConchLib::Policy_Policy *>& policies, Container::Value &p)
{
    p.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < policies.size(); ++i)
    {
        Container::Value ok_v;

        if (!policies[i])
            continue;

        serialize_a_policy(policies[i], ok_v);
        p.array.push_back(ok_v);
    }
}

//---------------------------------------------------------------------------
void RESTAPI::serialize_policy_public_policy(const std::vector<Policy_Public_Policy*>& policies, Container::Value &p)
{
    p.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < policies.size(); ++i)
    {
        if (!policies[i])
            continue;

        Container::Value policy, id, user, name, description, license;

        policy.type = Container::Value::CONTAINER_TYPE_OBJECT;

        id.type = Container::Value::CONTAINER_TYPE_INTEGER;
        id.l = policies[i]->id;
        policy.obj["id"] = id;

        user.type = Container::Value::CONTAINER_TYPE_INTEGER;
        user.l = policies[i]->user;
        policy.obj["user"] = user;

        name.type = Container::Value::CONTAINER_TYPE_STRING;
        name.s = policies[i]->name;
        policy.obj["name"] = name;

        description.type = Container::Value::CONTAINER_TYPE_STRING;
        description.s = policies[i]->description;
        policy.obj["description"] = description;

        if (policies[i]->license.size())
        {
            license.type = Container::Value::CONTAINER_TYPE_STRING;
            license.s = policies[i]->license;
            policy.obj["license"] = license;
        }

        p.array.push_back(policy);
    }
}

//---------------------------------------------------------------------------
void RESTAPI::serialize_policies_get_policies_names(const std::vector<std::pair<int, std::string> >& policies, Container::Value &p)
{
    p.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < policies.size(); ++i)
    {
        Container::Value policy, id, name;

        policy.type = Container::Value::CONTAINER_TYPE_OBJECT;

        id.type = Container::Value::CONTAINER_TYPE_INTEGER;
        id.l = policies[i].first;
        policy.obj["id"] = id;

        name.type = Container::Value::CONTAINER_TYPE_STRING;
        name.s = policies[i].second;
        policy.obj["name"] = name;

        p.array.push_back(policy);
    }
}

//---------------------------------------------------------------------------
int RESTAPI::parse_mediaconch_nok(Container::Value *nok_v, RESTAPI::MediaConch_Nok **nok)
{
    if (!nok_v)
    {
        *nok = NULL;
        return 0;
    }

    *nok = new MediaConch_Nok;
    if (nok_v->type != Container::Value::CONTAINER_TYPE_OBJECT)
    {
        delete *nok;
        *nok = NULL;
        return -1;
    }

    Container::Value *error = model->get_value_by_key(*nok_v, "error");
    if (!error || error->type != Container::Value::CONTAINER_TYPE_STRING)
    {
        delete *nok;
        *nok = NULL;
        return -1;
    }

    (*nok)->error = error->s;

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::parse_analyze_arg(Container::Value *v, std::vector<Checker_Analyze_Arg>& args)
{
    if (v->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return -1;

    for (size_t i = 0; i < v->array.size(); ++i)
    {
        Container::Value *obj = &v->array[i];

        if (obj->type != Container::Value::CONTAINER_TYPE_OBJECT)
            return -1;

        Container::Value *file = model->get_value_by_key(*obj, "file");
        Container::Value *user = model->get_value_by_key(*obj, "user");
        Container::Value *id = model->get_value_by_key(*obj, "id");
        Container::Value *force = model->get_value_by_key(*obj, "force");
        Container::Value *plugins = model->get_value_by_key(*obj, "plugins");

        if (!file || !id || file->type != Container::Value::CONTAINER_TYPE_STRING ||
            id->type != Container::Value::CONTAINER_TYPE_INTEGER)
            return -1;

        Checker_Analyze_Arg arg;
        arg.file = file->s;
        arg.id = id->l;

        if (user && user->type == Container::Value::CONTAINER_TYPE_INTEGER)
            arg.user = user->l;

        if (force && force->type == Container::Value::CONTAINER_TYPE_BOOL)
        {
            arg.has_force_analyze = true;
            arg.force_analyze = force->b;
        }

        if (plugins && plugins->type == Container::Value::CONTAINER_TYPE_ARRAY)
        {
            for (size_t j = 0; j < plugins->array.size(); ++j)
                if (plugins->array[i].type == Container::Value::CONTAINER_TYPE_STRING)
                    arg.plugins.push_back(plugins->array[j].s);
        }

        args.push_back(arg);
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::parse_report_reports(Container::Value *v, std::vector<Report>& reports)
{
    if (v->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return -1;

    for (size_t i = 0; i < v->array.size(); ++i)
    {
        Container::Value *obj = &v->array[i];

        if (obj->type != Container::Value::CONTAINER_TYPE_STRING)
            return -1;

        reports.push_back(string_to_Report(obj->s));
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::parse_generic_nok(Container::Value *v, int& id, Reason& error)
{
    if (v->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return -1;

    Container::Value *id_v, *error_v;

    id_v = model->get_value_by_key(*v, "id");
    error_v = model->get_value_by_key(*v, "error");

    if (!id_v || id_v->type != Container::Value::CONTAINER_TYPE_INTEGER ||
        !error_v || error_v->type != Container::Value::CONTAINER_TYPE_STRING)
        return -1;

    id = id_v->l;
    error = string_to_Reason(error_v->s);
    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::parse_analyze_ok(Container::Value *v, std::vector<Checker_Analyze_Ok*>& oks)
{
    if (v->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return -1;

    for (size_t i = 0; i < v->array.size(); ++i)
    {
        Container::Value *obj = &v->array[i];

        if (obj->type != Container::Value::CONTAINER_TYPE_OBJECT)
            return -1;

        Container::Value *inId, *outId, *create;

        inId = model->get_value_by_key(*obj, "inId");
        outId = model->get_value_by_key(*obj, "outId");
        create = model->get_value_by_key(*obj, "create");

        if (!inId || inId->type != Container::Value::CONTAINER_TYPE_INTEGER ||
            !outId || outId->type != Container::Value::CONTAINER_TYPE_INTEGER)
            return -1;

        Checker_Analyze_Ok *ok = new Checker_Analyze_Ok;
        ok->inId = inId->l;
        ok->outId = outId->l;
        if (create && create->type == Container::Value::CONTAINER_TYPE_BOOL)
            ok->create = create->b;

        oks.push_back(ok);
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::parse_status_ok(Container::Value *v, std::vector<Checker_Status_Ok*>& oks)
{
    if (v->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return -1;

    for (size_t i = 0; i < v->array.size(); ++i)
    {
        Container::Value *obj = &v->array[i];

        if (obj->type != Container::Value::CONTAINER_TYPE_OBJECT)
            return -1;

        Container::Value *id, *finished, *has_error, *error_log, *done, *tool, *generated_id, *source_id;

        id = model->get_value_by_key(*obj, "id");
        finished = model->get_value_by_key(*obj, "finished");
        has_error = model->get_value_by_key(*obj, "has_error");
        error_log = model->get_value_by_key(*obj, "error_log");
        done = model->get_value_by_key(*obj, "done");
        tool = model->get_value_by_key(*obj, "tool");
        generated_id = model->get_value_by_key(*obj, "generated_id");
        source_id = model->get_value_by_key(*obj, "source_id");

        if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER ||
            !finished || finished->type != Container::Value::CONTAINER_TYPE_BOOL)
            return -1;

        Checker_Status_Ok *ok = new Checker_Status_Ok;
        ok->id = id->l;
        ok->finished = finished->b;

        if (!ok->finished && done && done->type == Container::Value::CONTAINER_TYPE_REAL)
        {
            ok->percent = new double;
            *ok->percent = done->d;
        }
        else if (!ok->finished && done->type == Container::Value::CONTAINER_TYPE_INTEGER)
        {
            ok->percent = new double;
            *ok->percent = (double)done->l;
        }
        else if (!ok->finished)
        {
            ok->percent = new double;
            *ok->percent = (double)0;
        }

        if (ok->finished && tool && tool->type == Container::Value::CONTAINER_TYPE_INTEGER)
        {
            ok->tool = new Report;
            *ok->tool = (Report)tool->l;
        }

        if (ok->finished && has_error && has_error->type == Container::Value::CONTAINER_TYPE_BOOL)
        {
            ok->has_error = has_error->b;
            if (ok->has_error && error_log && error_log->type == Container::Value::CONTAINER_TYPE_STRING)
                ok->error_log = error_log->s;
        }

        if (generated_id && generated_id->type == Container::Value::CONTAINER_TYPE_INTEGER)
            ok->generated_id = generated_id->l;

        if (source_id && source_id->type == Container::Value::CONTAINER_TYPE_INTEGER)
            ok->source_id = source_id->l;

        oks.push_back(ok);
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::parse_report_ok(Container::Value *v, Checker_Report_Ok& ok)
{
    if (v->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return -1;

    Container::Value *report, *valid;

    report = model->get_value_by_key(*v, "report");
    valid = model->get_value_by_key(*v, "valid");

    if (report)
    {
        if (report->type != Container::Value::CONTAINER_TYPE_STRING)
            return -1;
        ok.report = report->s;
    }

    if (valid)
    {
        if (valid->type != Container::Value::CONTAINER_TYPE_BOOL)
            return -1;
        ok.has_valid = true;
        ok.valid = valid->b;
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::parse_list_file(Container::Value *v, std::string& file, int& id)
{
    if (v->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return -1;

    Container::Value *id_v, *file_v;

    file_v = model->get_value_by_key(*v, "file");
    id_v = model->get_value_by_key(*v, "id");

    if (!id_v || id_v->type != Container::Value::CONTAINER_TYPE_INTEGER ||
        !file_v || file_v->type != Container::Value::CONTAINER_TYPE_STRING)
        return -1;

    id = id_v->l;
    file = file_v->s;
    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::parse_validate_ok(Container::Value *v, std::vector<Checker_Validate_Ok*>& oks)
{
    if (v->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return -1;

    for (size_t i = 0; i < v->array.size(); ++i)
    {
        Checker_Validate_Ok* ok = new Checker_Validate_Ok;
        Container::Value *id, *valid;
        id = model->get_value_by_key(v->array[i], "id");
        valid = model->get_value_by_key(v->array[i], "valid");

        if (id)
        {
            if (id->type != Container::Value::CONTAINER_TYPE_INTEGER)
                return -1;
            ok->id = id->l;
        }

        if (valid)
        {
            if (valid->type != Container::Value::CONTAINER_TYPE_BOOL)
                return -1;
            ok->valid = valid->b;
        }
        oks.push_back(ok);
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::parse_policy_nok(Container::Value *nok_v, RESTAPI::Policy_Nok **nok)
{
    if (!nok_v)
        return 0;

    *nok = new Policy_Nok;
    if (nok_v->type != Container::Value::CONTAINER_TYPE_OBJECT)
    {
        delete *nok;
        *nok = NULL;
        return -1;
    }

    Container::Value *error = model->get_value_by_key(*nok_v, "error");
    if (!error || error->type != Container::Value::CONTAINER_TYPE_STRING)
    {
        delete *nok;
        *nok = NULL;
        return -1;
    }

    (*nok)->error = error->s;

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::parse_policies_get_policies(Container::Value *p, std::vector<MediaConchLib::Policy_Policy *>& policies)
{
    if (p->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return -1;

    for (size_t i = 0; i < p->array.size(); ++i)
    {
        MediaConchLib::Policy_Policy *ok = parse_a_policy(&p->array[i]);

        if (ok)
            policies.push_back(ok);
    }
    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::parse_policy_public_policy(Container::Value *p, std::vector<Policy_Public_Policy*>& policies)
{
    if (p->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return -1;

    for (size_t i = 0; i < p->array.size(); ++i)
    {
        if (p->array[i].type != Container::Value::CONTAINER_TYPE_OBJECT)
            continue;

        Container::Value *id = model->get_value_by_key(p->array[i], "id");
        Container::Value *user = model->get_value_by_key(p->array[i], "user");
        Container::Value *name = model->get_value_by_key(p->array[i], "name");
        Container::Value *description = model->get_value_by_key(p->array[i], "description");
        Container::Value *license = model->get_value_by_key(p->array[i], "license");
        if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER ||
            !user || user->type != Container::Value::CONTAINER_TYPE_INTEGER ||
            !name || name->type != Container::Value::CONTAINER_TYPE_STRING ||
            !description || description->type != Container::Value::CONTAINER_TYPE_STRING)
            return -1;

        Policy_Public_Policy *policy = new Policy_Public_Policy;
        policy->id = id->l;
        policy->user = user->l;
        policy->name = name->s;
        policy->description = description->s;
        if (license && license->type == Container::Value::CONTAINER_TYPE_STRING)
            policy->license = license->s;

        policies.push_back(policy);
    }
    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::parse_policies_get_policies_names(Container::Value *p, std::vector<std::pair<int, std::string> >& policies)
{
    if (p->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return -1;

    for (size_t i = 0; i < p->array.size(); ++i)
    {
        Container::Value *policy = &p->array[i];
        if (policy->type != Container::Value::CONTAINER_TYPE_OBJECT)
            return -1;

        Container::Value *id = model->get_value_by_key(*policy, "id");
        if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
            return -1;

        std::string n;
        Container::Value *name = model->get_value_by_key(*policy, "name");
        if (name && name->type == Container::Value::CONTAINER_TYPE_STRING)
            n = name->s;

        policies.push_back(std::make_pair(id->l, n));
    }
    return 0;
}

//---------------------------------------------------------------------------
MediaConchLib::Policy_Policy* RESTAPI::parse_a_policy(Container::Value *policy)
{
    if (!policy || policy->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id = model->get_value_by_key(*policy, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    MediaConchLib::Policy_Policy *ok = new MediaConchLib::Policy_Policy;
    ok->id = id->l;

    Container::Value *parent_id = model->get_value_by_key(*policy, "parent_id");
    if (parent_id && parent_id->type == Container::Value::CONTAINER_TYPE_INTEGER)
        ok->parent_id = parent_id->l;

    Container::Value *is_system = model->get_value_by_key(*policy, "is_system");
    if (is_system && is_system->type == Container::Value::CONTAINER_TYPE_BOOL)
        ok->is_system = is_system->b;

    Container::Value *is_public = model->get_value_by_key(*policy, "is_public");
    if (is_public && is_public->type == Container::Value::CONTAINER_TYPE_BOOL)
        ok->is_public = is_public->b;

    Container::Value *kind = model->get_value_by_key(*policy, "kind");
    if (kind && kind->type == Container::Value::CONTAINER_TYPE_STRING)
        ok->kind = kind->s;

    Container::Value *type = model->get_value_by_key(*policy, "type");
    if (type && type->type == Container::Value::CONTAINER_TYPE_STRING)
        ok->type = type->s;
    else
        ok->type = "and";

    Container::Value *name = model->get_value_by_key(*policy, "name");
    if (name && name->type == Container::Value::CONTAINER_TYPE_STRING)
        ok->name = name->s;

    Container::Value *description = model->get_value_by_key(*policy, "description");
    if (description && description->type == Container::Value::CONTAINER_TYPE_STRING)
        ok->description = description->s;

    Container::Value *license = model->get_value_by_key(*policy, "license");
    if (license && license->type == Container::Value::CONTAINER_TYPE_STRING)
        ok->license = license->s;

    Container::Value *children = model->get_value_by_key(*policy, "children");
    if (children && children->type == Container::Value::CONTAINER_TYPE_ARRAY)
    {
        for (size_t i = 0; i < children->array.size(); ++i)
        {
            if (children->array[i].type != Container::Value::CONTAINER_TYPE_OBJECT)
                continue;

            //If kind is not present, it is a rule
            Container::Value *child_kind = model->get_value_by_key(children->array[i], "kind");
            if (child_kind)
            {
                MediaConchLib::Policy_Policy* p = parse_a_policy(&children->array[i]);
                if (!p)
                    continue;

                MediaConchLib::XSLT_Child child;
                child.policy = p;
                ok->children.push_back(std::make_pair(0, child));
            }
            else
            {
                MediaConchLib::XSLT_Policy_Rule* r = parse_a_xslt_policy_rule(&children->array[i]);
                if (!r)
                    continue;

                MediaConchLib::XSLT_Child child;
                child.rule = r;
                ok->children.push_back(std::make_pair(0, child));
            }
        }
    }

    return ok;
}

//---------------------------------------------------------------------------
MediaConchLib::XSLT_Policy_Rule* RESTAPI::parse_a_xslt_policy_rule(Container::Value *rule)
{
    if (!rule || rule->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *id = model->get_value_by_key(*rule, "id");
    if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return NULL;

    MediaConchLib::XSLT_Policy_Rule *ok = new MediaConchLib::XSLT_Policy_Rule;
    ok->id = id->l;

    Container::Value *name = model->get_value_by_key(*rule, "name");
    if (name && name->type == Container::Value::CONTAINER_TYPE_STRING)
        ok->name = name->s;

    Container::Value *tracktype = model->get_value_by_key(*rule, "tracktype");
    if (tracktype && tracktype->type == Container::Value::CONTAINER_TYPE_STRING)
        ok->tracktype = tracktype->s;

    Container::Value *field = model->get_value_by_key(*rule, "field");
    if (field && field->type == Container::Value::CONTAINER_TYPE_STRING)
        ok->field = field->s;

    Container::Value *scope = model->get_value_by_key(*rule, "scope");
    if (scope && scope->type == Container::Value::CONTAINER_TYPE_STRING)
        ok->scope = scope->s;

    Container::Value *occurrence = model->get_value_by_key(*rule, "occurrence");
    if (occurrence && occurrence->type == Container::Value::CONTAINER_TYPE_INTEGER)
        ok->occurrence = occurrence->l;

    Container::Value *ope = model->get_value_by_key(*rule, "ope");
    if (ope && ope->type == Container::Value::CONTAINER_TYPE_STRING)
        ok->ope = ope->s;

    Container::Value *value = model->get_value_by_key(*rule, "value");
    if (value && value->type == Container::Value::CONTAINER_TYPE_STRING)
        ok->value = value->s;

    return ok;
}

}
