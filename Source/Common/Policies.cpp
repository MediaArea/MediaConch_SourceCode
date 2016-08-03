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
#include <iostream>
#include <sstream>
#include <string.h>
#include <ZenLib/Ztring.h>
#include <ZenLib/ZtringList.h>
#include <ZenLib/Dir.h>
#include <ZenLib/File.h>
#include "Policies.h"
#include "Core.h"
#include "Policy.h"
#include "XsltPolicy.h"
#include "UnknownPolicy.h"
//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// Policies
//***************************************************************************

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

std::map<std::string, std::list<std::string> > Policies::existing_type = std::map<std::string, std::list<std::string> >();
std::list<Policies::validatorType> Policies::existing_validator = std::list<Policies::validatorType>();
std::list<std::string> Policies::existing_xsltOperator = std::list<std::string>();
size_t Policies::policy_global_id = 0;

//---------------------------------------------------------------------------
Policies::Policies(Core *c) : core(c)
{
}

Policies::~Policies()
{
    std::map<int, std::map<size_t, Policy *> >::iterator it = policies.begin();
    for (; it != policies.end(); ++it)
    {
        std::map<size_t, Policy *>::iterator it_p = it->second.begin();
        for (; it_p != it->second.end(); ++it_p)
            delete it_p->second;
        it->second.clear();
    }

    policies.clear();
    xmlCleanupParser();
}

// Policy
int Policies::create_xslt_policy(int user, const std::string& type, int parent_id, std::string& err)
{
    Policy *parent = NULL;
    if (parent_id != -1)
    {
        parent = get_policy(user, parent_id, err);
        if (!parent || parent->type != POLICY_XSLT)
        {
            err = "Parent ID is not existing or understandable";
            return -1;
        }
    }

    XsltPolicy *p = new XsltPolicy(this, !core->accepts_https());
    find_new_policy_name(user, p->name);
    p->node_name = p->name;
    p->ope = type;

    // Policy filename
    if (!parent)
        find_save_name(user, NULL, p->filename);
    else
    {
        ((XsltPolicy*)parent)->nodes.push_back(p);
        p->parent_id = parent_id;
    }

    policies[user][p->id] = p;
    return (int)p->id;
}

int Policies::import_policy_from_memory(int user, const std::string& memory, std::string& err, const char* filename, bool is_system_policy)
{
    if (!memory.length())
    {
        err = "The policy is empty";
        return -1;
    }

    std::string save_name;
    if (is_system_policy && filename)
        save_name = filename;
    else
        find_save_name(user, NULL, save_name);

    Policy *p = NULL;
    int ret = -1;

    p = new XsltPolicy(this, !core->accepts_https());
    ret = p->import_schema_from_memory(memory.c_str(), memory.length(), save_name);
    if (ret < 0)
    {
        if (p)
            delete p;

        p = new UnknownPolicy(this, !core->accepts_https());
        ret = p->import_schema_from_memory(memory.c_str(), memory.length(), save_name);
        if (ret < 0)
            err = p->get_error();
    }

    p->is_system = is_system_policy;
    if (ret < 0)
        delete p;
    else
        policies[user][p->id] = p;

    return (int)p->id;
}

int Policies::save_policy(int user, int id, std::string& err)
{
    return export_policy(user, NULL, id, err);
}

int Policies::duplicate_policy(int user, int id, std::string& err)
{
    Policy *old = get_policy(user, id, err);
    if (!old)
        return -1;

    Policy *p = NULL;
    if (old->type == POLICY_XSLT)
        p = new XsltPolicy((XsltPolicy*)old);
    else if (old->type == POLICY_UNKNOWN)
        p = new UnknownPolicy((UnknownPolicy*)old);

    if (!p)
    {
        err = "policy cannot be duplicate";
        return -1;
    }

    policies[user][p->id] = p;

    // Policy filename
    find_save_name(user, NULL, p->filename);
    size_t pos = p->id;

    if (old->type == POLICY_UNKNOWN)
        save_policy(user, id, err);

    return (int)pos;
}

int Policies::export_policy(int user, const char* filename, int id, std::string& err)
{
    Policy *p = get_policy(user, id, err);
    if (!p)
        return -1;

    if (filename == NULL)
    {
        if (p->is_system)
            return 0;

        filename = p->filename.c_str();
    }

    return p->export_schema(filename, err);
}

int Policies::dump_policy_to_memory(int user, int id, std::string& memory, std::string& err)
{
    Policy *p = get_policy(user, id, err);
    if (!p)
        return -1;

    if (p->dump_schema(memory) < 0)
    {
        err = "Cannot dump the schema to memory";
        return -1;
    }

    return 0;
}

