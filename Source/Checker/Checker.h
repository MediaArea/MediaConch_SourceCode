/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Core functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef CheckerH
#define CheckerH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "ThirdParty/tfsxml/tfsxml.h"

#include <string>
#include <vector>
#include <map>

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

//---------------------------------------------------------------------------
class PolicyChecker
{
public:
    PolicyChecker();
    ~PolicyChecker();

    bool full_parse();
    void add_policy(const std::string& policy);
    int analyze(const std::string& report, bool verbose, std::string& out);

private:
    class Element
    {
    public:
        Element() {};
        virtual ~Element() {};


        virtual bool result()=0;
        virtual void resolve()=0;
        virtual std::string to_string(size_t level, bool verbose=false)=0;

        std::string name;
    };

    class RuleElement : public Element {
    public:
        RuleElement();
        ~RuleElement();

        void reset();

        virtual bool result();
        virtual void resolve();
        virtual std::string to_string(size_t level, bool verbose=false);

        std::string scope;
        std::string field;
        std::string tracktype;
        std::string occurrence;
        std::string operande;
        std::string xpath;
        std::string requested;
        std::map<std::string, std::string> values;

    private:
        bool resolved;
        bool pass;
    };

    class PolicyElement : public Element {
    public:
        PolicyElement();
        ~PolicyElement();

        virtual bool result();
        virtual void resolve();
        virtual std::string to_string(size_t level, bool verbose=false);

        std::string type;
        std::string version;
        std::string description;
        std::vector<std::string> tags;
        std::vector<Element*> children;

    private:
        bool resolved;
        size_t pass_count;
        size_t fail_count;
    };

    RuleElement* parse_rule(tfsxml_string& tfsxml_priv);
    PolicyElement* parse_policy(tfsxml_string& tfsxml_priv);
    void parse_mi_track(tfsxml_string& tfsxml_priv);

    std::vector<PolicyElement*> policies;
    std::vector<RuleElement*> rules;
};

}

#endif
