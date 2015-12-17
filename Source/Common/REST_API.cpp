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

const std::string RESTAPI::API_VERSION = "1";

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
RESTAPI::Analyze_Res::~Analyze_Res()
{
    for (size_t i = 0; i < ok.size(); ++i)
        delete ok[i];
    ok.clear();

    for (size_t i = 0; i < nok.size(); ++i)
        delete nok[i];
    nok.clear();
}

//---------------------------------------------------------------------------
RESTAPI::Status_Res::~Status_Res()
{
    for (size_t i = 0; i < ok.size(); ++i)
        delete ok[i];
    ok.clear();

    for (size_t i = 0; i < nok.size(); ++i)
        delete nok[i];
    nok.clear();
}

//---------------------------------------------------------------------------
RESTAPI::Report_Res::~Report_Res()
{
    for (size_t i = 0; i < nok.size(); ++i)
        delete nok[i];
    nok.clear();
}

//---------------------------------------------------------------------------
RESTAPI::Retry_Res::~Retry_Res()
{
    for (size_t i = 0; i < nok.size(); ++i)
        delete nok[i];
    nok.clear();
}

//---------------------------------------------------------------------------
RESTAPI::Clear_Res::~Clear_Res()
{
    for (size_t i = 0; i < nok.size(); ++i)
        delete nok[i];
    nok.clear();
}

//***************************************************************************
// Serialize: Request
//***************************************************************************

//---------------------------------------------------------------------------
std::string RESTAPI::serialize_analyze_req(Analyze_Req& req)
{
    Container::Value v, child;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj.push_back(std::make_pair("args", serialize_analyze_args(req.args)));

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj.push_back(std::make_pair("ANALYZE", child));

    std::string ret = model->serialize(v);
    if (!ret.length())
        error = model->get_error();
    return ret;
}

//---------------------------------------------------------------------------
std::string RESTAPI::serialize_status_req(Status_Req& req)
{
    //URI
    std::stringstream ss;

    if (req.ids.size() > 0)
        ss << "?";
    for (size_t i = 0; i < req.ids.size(); ++i)
    {
        if (i)
            ss << "&";
        ss << "id=" << req.ids[i];
    }
    return ss.str();
}

//---------------------------------------------------------------------------
std::string RESTAPI::serialize_report_req(Report_Req& req)
{
    Container::Value v, child;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj.push_back(std::make_pair("ids", serialize_ids(req.ids)));
    child.obj.push_back(std::make_pair("reports", serialize_report_reports(req.reports)));

    if (req.policies_names.size())
        child.obj.push_back(std::make_pair("policies_names", serialize_report_arr_str(req.policies_names)));
    if (req.policies_contents.size())
        child.obj.push_back(std::make_pair("policies_contents", serialize_report_arr_str(req.policies_contents)));

    if (req.display_name.length())
        child.obj.push_back(std::make_pair("display_name", serialize_report_string(req.display_name)));
    if (req.display_content.length())
        child.obj.push_back(std::make_pair("display_content", serialize_report_string(req.display_content)));

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj.push_back(std::make_pair("REPORT", child));

    std::string ret = model->serialize(v);
    if (!ret.length())
        error = model->get_error();
    return ret;
}

//---------------------------------------------------------------------------
std::string RESTAPI::serialize_retry_req(Retry_Req& req)
{
    Container::Value v, child;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj.push_back(std::make_pair("ids", serialize_ids(req.ids)));

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj.push_back(std::make_pair("RETRY", child));

    std::string ret = model->serialize(v);
    if (!ret.length())
        error = model->get_error();
    return ret;
}

//---------------------------------------------------------------------------
std::string RESTAPI::serialize_clear_req(Clear_Req& req)
{
    //URI
    std::stringstream ss;

    if (req.ids.size() > 0)
        ss << "?";
    for (size_t i = 0; i < req.ids.size(); ++i)
    {
        if (i)
            ss << "&";
        ss << "id=" << req.ids[i];
    }
    return ss.str();
}

//***************************************************************************
// Serialize: Result
//***************************************************************************

//---------------------------------------------------------------------------
std::string RESTAPI::serialize_analyze_res(Analyze_Res& res)
{
    Container::Value v, child, nok;

    nok.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < res.nok.size(); ++i)
        nok.array.push_back(serialize_generic_nok(res.nok[i]->id, res.nok[i]->error));

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj.push_back(std::make_pair("ok", serialize_analyze_oks(res.ok)));
    child.obj.push_back(std::make_pair("nok", nok));

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj.push_back(std::make_pair("ANALYZE_RESULT", child));

    std::string ret = model->serialize(v);
    if (!ret.length())
        error = model->get_error();
    return ret;
}