Policy* Policies::get_policy(int user, int id, std::string& err)
{
    std::map<int, std::map<size_t, Policy *> >::iterator it = policies.find(user);

    if (it == policies.end())
    {
        err = "User Policies are not existing";
        return NULL;
    }

    std::map<size_t, Policy *>::iterator it_p = it->second.find(id);
    if (it_p == it->second.end() || !it_p->second)
    {
        err = "Policy not existing";
        return NULL;
    }

    return it_p->second;
}

int Policies::policy_get_name(int user, int id, std::string& name, std::string& err)
{
    Policy *p = get_policy(user, id, err);
    if (!p)
        return -1;

    name = p->name;
    return 0;
}

size_t Policies::get_policies_size(int user) const
{
    std::map<int, std::map<size_t, Policy *> >::const_iterator it = policies.find(user);

    if (it == policies.end())
        return 0;

    return it->second.size();
}

void Policies::get_policies(int user, std::vector<MediaConchLib::Policy_Policy*>& ps)
{
    std::map<int, std::map<size_t, Policy *> >::iterator it = policies.find(user);

    if (it == policies.end())
        return;


    std::map<size_t, Policy*>::iterator it_p = it->second.begin();
    for (; it_p != it->second.end(); ++it_p)
    {
        if (!it_p->second)
            continue;

        MediaConchLib::Policy_Policy *p = NULL;

        //Unknown...
        if (it_p->second->type != POLICY_XSLT)
        {
            p = new MediaConchLib::Policy_Policy;
            p->id = it_p->second->id;
            p->name = it_p->second->name;
        }
        else
        {
            if (((XsltPolicy*)it_p->second)->kind == XSLT_POLICY_POLICY && ((XsltPolicy*)it_p->second)->parent_id == (size_t)-1)
            {
                XsltPolicy* node = (XsltPolicy*)it_p->second;
                // XSLT Policy
                p = new MediaConchLib::Policy_Policy;
                p->id = it_p->second->id;
                if (node->ope.length())
                    p->type = node->ope;
                else
                    p->type = "and";
                p->name = node->name;
                p->description = node->description;
            }
            else
            {
                XsltPolicyNode* node = (XsltPolicyNode*)it_p->second;
                //TODO sub-nodes
                if (node->kind == XSLT_POLICY_POLICY)
                {
                    // // XSLT Policy
                    // REST_API::Policy_Policy *p = new REST_API::Policy_Policy;
                    // p->id = node->id;
                    // p->parent_id = node->parent_id;
                    // p->type = node->ype;
                    // p->name = node->name;
                    // p->description = node->description;
                }
                else
                {
                    //TODO XSLT RULE
                }
                continue;
            }
        }

        if (p)
            ps.push_back(p);
    }
}

int Policies::erase_xslt_policy_node(std::map<size_t, Policy *>& user_policies, int id, std::string& err)
{
    std::map<size_t, Policy *>::iterator it = user_policies.find(id);
    if (it == user_policies.end())
    {
        err = "Policy is not exisiting";
        return -1;
    }

    XsltPolicy *policy = (XsltPolicy*)it->second;
    for (size_t i = 0; i < policy->nodes.size(); ++i)
    {
        if (policy->nodes[i] && policy->nodes[i]->kind == XSLT_POLICY_POLICY)
        {
            if (erase_xslt_policy_node(user_policies, ((XsltPolicy*)policy->nodes[i])->id, err) < 0)
                return -1;
        }

        delete policy->nodes[i];
        policy->nodes[i] = NULL;
    }

    policy->nodes.clear();
    return 0;
}

int Policies::erase_policy(int user, int id, std::string& err)
{
    std::map<int, std::map<size_t, Policy *> >::iterator it = policies.find(user);
    if (it == policies.end())
    {
        err = "User policies are not exisiting";
        return -1;
    }

    Policy *p = get_policy(user, id, err);
    if (!p)
        return -1;

    if (p->type != POLICY_XSLT || ((XsltPolicy*)p)->parent_id == (size_t)-1)
        remove_saved_policy(p);

    if (p->type == POLICY_XSLT)
        erase_xslt_policy_node(policies[user], p->id, err);

    delete p;
    policies[user].erase(policies[user].find(id));

    return 0;
}

