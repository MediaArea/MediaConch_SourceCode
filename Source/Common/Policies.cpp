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
#include "JS_Tree.h"
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
        std::map<size_t, Policy*>::iterator it_p = it->second.begin();
        while (it_p != it->second.end())
        {
            if (!it_p->second || it_p->second->is_system)
            {
                ++it_p;
                continue;
            }

            if (it_p->second->type != POLICY_XSLT)
            {
                delete it_p->second;
                it_p->second = NULL;
                it->second.erase(it_p);
            }
            else if (((XsltPolicy*)it_p->second)->parent_id == (size_t)-1)
                delete_xslt_sub_policy(it->second, (XsltPolicy*)it_p->second);
            else
                it->second.erase(it_p);

            it_p = it->second.begin();
        }
        it->second.clear();
    }
    policies.clear();

    for (size_t i = 0; i < system_policies.size(); ++i)
        delete system_policies[i];
    system_policies.clear();

    xmlCleanupParser();
}

void Policies::delete_xslt_sub_policy(std::map<size_t, Policy*>& policies, XsltPolicy* p)
{
    for (size_t i = 0; i < p->nodes.size(); ++i)
    {
        if (!p->nodes[i])
            continue;
        if (p->nodes[i]->kind == XSLT_POLICY_RULE)
            delete p->nodes[i];
        else
            delete_xslt_sub_policy(policies, (XsltPolicy*)p->nodes[i]);
        p->nodes[i] = NULL;
    }
    p->nodes.clear();

    std::map<size_t, Policy*>::iterator it = policies.find(p->id);
    if (it != policies.end())
        policies.erase(it);
    delete p;
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

    if (policies.find(user) == policies.end())
        add_system_policies_to_user_policies(user);

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
    {
        delete p;
        return -1;
    }
    else if (is_system_policy)
        system_policies.push_back(p);
    else
    {
        if (policies.find(user) == policies.end())
            add_system_policies_to_user_policies(user);
        add_recursively_policy_to_user_policies(user, p);
    }

    return (int)p->id;
}

int Policies::import_policy_from_file(int user, const std::string& file, std::string& err)
{
    if (!file.length())
    {
        err = "The policy is not exiting";
        return -1;
    }

    std::string save_name = file;

    Policy *p = NULL;
    int ret = -1;

    p = new XsltPolicy(this, !core->accepts_https());
    ret = p->import_schema(file, save_name);
    if (ret < 0)
    {
        if (p)
            delete p;

        p = new UnknownPolicy(this, !core->accepts_https());
        ret = p->import_schema(file, save_name);
        if (ret < 0)
            err = p->get_error();
    }

    p->is_system = false;
    if (ret < 0)
    {
        delete p;
        return -1;
    }


    if (policies.find(user) == policies.end())
        add_system_policies_to_user_policies(user);
    add_recursively_policy_to_user_policies(user, p);

    return (int)p->id;
}

int Policies::save_policy(int user, int id, std::string& err)
{
    Policy *policy = get_policy(user, id, err);
    if (!policy)
        return -1;

    if (policy->type == POLICY_XSLT && ((XsltPolicy*)policy)->parent_id != (size_t)-1)
        return save_policy(user, ((XsltPolicy*)policy)->parent_id, err);

    return export_policy(user, NULL, id, err, true);
}

