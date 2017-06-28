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
#include "Reports.h"
#include "XsltPolicy.h"
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
int DaemonClient::init(std::string& err)
{
#ifdef HAVE_LIBEVENT
    http_client = new LibEventHttp;
#else
    err = "No HTTP library configured.";
    return -1;
#endif

    std::string server;
    int port = -1;
    mcl->get_daemon_address(server, port);

    if (server.length() != 0)
        http_client->set_address(server);
    if (port != -1)
        http_client->set_port(port);

    return http_client->init(err);
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
void DaemonClient::reset()
{
    if (http_client)
        http_client->reset_daemon_id();
}

//---------------------------------------------------------------------------
bool DaemonClient::is_init(std::string& err)
{
    if (!http_client)
    {
        err = "HTTP Library is not initialized.";
        return false;
    }

    return true;
}

//***************************************************************************
// MediaConch
//***************************************************************************

//---------------------------------------------------------------------------
#define COMMON_HTTP_REQ_RES(NAME, RES_ERR)                                                        \
    if (!is_init(err))                                                                            \
        return RES_ERR;                                                                           \
                                                                                                  \
    if (http_client->start(err) < 0)                                                              \
        return RES_ERR;                                                                           \
                                                                                                  \
    if (http_client->send_request(req, err) < 0)                                                  \
    {                                                                                             \
        if (http_client->get_http_code() == 0)                                                    \
            err = "Cannot connect to the daemon.";                                                \
        return RES_ERR;                                                                           \
    }                                                                                             \
                                                                                                  \
    std::string data;                                                                             \
    http_client->get_result(data);                                                                \
    http_client->stop();                                                                          \
                                                                                                  \
    if (!data.length())                                                                           \
    {                                                                                             \
        err = "HTTP answer is empty.";                                                            \
        return RES_ERR;                                                                           \
    }                                                                                             \
                                                                                                  \
    RESTAPI rest;                                                                                 \
    res = rest.parse_##NAME##_res(data, err);                                                     \
    if (!res)                                                                                     \
        return RES_ERR;                                                                           \

//***************************************************************************
// MediaConch
//***************************************************************************

//---------------------------------------------------------------------------
int DaemonClient::mediaconch_get_plugins(std::vector<std::string>& plugins, std::string& err)
{
    RESTAPI::MediaConch_Get_Plugins_Req req;
    RESTAPI::MediaConch_Get_Plugins_Res *res = NULL;

    COMMON_HTTP_REQ_RES(mediaconch_get_plugins, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
    {
        for (size_t i = 0; i < res->plugins.size(); ++i)
            plugins.push_back(res->plugins[i]);
        ret = 0;
    }

    delete res;
    return ret;
}

//---------------------------------------------------------------------------
int DaemonClient::mediaconch_watch_folder(const std::string& folder, const std::string& folder_reports,
                                          const std::vector<std::string>& plugins, const std::vector<std::string>& policies,
                                          long *in_user, bool recursive,
                                          const std::vector<std::pair<std::string,std::string> >& options,
                                          long& user_id, std::string& err)
{
    RESTAPI::MediaConch_Watch_Folder_Req  req;
    RESTAPI::MediaConch_Watch_Folder_Res *res = NULL;

    req.folder = folder;
    req.folder_reports = folder_reports;
    req.user = in_user;
    req.recursive = recursive;

    for (size_t i = 0; i < plugins.size(); ++i)
        req.plugins.push_back(plugins[i]);

    for (size_t i = 0; i < policies.size(); ++i)
        req.policies.push_back(policies[i]);


    for (size_t i = 0; i < options.size(); ++i)
        req.options.push_back(std::make_pair(options[i].first, options[i].second));

    COMMON_HTTP_REQ_RES(mediaconch_watch_folder, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
    {
        user_id = res->user;
        ret = 0;
    }

    delete res;
    return ret;
}

//---------------------------------------------------------------------------
int DaemonClient::mediaconch_edit_watch_folder(const std::string& folder, const std::string& folder_reports,
                                               std::string& err)
{
    RESTAPI::MediaConch_Edit_Watch_Folder_Req  req;
    RESTAPI::MediaConch_Edit_Watch_Folder_Res *res = NULL;

    req.folder = folder;
    req.folder_reports = folder_reports;

    COMMON_HTTP_REQ_RES(mediaconch_edit_watch_folder, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
        ret = 0;

    delete res;
    return ret;
}

//---------------------------------------------------------------------------
int DaemonClient::mediaconch_list_watch_folders(std::vector<std::string>& folders, std::string& err)
{
    RESTAPI::MediaConch_List_Watch_Folders_Req  req;
    RESTAPI::MediaConch_List_Watch_Folders_Res *res = NULL;

    COMMON_HTTP_REQ_RES(mediaconch_list_watch_folders, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
    {
        for (size_t i = 0; i < res->folders.size(); ++i)
            folders.push_back(res->folders[i]);
        ret = 0;
    }

    delete res;
    return ret;
}

//---------------------------------------------------------------------------
int DaemonClient::mediaconch_remove_watch_folder(const std::string& folder, std::string& err)
{
    RESTAPI::MediaConch_Remove_Watch_Folder_Req  req;
    RESTAPI::MediaConch_Remove_Watch_Folder_Res *res = NULL;

    req.folder = folder;

    COMMON_HTTP_REQ_RES(mediaconch_remove_watch_folder, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
        ret = 0;

    delete res;
    return ret;
}

//***************************************************************************
// Checker
//***************************************************************************

//---------------------------------------------------------------------------
int DaemonClient::checker_list(int user, std::vector<std::string>& vec, std::string& err)
{
    RESTAPI::Checker_List_Req  req;
    RESTAPI::Checker_List_Res *res = NULL;

    req.user = user;

    COMMON_HTTP_REQ_RES(checker_list, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
    {
        for (size_t i = 0; i < res->files.size(); ++i)
            vec.push_back(res->files[i]->file);
        ret = 0;
    }

    delete res;
    return ret;
}

//---------------------------------------------------------------------------
int DaemonClient::checker_file_from_id(int user, long id, std::string& filename, std::string& err)
{
    RESTAPI::Checker_File_From_Id_Req  req;
    RESTAPI::Checker_File_From_Id_Res *res = NULL;

    req.id = id;
    req.user = user;

    COMMON_HTTP_REQ_RES(checker_file_from_id, -1)

    if (res->nok)
        err = res->nok->error;
    else
        filename = res->file;
    delete res;
    return 0;
}

//---------------------------------------------------------------------------
long DaemonClient::checker_id_from_filename(int user, const std::string& filename,
                                            const std::vector<std::pair<std::string,std::string> >& options,
                                            std::string& err)
{
    RESTAPI::Checker_Id_From_Filename_Req req;
    RESTAPI::Checker_Id_From_Filename_Res *res = NULL;

    req.user = user;
    req.filename = filename;
    for (size_t i = 0; i < options.size(); ++i)
        req.options.push_back(std::make_pair(options[i].first, options[i].second));

    COMMON_HTTP_REQ_RES(checker_id_from_filename, -1)

    long id = -1;
    if (res->nok)
        err = res->nok->error;
    else
        id = res->id;

    delete res;
    return id;
}

//---------------------------------------------------------------------------
int DaemonClient::checker_file_information(int user, long id, MediaConchLib::Checker_FileInfo& info, std::string& err)
{
    RESTAPI::Checker_File_Information_Req  req;
    RESTAPI::Checker_File_Information_Res *res = NULL;
    req.user = user;
    req.id = id;

    COMMON_HTTP_REQ_RES(checker_file_information, -1)

    if (res->nok)
        err = res->nok->error;
    else
    {
        info.filename = res->filename;
        info.file_last_modification = res->file_last_modification;
        for (size_t i = 0; i < res->generated_id.size(); ++i)
            info.generated_id.push_back(res->generated_id[i]);
        info.source_id = res->source_id;
        info.generated_time = res->generated_time;
        info.generated_log = res->generated_log;
        info.generated_error_log = res->generated_error_log;
        info.analyzed = res->analyzed;
        info.has_error = res->has_error;
        info.error_log = res->error_log;
        for (size_t i = 0; i < res->options.size(); ++i)
            info.options.push_back(std::make_pair(res->options[i].first, res->options[i].second));
    }

    delete res;
    return 0;
}

//---------------------------------------------------------------------------
int DaemonClient::default_values_for_type(const std::string& type, std::vector<std::string>& values, std::string& err)
{
    RESTAPI::Default_Values_For_Type_Req req;
    RESTAPI::Default_Values_For_Type_Res *res = NULL;
    req.type = type;

    COMMON_HTTP_REQ_RES(default_values_for_type, -1)

    if (res->nok)
        err = res->nok->error;
    else
        for (size_t i = 0; i < res->values.size(); ++i)
            values.push_back(res->values[i]);

    delete res;
    return 0;
}

//---------------------------------------------------------------------------
int DaemonClient::checker_analyze(int user, const std::string& file, const std::vector<std::string>& plugins,
                                  const std::vector<std::pair<std::string,std::string> >& options,
                                  bool& registered, bool force_analyze, bool mil_analyze, long& file_id, std::string& err)
{
    RESTAPI::Checker_Analyze_Req  req;
    RESTAPI::Checker_Analyze_Arg  arg;
    RESTAPI::Checker_Analyze_Res *res = NULL;

    arg.user = user;
    arg.id = 0;
    arg.mil_analyze = mil_analyze;

    for (size_t i = 0; i < plugins.size(); ++i)
        arg.plugins.push_back(plugins[i]);

    for (size_t i = 0; i < options.size(); ++i)
        arg.options.push_back(std::make_pair(options[i].first, options[i].second));

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

    COMMON_HTTP_REQ_RES(checker_analyze, -1)

    if (res->nok.size() != 0)
    {
        if (res->nok[0])
            err = res->nok[0]->error;
        else
            err = "Internal error during analyze.";
        delete res;
        return -1;
    }
    else if (res->ok.size() != 1)
    {
        err = "Internal error during analyze.";
        delete res;
        return -1;
    }

    registered = !res->ok[0]->create;

    file_id = res->ok[0]->outId;
    delete res;
    return 0;
}

//---------------------------------------------------------------------------
int DaemonClient::checker_status(int user, long file_id, MediaConchLib::Checker_StatusRes& st_res, std::string& err)
{
    RESTAPI::Checker_Status_Req  req;
    RESTAPI::Checker_Status_Res *res = NULL;
    req.user = user;
    req.ids.push_back(file_id);

    COMMON_HTTP_REQ_RES(checker_status, -1)

    if (res->nok.size() != 0)
    {
        if (res->nok[0])
            err = res->nok[0]->error;
        else
            err = "Internal error during analyze.";
        delete res;
        return -1;
    }
    else if (res->ok.size() != 1)
    {
        err = "Internal error during analyze.";
        delete res;
        return -1;
    }

    RESTAPI::Checker_Status_Ok *ok = res->ok[0];

    st_res.finished = ok->finished;

    if (ok->finished && ok->tool)
    {
        st_res.tool = new int;
        if (*ok->tool == RESTAPI::VERAPDF)
            *st_res.tool = (int)MediaConchLib::report_MediaVeraPdf;
        else if (*ok->tool == RESTAPI::DPFMANAGER)
            *st_res.tool = (int)MediaConchLib::report_MediaDpfManager;
        else
            *st_res.tool = (int)MediaConchLib::report_MediaConch;
    }

    if (ok->generated_id.size())
    {
        for (size_t i = 0; i < ok->generated_id.size(); ++i)
            st_res.generated_id.push_back(ok->generated_id[i]);
    }

    if (ok->source_id >= 0)
        st_res.source_id = ok->source_id;

    if (ok->has_error)
    {
        st_res.has_error = true;
        st_res.error_log = ok->error_log;
    }

    if (!ok->finished)
    {
        st_res.percent = new double;
        if(ok->percent)
            *st_res.percent = *ok->percent;
        else
            *st_res.percent = 0.0;
    }

    delete res;
    return 0;
}

//---------------------------------------------------------------------------
int DaemonClient::checker_clear(int user, const std::vector<long>& files, std::string& err)
{
    RESTAPI::Checker_Clear_Req  req;
    RESTAPI::Checker_Clear_Res *res = NULL;
    req.user = user;
    for (size_t i = 0; i < files.size(); ++i)
        req.ids.push_back(files[i]);

    COMMON_HTTP_REQ_RES(checker_clear, -1)

    if (res->nok.size())
    {
        if (res->nok[0])
            err = res->nok[0]->error;
        else
            err = "Cannot clear the ids.";
        delete res;
        return -1;
    }
    else if (res->ok.size() != files.size())
    {
        err = "Internal error.";
        delete res;
        return -1;
    }

    delete res;
    return 0;
}

//---------------------------------------------------------------------------
int DaemonClient::checker_stop(int user, const std::vector<long>& files, std::string& err)
{
    RESTAPI::Checker_Stop_Req  req;
    RESTAPI::Checker_Stop_Res *res = NULL;
    req.user = user;
    for (size_t i = 0; i < files.size(); ++i)
        req.ids.push_back(files[i]);

    COMMON_HTTP_REQ_RES(checker_stop, -1)

    if (res->nok.size())
    {
        if (res->nok[0])
            err = res->nok[0]->error;
        else
            err = "Cannot stop the ids.";
        delete res;
        return -1;
    }
    else if (res->ok.size() != files.size())
    {
        err = "Internal error.";
        delete res;
        return -1;
    }

    delete res;
    return 0;
}

//---------------------------------------------------------------------------
int DaemonClient::checker_get_report(CheckerReport& cr, MediaConchLib::Checker_ReportRes* result, std::string& err)
{
    // FILE
    RESTAPI::Checker_Report_Req  req;
    RESTAPI::Checker_Report_Res *res = NULL;

    req.user = cr.user;

    for (size_t i = 0; i < cr.files.size(); ++i)
        req.ids.push_back(cr.files[i]);

    // REPORT KIND
    if (cr.report_set[MediaConchLib::report_MediaConch])
        req.reports.push_back(RESTAPI::IMPLEMENTATION);
    if (cr.report_set[MediaConchLib::report_MediaInfo])
        req.reports.push_back(RESTAPI::MEDIAINFO);
    if (cr.report_set[MediaConchLib::report_MediaTrace])
        req.reports.push_back(RESTAPI::MEDIATRACE);
    if (cr.report_set[MediaConchLib::report_MediaVeraPdf])
        req.reports.push_back(RESTAPI::VERAPDF);
    if (cr.report_set[MediaConchLib::report_MediaDpfManager])
        req.reports.push_back(RESTAPI::DPFMANAGER);

    // POLICY
    if (cr.policies_ids.size())
    {
        req.reports.push_back(RESTAPI::POLICY);
        for (size_t i = 0; i < cr.policies_ids.size(); ++i)
            req.policies_ids.push_back(cr.policies_ids[i]);
    }
    if (cr.policies_contents.size())
    {
        req.reports.push_back(RESTAPI::POLICY);
        for (size_t i = 0; i < cr.policies_contents.size(); ++i)
            req.policies_contents.push_back(cr.policies_contents[i]);
    }

    // FORMAT
    if (cr.format == MediaConchLib::format_Max)
    {
        if (cr.display_name)
            req.display_name = *cr.display_name;
        else if (!cr.display_content)
            req.display_name = MediaConchLib::display_xml_name;
    }
    else if (cr.format == MediaConchLib::format_MaXml)
        req.display_name = MediaConchLib::display_maxml_name;
    else if (cr.format == MediaConchLib::format_Text)
        req.display_name = MediaConchLib::display_text_name;
    else if (cr.format == MediaConchLib::format_Html)
        req.display_name = MediaConchLib::display_html_name;
    else if (cr.format == MediaConchLib::format_JsTree)
        req.display_name = MediaConchLib::display_jstree_name;
    else if (cr.format == MediaConchLib::format_Simple)
        req.display_name = MediaConchLib::display_simple_name;
    else if (cr.format == MediaConchLib::format_CSV)
        req.display_name = MediaConchLib::display_csv_name;

    if (cr.display_content)
        req.display_content = *cr.display_content;

    std::map<std::string, std::string>::const_iterator it = cr.options.begin();
    for (; it != cr.options.end(); ++it)
    {
        if (!it->first.size())
            continue;
        req.options[it->first.c_str()] = it->second;
    }

    COMMON_HTTP_REQ_RES(checker_report, -1)

    if (res->nok)
    {
        err = res->nok->error;
        delete res;
        return -1;
    }

    if (!res->ok)
    {
        err = "Checker_Report result not valid";
        delete res;
        return -1;
    }

    result->report = res->ok->report;
    result->has_valid = res->ok->has_valid;
    result->valid = res->ok->valid;

    delete res;
    return 0;
}

//---------------------------------------------------------------------------
int DaemonClient::checker_validate(int user, MediaConchLib::report report,
                                   const std::vector<long>& files,
                                   const std::vector<size_t>& policies_ids,
                                   const std::vector<std::string>& policies_contents,
                                   const std::map<std::string, std::string>& options,
                                   std::vector<MediaConchLib::Checker_ValidateRes*>& result,
                                   std::string& err)
{
    // FILE
    RESTAPI::Checker_Validate_Req  req;
    RESTAPI::Checker_Validate_Res *res = NULL;

    req.user = user;

    for (size_t i = 0; i < files.size(); ++i)
        req.ids.push_back(files[i]);

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

    std::map<std::string, std::string>::const_iterator it = options.begin();
    for (; it != options.end(); ++it)
    {
        if (!it->first.size())
            continue;
        req.options[it->first.c_str()] = it->second;
    }

    COMMON_HTTP_REQ_RES(checker_validate, -1)

    if (res->nok)
    {
        err = res->nok->error;
        delete res;
        return -1;
    }

    if (!res->ok.size())
    {
        err = "Checker_Validate result not valid";
        delete res;
        return -1;
    }

    for (size_t i = 0; i < res->ok.size(); ++i)
    {
        MediaConchLib::Checker_ValidateRes* v = new MediaConchLib::Checker_ValidateRes;
        v->id = res->ok[i]->id;
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
int DaemonClient::xslt_policy_create(int user, const std::string& type, int parent_id, std::string& err)
{
    RESTAPI::XSLT_Policy_Create_Req  req;
    RESTAPI::XSLT_Policy_Create_Res *res = NULL;

    req.user = user;
    req.parent_id = parent_id;
    req.type = type;

    COMMON_HTTP_REQ_RES(xslt_policy_create, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
        ret = res->id;

    delete res;
    return ret;
}

//---------------------------------------------------------------------------
int DaemonClient::policy_import(int user, const std::string& memory, std::string& err)
{
    RESTAPI::Policy_Import_Req  req;
    RESTAPI::Policy_Import_Res *res = NULL;

    req.xml = memory;
    req.user = user;

    COMMON_HTTP_REQ_RES(policy_import, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
        ret = res->id;

    delete res;
    return ret;
}

//---------------------------------------------------------------------------
int DaemonClient::policy_duplicate(int user, int id, int dst_policy_id, int *dst_user, bool must_be_public, std::string& err)
{
    RESTAPI::Policy_Duplicate_Req  req;
    RESTAPI::Policy_Duplicate_Res *res = NULL;

    req.id = id;
    req.dst_policy_id = dst_policy_id;
    req.user = user;
    req.dst_user = dst_user;
    req.must_be_public = must_be_public;

    COMMON_HTTP_REQ_RES(policy_duplicate, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
        ret = res->id;

    delete res;
    return ret;
}

//---------------------------------------------------------------------------
int DaemonClient::policy_move(int user, int id, int dst_policy_id, std::string& err)
{
    RESTAPI::Policy_Move_Req  req;
    RESTAPI::Policy_Move_Res *res = NULL;

    req.id = id;
    req.dst_policy_id = dst_policy_id;
    req.user = user;

    COMMON_HTTP_REQ_RES(policy_move, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
        ret = res->id;

    delete res;
    return ret;
}

//---------------------------------------------------------------------------
int DaemonClient::policy_remove(int user, int id, std::string& err)
{
    RESTAPI::Policy_Remove_Req  req;
    RESTAPI::Policy_Remove_Res *res = NULL;

    req.id = id;
    req.user = user;

    COMMON_HTTP_REQ_RES(policy_remove, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
        ret = 0;

    delete res;
    return ret;
}

//---------------------------------------------------------------------------
int DaemonClient::policy_save(int user, int id, std::string& err)
{
    RESTAPI::Policy_Save_Req  req;
    RESTAPI::Policy_Save_Res *res = NULL;

    req.id = id;
    req.user = user;

    COMMON_HTTP_REQ_RES(policy_save, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
        ret = 0;

    delete res;
    return ret;
}

//---------------------------------------------------------------------------
int DaemonClient::policy_dump(int user, int id, bool must_be_public, std::string& memory, std::string& err)
{
    RESTAPI::Policy_Dump_Req  req;
    RESTAPI::Policy_Dump_Res *res = NULL;

    req.id = id;
    req.user = user;
    req.must_be_public = must_be_public;

    COMMON_HTTP_REQ_RES(policy_dump, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
    {
        ret = 0;
        memory = res->xml;
    }

    delete res;
    return ret;
}

//---------------------------------------------------------------------------
int DaemonClient::policy_change_info(int user, int id, const std::string& name, const std::string& description,
                                     const std::string& license, std::string& err)
{
    RESTAPI::Policy_Change_Info_Req  req;
    RESTAPI::Policy_Change_Info_Res *res = NULL;

    req.id = id;
    req.name = name;
    req.description = description;
    req.user = user;
    req.license = license;

    COMMON_HTTP_REQ_RES(policy_change_info, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
        ret = 0;

    delete res;
    return ret;
}

//---------------------------------------------------------------------------
int DaemonClient::policy_change_type(int user, int id, const std::string& type, std::string& err)
{
    RESTAPI::Policy_Change_Type_Req  req;
    RESTAPI::Policy_Change_Type_Res *res = NULL;

    req.id = id;
    req.type = type;
    req.user = user;

    COMMON_HTTP_REQ_RES(policy_change_type, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
        ret = 0;

    delete res;
    return ret;
}

//---------------------------------------------------------------------------
int DaemonClient::policy_change_is_public(int user, int id, bool is_public, std::string& err)
{
    RESTAPI::Policy_Change_Is_Public_Req  req;
    RESTAPI::Policy_Change_Is_Public_Res *res = NULL;

    req.id = id;
    req.is_public = is_public;
    req.user = user;

    COMMON_HTTP_REQ_RES(policy_change_is_public, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
        ret = 0;

    delete res;
    return ret;
}

//---------------------------------------------------------------------------
int DaemonClient::policy_get(int user, int id, const std::string& format, bool must_be_public,
                             MediaConchLib::Get_Policy& policy, std::string& err)
{
    RESTAPI::Policy_Get_Req  req;
    RESTAPI::Policy_Get_Res *res = NULL;

    req.id = id;
    req.user = user;
    req.format = format;
    req.must_be_public = must_be_public;

    COMMON_HTTP_REQ_RES(policy_get, -1)

    if (res->nok)
    {
        err = res->nok->error;
        delete res;
        return -1;
    }
    else if (res->policy)
    {
        policy.format = format;
        policy.policy = new MediaConchLib::Policy_Policy(res->policy);
    }
    else if (res->policyTree.size())
    {
        policy.jstree = new std::string;
        *policy.jstree = res->policyTree;
        policy.format = format;
    }

    delete res;
    return 0;
}

//---------------------------------------------------------------------------
int DaemonClient::policy_get_name(int user, int id, std::string& name, std::string& err)
{
    RESTAPI::Policy_Get_Name_Req  req;
    RESTAPI::Policy_Get_Name_Res *res = NULL;

    req.id = id;
    req.user = user;

    COMMON_HTTP_REQ_RES(policy_get_name, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
    {
        ret = 0;
        name = res->name;
    }

    delete res;
    return ret;
}

//---------------------------------------------------------------------------
int DaemonClient::policy_get_policies(int user, const std::vector<int>& ids, const std::string& format,
                                      MediaConchLib::Get_Policies& policies, std::string& err)
{
    RESTAPI::Policy_Get_Policies_Req  req;
    RESTAPI::Policy_Get_Policies_Res *res = NULL;

    req.user = user;
    req.ids = ids;
    req.format = format;

    COMMON_HTTP_REQ_RES(policy_get_policies, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
    {
        policies.format = format;
        if (format == "JSTREE")
        {
            policies.jstree = new std::string;
            *policies.jstree = res->policiesTree;
        }
        else
        {
            policies.policies = new std::vector<MediaConchLib::Policy_Policy*>;
            for (size_t i = 0; i < res->policies.size(); ++i)
            {
                if (!res->policies[i])
                    continue;

                MediaConchLib::Policy_Policy* p = new MediaConchLib::Policy_Policy(res->policies[i]);
                policies.policies->push_back(p);
            }
        }

        ret = 0;
    }

    delete res;
    return ret;
}

//---------------------------------------------------------------------------
int DaemonClient::policy_get_public_policies(std::vector<MediaConchLib::Policy_Public_Policy*>& policies, std::string& err)
{
    RESTAPI::Policy_Get_Public_Policies_Req  req;
    RESTAPI::Policy_Get_Public_Policies_Res *res = NULL;

    COMMON_HTTP_REQ_RES(policy_get_public_policies, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
    {
        for (size_t i = 0; i < res->policies.size(); ++i)
        {
            if (!res->policies[i])
                continue;

            MediaConchLib::Policy_Public_Policy* p = new MediaConchLib::Policy_Public_Policy;
            p->id = res->policies[i]->id;
            p->user = res->policies[i]->user;
            p->name = res->policies[i]->name;
            p->description = res->policies[i]->description;
            policies.push_back(p);
        }
        ret = 0;
    }

    delete res;
    return ret;
}

//---------------------------------------------------------------------------
int DaemonClient::policy_get_policies_names_list(int user, std::vector<std::pair<int, std::string> >& policies,
                                                 std::string& err)
{
    RESTAPI::Policy_Get_Policies_Names_List_Req  req;
    RESTAPI::Policy_Get_Policies_Names_List_Res *res = NULL;

    req.user = user;

    COMMON_HTTP_REQ_RES(policy_get_policies_names_list, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
    {
        policies = res->policies;
        ret = 0;
    }

    delete res;
    return ret;
}

//---------------------------------------------------------------------------
size_t DaemonClient::policy_get_policies_count(int user, std::string& err)
{
    RESTAPI::Policy_Get_Policies_Count_Req  req;
    RESTAPI::Policy_Get_Policies_Count_Res *res = NULL;

    req.user = user;

    COMMON_HTTP_REQ_RES(policy_get_policies_count, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
        ret = res->size;

    delete res;
    return (size_t)ret;
}

//---------------------------------------------------------------------------
int DaemonClient::policy_clear_policies(int user, std::string& err)
{
    RESTAPI::Policy_Clear_Policies_Req  req;
    RESTAPI::Policy_Clear_Policies_Res *res = NULL;

    req.user = user;

    COMMON_HTTP_REQ_RES(policy_clear_policies, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
        ret = 0;

    delete res;
    return ret;
}

//---------------------------------------------------------------------------
int DaemonClient::xslt_policy_create_from_file(int user, long file_id, std::string& err)
{
    RESTAPI::XSLT_Policy_Create_From_File_Req  req;
    RESTAPI::XSLT_Policy_Create_From_File_Res *res = NULL;

    req.id = file_id;
    req.user = user;

    COMMON_HTTP_REQ_RES(xslt_policy_create_from_file, -1)

    int policy_id = -1;
    if (res->nok)
        err = res->nok->error;
    else
        policy_id = res->policy_id;
    delete res;
    return policy_id;
}

//---------------------------------------------------------------------------
int DaemonClient::xslt_policy_rule_create(int user, int policy_id, std::string& err)
{
    RESTAPI::XSLT_Policy_Rule_Create_Req  req;
    RESTAPI::XSLT_Policy_Rule_Create_Res *res = NULL;

    req.policy_id = policy_id;
    req.user = user;

    COMMON_HTTP_REQ_RES(xslt_policy_rule_create, -1)

    int rule = -1;
    if (res->nok)
        err = res->nok->error;
    else
        rule = res->id;
    delete res;
    return rule;
}

//---------------------------------------------------------------------------
XsltPolicyRule *DaemonClient::xslt_policy_rule_get(int user, int policy_id, int id, std::string& err)
{
    RESTAPI::XSLT_Policy_Rule_Get_Req  req;
    RESTAPI::XSLT_Policy_Rule_Get_Res *res = NULL;

    req.policy_id = policy_id;
    req.id = id;
    req.user = user;

    COMMON_HTTP_REQ_RES(xslt_policy_rule_get, NULL)

    XsltPolicyRule *rule = NULL;
    if (res->nok)
        err = res->nok->error;
    else
    {
        rule = new XsltPolicyRule;
        rule->id = res->rule.id;
        rule->node_name = res->rule.name;
        rule->track_type = res->rule.tracktype;
        rule->field = res->rule.field;
        rule->scope = res->rule.scope;
        rule->occurrence = res->rule.occurrence;
        rule->ope = rule->ope;
        rule->value = res->rule.value;
    }

    delete res;
    return rule;
}

//---------------------------------------------------------------------------
int DaemonClient::xslt_policy_rule_edit(int user, int policy_id, int rule_id, const XsltPolicyRule *rule, std::string& err)
{
    RESTAPI::XSLT_Policy_Rule_Edit_Req  req;
    RESTAPI::XSLT_Policy_Rule_Edit_Res *res = NULL;

    req.policy_id = policy_id;

    req.rule.id = rule_id;
    req.rule.name = rule->node_name;
    req.rule.tracktype = rule->track_type;
    req.rule.field = rule->field;
    req.rule.scope = rule->scope;
    req.rule.occurrence = rule->occurrence;
    req.rule.ope = rule->ope;
    req.rule.value = rule->value;
    req.user = user;

    COMMON_HTTP_REQ_RES(xslt_policy_rule_edit, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
        ret = 0;
    delete res;
    return ret;
}

//---------------------------------------------------------------------------
int DaemonClient::xslt_policy_rule_duplicate(int user, int policy_id, int rule_id, int dst_policy_id, std::string& err)
{
    RESTAPI::XSLT_Policy_Rule_Duplicate_Req  req;
    RESTAPI::XSLT_Policy_Rule_Duplicate_Res *res = NULL;

    req.policy_id = policy_id;
    req.dst_policy_id = dst_policy_id;
    req.id = rule_id;
    req.user = user;

    COMMON_HTTP_REQ_RES(xslt_policy_rule_duplicate, -1)

    int rule = -1;
    if (res->nok)
        err = res->nok->error;
    else
        rule = res->id;

    delete res;
    return rule;
}

//---------------------------------------------------------------------------
int DaemonClient::xslt_policy_rule_move(int user, int policy_id, int rule_id, int dst_policy_id, std::string& err)
{
    RESTAPI::XSLT_Policy_Rule_Move_Req  req;
    RESTAPI::XSLT_Policy_Rule_Move_Res *res = NULL;

    req.policy_id = policy_id;
    req.dst_policy_id = dst_policy_id;
    req.id = rule_id;
    req.user = user;

    COMMON_HTTP_REQ_RES(xslt_policy_rule_move, -1)

    int rule = -1;
    if (res->nok)
        err = res->nok->error;
    else
        rule = res->id;

    delete res;
    return rule;
}

//---------------------------------------------------------------------------
int DaemonClient::xslt_policy_rule_delete(int user, int policy_id, int rule_id, std::string& err)
{
    RESTAPI::XSLT_Policy_Rule_Delete_Req  req;
    RESTAPI::XSLT_Policy_Rule_Delete_Res *res = NULL;

    req.policy_id = policy_id;
    req.id = rule_id;
    req.user = user;

    COMMON_HTTP_REQ_RES(xslt_policy_rule_delete, -1)

    int ret = -1;
    if (res->nok)
        err = res->nok->error;
    else
        ret = 0;

    delete res;
    return ret;
}

}