int Policies::clear_policies(int user, std::string&)
{
    std::map<int, std::map<size_t, Policy*> >::iterator it = policies.find(user);
    if (it == policies.end())
        return 0;

    std::map<size_t, Policy*>::iterator it_p = it->second.begin();
    for (; it_p != it->second.end(); ++it_p)
    {
        if (it_p->second)
            remove_saved_policy(it_p->second);
        delete it_p->second;
    }
    policies[user].clear();
    return 0;
}

int Policies::policy_change_name(int user, int id, const std::string& name, const std::string& description, std::string& err)
{
    Policy *p = get_policy(user, id, err);
    if (!p)
        return -1;

    p->name = name;
    if (p->type == POLICY_XSLT)
        ((XsltPolicy*)p)->node_name = p->name;
    p->description = description;

    return 0;
}

xmlDocPtr Policies::create_doc(int user, int id)
{
    std::string err;
    Policy *p = get_policy(user, id, err);
    if (!p)
        return NULL;

    return p->create_doc();
}

bool Policies::policy_exists(int user, const std::string& policy)
{
    std::map<int, std::map<size_t, Policy *> >::iterator it = policies.find(user);

    if (it == policies.end())
        return false;

    std::map<size_t, Policy *>::iterator it_p = it->second.begin();
    for (; it_p != it->second.end(); ++it_p)
    {
        if (it_p->second && it_p->second->name == policy)
            return true;
    }

    return false;
}

int Policies::create_xslt_policy_from_file(int user, const std::string& file, std::string& err)
{
    std::bitset<MediaConchLib::report_Max> report_set;
    std::vector<std::string> files;
    std::map<std::string, std::string> options;
    std::vector<size_t> policies_ids;
    std::vector<std::string> policies_contents;
    MediaConchLib::Checker_ReportRes result;

    report_set.set(MediaConchLib::report_MediaInfo);
    files.push_back(file);

    core->checker_get_report(user, report_set, MediaConchLib::format_Xml, files,
                             policies_ids, policies_contents,
                             options, &result,
                             NULL, NULL);
    if (!result.valid || !result.report.length())
    {
        err = "Implementation report not found";
        return -1;
    }

    Policy *p = new XsltPolicy(this, !core->accepts_https());

    //Policy filename
    find_save_name(user, NULL, p->filename);

    size_t name_pos = file.rfind("/");
    if (name_pos == std::string::npos)
        name_pos = 0;
    else
        name_pos++;
    p->name = file.substr(name_pos, std::string::npos);

    int pos = -1;
    if (((XsltPolicy*)p)->create_policy_from_mi(result.report) < 0)
        delete p;
    else
    {
        policies[user][p->id] = p;
        pos = p->id;
    }

    return pos;
}

int Policies::policy_get_policy_id(Policy* p, std::vector<std::string>& xslt_policies, std::string& err)
{
    if (!p)
        return -1;

    std::string policy;
    if (p->type == POLICY_XSLT)
    {
        if (((XsltPolicy*)p)->get_final_xslt(policy) < 0)
        {
            err = "Policy cannot be dumped";
            return -1;
        }
    }
    else
    {
        if (p->dump_schema(policy) < 0)
        {
            err = "Policy cannot be dump";
            return -1;
        }
    }

    xslt_policies.push_back(policy);

    return 0;
}

int Policies::policy_get_policy_content(const std::string& policy, std::vector<std::string>& xslt_policies, std::string& err)
{
    int ret = -1;
    Policy *p = new XsltPolicy(this, !core->accepts_https());

    ret = p->import_schema_from_memory(policy.c_str(), policy.length(), "");
    if (ret < 0)
    {
        if (p)
            delete p;

        p = new UnknownPolicy(this, !core->accepts_https());
        ret = p->import_schema_from_memory(policy.c_str(), policy.length(), "");
        if (ret < 0)
        {
            err = p->get_error();
            delete p;
            return -1;
        }
    }

    ret = policy_get_policy_id(p, xslt_policies, err);
    delete p;
    return ret;
}

int Policies::policy_get_policies(int user, const std::vector<size_t>* policies_ids,
                                  const std::vector<std::string>* policies_contents,
                                  std::vector<std::string>& xslt_policies, std::string& err)
{
    if (!policies_ids && !policies_contents)
    {
        err = "No policy to apply";
        return -1;
    }

    if (policies_ids)
    {
        for (size_t i = 0; i < policies_ids->size(); ++i)
            if (policy_get_policy_id(get_policy(user, policies_ids->at(i), err), xslt_policies, err) < 0)
                return -1;
    }

    if (policies_contents)
    {
        for (size_t i = 0; i < policies_contents->size(); ++i)
            if (policy_get_policy_content(policies_contents->at(i), xslt_policies, err) < 0)
                return -1;
    }

    return 0;
}