int Policies::duplicate_policy(int user, int id, int dst_policy_id, int *dst_user, bool must_be_public, std::string& err, bool copy_name)
{
    Policy *old = get_policy(user, id, err);
    if (!old)
        return -1;

    if (must_be_public && !old->is_public)
    {
        err = "This policy is not a public policy";
        return -1;
    }

    int destination_user = user;
    if (dst_user)
        destination_user = *dst_user;

    Policy *p = NULL;
    if (dst_policy_id == -1)
    {
        if (old->type == POLICY_XSLT)
        {
            p = new XsltPolicy((XsltPolicy*)old);
            ((XsltPolicy*)p)->parent_id = -1;
            if (copy_name && ((XsltPolicy*)old)->parent_id == (size_t)-1 && ((XsltPolicy*)p)->node_name.size())
            {
                ((XsltPolicy*)p)->node_name += "_copy";
                p->name = ((XsltPolicy*)p)->node_name;
            }
        }
        else if (old->type == POLICY_UNKNOWN)
        {
            p = new UnknownPolicy((UnknownPolicy*)old);
            if (copy_name && p->name.size())
                p->name += "_copy";
        }
    }
    else
    {
        if (old->type != POLICY_XSLT)
        {
            error = "This kind of policy cannot be duplicated as a sub-policy.";
            return -1;
        }

        Policy *destination = get_policy(destination_user, dst_policy_id, err);
        if (!destination)
            return -1;

        if (destination->is_system)
        {
            err = "Destination policy should not be a system policy";
            return -1;
        }

        p = new XsltPolicy((XsltPolicy*)old);
        ((XsltPolicy*)p)->parent_id = destination->id;
        ((XsltPolicy*)destination)->nodes.push_back((XsltPolicy*)p);
        if (copy_name && ((XsltPolicy*)old)->parent_id == destination->id && ((XsltPolicy*)p)->node_name.size())
            ((XsltPolicy*)p)->node_name += "_copy";
        p->name = ((XsltPolicy*)p)->node_name;
        p->filename = std::string();
    }

    if (!p)
    {
        err = "policy cannot be duplicated";
        return -1;
    }

    add_recursively_policy_to_user_policies(destination_user, p);

    find_save_name(destination_user, NULL, p->filename, p->name.c_str());
    if (p->type == POLICY_UNKNOWN)
        export_policy(destination_user, p->filename.c_str(), old->id, err, false);

    return (int)p->id;
}

int Policies::move_policy(int user, int id, int dst_policy_id, std::string& err)
{
    int new_id = duplicate_policy(user, id, dst_policy_id, NULL, false, err, false);
    if (new_id < 0)
        return -1;

    if (remove_policy(user, id, err) < 0)
    {
        std::string tmp;
        remove_policy(user, new_id, tmp);
        return -1;
    }

    return new_id;
}

int Policies::export_policy(int user, const char* filename, int id, std::string& err, bool is_save)
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

    p->set_keep_public(is_save);

    return p->export_schema(filename, err);
}

