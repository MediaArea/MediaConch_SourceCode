/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "Checker.h"

#include <functional>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <sstream>

//TODO: mmt
//TODO: reference_file

//---------------------------------------------------------------------------
// Helpers
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
std::string xml_encode(const std::string& data)
{
    std::stringstream ss;
    for (size_t pos=0; pos<data.size(); pos++)
    {
        switch (data[pos])
        {
            case '\'': ss << "&apos;"; break;
            case '"': ss << "&quot;"; break;
            case '&': ss << "&amp;"; break;
            case '<': ss << "&lt;"; break;
            case '>': ss << "&gt;"; break;
            case '\t': ss << "&#x9;"; break;
            case '\n': ss << "&#xA;"; break;
            case '\r':
                ss << "&#xA;";
                if (pos+1<data.size() && data[pos+1]=='\n') // translate the #xD #xA sequence to a single #xA character
                    pos++;
            break;
            default: if ((unsigned char)data[pos]>=0x20) ss << data[pos]; // Ignore others control characters
        }
    }
    return ss.str();
}

//---------------------------------------------------------------------------
std::string indent(size_t level)
{
    return std::string( level*2, ' ');
}

//---------------------------------------------------------------------------
int tfsxml_next_named(tfsxml_string* tfsxml_priv, tfsxml_string* result, const char* name)
{
    while (!tfsxml_next(tfsxml_priv, result))
    {
        if (!tfsxml_strcmp_charp(*result, name))
            return 0;
    }

    return -1;
}

//---------------------------------------------------------------------------
std::string tokenize(const std::string& scope, const std::string& list, const std::string& delimiter)
{
    std::stringstream ss;

    //TODO: mmt
    if (scope.empty() || scope=="mi")
    {
        std::istringstream iss(list);
        for (std::string token; std::getline(iss, token, '/');)
            ss << "/mi:" << token;
    }

    return ss.str();
}