// XSLT Rule
int Policies::create_xslt_policy_rule(int user, int policy_id, std::string& err)
{
    Policy *p = get_policy(user, policy_id, err);
    if (!p)
        return -1;

    if (p->type != POLICY_XSLT)
    {
        err = "policy rule cannot be created";
        return -1;
    }
    XsltPolicy *policy = (XsltPolicy*)p;
    XsltPolicyRule *rule = new XsltPolicyRule;
    rule->id = rule->rule_id++;

    rule->node_name = "New Rule";
    policy->nodes.push_back(rule);

    return (int)rule->id;
}

XsltPolicyRule *Policies::get_xslt_policy_rule(int user, int policy_id, int id, std::string& err)
{
    Policy *p = get_policy(user, policy_id, err);
    if (!p)
        return NULL;

    if (p->type != POLICY_XSLT)
    {
        err = "policy rule cannot be created";
        return NULL;
    }

    XsltPolicy *policy = (XsltPolicy*)p;
    return policy->get_policy_rule(id, err);
}

int Policies::edit_xslt_policy_rule(int user, int policy_id, int rule_id, const XsltPolicyRule *rule, std::string& err)
{
    Policy *p = get_policy(user, policy_id, err);
    if (!p)
        return -1;

    if (p->type != POLICY_XSLT)
    {
        err = "policy rule cannot be created";
        return -1;
    }

    XsltPolicy *policy = (XsltPolicy*)p;
    XsltPolicyRule *r = policy->get_policy_rule(rule_id, err);
    if (!r)
        return -1;

    return r->edit_policy_rule(rule, err);
}

int Policies::duplicate_xslt_policy_rule(int user, int policy_id, int rule_id, std::string& err)
{
    Policy *p = get_policy(user, policy_id, err);
    if (!p)
        return -1;

    if (p->type != POLICY_XSLT)
    {
        err = "Parent policy is not an XSLT policy";
        return -1;
    }

    XsltPolicy *policy = (XsltPolicy*)p;
    XsltPolicyRule *r = policy->get_policy_rule(rule_id, err);
    if (!r)
        return -1;

    XsltPolicyRule *rule = new XsltPolicyRule(r);

    rule->id = rule->rule_id++;
    policy->nodes.push_back(rule);

    return (int)rule->id;
}

int Policies::delete_xslt_policy_rule(int user, int policy_id, int rule_id, std::string& err)
{
    Policy *p = get_policy(user, policy_id, err);
    if (!p)
        return -1;

    if (p->type != POLICY_XSLT)
    {
        err = "policy rule cannot be created";
        return -1;
    }

    XsltPolicy *policy = (XsltPolicy*)p;
    return policy->delete_policy_rule(rule_id, err);
}

// Helper
void Policies::create_values_from_csv()
{
    ZenLib::ZtringList list;
    list.Separator_Set(0, __T(","));
    list.Write(core->Menu_Option_Preferences_Option(__T("MAXML_StreamKinds"), ZenLib::Ztring()));
    for (size_t i = 0; i < list.size(); ++i)
    {
        std::list<std::string> fields;
        fields.push_back("");

        ZenLib::ZtringList listField;
        listField.Separator_Set(0, __T(","));
        listField.Write(core->Menu_Option_Preferences_Option(__T("MAXML_Fields"), list[i]));
        for (size_t j = 0; j < listField.size(); ++j)
            fields.push_back(listField[j].To_UTF8());

        existing_type[list[i].To_UTF8()] = fields;
    }

    validatorType validators[] = {
        { "", "", "" },
        { "=", "is_equal", "Equal" },
        { "!=", "is_not_equal", "Not Equal" },
        { ">", "is_greater_than", "Greater than" },
        { ">=", "is_greater_or_equal_than", "Greater or equal" },
        { "<", "is_less_than", "Less" },
        { "<=", "is_less_or_equal_than", "Less or equal" }
    };

    for (size_t i=0; i < (sizeof(validators) / sizeof(*validators)); i++)
        existing_validator.push_back(validators[i]);

    std::string xsltOperators[] =
    {
        "is_equal",
        "is_not_equal",
        "is_greater_than",
        "is_less_than",
        "is_greater_or_equal_than",
        "is_less_or_equal_than",
        "exists",
        "does_not_exist",
        "contains_string",
        "is_true",
    };

    for (size_t i=0; i < (sizeof(xsltOperators) / sizeof(*xsltOperators)); i++)
        existing_xsltOperator.push_back(xsltOperators[i]);
}

