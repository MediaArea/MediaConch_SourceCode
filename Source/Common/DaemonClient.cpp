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
#include "DaemonClient.h"
#include "MediaConchLib.h"
#include "Http.h"
#include "LibEventHttp.h"
#include "REST_API.h"
#ifdef _WIN32
#include <ZenLib/Ztring.h>
#include <Winsock2.h>
#endif //_WIN32
#ifdef _WIN32
#else
#include <stdlib.h>
#endif //_WIN32

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// DaemonClient
//***************************************************************************

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
DaemonClient::DaemonClient(MediaConchLib* m) : mcl(m), http_client(NULL)
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        printf("WSAStartup failed\n");
    }
#endif //_WIN32
}

//---------------------------------------------------------------------------
DaemonClient::~DaemonClient()
{
#ifdef _WIN32
    WSACleanup();
#endif //_WIN32
}

//---------------------------------------------------------------------------
int DaemonClient::init()
{
#ifdef HAVE_LIBEVENT
    http_client = new LibEventHttp;
#else
    return -1;
#endif

    std::string server;
    int port = -1;
    mcl->get_daemon_address(server, port);

    if (server.length() != 0)
        http_client->set_address(server);
    if (port != -1)
        http_client->set_port(port);

    http_client->init();

    return 0;
}

//---------------------------------------------------------------------------
int DaemonClient::close()
{
    if (http_client)
    {
        delete http_client;
        http_client = NULL;
    }
    return 0;
}

//***************************************************************************
// Checker
//***************************************************************************

//---------------------------------------------------------------------------
int DaemonClient::checker_list(std::vector<std::string>& vec)
{
    if (!http_client)
        return MediaConchLib::errorHttp_INIT;

    RESTAPI::Checker_List_Req req;

    int ret = http_client->start();
    if (ret < 0)
        return ret;
    ret = http_client->send_request(req);
    if (ret < 0)
        return ret;

    std::string data = http_client->get_result();
    http_client->stop();
    if (!data.length())
        return http_client->get_error();

    RESTAPI rest;
    RESTAPI::Checker_List_Res *res = rest.parse_list_res(data);
    if (!res)
        return MediaConchLib::errorHttp_INVALID_DATA;

    for (size_t i = 0; i < res->files.size(); ++i)
        vec.push_back(res->files[i]->file);

    delete res;
    return MediaConchLib::errorHttp_NONE;
}

//---------------------------------------------------------------------------
int DaemonClient::checker_file_from_id(int id, std::string& filename)
{
    if (!http_client)
        return MediaConchLib::errorHttp_INIT;

    RESTAPI::Checker_File_From_Id_Req req;
    req.id = id;

    int ret = http_client->start();
    if (ret < 0)
        return ret;

    ret = http_client->send_request(req);
    if (ret < 0)
        return ret;

    std::string data = http_client->get_result();
    http_client->stop();
    if (!data.length())
        return http_client->get_error();

    RESTAPI rest;
    RESTAPI::Checker_File_From_Id_Res *res = rest.parse_file_from_id_res(data);
    if (!res)
        return MediaConchLib::errorHttp_INVALID_DATA;

    filename = res->file;
    delete res;
    return MediaConchLib::errorHttp_NONE;
}

//---------------------------------------------------------------------------
int DaemonClient::default_values_for_type(const std::string& type, std::vector<std::string>& values)
{
    if (!http_client)
        return MediaConchLib::errorHttp_INIT;

    RESTAPI::Default_Values_For_Type_Req req;
    req.type = type;

    int ret = http_client->start();
    if (ret < 0)
        return ret;

    ret = http_client->send_request(req);
    if (ret < 0)
        return ret;

    std::string data = http_client->get_result();
    http_client->stop();
    if (!data.length())
        return http_client->get_error();

    RESTAPI rest;
    RESTAPI::Default_Values_For_Type_Res *res = rest.parse_default_values_for_type_res(data);
    if (!res)
        return MediaConchLib::errorHttp_INVALID_DATA;

    for (size_t i = 0; i < res->values.size(); ++i)
        values.push_back(res->values[i]);
    delete res;
    return MediaConchLib::errorHttp_NONE;
}

