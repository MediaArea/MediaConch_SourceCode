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
#include "REST_API.h"
#include "Container.h"
#include "NoContainer.h"
#include "Json.h"

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// RESTAPI
//***************************************************************************

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
// Serialize: Request
//***************************************************************************

//---------------------------------------------------------------------------
std::string RESTAPI::serialize_check_req(Check_Req& req)
{
    Container::Value v, child, schematron;

    schematron.type = Container::Value::CONTAINER_TYPE_STRING;
    schematron.s = req.schematron;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj.push_back(std::make_pair("args", serialize_check_args(req.args)));
    child.obj.push_back(std::make_pair("schematron", schematron));

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj.push_back(std::make_pair("CHECK", child));

    std::string ret = model->serialize(v);
    if (!ret.length())
        error = model->get_error();
    return ret;
}

//---------------------------------------------------------------------------
std::string RESTAPI::serialize_status_req(Status_Req& req)
{
    Container::Value v, child;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj.push_back(std::make_pair("ids", serialize_ids(req.ids)));

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj.push_back(std::make_pair("STATUS", child));

    std::string ret = model->serialize(v);
    if (!ret.length())
        error = model->get_error();
    return ret;
}

//---------------------------------------------------------------------------
std::string RESTAPI::serialize_report_req(Report_Req& req)
{
    Container::Value v, child;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj.push_back(std::make_pair("ids", serialize_ids(req.ids)));
    child.obj.push_back(std::make_pair("reports", serialize_report_reports(req.reports)));

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
    Container::Value v, child;

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj.push_back(std::make_pair("ids", serialize_ids(req.ids)));

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj.push_back(std::make_pair("CLEAR", child));

    std::string ret = model->serialize(v);
    if (!ret.length())
        error = model->get_error();
    return ret;
}

//***************************************************************************
// Serialize: Result
//***************************************************************************