bool Policies::check_test_type(const std::string& type)
{
    std::map<std::string, std::list<std::string> >::iterator it = existing_type.begin();
    std::map<std::string, std::list<std::string> >::iterator ite = existing_type.end();

    for (; it != ite; ++it)
        if (!type.compare(it->first))
            return true;

    return false;
}

bool Policies::check_test_field(const std::string& field)
{
    std::map<std::string, std::list<std::string> >::iterator itType = existing_type.begin();
    std::map<std::string, std::list<std::string> >::iterator iteType = existing_type.end();

    for (; itType != iteType; ++itType)
    {
        std::list<std::string>::iterator it = itType->second.begin();
        std::list<std::string>::iterator ite = itType->second.end();

        for (; it != ite; ++it)
            if (!field.compare(*it))
                return true;
    }

    return false;
}

bool Policies::check_test_validator(const std::string& validator)
{
    std::list<validatorType>::iterator it = existing_validator.begin();
    std::list<validatorType>::iterator ite = existing_validator.end();

    for (; it != ite; ++it)
        if (!validator.compare(it->value))
            return true;

    return false;
}

std::string Policies::parse_test_value(std::string& sub, const std::string& before, const std::string& after)
{
    if (sub.compare(0, before.length(), before))
        return std::string();

    sub = sub.substr(before.length());
    std::string::size_type pos = sub.find(after);
    if (pos == std::string::npos)
        return std::string();

    std::string ret = sub.substr(0, pos);
    sub = sub.substr(pos + after.length());

    return ret;
}

std::string Policies::parse_test_field(std::string& sub, const std::string& before)
{
    if (sub.compare(0, before.length(), before))
        return std::string();

    sub = sub.substr(before.length());
    if (!sub.length())
        return std::string();

    std::string::size_type pos = sub.find(" ");
    std::string ret = sub.substr(0, pos);
    if (pos == std::string::npos)
    {
        sub = sub.substr(sub.length());
        return ret;
    }
    sub = sub.substr(pos + 1);

    return ret;
}

void Policies::find_save_name(int user, const char* basename, std::string& save_name)
{
    std::stringstream data_path;

    data_path << Core::get_local_data_path();
    data_path << "policies/" << user << "/";

    ZenLib::Ztring z_path = ZenLib::Ztring().From_UTF8(data_path.str());
    if (!ZenLib::Dir::Exists(z_path))
        ZenLib::Dir::Create(z_path);

    if (basename)
    {
        std::string base(basename);
        z_path = ZenLib::Ztring().From_UTF8(base);
        if (base.find(data_path.str()) == 0 && ZenLib::File::Exists(z_path))
        {
            save_name = basename;
            return;
        }
    }

    for (size_t i = 0; 1; ++i)
    {
        std::stringstream ss;
        ss << data_path.str() << "policy";
        if (i)
            ss << i;
        ss << ".xsl";

        ZenLib::Ztring z_path = ZenLib::Ztring().From_UTF8(ss.str());
        if (!ZenLib::File::Exists(z_path))
        {
            save_name = ss.str();
            break;
        }
    }
}

void Policies::find_new_policy_name(int user, std::string& name)
{
    name = "New policy";
    std::map<int, std::map<size_t, Policy *> >::iterator it = policies.find(user);
    if (it == policies.end())
        return;

    for (size_t i = 0; 1; ++i)
    {
        std::stringstream ss;
        ss << name;
        if (i)
            ss << " " << i;

        size_t j = 0;
        for (; j < it->second.size(); ++j)
            if (it->second[j] && it->second[j]->name == ss.str())
                break;
        if (j == it->second.size())
        {
            name = ss.str();
            break;
        }
    }
}

void Policies::remove_saved_policy(const Policy* policy)
{
    if (!policy->filename.length())
        return;

    if (policy->type == POLICY_XSLT && ((const XsltPolicy*)policy)->parent_id == (size_t)-1)
        return;

    ZenLib::Ztring z_path = ZenLib::Ztring().From_UTF8(policy->filename);
    if (!ZenLib::File::Exists(z_path))
        return;

    ZenLib::File::Delete(z_path);
}

int Policies::transform_with_xslt_memory(const std::string& report, const std::string& memory,
                                         std::string& result)
{
    return core->transform_with_xslt_memory(report, memory, result);
}

}