//---------------------------------------------------------------------------
int DaemonClient::checker_analyze(const std::string& file, bool& registered, bool force_analyze)
{
    if (!http_client)
        return MediaConchLib::errorHttp_INIT;

    RESTAPI::Checker_Analyze_Req req;
    RESTAPI::Checker_Analyze_Arg arg;

    arg.id = 0;

    std::string real_file(file);
#ifdef _WIN32
    ZenLib::Ztring path = ZenLib::Ztring().From_UTF8(real_file);

    DWORD path_size = GetFullPathName(path.c_str(), 0, NULL, NULL);
    ZenLib::Char* tmp = new ZenLib::Char[path_size + 1];
    if (GetFullPathName(path.c_str(), path_size + 1, tmp, NULL))
    {
        path = ZenLib::Ztring(tmp);
        real_file = path.To_UTF8();
    }
    delete [] tmp;
#else
    char *path = realpath(file.c_str(), NULL);
    if (path)
    {
        real_file = std::string(path);
        free(path);
    }
#endif //_WIN32

    arg.file = real_file;
    if (force_analyze)
    {
        arg.has_force_analyze = true;
        arg.force_analyze = force_analyze;
    }
    req.args.push_back(arg);

    int ret = http_client->start();
    if (ret < 0)
        return ret;
    ret = http_client->send_request(req);
    if (ret < 0)
        return ret;

    std::string data = http_client->get_result();
    http_client->stop();
    if (!data.length())
        return http_client->get_error();

    RESTAPI rest;
    RESTAPI::Checker_Analyze_Res *res = rest.parse_analyze_res(data);
    if (!res)
        return MediaConchLib::errorHttp_INVALID_DATA;

    if (res->ok.size() != 1)
    {
        delete res;
        return MediaConchLib::errorHttp_INVALID_DATA;
    }

    registered = !res->ok[0]->create;

    file_ids[file] = res->ok[0]->outId;
    delete res;
    return MediaConchLib::errorHttp_NONE;
}

//---------------------------------------------------------------------------
int DaemonClient::checker_is_done(const std::string& file, double& done, MediaConchLib::report& report_kind)
{
    if (!http_client)
        return MediaConchLib::errorHttp_INIT;

    RESTAPI::Checker_Status_Req req;
    std::map<std::string, int>::iterator it = file_ids.find(file);
    if (it == file_ids.end())
        return MediaConchLib::errorHttp_MAX;
    req.ids.push_back(it->second);

    int ret = http_client->start();
    if (ret < 0)
        return ret;

    ret = http_client->send_request(req);
    if (ret < 0)
        return ret;

    std::string data = http_client->get_result();
    http_client->stop();
    if (!data.length())
        return MediaConchLib::errorHttp_INVALID_DATA;

    RESTAPI rest;
    RESTAPI::Checker_Status_Res *res = rest.parse_status_res(data);
    if (!res || res->ok.size() != 1)
        return MediaConchLib::errorHttp_INVALID_DATA;

    RESTAPI::Checker_Status_Ok *ok = res->ok[0];

    if (ok->finished)
    {
        report_kind = MediaConchLib::report_MediaConch;
        if (ok->has_tool)
        {
            if (ok->tool == RESTAPI::VERAPDF)
                report_kind = MediaConchLib::report_MediaVeraPdf;
            else if (ok->tool == RESTAPI::DPFMANAGER)
                report_kind = MediaConchLib::report_MediaDpfManager;
        }
        delete res;
        return MediaConchLib::errorHttp_TRUE;
    }

    if (ok->has_percent)
        done = ok->done;
    else
        done = 0.0;

    report_kind = MediaConchLib::report_MediaConch;

    delete res;
    return MediaConchLib::errorHttp_NONE;
}

