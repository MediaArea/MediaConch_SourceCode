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
#include "Path.h"

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
        typedef enum Level {
            LEVEL_NONE = 0,
            LEVEL_INFO,
            LEVEL_WARN,
            LEVEL_FAIL,
        };

        typedef enum Result {
            RESULT_PASS = 0,
            RESULT_INFO,
            RESULT_WARN,
            RESULT_FAIL,
        };

        Element() {};
        virtual ~Element() {};

        virtual void resolve()=0;
        virtual Result result()=0;
        virtual Level error_level()=0;
        virtual std::string to_string(size_t level, bool verbose=false)=0;

        std::string name;
    };

    class RuleElement : public Element {
    public:
        RuleElement();
        ~RuleElement();

        void reset();
        bool compare(const std::string& value);

        virtual void resolve();
        virtual Result result();
        virtual Level error_level();
        virtual std::string to_string(size_t level, bool verbose=false);

        std::vector<PathElement> path;
        std::string scope;
        std::string level;
        std::string field;
        std::string tracktype;
        std::string occurrence;
        std::string operand;
        std::string xpath;
        std::string requested;
        std::vector<std::string> values;
        std::vector<std::string> failing_values;

    private:
        bool resolved;
        bool pass;
    };

    class PolicyElement : public Element {
    public:
        PolicyElement();
        ~PolicyElement();

        virtual void resolve();
        virtual Result result();
        virtual Level error_level();
        virtual std::string to_string(size_t level, bool verbose=false);

        std::string type;
        std::string level;
        std::string version;
        std::string description;
        std::vector<std::string> tags;
        std::vector<Element*> children;

    private:
        bool resolved;
        size_t pass_count;
        size_t info_count;
        size_t warn_count;
        size_t fail_count;
    };

    RuleElement* parse_rule(tfsxml_string& tfsxml_priv);
    PolicyElement* parse_policy(tfsxml_string& tfsxml_priv);
    void parse_node(tfsxml_string& tfsxml_priv, std::vector<RuleElement*> rules, size_t level);

    std::vector<PolicyElement*> policies;
    std::vector<RuleElement*> rules;
};

}

#endif
