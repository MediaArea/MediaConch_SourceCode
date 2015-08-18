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
#include "Json.h"
//---------------------------------------------------------------------------

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
    model = new Json;
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
    child.obj.push_back(std::make_pair("Schematron", schematron));

    v.type = Container::Value::CONTAINER_TYPE_OBJECT;
    v.obj.push_back(std::make_pair("CHECK", child));

    std::string ret = model->serialize(v);
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
    return ret;
}

//***************************************************************************
// HELPER
//***************************************************************************

//---------------------------------------------------------------------------
Container::Value RESTAPI::serialize_check_args(std::vector<Check_Arg>& args)
{
    Container::Value args_val;

    args_val.type = Container::Value::CONTAINER_TYPE_OBJECT;

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

}