//---------------------------------------------------------------------------
int DaemonClient::checker_get_report(const std::bitset<MediaConchLib::report_Max>& report_set,
                                     MediaConchLib::format f, const std::vector<std::string>& files,
                                     const std::vector<size_t>& policies_ids,
                                     const std::vector<std::string>& policies_contents,
                                     const std::map<std::string, std::string>& options,
                                     MediaConchLib::Checker_ReportRes* result,
                                     const std::string* display_name,
                                     const std::string* display_content)
{
    if (!http_client)
        return -1;

    // FILE
    RESTAPI::Checker_Report_Req req;
    for (size_t i = 0; i < files.size(); ++i)
    {
        std::map<std::string, int>::iterator it = file_ids.find(files[i]);
        if (it == file_ids.end())
            continue;
        req.ids.push_back(it->second);
    }

    // REPORT KIND
    if (report_set[MediaConchLib::report_MediaConch])
        req.reports.push_back(RESTAPI::IMPLEMENTATION);
    if (report_set[MediaConchLib::report_MediaInfo])
        req.reports.push_back(RESTAPI::MEDIAINFO);
    if (report_set[MediaConchLib::report_MediaTrace])
        req.reports.push_back(RESTAPI::MEDIATRACE);
    if (report_set[MediaConchLib::report_MediaVeraPdf])
        req.reports.push_back(RESTAPI::VERAPDF);
    if (report_set[MediaConchLib::report_MediaDpfManager])
        req.reports.push_back(RESTAPI::DPFMANAGER);

    // POLICY
    if (policies_ids.size())
    {
        req.reports.push_back(RESTAPI::POLICY);
        for (size_t i = 0; i < policies_ids.size(); ++i)
            req.policies_ids.push_back(policies_ids[i]);
    }
    if (policies_contents.size())
    {
        req.reports.push_back(RESTAPI::POLICY);
        for (size_t i = 0; i < policies_contents.size(); ++i)
            req.policies_contents.push_back(policies_contents[i]);
    }

    // FORMAT
    if (f == MediaConchLib::format_Xml)
    {
        if (display_name)
            req.display_name = *display_name;
        else if (!display_content)
            req.display_name = MediaConchLib::display_xml_name;
    }
    else if (f == MediaConchLib::format_MaXml)
        req.display_name = MediaConchLib::display_maxml_name;
    else if (f == MediaConchLib::format_Text)
        req.display_name = MediaConchLib::display_text_name;
    else if (f == MediaConchLib::format_Html)
        req.display_name = MediaConchLib::display_html_name;
    else if (f == MediaConchLib::format_JsTree)
        req.display_name = MediaConchLib::display_jstree_name;

    if (display_content)
        req.display_content = *display_content;

    if (options.find("verbosity") != options.end() && options.at("verbosity").length())
    {
        req.has_verbosity = true;
        const std::string& verbosity = options.at("verbosity");
        req.verbosity = strtol(verbosity.c_str(), NULL, 10);
    }

    http_client->start();
    if (http_client->send_request(req) < 0)
        return -1;

    std::string data = http_client->get_result();
    http_client->stop();
    if (!data.length())
        return -1;

    RESTAPI rest;
    RESTAPI::Checker_Report_Res *res = rest.parse_report_res(data);
    if (!res)
        return -1;

    if (!res->ok.report.length())
    {
        delete res;
        return -1;
    }

    result->report = res->ok.report;

    result->has_valid = res->ok.has_valid;
    result->valid = res->ok.valid;

    delete res;
    return 0;
}

//---------------------------------------------------------------------------
int DaemonClient::checker_validate(MediaConchLib::report report,
                                   const std::vector<std::string>& files,
                                   const std::vector<size_t>& policies_ids,
                                   const std::vector<std::string>& policies_contents,
                                   std::vector<MediaConchLib::Checker_ValidateRes*>& result)
{
    if (!http_client)
        return -1;

    // FILE
    RESTAPI::Checker_Validate_Req req;
    std::map<int, std::string> saved_ids;
    for (size_t i = 0; i < files.size(); ++i)
    {
        std::map<std::string, int>::iterator it = file_ids.find(files[i]);
        if (it == file_ids.end())
            continue;
        req.ids.push_back(it->second);
        saved_ids[it->second] = files[i];
    }

    // REPORT KIND
    if (report == MediaConchLib::report_MediaConch)
        req.report = RESTAPI::IMPLEMENTATION;
    else if (report == MediaConchLib::report_MediaVeraPdf)
        req.report = RESTAPI::VERAPDF;
    else if (report == MediaConchLib::report_MediaDpfManager)
        req.report = RESTAPI::DPFMANAGER;
    else if (policies_ids.size())
    {
        // POLICY
        req.report = RESTAPI::POLICY;
        for (size_t i = 0; i < policies_ids.size(); ++i)
            req.policies_ids.push_back(policies_ids[i]);
    }
    else if (policies_contents.size())
    {
        // POLICY
        req.report = RESTAPI::POLICY;
        for (size_t i = 0; i < policies_contents.size(); ++i)
            req.policies_contents.push_back(policies_contents[i]);
    }
    else
        return -1;

    http_client->start();
    if (http_client->send_request(req) < 0)
        return -1;

    std::string data = http_client->get_result();
    http_client->stop();
    if (!data.length())
        return -1;

    RESTAPI rest;
    RESTAPI::Checker_Validate_Res *res = rest.parse_validate_res(data);
    if (!res)
        return -1;

    if (!res->ok.size())
    {
        delete res;
        return -1;
    }

    for (size_t i = 0; i < res->ok.size(); ++i)
    {
        MediaConchLib::Checker_ValidateRes* v = new MediaConchLib::Checker_ValidateRes;
        v->file = saved_ids[res->ok[i]->id];
        v->valid = res->ok[i]->valid;
        result.push_back(v);
    }

    delete res;
    return 0;
}