//---------------------------------------------------------------------------
std::string RESTAPI::serialize_status_res(Status_Res& res)
{
    Container::Value v, child, nok;

    nok.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < res.nok.size(); ++i)
        nok.array.push_back(serialize_generic_nok(res.nok[i]->id, res.nok[i]->error));

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj.push_back(std::make_pair("ok", serialize_status_oks(res.ok)));
    child.obj.push_back(std::make_pair("nok", nok));

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj.push_back(std::make_pair("STATUS_RESULT", child));

    std::string ret = model->serialize(v);
    if (!ret.length())
        error = model->get_error();
    return ret;
}

//---------------------------------------------------------------------------
std::string RESTAPI::serialize_report_res(Report_Res& res)
{
    Container::Value v, child, nok;

    nok.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < res.nok.size(); ++i)
        nok.array.push_back(serialize_generic_nok(res.nok[i]->id, res.nok[i]->error));

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj.push_back(std::make_pair("ok", serialize_report_ok(res.ok)));
    child.obj.push_back(std::make_pair("nok", nok));

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj.push_back(std::make_pair("REPORT_RESULT", child));

    std::string ret = model->serialize(v);
    if (!ret.length())
        error = model->get_error();
    return ret;
}

//---------------------------------------------------------------------------
std::string RESTAPI::serialize_retry_res(Retry_Res& res)
{
    Container::Value v, child, nok;

    nok.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < res.nok.size(); ++i)
        nok.array.push_back(serialize_generic_nok(res.nok[i]->id, res.nok[i]->error));

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj.push_back(std::make_pair("ok", serialize_ids(res.ok)));
    child.obj.push_back(std::make_pair("nok", nok));

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj.push_back(std::make_pair("RETRY_RESULT", child));

    std::string ret = model->serialize(v);
    if (!ret.length())
        error = model->get_error();
    return ret;
}

//---------------------------------------------------------------------------
std::string RESTAPI::serialize_clear_res(Clear_Res& res)
{
    Container::Value v, child, nok;

    nok.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < res.nok.size(); ++i)
        nok.array.push_back(serialize_generic_nok(res.nok[i]->id, res.nok[i]->error));

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj.push_back(std::make_pair("ok", serialize_ids(res.ok)));
    child.obj.push_back(std::make_pair("nok", nok));

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj.push_back(std::make_pair("CLEAR_RESULT", child));

    std::string ret = model->serialize(v);
    if (!ret.length())
        error = model->get_error();
    return ret;
}

//***************************************************************************
// Serialize: Result
//***************************************************************************