//---------------------------------------------------------------------------
namespace MediaConch {

//---------------------------------------------------------------------------
// Policy
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
PolicyChecker::PolicyElement::PolicyElement() : resolved(false), pass_count(0), info_count(0), warn_count(0), fail_count(0)
{
}

//---------------------------------------------------------------------------
PolicyChecker::PolicyElement::~PolicyElement()
{
    while (children.size())
    {
        delete children.back();
        children.pop_back();
    }
}

//---------------------------------------------------------------------------
PolicyChecker::Element::Level PolicyChecker::PolicyElement::error_level()
{
    if (level=="info")
        return LEVEL_INFO;
    else if (level=="warn")
        return LEVEL_WARN;
    else if (level=="fail")
        return LEVEL_FAIL;

    return LEVEL_NONE;
}

//---------------------------------------------------------------------------
void PolicyChecker::PolicyElement::resolve()
{
    for (size_t pos=0; pos<children.size(); pos++)
    {
        switch (children[pos]->result())
        {
        case RESULT_PASS:
            pass_count++; break;
        case RESULT_INFO:
            info_count++; break;
        case RESULT_WARN:
            warn_count++; break;
        case RESULT_FAIL:
            fail_count++; break;
        default:
            break;
        }
    }

    resolved=true;
}

//---------------------------------------------------------------------------
PolicyChecker::Element::Result PolicyChecker::PolicyElement::result()
{
    if(!resolved)
        resolve();

    Result to_return=RESULT_PASS;

    if (type=="and")
    {
        if (fail_count)
            to_return=RESULT_FAIL;
        else if (warn_count)
            to_return=RESULT_WARN;
        else if (info_count)
            to_return=RESULT_INFO;
        else
            to_return=RESULT_PASS;
    }
    else if (type=="or")
    {
        if (fail_count && !warn_count && !info_count && !pass_count)
            to_return=RESULT_FAIL;
        else if (warn_count && !info_count && !pass_count)
            to_return=RESULT_WARN;
        else if (info_count && !pass_count)
            to_return=RESULT_INFO;
        else
            to_return=RESULT_PASS;
    }

    if (error_level()>LEVEL_NONE)
    {
        if (to_return>RESULT_INFO && error_level()==LEVEL_INFO)
            to_return=RESULT_INFO;
        else if (to_return>RESULT_WARN && error_level()==LEVEL_WARN)
            to_return=RESULT_WARN;
    }



    return to_return;
}

//---------------------------------------------------------------------------
std::string PolicyChecker::PolicyElement::to_string(size_t level, bool verbose)
{
    if(!resolved)
        resolve();

    std::string outcome_str;
    switch (result())
    {
    case RESULT_FAIL:
        outcome_str="fail"; break;
    case RESULT_WARN:
        outcome_str="warn"; break;
    case RESULT_INFO:
        outcome_str="info"; break;
    case RESULT_PASS:
        outcome_str="pass"; break;
    }

    std::stringstream ss;

    ss << indent(level) << "<policy name=\"" << xml_encode(name) << "\" type=\"" << xml_encode(type);
    if (!this->level.empty())
        ss << "\" level=\"" << xml_encode(this->level);
    ss << "\" rules_run=\"" << children.size() << "\" pass_count=\"" << pass_count << "\" info_count=\"" << info_count << "\" warn_count=\"" << warn_count
       << "\" fail_count=\"" << fail_count << "\" outcome=\"" << outcome_str << "\">" << std::endl;
    level++;
    if (!description.empty())
        ss << indent(level) << "<description>" << description << "</description>" << std::endl;
    for (size_t pos=0; pos<tags.size(); pos++)
        ss << indent(level) << "<tag>" << tags[pos] << "</tag>" << std::endl;
    for (size_t pos=0; pos<children.size(); pos++)
        ss << children[pos]->to_string(level, verbose);
    level--;
    ss << indent(level) << "</policy>" << std::endl;

    return ss.str();
}

//---------------------------------------------------------------------------
// Rule
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
PolicyChecker::RuleElement::RuleElement() : resolved(false), pass(false)
{
}

//---------------------------------------------------------------------------
PolicyChecker::RuleElement::~RuleElement()
{
}

//---------------------------------------------------------------------------
void PolicyChecker::RuleElement::reset()
{
    resolved=false;
    pass=false;
    values.clear();
}

//---------------------------------------------------------------------------
bool  PolicyChecker::RuleElement::compare(const std::string& value)
{
    bool to_return = false;

    char* val_end=NULL;
    double val = strtod(value.c_str(), &val_end);
    char* ref_end=NULL;
    double ref = strtod(requested.c_str(), &ref_end);

    if (operand=="starts with")
    {
        to_return = value.rfind(requested, 0) == 0;
    }
    else if (operand=="must not start with")
    {
        to_return = value.rfind(requested, 0) != 0;
    }
    else if (operand == "<")
    {
        if (!strlen(val_end) && !strlen(ref_end))
            to_return = val < ref;
        else
            to_return = strcmp(value.c_str(), requested.c_str()) < 0;
    }
    else if (operand == "<=")
    {
        if (!strlen(val_end) && !strlen(ref_end))
            to_return = val <= ref;
        else
            to_return = strcmp(value.c_str(), requested.c_str()) <= 0;
    }
    else if (operand == "=")
    {
        if (!strlen(val_end) && !strlen(ref_end))
            to_return = val == ref;
        else
            to_return = strcmp(value.c_str(), requested.c_str()) == 0;
    }
    else if (operand == ">=")
    {
        if (!strlen(val_end) && !strlen(ref_end))
            to_return = val >= ref;
        else
            to_return = strcmp(value.c_str(), requested.c_str()) >= 0;
    }
    else if (operand == ">")
    {
        if (!strlen(val_end) && !strlen(ref_end))
            to_return = val > ref;
        else
            to_return = strcmp(value.c_str(), requested.c_str()) > 0;
    }

    if (!to_return)
        failing_values.push_back(value);

    return to_return;
}

//---------------------------------------------------------------------------
PolicyChecker::Element::Level PolicyChecker::RuleElement::error_level()
{
    if (level=="info")
        return LEVEL_INFO;
    else if (level=="warn")
        return LEVEL_WARN;
    else if (level=="fail")
        return LEVEL_FAIL;

    return LEVEL_NONE;
}

//---------------------------------------------------------------------------
void PolicyChecker::RuleElement::resolve()
{
    if (operand=="exists")
    {
            pass=!values.empty();
    }
    else if (operand=="must not exist")
    {
            pass=values.empty();
    }
    else if (operand=="starts with" || operand=="must not start with" || operand=="<" || operand=="<=" || operand=="=" || operand==">=" || operand==">")
    {
       if (values.empty())
          pass = operand=="must not start with" ? true : false;
       else if (occurrence=="all")
           pass = std::all_of(values.begin(), values.end(), std::bind(&PolicyChecker::RuleElement::compare, this, std::placeholders::_1));
       else
           pass = std::any_of(values.begin(), values.end(), std::bind(&PolicyChecker::RuleElement::compare, this, std::placeholders::_1));
    }

    resolved=true;
}

//---------------------------------------------------------------------------
PolicyChecker::Element::Result PolicyChecker::RuleElement::result()
{
    if(!resolved)
        resolve();

    if (!pass)
    {
        if (error_level()==LEVEL_INFO)
            return RESULT_INFO;
        else if (error_level()==LEVEL_WARN)
            return RESULT_WARN;
        else
            return RESULT_FAIL;
    }
    return RESULT_PASS;
}

//---------------------------------------------------------------------------
std::string PolicyChecker::RuleElement::to_string(size_t level, bool verbose)
{
    std::stringstream ss;

    std::string outcome_str;
    switch (result())
    {
    case RESULT_FAIL:
        outcome_str="fail"; break;
    case RESULT_WARN:
        outcome_str="warn"; break;
    case RESULT_INFO:
        outcome_str="info"; break;
    case RESULT_PASS:
        outcome_str="pass"; break;
    }

    std::string value_str;
    if (result()>RESULT_PASS)
        value_str=failing_values.size() ? failing_values.front() : std::string();
    else if (verbose)
        value_str=values.size() ? values.front() : std::string();

    ss << indent(level) << "<rule name=\"" << xml_encode(name) << "\"";
    if (!scope.empty())
        ss << " scope=\"" << xml_encode(scope) << "\"";
    ss << " value=\"" << xml_encode(field) << "\" tracktype=\"" << xml_encode(tracktype) << "\" occurrence=\"" << xml_encode(occurrence)
       << "\" operator=\"" << xml_encode(operand) << "\" xpath=\"" << xpath << "\"";
    if (!this->level.empty())
        ss << " level=\"" << xml_encode(this->level) << "\"";
    if (result()>RESULT_PASS || verbose)
        ss << " requested=\"" << xml_encode(requested) << "\" actual=\"" << xml_encode(value_str) << "\"";
    ss << " outcome=\"" << outcome_str << "\"/>" << std::endl;

    return ss.str();
}

//---------------------------------------------------------------------------
// PolicyChecker
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
PolicyChecker::PolicyChecker() : supported(true), full(false)
{
}

//---------------------------------------------------------------------------
PolicyChecker::~PolicyChecker()
{
    while (policies.size())
    {
        delete policies.back();
        policies.pop_back();
    }
}

//---------------------------------------------------------------------------
bool PolicyChecker::is_policy_supported()
{
    return supported;
}


//---------------------------------------------------------------------------
bool PolicyChecker::full_parse()
{
    return full;
}

//---------------------------------------------------------------------------
PolicyChecker::RuleElement* PolicyChecker::parse_rule(tfsxml_string& tfsxml_priv)
{
    RuleElement* rule=new RuleElement();

    tfsxml_string attribute_name;
    tfsxml_string attribute_value;
    while (!tfsxml_attr(&tfsxml_priv, &attribute_name, &attribute_value))
    {
        if (!tfsxml_strcmp_charp(attribute_name, "name"))
            rule->name=tfsxml_decode(attribute_value);
        else if (!tfsxml_strcmp_charp(attribute_name, "scope"))
            rule->scope=tfsxml_decode(attribute_value);
        else if (!tfsxml_strcmp_charp(attribute_name, "level"))
            rule->level=tfsxml_decode(attribute_value);
        else if (!tfsxml_strcmp_charp(attribute_name, "value"))
            rule->field=tfsxml_decode(attribute_value);
        else if (!tfsxml_strcmp_charp(attribute_name, "tracktype"))
            rule->tracktype=tfsxml_decode(attribute_value);
        else if (!tfsxml_strcmp_charp(attribute_name, "occurrence"))
            rule->occurrence=tfsxml_decode(attribute_value);
        else if (!tfsxml_strcmp_charp(attribute_name, "operator"))
            rule->operand=tfsxml_decode(attribute_value);
    }

    std::string occurrence=rule->occurrence;
    if (occurrence.empty() || occurrence=="all" || occurrence=="any")
        occurrence="*";

    tfsxml_string value;
    if (!tfsxml_value(&tfsxml_priv, &value))
        rule->requested=std::string(value.buf, value.len);

    // Check for presence of _StringX or TimeCode* element in policy
    size_t index=rule->field.find_last_not_of("0123456789");
    if ((index!=std::string::npos && index>=6 && rule->field.substr(index-6, 7)=="_String") || rule->field.find("TimeCode")==0)
        full=true;

// Check for currently unsupported features
    if (rule->scope=="mmt" || rule->requested=="compare")
        supported=false;

    if (rule->scope.empty() || rule->scope=="mi")
    {
        std::stringstream ss;
        ss << "mi:MediaInfo/mi:track[@type='" << rule->tracktype << "'][" << occurrence << "]" << tokenize(rule->scope, rule->field, "/");
        rule->xpath=ss.str();
        rule->path=parse_path(rule->xpath);
    }

    rules.push_back(rule);

    return rule;
}

//---------------------------------------------------------------------------
PolicyChecker::PolicyElement* PolicyChecker::parse_policy(tfsxml_string& tfsxml_priv)
{
    PolicyElement* policy=new PolicyElement();

    tfsxml_string attribute_name;
    tfsxml_string attribute_value;
    while (!tfsxml_attr(&tfsxml_priv, &attribute_name, &attribute_value))
    {
        if (!tfsxml_strcmp_charp(attribute_name, "name"))
            policy->name=tfsxml_decode(attribute_value);
        else if (!tfsxml_strcmp_charp(attribute_name, "type"))
            policy->type=tfsxml_decode(attribute_value);
        else if (!tfsxml_strcmp_charp(attribute_name, "level"))
            policy->level=tfsxml_decode(attribute_value);
        else if (!tfsxml_strcmp_charp(attribute_name, "version"))
            policy->version=tfsxml_decode(attribute_value);
    }

   tfsxml_string result;
    if (!tfsxml_enter(&tfsxml_priv))
    {
        while (!tfsxml_next(&tfsxml_priv, &result))
        {
            if (!tfsxml_strcmp_charp(result, "description"))
            {
                tfsxml_string result;
                if (!tfsxml_value(&tfsxml_priv, &result))
                    policy->description=std::string(result.buf, result.len);
            }
            else if (!tfsxml_strcmp_charp(result, "tag"))
            {
                tfsxml_string result;
                if (!tfsxml_value(&tfsxml_priv, &result))
                    policy->tags.push_back(std::string(result.buf, result.len));
            }
            else if (!tfsxml_strcmp_charp(result, "policy"))
                policy->children.push_back(parse_policy(tfsxml_priv));
            else if (!tfsxml_strcmp_charp(result, "rule"))
                policy->children.push_back(parse_rule(tfsxml_priv));
        }
    }

    return policy;
}

//---------------------------------------------------------------------------
void PolicyChecker::add_policy(const std::string& policy)
{
    tfsxml_string tfsxml_priv;
    tfsxml_string result;

    tfsxml_init(&tfsxml_priv, (const void*)policy.c_str(), policy.size());
    while (!tfsxml_next(&tfsxml_priv, &result))
    {
        if (!tfsxml_strcmp_charp(result, "policy"))
        {
            PolicyElement* p=parse_policy(tfsxml_priv);
            if(p)
                policies.push_back(p);
            break;
        }
    }
}

//---------------------------------------------------------------------------
void PolicyChecker::parse_node(tfsxml_string& tfsxml_priv, std::vector<RuleElement*> rules, size_t level)
{
    if (rules.empty())
        return;

    tfsxml_string tfsxml_priv_save = tfsxml_priv;
    if (!tfsxml_enter(&tfsxml_priv))
    {
        std::map<PathElement*, size_t> occurrences;

        tfsxml_string result;
        while (!tfsxml_next(&tfsxml_priv, &result))
        {
            std::vector<RuleElement*> matching_rules;

            std::string field = std::string(result.buf, result.len);
            for (size_t pos = 0; pos < rules.size(); pos++)
            {
                if (level < rules[pos]->path.size() && path_is_matching(tfsxml_priv, result, rules[pos]->path[level], occurrences[&rules[pos]->path[level]]))
                {
                    tfsxml_string tfsxml_priv_copy = tfsxml_priv;
                    if (level == rules[pos]->path.size() - 1)
                    {
                        tfsxml_string value;
                        if (!tfsxml_value(&tfsxml_priv_copy, &value))
                            rules[pos]->values.push_back(std::string(value.buf, value.len));
                    }
                    else
                        matching_rules.push_back(rules[pos]);
                }
            }

            if (!matching_rules.empty())
                parse_node(tfsxml_priv, matching_rules, level + 1);
        }
    }
    tfsxml_priv=tfsxml_priv_save;
}

//---------------------------------------------------------------------------
int PolicyChecker::analyze(const std::string& report, bool verbose, std::string& out)
{
    std::stringstream ss;
    size_t level=0;

    ss << indent(level++) << "<MediaConch xmlns=\"https://mediaarea.net/mediaconch\" xmlns:mmt=\"https://mediaarea.net/micromediatrace\" xmlns:mi=\"https://mediaarea.net/mediainfo\" version=\"0.3\">" << std::endl;

    tfsxml_string tfsxml_priv;
    tfsxml_string result;
    tfsxml_init(&tfsxml_priv, (const void*)report.c_str(), report.size());
    while (!tfsxml_next_named(&tfsxml_priv, &result, "MediaArea"))
    {
        if (!tfsxml_enter(&tfsxml_priv))
        {
            while (!tfsxml_next_named(&tfsxml_priv, &result, "media"))
            {
                std::string media;
                tfsxml_string attribute_name;
                tfsxml_string attribute_value;
                while (!tfsxml_attr(&tfsxml_priv, &attribute_name, &attribute_value))
                {
                    if (!tfsxml_strcmp_charp(attribute_name, "ref"))
                        media=std::string(attribute_value.buf, attribute_value.len);
                }

                ss << indent(level++) << "<media ref=\"" << media << "\">" << std::endl;
                parse_node(tfsxml_priv, rules, 0);
                for (size_t pos=0; pos<policies.size(); pos++)
                    ss << policies[pos]->to_string(level, verbose);
                ss << indent(--level) << "</media>" << std::endl;

                // reset states
                for (size_t pos=0; pos < rules.size(); pos++)
                    rules[pos]->reset();
            }
        }
    }

    ss << indent(--level) << "</MediaConch>" << std::endl;

    out = ss.str();
    return 0;
}

}

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(mediaconch_policychecker) {
    emscripten::class_<MediaConch::PolicyChecker>("PolicyChecker")
        .constructor()
        .function("is_policy_supported", &MediaConch::PolicyChecker::is_policy_supported)
        .function("full_parse", &MediaConch::PolicyChecker::full_parse)
        .function("add_policy", &MediaConch::PolicyChecker::add_policy)
        .function("analyze", emscripten::optional_override([](MediaConch::PolicyChecker& self, const std::string& report, bool verbose) {
            std::string out;
            self.MediaConch::PolicyChecker::analyze(report, verbose, out);

            return out;
        }))
    ;
}
#endif //__EMSCRIPTEN__