//***************************************************************************
// Policy
//***************************************************************************
//---------------------------------------------------------------------------
int DaemonClient::policy_import(const std::string& memory, std::string& err)
{
}

//---------------------------------------------------------------------------
int DaemonClient::xslt_policy_create(int parent_id, std::string& err)
{
}

//---------------------------------------------------------------------------
int DaemonClient::policy_duplicate(int id, std::string& err)
{
}

//---------------------------------------------------------------------------
int DaemonClient::policy_remove(int id, std::string& err)
{
}

//---------------------------------------------------------------------------
int DaemonClient::policy_save(int id, std::string& err)
{
}

//---------------------------------------------------------------------------
int DaemonClient::policy_dump(int id, std::string& memory, std::string& err)
{
}

//---------------------------------------------------------------------------
int DaemonClient::policy_change_name(int id, const std::string& name, const std::string& description, std::string& err)
{
}

//---------------------------------------------------------------------------
Policy* DaemonClient::policy_get(int id, std::string& err)
{
}

//---------------------------------------------------------------------------
int DaemonClient::policy_get_name(int id, std::string& name, std::string& err)
{
}

//---------------------------------------------------------------------------
void DaemonClient::policy_get_policies(std::vector<std::pair<size_t, std::string> >&)
{
}

//---------------------------------------------------------------------------
size_t DaemonClient::policy_get_policies_count()
{
}

//---------------------------------------------------------------------------
void DaemonClient::policy_clear_policies()
{
}

//---------------------------------------------------------------------------
int DaemonClient::xslt_policy_create_from_file(const std::string& file)
{
    if (!http_client)
        return MediaConchLib::errorHttp_INIT;

    std::map<std::string, int>::iterator it = file_ids.find(file);
    if (it == file_ids.end())
        return MediaConchLib::errorHttp_MAX;
    int id = it->second;

    RESTAPI::XSLT_Policy_Create_From_File_Req req;
    req.id = id;

    int ret = http_client->start();
    if (ret < 0)
        return ret;

    ret = http_client->send_request(req);
    if (ret < 0)
        return ret;

    std::string data = http_client->get_result();
    http_client->stop();
    if (!data.length())
        return http_client->get_error();

    RESTAPI rest;
    RESTAPI::XSLT_Policy_Create_From_File_Res *res = rest.parse_xslt_policy_create_from_file_res(data);
    if (!res)
        return MediaConchLib::errorHttp_INVALID_DATA;

    int policy = res->policy_id;
    delete res;
    return policy;
}

//---------------------------------------------------------------------------
int DaemonClient::xslt_policy_rule_create(int policy_id, std::string& err)
{
}

//---------------------------------------------------------------------------
int DaemonClient::xslt_policy_rule_edit(int policy_id, int rule_id, const XsltPolicyRule *rule, std::string& err)
{
}

//---------------------------------------------------------------------------
int DaemonClient::xslt_policy_rule_duplicate(int policy_id, int rule_id, std::string& err)
{
}

//---------------------------------------------------------------------------
int DaemonClient::xslt_policy_rule_delete(int policy_id, int rule_id, std::string& err)
{
}

}