//---------------------------------------------------------------------------
std::string RESTAPI::serialize_check_res(Check_Res& res)
{
    Container::Value v, child, nok;

    nok.type = Container::Value::CONTAINER_TYPE_ARRAY;
    for (size_t i = 0; i < res.nok.size(); ++i)
    {
        nok.array.push_back(serialize_generic_nok(res.nok[i].id, res.nok[i].error));
    }

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj.push_back(std::make_pair("ok", serialize_check_oks(res.ok)));
    child.obj.push_back(std::make_pair("nok", nok));

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj.push_back(std::make_pair("CHECK_RESULT", child));

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
    {
        nok.array.push_back(serialize_generic_nok(res.nok[i].id, res.nok[i].error));
    }

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
    {
        nok.array.push_back(serialize_generic_nok(res.nok[i].id, res.nok[i].error));
    }

    child.type = Container::Value::CONTAINER_TYPE_OBJECT;
    child.obj.push_back(std::make_pair("ok", serialize_report_oks(res.ok)));
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
    {
        nok.array.push_back(serialize_generic_nok(res.nok[i].id, res.nok[i].error));
    }

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
    {
        nok.array.push_back(serialize_generic_nok(res.nok[i].id, res.nok[i].error));
    }

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
RESTAPI::Check_Req *RESTAPI::parse_check_req(std::string data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECK");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;
    Check_Req *req = new Check_Req;

    Container::Value *args, *schematron;
    args = model->get_value_by_key(*child, "args");
    schematron = model->get_value_by_key(*child, "schematron");

    if (!args || !schematron ||
        schematron->type != Container::Value::CONTAINER_TYPE_STRING)
    {
        delete req;
        return NULL;
    }
    req->schematron = schematron->s;
    if (parse_check_arg(args, req->args))
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

    Container::Value *ids, *reports;
    ids = model->get_value_by_key(*child, "ids");
    reports = model->get_value_by_key(*child, "reports");

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
    if (parse_report_arg(reports, req->reports))
    {
        delete req;
        return NULL;
    }
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
RESTAPI::Check_Res *RESTAPI::parse_check_res(std::string data)
{
    Container::Value v, *child;

    if (model->parse(data, v))
    {
        error = model->get_error();
        return NULL;
    }

    child = model->get_value_by_key(v, "CHECK_RESULT");
    if (!child || child->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return NULL;

    Container::Value *ok, *nok;
    ok = model->get_value_by_key(*child, "ok");
    nok = model->get_value_by_key(*child, "nok");

    Check_Res *res = new Check_Res;
    if (ok && parse_check_ok(ok, res->ok))
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
        Check_Nok tmp;

        if (parse_generic_nok(&nok->array[i], tmp.id, tmp.error))
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
        Status_Nok tmp;

        if (parse_generic_nok(&nok->array[i], tmp.id, tmp.error))
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
        Report_Nok tmp;

        if (parse_generic_nok(&nok->array[i], tmp.id, tmp.error))
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
        Retry_Nok tmp;

        if (parse_generic_nok(&nok->array[i], tmp.id, tmp.error))
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
        Clear_Nok tmp;

        if (parse_generic_nok(&nok->array[i], tmp.id, tmp.error))
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
Container::Value RESTAPI::serialize_check_args(std::vector<Check_Arg>& args)
{
    Container::Value args_val;

    args_val.type = Container::Value::CONTAINER_TYPE_ARRAY;

    for (size_t i = 0; i < args.size(); ++i)
    {
        Container::Value arg, file, id;
        arg.type = Container::Value::CONTAINER_TYPE_OBJECT;

        file.type = Container::Value::CONTAINER_TYPE_STRING;
        file.s = args[i].file;
        args_val.obj.push_back(std::make_pair("file", file));

        id.type = Container::Value::CONTAINER_TYPE_INTEGER;
        id.l = args[i].id;
        args_val.obj.push_back(std::make_pair("id", id));
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
Container::Value RESTAPI::serialize_report_reports(std::vector<Report_Arg>& args)
{
    Container::Value args_val;

    args_val.type = Container::Value::CONTAINER_TYPE_ARRAY;

    for (size_t i = 0; i < args.size(); ++i)
    {
        Container::Value arg, kind, offset, length;
        arg.type = Container::Value::CONTAINER_TYPE_OBJECT;

        kind.type = Container::Value::CONTAINER_TYPE_STRING;
        kind.s = get_Report_string(args[i].kind);
        arg.obj.push_back(std::make_pair("kind", kind));

        offset.type = Container::Value::CONTAINER_TYPE_INTEGER;
        offset.l = args[i].has_offset ? args[i].offset : -1;
        arg.obj.push_back(std::make_pair("offset", offset));

        length.type = Container::Value::CONTAINER_TYPE_INTEGER;
        length.l = args[i].has_length ? args[i].length : -1;
        arg.obj.push_back(std::make_pair("length", length));

        args_val.array.push_back(arg);
    }

    return args_val;
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
Container::Value RESTAPI::serialize_check_oks(std::vector<Check_Ok>& array)
{
    Container::Value ok;

    ok.type = Container::Value::CONTAINER_TYPE_ARRAY;

    for (size_t i = 0; i < array.size(); ++i)
    {
        Container::Value v, in, out;

        v.type = Container::Value::CONTAINER_TYPE_OBJECT;

        in.type = Container::Value::CONTAINER_TYPE_INTEGER;
        in.l = array[i].inId;
        v.obj.push_back(std::make_pair("inId", in));

        out.type = Container::Value::CONTAINER_TYPE_INTEGER;
        out.l = array[i].outId;
        v.obj.push_back(std::make_pair("outId", out));
        ok.array.push_back(v);
    }

    return ok;
}

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_status_oks(std::vector<Status_Ok>& array)
{
    Container::Value ok;

    ok.type = Container::Value::CONTAINER_TYPE_ARRAY;

    for (size_t i = 0; i < array.size(); ++i)
    {
        Container::Value v, id, finished, validity;

        v.type = Container::Value::CONTAINER_TYPE_OBJECT;

        id.type = Container::Value::CONTAINER_TYPE_INTEGER;
        id.l = array[i].id;
        v.obj.push_back(std::make_pair("id", id));

        finished.type = Container::Value::CONTAINER_TYPE_BOOL;
        finished.b = array[i].finished;
        v.obj.push_back(std::make_pair("finished", finished));

        if (array[i].has_validity)
        {
            validity.type = Container::Value::CONTAINER_TYPE_BOOL;
            validity.b = array[i].validity;
            v.obj.push_back(std::make_pair("validity", validity));
        }

        ok.array.push_back(v);
    }

    return ok;
}

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_report_oks(std::vector<Report_Ok>& array)
{
    Container::Value ok;

    ok.type = Container::Value::CONTAINER_TYPE_ARRAY;

    for (size_t i = 0; i < array.size(); ++i)
    {
        Container::Value v, id, policy, implementation, mi_xml, mediatrace;

        v.type = Container::Value::CONTAINER_TYPE_OBJECT;

        id.type = Container::Value::CONTAINER_TYPE_INTEGER;
        id.l = array[i].id;
        v.obj.push_back(std::make_pair("id", id));

        if (array[i].policy.length() > 0)
        {
            policy.type = Container::Value::CONTAINER_TYPE_STRING;
            policy.s = array[i].policy;
            v.obj.push_back(std::make_pair("policy", policy));
        }

        if (array[i].implementation.length() > 0)
        {
            implementation.type = Container::Value::CONTAINER_TYPE_STRING;
            implementation.s = array[i].implementation;
            v.obj.push_back(std::make_pair("implementation", implementation));
        }

        if (array[i].mi_xml.length() > 0)
        {
            mi_xml.type = Container::Value::CONTAINER_TYPE_STRING;
            mi_xml.s = array[i].mi_xml;
            v.obj.push_back(std::make_pair("mi_xml", mi_xml));
        }

        if (array[i].mediatrace.length() > 0)
        {
            mediatrace.type = Container::Value::CONTAINER_TYPE_STRING;
            mediatrace.s = array[i].mediatrace;
            v.obj.push_back(std::make_pair("mediatrace", mediatrace));
        }

        ok.array.push_back(v);
    }

    return ok;
}

//---------------------------------------------------------------------------
int RESTAPI::parse_check_arg(Container::Value *v, std::vector<Check_Arg>& args)
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

        Check_Arg arg;
        arg.file = file->s;
        arg.id = id->l;
        args.push_back(arg);
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::parse_report_arg(Container::Value *v, std::vector<Report_Arg>& args)
{
    if (v->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return -1;

    for (size_t i = 0; i < v->array.size(); ++i)
    {
        Container::Value *obj = &v->array[i];

        if (obj->type != Container::Value::CONTAINER_TYPE_OBJECT)
            return -1;

        Container::Value *kind, *offset, *length;

        kind = model->get_value_by_key(*obj, "kind");
        offset = model->get_value_by_key(*obj, "offset");
        length = model->get_value_by_key(*obj, "length");

        if (!kind || kind->type != Container::Value::CONTAINER_TYPE_STRING)
            return -1;

        Report_Arg arg;
        arg.kind = string_to_Report(kind->s);

        if (offset && offset->type == Container::Value::CONTAINER_TYPE_INTEGER)
        {
            arg.offset = offset->l;
            arg.has_offset = true;
        }
        if (length && length->type == Container::Value::CONTAINER_TYPE_INTEGER)
        {
            arg.length = length->l;
            arg.has_length = true;
        }
        args.push_back(arg);
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
int RESTAPI::parse_check_ok(Container::Value *v, std::vector<Check_Ok>& oks)
{
    if (v->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return -1;

    for (size_t i = 0; i < v->array.size(); ++i)
    {
        Container::Value *obj = &v->array[i];

        if (obj->type != Container::Value::CONTAINER_TYPE_OBJECT)
            return -1;

        Container::Value *inId, *outId;

        inId = model->get_value_by_key(*obj, "inId");
        outId = model->get_value_by_key(*obj, "outId");

        if (!inId || inId->type != Container::Value::CONTAINER_TYPE_INTEGER ||
            !outId || outId->type != Container::Value::CONTAINER_TYPE_INTEGER)
            return -1;

        Check_Ok ok;
        ok.inId = inId->l;
        ok.outId = outId->l;

        oks.push_back(ok);
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::parse_status_ok(Container::Value *v, std::vector<Status_Ok>& oks)
{
    if (v->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return -1;

    for (size_t i = 0; i < v->array.size(); ++i)
    {
        Container::Value *obj = &v->array[i];

        if (obj->type != Container::Value::CONTAINER_TYPE_OBJECT)
            return -1;

        Container::Value *id, *finished, *validity;

        id = model->get_value_by_key(*obj, "id");
        finished = model->get_value_by_key(*obj, "finished");
        validity = model->get_value_by_key(*obj, "validity");

        if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER ||
            !finished || finished->type != Container::Value::CONTAINER_TYPE_BOOL)
            return -1;

        Status_Ok ok;
        ok.id = id->l;
        ok.finished = finished->b;

        if (!validity)
            ok.has_validity = false;
        else if (validity->type == Container::Value::CONTAINER_TYPE_BOOL)
        {
            if (!ok.finished)
                return -1;
            ok.has_validity = true;
            ok.validity = validity->b;
        }
        else
            return -1;

        oks.push_back(ok);
    }

    return 0;
}

//---------------------------------------------------------------------------
int RESTAPI::parse_report_ok(Container::Value *v, std::vector<Report_Ok>& oks)
{
    if (v->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return -1;

    for (size_t i = 0; i < v->array.size(); ++i)
    {
        Container::Value *obj = &v->array[i];

        if (obj->type != Container::Value::CONTAINER_TYPE_OBJECT)
            return -1;

        Container::Value *id, *policy, *implementation, *mi_xml, *mediatrace;

        id = model->get_value_by_key(*obj, "id");
        policy = model->get_value_by_key(*obj, "policy");
        implementation = model->get_value_by_key(*obj, "implementation");
        mi_xml = model->get_value_by_key(*obj, "mi_xml");
        mediatrace = model->get_value_by_key(*obj, "mediatrace");

        if (!id || id->type != Container::Value::CONTAINER_TYPE_INTEGER)
            return -1;

        Report_Ok ok;
        ok.id = id->l;

        if (policy)
        {
            if (policy->type != Container::Value::CONTAINER_TYPE_STRING)
                return -1;
            ok.policy = policy->s;
        }
        if (implementation)
        {
            if (implementation->type != Container::Value::CONTAINER_TYPE_STRING)
                return -1;
            ok.implementation = implementation->s;
        }
        if (mi_xml)
        {
            if (mi_xml->type != Container::Value::CONTAINER_TYPE_STRING)
                return -1;
            ok.mi_xml = mi_xml->s;
        }
        if (mediatrace)
        {
            if (mediatrace->type != Container::Value::CONTAINER_TYPE_STRING)
                return -1;
            ok.mediatrace = mediatrace->s;
        }

        oks.push_back(ok);
    }

    return 0;
}

}