int Policies::dump_policy_to_memory(int user, int id, bool must_be_public, std::string& memory, std::string& err)
{
    Policy *p = get_policy(user, id, err);
    if (!p)
        return -1;

    if (must_be_public && !p->is_public)
    {
        err = "This policy is not a public policy";
        return -1;
    }

    p->set_keep_public(false);

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

    // Add system policy
    if (it == policies.end())
        add_system_policies_to_user_policies(user);

    it = policies.find(user);

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

int Policies::xslt_policy_child_to_mcl_policy(XsltPolicyNode *n, MediaConchLib::Policy_Policy *policy, std::string& error)
{
    if (!n)
        return 0;

    if (n->kind == XSLT_POLICY_POLICY)
    {
        XsltPolicy *node = (XsltPolicy*)n;
        MediaConchLib::Policy_Policy *p = xslt_policy_to_mcl_policy(node, error);
        if (p)
        {
            MediaConchLib::XSLT_Child child;
            child.policy = p;
            policy->children.push_back(std::make_pair(0, child));
        }
    }
    else
    {
        XsltPolicyRule *node = (XsltPolicyRule*)n;
        MediaConchLib::XSLT_Policy_Rule *r = xslt_policy_rule_to_mcl_policy(node, error);
        if (r)
        {
            MediaConchLib::XSLT_Child child;
            child.rule = r;
            policy->children.push_back(std::make_pair(1, child));
        }
    }

    return 0;
}

MediaConchLib::XSLT_Policy_Rule* Policies::xslt_policy_rule_to_mcl_policy(XsltPolicyRule *rule, std::string&)
{
    MediaConchLib::XSLT_Policy_Rule *r = new MediaConchLib::XSLT_Policy_Rule;
    r->id = rule->id;
    r->name = rule->node_name;
    if (!r->name.length())
        rule->create_default_name(r->name);
    r->tracktype = rule->track_type;
    r->field = rule->field;
    r->scope = rule->scope;
    r->occurrence = rule->occurrence;
    r->ope = rule->ope;
    r->value = rule->value;

    return r;
}

MediaConchLib::Policy_Policy *Policies::xslt_policy_to_mcl_policy(XsltPolicy *policy, std::string& error)
{
    MediaConchLib::Policy_Policy *p = new MediaConchLib::Policy_Policy;
    p->id = policy->id;
    p->parent_id = policy->parent_id;
    if (policy->ope.length())
        p->type = policy->ope;
    else
        p->type = "and";
    p->name = policy->name;
    p->description = policy->description;
    p->license = policy->license;
    p->is_system = policy->is_system;
    p->is_public = policy->is_public;
    p->kind = "XSLT";

    for (size_t i = 0; i < policy->nodes.size(); ++i)
        xslt_policy_child_to_mcl_policy(policy->nodes[i], p, error);


    return p;
}

MediaConchLib::Policy_Policy* Policies::policy_to_mcl_policy(Policy *policy, std::string& error)
{
    if (!policy)
        return NULL;

    MediaConchLib::Policy_Policy* p = NULL;
    //Unknown...
    if (policy->type != POLICY_XSLT)
    {
        p = new MediaConchLib::Policy_Policy;
        p->id = policy->id;
        p->name = policy->name;
        p->is_system = policy->is_system;
        p->kind = "UNKNOWN";
    }
    else
    {
        if (((XsltPolicy*)policy)->kind == XSLT_POLICY_POLICY)
        {
            XsltPolicy* node = (XsltPolicy*)policy;

            // XSLT Policy
            p = xslt_policy_to_mcl_policy(node, error);
        }
    }

    return p;
}

int Policies::policy_get(int user, int id, const std::string& format, bool must_be_public,
                         MediaConchLib::Get_Policy& policy, std::string& err)
{
    Policy *p = get_policy(user, id, err);
    if (!p)
        return -1;

    if (must_be_public && !p->is_public)
    {
        err = "This policy is not a public policy";
        return -1;
    }

    MediaConchLib::Policy_Policy *pp = policy_to_mcl_policy(p, err);

    policy.format = format;
    if (format == "JSTREE")
    {
        std::string json;
        if (JsTree::policy_to_js_tree(pp, json, err) < 0)
            return -1;
        policy.jstree = new std::string;
        *policy.jstree = "{\"policyTree\":" + json + "}";
    }
    else
        policy.policy = pp;

    return 0;
}

int Policies::policy_get_name(int user, int id, std::string& name, std::string& err)
{
    Policy *p = get_policy(user, id, err);
    if (!p)
        return -1;

    name = p->name;
    return 0;
}

size_t Policies::get_policies_size(int user, std::string&)
{
    size_t count = 0;
    std::map<int, std::map<size_t, Policy *> >::const_iterator it = policies.find(user);

    if (it == policies.end())
        add_system_policies_to_user_policies(user);

    it = policies.find(user);
    if (it == policies.end())
        return count;

    std::map<size_t, Policy *>::const_iterator it_p = it->second.begin();
    for (; it_p != it->second.end(); ++it_p)
    {
        if (!it_p->second)
            continue;

        if (it_p->second->type != POLICY_XSLT || ((XsltPolicy*)it_p->second)->parent_id == (size_t)-1)
            ++count;
    }

    return count;
}

int Policies::get_policies(int user, const std::vector<int>& ids, const std::string& format,
                            MediaConchLib::Get_Policies& ps, std::string&)
{
    std::map<int, std::map<size_t, Policy *> >::iterator it = policies.find(user);

    if (it == policies.end())
        add_system_policies_to_user_policies(user);

    it = policies.find(user);
    if (it == policies.end())
        return 0;

    std::vector<MediaConchLib::Policy_Policy*> vec;
    std::map<size_t, Policy*>::iterator it_p = it->second.begin();
    for (; it_p != it->second.end(); ++it_p)
    {
        if (!it_p->second)
            continue;

        if (ids.size())
        {
            size_t i = 0;
            for (; i < ids.size(); ++i)
            {
                if (ids[i] == (int)it_p->first)
                    break;
            }

            if (i == ids.size())
                continue;
        }
        else if (it_p->second && it_p->second->type == POLICY_XSLT && ((XsltPolicy*)it_p->second)->parent_id != (size_t)-1)
            continue;

        std::string err;
        MediaConchLib::Policy_Policy *p = policy_to_mcl_policy(it_p->second, err);

        if (p)
            vec.push_back(p);
    }

    ps.format = format;
    if (format == "JSTREE")
    {
        std::string err;
        ps.jstree = new std::string;
        JsTree::policies_to_js_tree(vec, *ps.jstree, err);

        for (size_t i = 0; i < vec.size(); ++i)
            delete vec[i];
        vec.clear();
    }
    else
    {
        ps.policies = new std::vector<MediaConchLib::Policy_Policy*>;
        *ps.policies = vec;
    }

    return 0;
}

int Policies::get_public_policies(std::vector<MediaConchLib::Policy_Public_Policy*>& ps, std::string&)
{
    std::map<int, std::map<size_t, Policy*> >::iterator it = policies.begin();
    for (; it != policies.end(); ++it)
    {
        std::map<size_t, Policy *>::iterator it_p = it->second.begin();
        for (; it_p != it->second.end(); ++it_p)
        {
            if (!it_p->second)
                continue;

            if (!it_p->second->is_public)
                continue;

            MediaConchLib::Policy_Public_Policy *p = new MediaConchLib::Policy_Public_Policy;
            p->id = (long)it_p->second->id;
            p->user = it->first;
            p->name = it_p->second->name;
            p->description = it_p->second->description;
            p->license = it_p->second->license;

            ps.push_back(p);
        }
    }

    return 0;
}

int Policies::get_policies_names_list(int user, std::vector<std::pair<int, std::string> >& ps, std::string&)
{
    std::map<int, std::map<size_t, Policy*> >::iterator it = policies.find(user);

    if (it == policies.end())
        add_system_policies_to_user_policies(user);

    it = policies.find(user);

    if (it == policies.end())
        return 0;

    std::map<size_t, Policy*>::iterator it_p = it->second.begin();
    for (; it_p != it->second.end(); ++it_p)
    {
        if (!it_p->second)
            continue;

        if (it_p->second->type == Policies::POLICY_XSLT && ((XsltPolicy*)it_p->second)->parent_id != (size_t)-1)
            continue;

        ps.push_back(std::make_pair(it_p->second->id, it_p->second->name));
    }

    return 0;
}

int Policies::erase_xslt_policy_node(std::map<size_t, Policy *>& user_policies, int id, std::string& err)
{
    std::map<size_t, Policy *>::iterator it = user_policies.find(id);
    if (it == user_policies.end())
    {
        err = "Policy is not existing";
        return -1;
    }

    XsltPolicy *policy = (XsltPolicy*)it->second;
    for (size_t i = 0; i < policy->nodes.size(); ++i)
    {
        if (policy->nodes[i] && policy->nodes[i]->kind == XSLT_POLICY_POLICY)
        {
            if (erase_xslt_policy_node(user_policies, ((XsltPolicy*)policy->nodes[i])->id, err) < 0)
                return -1;
            user_policies.erase(user_policies.find(((XsltPolicy*)policy->nodes[i])->id));
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
        err = "User policies are not existing";
        return -1;
    }

    return remove_policy(user, id, err);
}

int Policies::remove_policy(int user, int id, std::string& err)
{
    Policy *p = get_policy(user, id, err);
    if (!p)
        return -1;

    if (p->is_system)
        return 0;

    if (p->type == POLICY_XSLT)
    {
        XsltPolicy* policy = (XsltPolicy*)p;
        erase_xslt_policy_node(policies[user], p->id, err);
        if (policy->parent_id == (size_t)-1)
            remove_saved_policy(p);
        else
        {
            Policy* tmp = get_policy(user, policy->parent_id, err);
            if (tmp)
            {
                size_t i = 0;
                for (; i < ((XsltPolicy*)tmp)->nodes.size(); ++i)
                    if (((XsltPolicy*)tmp)->nodes[i] && ((XsltPolicy*)tmp)->nodes[i]->kind == XSLT_POLICY_POLICY
                        && ((XsltPolicy*)((XsltPolicy*)tmp)->nodes[i])->id == (size_t)id)
                        break;

                if (i != ((XsltPolicy*)tmp)->nodes.size())
                    ((XsltPolicy*)tmp)->nodes.erase(((XsltPolicy*)tmp)->nodes.begin() + i);
            }
        }
    }
    else
        remove_saved_policy(p);

    delete p;
    policies[user].erase(policies[user].find(id));

    return 0;
}

int Policies::clear_policies(int user, std::string& err)
{
    std::map<int, std::map<size_t, Policy*> >::iterator it = policies.find(user);
    if (it == policies.end())
        return 0;

    std::map<size_t, Policy*>::iterator it_p = it->second.begin();
    while (it_p != it->second.end())
    {
        if (!it_p->second || it_p->second->is_system)
        {
            ++it_p;
            continue;
        }

        if (remove_policy(user, (int)it_p->first, err) < 0)
            return -1;

        it_p = it->second.begin();
    }
    return 0;
}

int Policies::policy_change_info(int user, int id, const std::string& name, const std::string& description,
                                 const std::string& license, std::string& err)
{
    Policy *p = get_policy(user, id, err);
    if (!p)
        return -1;

    if (p->is_system)
    {
        err = "Cannot change a system policy";
        return -1;
    }

    p->name = name;
    if (p->type == POLICY_XSLT)
        ((XsltPolicy*)p)->node_name = p->name;
    p->description = description;
    if (license.size())
        p->license = license;

    return 0;
}

int Policies::policy_change_type(int user, int id, const std::string& type, std::string& err)
{
    Policy *p = get_policy(user, id, err);
    if (!p)
        return -1;

    if (p->is_system)
    {
        err = "Cannot change a system policy";
        return -1;
    }

    if (p->type != POLICY_XSLT)
    {
        err = "Not an XSLT policy";
        return -1;
    }

    ((XsltPolicy*)p)->ope = type;

    return 0;
}

int Policies::policy_change_is_public(int user, int id, bool is_public, std::string& err)
{
    Policy *p = get_policy(user, id, err);
    if (!p)
        return -1;

    if (p->is_system)
    {
        err = "Cannot change a system policy";
        return -1;
    }

    if (p->type != POLICY_XSLT)
    {
        err = "Not an XSLT policy";
        return -1;
    }

    if (((XsltPolicy*)p)->parent_id != (size_t)-1)
    {
        err = "Not a root policy";
        return -1;
    }

    p->is_public = is_public;

    return 0;
}

xmlDocPtr Policies::create_doc(int user, int id)
{
    std::string err;
    Policy *p = get_policy(user, id, err);
    if (!p)
        return NULL;

    p->set_keep_public(true);
    return p->create_doc();
}

bool Policies::policy_exists(int user, const std::string& policy)
{
    std::map<int, std::map<size_t, Policy *> >::iterator it = policies.find(user);

    if (it == policies.end())
        add_system_policies_to_user_policies(user);

    it = policies.find(user);
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

int Policies::create_xslt_policy_from_file(int user, long file_id, std::string& err)
{
    MediaConchLib::Checker_ReportRes result;

    CheckerReport cr;
    cr.user = user;
    cr.files.push_back(file_id);
    cr.report_set.set(MediaConchLib::report_MediaInfo);
    cr.format = MediaConchLib::format_Xml;
    if (core->reports.checker_get_report(cr, &result, err) < 0)
        return -1;
    if (!result.valid || !result.report.length())
    {
        err = "Implementation report not found";
        return -1;
    }

    Policy *p = new XsltPolicy(this, !core->accepts_https());

    //Policy filename
    find_save_name(user, NULL, p->filename);

    std::string file;
    core->checker_file_from_id(user, file_id, file, err);
    size_t name_pos = file.rfind("/");
    if (name_pos == std::string::npos)
        name_pos = 0;
    else
        name_pos++;
    p->name = file.substr(name_pos, std::string::npos);
    ((XsltPolicy*)p)->node_name = p->name;

    int pos = -1;
    if (((XsltPolicy*)p)->create_policy_from_mi(result.report) < 0)
        delete p;
    else
    {
        if (policies.find(user) == policies.end())
            add_system_policies_to_user_policies(user);

        policies[user][p->id] = p;
        pos = p->id;
    }

    return pos;
}

int Policies::policy_get_policy_id(Policy* p, const std::map<std::string, std::string>& opts,
                                   std::vector<std::string>& xslt_policies, std::string& err)
{
    if (!p)
        return -1;

    std::string policy;
    if (p->type == POLICY_XSLT)
    {
        if (((XsltPolicy*)p)->get_final_xslt(policy, opts) < 0)
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

int Policies::policy_get_policy_content(const std::string& policy, const std::map<std::string, std::string>& opts,
                                        std::vector<std::string>& xslt_policies, std::string& err)
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

    ret = policy_get_policy_id(p, opts, xslt_policies, err);
    delete p;
    return ret;
}

int Policies::policy_get_policies(int user, const std::vector<size_t>* policies_ids,
                                  const std::vector<std::string>* policies_contents,
                                  const std::map<std::string, std::string>& opts,
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
            if (policy_get_policy_id(get_policy(user, policies_ids->at(i), err), opts, xslt_policies, err) < 0)
                return -1;
    }

    if (policies_contents)
    {
        for (size_t i = 0; i < policies_contents->size(); ++i)
            if (policy_get_policy_content(policies_contents->at(i), opts, xslt_policies, err) < 0)
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

    if (p->is_system)
    {
        err = "Cannot change a system policy";
        return -1;
    }

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

    if (p->is_system)
    {
        err = "Cannot change a system policy";
        return -1;
    }

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

int Policies::duplicate_xslt_policy_rule(int user, int policy_id, int rule_id, int dst_policy_id, std::string& err, bool copy_name)
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

    Policy *destination = get_policy(user, dst_policy_id, err);
    if (!destination)
        return -1;

    if (destination->type != POLICY_XSLT)
    {
        err = "Destination policy is not an XSLT policy";
        return -1;
    }

    if (destination->is_system)
    {
        err = "Destination policy should not be a system policy";
        return -1;
    }

    XsltPolicyRule *rule = new XsltPolicyRule(r);
    if (copy_name && ((XsltPolicy*)destination)->parent_id == policy->parent_id && rule->node_name.size())
        rule->node_name += "_copy";

    ((XsltPolicy*)destination)->nodes.push_back(rule);

    return (int)rule->id;
}

int Policies::move_xslt_policy_rule(int user, int policy_id, int rule_id, int dst_policy_id, std::string& err)
{
    int new_id = duplicate_xslt_policy_rule(user, policy_id, rule_id, dst_policy_id, err, false);
    if (new_id < 0)
        return -1;

    if (delete_xslt_policy_rule(user, policy_id, rule_id, err) < 0)
    {
        std::string tmp;
        delete_xslt_policy_rule(user, dst_policy_id, new_id, tmp);
        return -1;
    }

    return new_id;
}

int Policies::delete_xslt_policy_rule(int user, int policy_id, int rule_id, std::string& err)
{
    Policy *p = get_policy(user, policy_id, err);
    if (!p)
        return -1;

    if (p->is_system)
    {
        err = "Cannot change a system policy";
        return -1;
    }

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

void Policies::find_save_name(int user, const char* basename, std::string& save_name, const char *filename)
{
    std::stringstream data_path;

    data_path << Core::get_local_data_path();
    data_path << "policies/";

    ZenLib::Ztring z_path = ZenLib::Ztring().From_UTF8(data_path.str());
    if (!ZenLib::Dir::Exists(z_path))
        ZenLib::Dir::Create(z_path);

    data_path << user << "/";

    z_path = ZenLib::Ztring().From_UTF8(data_path.str());
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
        if (filename)
        {
            std::string filename_str(filename);
            unified_file_name(filename_str);
            ss << data_path.str() << filename_str;
        }
        else
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

    if (policy->type == POLICY_XSLT && ((const XsltPolicy*)policy)->parent_id != (size_t)-1)
        return;

    ZenLib::Ztring z_path = ZenLib::Ztring().From_UTF8(policy->filename);
    if (!ZenLib::File::Exists(z_path))
        return;

    ZenLib::File::Delete(z_path);
}

void Policies::add_system_policies_to_user_policies(int user)
{
    for (size_t i = 0; i < system_policies.size(); ++i)
    {
        if (!system_policies[i])
            continue;

        Policy* p = NULL;
        if (system_policies[i]->type == POLICY_XSLT)
            p = new XsltPolicy(*(XsltPolicy*)system_policies[i], true);
        else if (system_policies[i]->type == POLICY_UNKNOWN)
            p = new UnknownPolicy(*(UnknownPolicy*)system_policies[i]);

        if (p)
            add_recursively_policy_to_user_policies(user, p);
    }
}

void Policies::add_recursively_policy_to_user_policies(int user, Policy* p)
{
    if (!p)
        return;

    policies[user][p->id] = p;
    if (p->type != POLICY_XSLT)
        return;

    XsltPolicy *policy = (XsltPolicy*)p;
    for (size_t i = 0; i < policy->nodes.size(); ++i)
    {
        if (!policy->nodes[i])
            continue;

        if (policy->nodes[i]->kind == XSLT_POLICY_POLICY)
            add_recursively_policy_to_user_policies(user, (Policy*)(XsltPolicy*)policy->nodes[i]);
    }
}

int Policies::transform_with_xslt_memory(const std::string& report, const std::string& memory,
                                         const std::map<std::string, std::string>& opts,
                                         std::string& result)
{
    return core->reports.transform_with_xslt_memory(report, memory, opts, result);
}

//---------------------------------------------------------------------------
void Policies::unified_file_name(std::string& filename)
{
    size_t pos = 0;
    while (pos != std::string::npos)
    {
        pos = filename.find("/", pos);
        if (pos != std::string::npos)
            filename.replace(pos, 1, "");
    }

    pos = 0;
    while (pos != std::string::npos)
    {
        pos = filename.find(" ", pos);
        if (pos != std::string::npos)
            filename.replace(pos, 1, "");
    }
}

}
