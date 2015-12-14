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
#include "Core.h"
#include "Http.h"
#include "LibEventHttp.h"
#include "REST_API.h"

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// DaemonClient
//***************************************************************************

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
DaemonClient::DaemonClient(Core* c) : core(c), http_client(NULL)
{
}

//---------------------------------------------------------------------------
DaemonClient::~DaemonClient()
{
}

//---------------------------------------------------------------------------
int DaemonClient::init()
{
#ifndef HAVE_LIBEVENT
    return -1;
#endif

    http_client = new LibEventHttp;
    std::string server;
    int port = -1;
    core->get_daemon_address(server, port);

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

//---------------------------------------------------------------------------
int DaemonClient::analyze(const std::string& file, bool& registered)
{
    if (!http_client)
        return -1;

    RESTAPI::Analyze_Req req;
    RESTAPI::Analyze_Arg arg;

    arg.id = 0;
    arg.file = file;
    req.args.push_back(arg);

    if (http_client->start() < 0 || http_client->send_request(req) < 0)
        return -1;

    std::string data = http_client->get_result();
    http_client->stop();
    if (!data.length())
        return -1;

    RESTAPI rest;
    RESTAPI::Analyze_Res *res = rest.parse_analyze_res(data);
    if (!res || res->ok.size() != 1)
        return -1;

    registered = !res->ok[0]->create;

    file_ids[file] = res->ok[0]->outId;
    return 0;
}

//---------------------------------------------------------------------------
bool DaemonClient::is_done(const std::string& file, double& done)
{
    if (!http_client)
        return false;

    RESTAPI::Status_Req req;
    std::map<std::string, int>::iterator it = file_ids.find(file);
    if (it == file_ids.end())
        return false;
    req.ids.push_back(it->second);

    http_client->start();
    if (http_client->send_request(req) < 0)
        return false;

    std::string data = http_client->get_result();
    http_client->stop();
    if (!data.length())
        return false;

    RESTAPI rest;
    RESTAPI::Status_Res *res = rest.parse_status_res(data);
    if (!res || res->ok.size() != 1)
        return false;

    RESTAPI::Status_Ok *ok = res->ok[0];

    if (ok->finished)
        return true;

    if (ok->has_percent)
        done = ok->done;
    else
        done = 0.0;
    return false;
}

//---------------------------------------------------------------------------
int DaemonClient::get_report(const std::bitset<MediaConchLib::report_Max>& report_set,
                             MediaConchLib::format f, const std::vector<std::string>& files,
                             const std::vector<std::string>& policies_names,
                             const std::vector<std::string>& policies_contents,
                             MediaConchLib::ReportRes* result,
                             const std::string* display_name,
                             const std::string* display_content)
{
    if (!http_client)
        return -1;

    // FILE
    RESTAPI::Report_Req req;
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

    // POLICY
    if (policies_names.size())
    {
        req.reports.push_back(RESTAPI::POLICY);
        for (size_t i = 0; i < policies_names.size(); ++i)
            req.policies_names.push_back(policies_names[i]);
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

    http_client->start();
    if (http_client->send_request(req) < 0)
        return -1;

    std::string data = http_client->get_result();
    http_client->stop();
    if (!data.length())
        return -1;

    RESTAPI rest;
    RESTAPI::Report_Res *res = rest.parse_report_res(data);
    if (!res || res->ok.size() != 1)
        return -1;

    result->report = res->ok[0]->report;
    delete res;
    return 0;
}

//---------------------------------------------------------------------------
bool DaemonClient::validate_policy(const std::string& file, const std::string& policy,
                                   MediaConchLib::ReportRes* result,
                                   const std::string* display_name, const std::string* display_content)
{
    if (!http_client)
        return false;

    std::bitset<MediaConchLib::report_Max> report_set;

    std::vector<std::string> files;
    files.push_back(file);

    std::vector<std::string> policies_names;
    std::vector<std::string> policies_contents;
    policies_contents.push_back(policy);

    if (get_report(report_set, MediaConchLib::format_Xml, files,
                   policies_names, policies_contents,
                   result,
                   display_name, display_content) < 0)
        return false;
    return true;
}

}
