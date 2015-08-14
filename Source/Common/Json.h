/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// JSON functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef JsonH
#define JsonH
//---------------------------------------------------------------------------

#include <string>
#include <vector>
#include <jansson.h>

namespace MediaConch {

//***************************************************************************
// Class Json
//***************************************************************************


class Json
{
public:
    struct Value
    {
        enum JsonValueType
        {
            JSON_TYPE_INTEGER,
            JSON_TYPE_REAL,
            JSON_TYPE_STRING,
            JSON_TYPE_BOOL,
            JSON_TYPE_NULL,
            JSON_TYPE_ARRAY,
            JSON_TYPE_OBJECT,
        };

        JsonValueType                               type;
        long                                        l;
        double                                      d;
        bool                                        b;
        std::string                                 s;
        std::vector<Value>                          array;
        std::vector<std::pair<std::string, Value> > obj;
    };

public:
    //Constructor/Destructor
    Json();
    ~Json();

    int parse(std::string data, Value &v);
    std::string serialize(Value &v);
    Value* get_value_by_key(Value& v, std::string key);
    
    json_error_t* get_error() const { return error; }
private:
    json_t       *current_node;
    json_error_t *error;

    int parse_node(Value &v);
    int get_type_node(Value &v);
    json_t *set_basic_node(Value &v);
    json_t *serialize_node(Value &v);

    Json(const Json&);
    Json& operator=(const Json&);
};

}

#endif