//---------------------------------------------------------------------------
RESTAPI::Analyze_Req *RESTAPI::parse_analyze_req(std::string data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "ANALYZE");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;
    Analyze_Req *req = new Analyze_Req;

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
RESTAPI::Status_Req *RESTAPI::parse_status_req(std::string data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "STATUS");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Status_Req *req = new Status_Req;

    Container::Value *ids;
    ids = model->get_value_by_key(*child, "ids");

    if (!ids || ids->type != Container::Value::CONTAINER_TYPE_ARRAY)
    {
        delete req;
        return NULL;
    }

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
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Report_Req *RESTAPI::parse_report_req(std::string data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "REPORT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *ids, *reports, *policies_names, *policies_contents, *display_name, *display_content;
    ids = model->get_value_by_key(*child, "ids");
    reports = model->get_value_by_key(*child, "reports");
    policies_names = model->get_value_by_key(*child, "policies_names");
    policies_contents = model->get_value_by_key(*child, "policies_contents");
    display_name = model->get_value_by_key(*child, "display_name");
    display_content = model->get_value_by_key(*child, "display_content");

    if (!ids || !reports || ids->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return NULL;

    Report_Req *req = new Report_Req;
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

    if (policies_names && policies_names->type == Container::Value::CONTAINER_TYPE_ARRAY)
    {
        for (size_t i = 0; i < policies_names->array.size(); ++i)
            if (policies_names->array[i].type == Container::Value::CONTAINER_TYPE_STRING)
                req->policies_names.push_back(policies_names->array[i].s);
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

    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Retry_Req *RESTAPI::parse_retry_req(std::string data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "RETRY");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *ids;
    ids = model->get_value_by_key(*child, "ids");

    if (!ids || ids->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return NULL;

    Retry_Req *req = new Retry_Req;
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
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Clear_Req *RESTAPI::parse_clear_req(std::string data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CLEAR");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *ids;
    ids = model->get_value_by_key(*child, "ids");

    if (!ids || ids->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return NULL;

    Clear_Req *req = new Clear_Req;
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
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Analyze_Req *RESTAPI::parse_uri_analyze_req(const std::string&)
{
    Analyze_Req *req = new Analyze_Req;
    //TODO
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Status_Req *RESTAPI::parse_uri_status_req(const std::string& uri)
{
    Status_Req *req = new Status_Req;

    size_t end, start = 0;
    while (1)
    {
        end = uri.find("=", start);
        if (end == std::string::npos || uri.substr(start, end - start) != "id")
            goto finish;
        start = end + 1;

        end = uri.find("&", start);

        std::string id = uri.substr(start, end - start);
        if (!id.length())
            goto finish;

        req->ids.push_back(strtoll(id.c_str(), NULL, 10));
        if (end == std::string::npos)
            goto finish;
        start = end + 1;
    }

finish:
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Report_Req *RESTAPI::parse_uri_report_req(const std::string&)
{
    Report_Req *req = new Report_Req;
    //TODO
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Retry_Req *RESTAPI::parse_uri_retry_req(const std::string&)
{
    Retry_Req *req = new Retry_Req;
    //TODO
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Clear_Req *RESTAPI::parse_uri_clear_req(const std::string& uri)
{
    Clear_Req *req = new Clear_Req;
    size_t end, start = 0;
    while (1)
    {
        end = uri.find("=", start);
        if (end == std::string::npos || uri.substr(start, end - start) != "id")
            goto finish;
        start = end + 1;

        end = uri.find("&", start);

        std::string id = uri.substr(start, end - start);
        if (!id.length())
            goto finish;

        req->ids.push_back(strtoll(id.c_str(), NULL, 10));
        if (end == std::string::npos)
            goto finish;
        start = end + 1;
    }

finish:
    return req;
}

//---------------------------------------------------------------------------
RESTAPI::Analyze_Res *RESTAPI::parse_analyze_res(std::string data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "ANALYZE_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *ok, *nok;
    ok = model->get_value_by_key(*child, "ok");
    nok = model->get_value_by_key(*child, "nok");

    Analyze_Res *res = new Analyze_Res;

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
        Analyze_Nok *tmp = new Analyze_Nok;

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
RESTAPI::Status_Res *RESTAPI::parse_status_res(std::string data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "STATUS_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *ok, *nok;
    ok = model->get_value_by_key(*child, "ok");
    nok = model->get_value_by_key(*child, "nok");

    Status_Res *res = new Status_Res;
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
        Status_Nok *tmp = new Status_Nok;

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
RESTAPI::Report_Res *RESTAPI::parse_report_res(std::string data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "REPORT_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *ok, *nok;
    ok = model->get_value_by_key(*child, "ok");
    nok = model->get_value_by_key(*child, "nok");

    Report_Res *res = new Report_Res;
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
        Report_Nok *tmp = new Report_Nok;

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
RESTAPI::Retry_Res *RESTAPI::parse_retry_res(std::string data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "RETRY_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *ok, *nok;
    ok = model->get_value_by_key(*child, "ok");
    nok = model->get_value_by_key(*child, "nok");

    Retry_Res *res = new Retry_Res;
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
        Retry_Nok *tmp = new Retry_Nok;

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
RESTAPI::Clear_Res *RESTAPI::parse_clear_res(std::string data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CLEAR_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *ok, *nok;
    ok = model->get_value_by_key(*child, "ok");
    nok = model->get_value_by_key(*child, "nok");

    Clear_Res *res = new Clear_Res;
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
        Clear_Nok *tmp = new Clear_Nok;

        if (parse_generic_nok(&nok->array[i], tmp->id, tmp->error))
        {
            delete res;
            return NULL;
        }
        res->nok.push_back(tmp);
    }
    return res;
}

//***************************************************************************
// HELPER
//***************************************************************************

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_analyze_args(std::vector<Analyze_Arg>& args)
{
    Container::Value args_val;

    args_val.type = Container::Value::CONTAINER_TYPE_ARRAY;

    for (size_t i = 0; i < args.size(); ++i)
    {
        Container::Value arg, file, id;
        arg.type = Container::Value::CONTAINER_TYPE_OBJECT;

        file.type = Container::Value::CONTAINER_TYPE_STRING;
        file.s = args[i].file;
        arg.obj.push_back(std::make_pair("file", file));

        id.type = Container::Value::CONTAINER_TYPE_INTEGER;
        id.l = args[i].id;
        arg.obj.push_back(std::make_pair("id", id));
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
Container::Value RESTAPI::serialize_generic_nok(int id, Reason error)
{
    Container::Value nok, id_v, error_v;

    nok.type = Container::Value::CONTAINER_TYPE_OBJECT;

    id_v.type = Container::Value::CONTAINER_TYPE_INTEGER;
    id_v.l = id;
    nok.obj.push_back(std::make_pair("id", id_v));

    error_v.type = Container::Value::CONTAINER_TYPE_STRING;
    error_v.s = get_Reason_string(error);
    nok.obj.push_back(std::make_pair("error", error_v));
    return nok;
}

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_analyze_oks(std::vector<Analyze_Ok*>& array)
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
        v.obj.push_back(std::make_pair("inId", in));

        out.type = Container::Value::CONTAINER_TYPE_INTEGER;
        out.l = array[i]->outId;
        v.obj.push_back(std::make_pair("outId", out));

        create.type = Container::Value::CONTAINER_TYPE_BOOL;
        create.b = array[i]->create;
        v.obj.push_back(std::make_pair("create", create));

        ok.array.push_back(v);
    }

    return ok;
}

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_status_oks(std::vector<Status_Ok*>& array)
{
    Container::Value ok;

    ok.type = Container::Value::CONTAINER_TYPE_ARRAY;

    for (size_t i = 0; i < array.size(); ++i)
    {
        if (!array[i])
            continue;

        Container::Value v, id, finished, done;

        v.type = Container::Value::CONTAINER_TYPE_OBJECT;

        id.type = Container::Value::CONTAINER_TYPE_INTEGER;
        id.l = array[i]->id;
        v.obj.push_back(std::make_pair("id", id));

        finished.type = Container::Value::CONTAINER_TYPE_BOOL;
        finished.b = array[i]->finished;
        v.obj.push_back(std::make_pair("finished", finished));

        if (array[i]->has_percent)
        {
            done.type = Container::Value::CONTAINER_TYPE_REAL;
            done.d = array[i]->done;
            v.obj.push_back(std::make_pair("done", done));
        }

        ok.array.push_back(v);
    }

    return ok;
}

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_report_ok(Report_Ok& obj)
{
    Container::Value ok;

    ok.type = Container::Value::CONTAINER_TYPE_OBJECT;
    Container::Value report, valid;

    if (obj.report.length() > 0)
    {
        report.type = Container::Value::CONTAINER_TYPE_STRING;
        report.s = obj.report;
        ok.obj.push_back(std::make_pair("report", report));
    }

    if (obj.has_valid)
    {
        valid.type = Container::Value::CONTAINER_TYPE_BOOL;
        valid.b = obj.valid;
        ok.obj.push_back(std::make_pair("valid", valid));
    }
    return ok;
}

//---------------------------------------------------------------------------
int RESTAPI::parse_analyze_arg(Container::Value *v, std::vector<Analyze_Arg>& args)
{
    if (v->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return -1;

    for (size_t i = 0; i < v->array.size(); ++i)
    {
        Container::Value *obj = &v->array[i];

        if (obj->type != Container::Value::CONTAINER_TYPE_OBJECT)
            return -1;

        Container::Value *file, *id;

        file = model->get_value_by_key(*obj, "file");
        id = model->get_value_by_key(*obj, "id");

        if (!file || !id || file->type != Container::Value::CONTAINER_TYPE_STRING ||
            id->type != Container::Value::CONTAINER_TYPE_INTEGER)
            return -1;

        Analyze_Arg arg;
        arg.file = file->s;
        arg.id = id->l;
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
int RESTAPI::parse_analyze_ok(Container::Value *v, std::vector<Analyze_Ok*>& oks)
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

        Analyze_Ok *ok = new Analyze_Ok;
        ok->inId = inId->l;
        ok->outId = outId->l;
        if (create && create->type == Container::Value::CONTAINER_TYPE_BOOL)
            ok->create = create->b;

        oks.push_back(ok);
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::parse_status_ok(Container::Value *v, std::vector<Status_Ok*>& oks)
{
    if (v->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return -1;

    for (size_t i = 0; i < v->array.size(); ++i)
    {
        Container::Value *obj = &v->array[i];

        if (obj->type != Container::Value::CONTAINER_TYPE_OBJECT)
            return -1;

        Container::Value *id, *finished, *done;

        id = model->get_value_by_key(*obj, "id");
        finished = model->get_value_by_key(*obj, "finished");
        done = model->get_value_by_key(*obj, "done");

        if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER ||
            !finished || finished->type != Container::Value::CONTAINER_TYPE_BOOL)
            return -1;

        Status_Ok *ok = new Status_Ok;
        ok->id = id->l;
        ok->finished = finished->b;

        if (!done)
            ok->has_percent = false;
        else if (done->type == Container::Value::CONTAINER_TYPE_REAL)
        {
            ok->has_percent = true;
            ok->done = done->d;
        }
        else if (done->type == Container::Value::CONTAINER_TYPE_INTEGER)
        {
            ok->has_percent = true;
            ok->done = (double)done->l;
        }
        else
            return -1;

        oks.push_back(ok);
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::parse_report_ok(Container::Value *v, Report_Ok& ok)
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

}
