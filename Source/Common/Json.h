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
#include "Container.h"

namespace MediaConch {

//***************************************************************************
// Class Json
//***************************************************************************


class Json : public Container
{
public:
public:
    //Constructor/Destructor
    Json();
    ~Json();

    virtual int parse(std::string data, Value &v);
    virtual std::string serialize(Value &v);
    virtual Value* get_value_by_key(Value& v, std::string key);
    virtual Value* get_value_in_array_by_key(Value& v, std::string key);
    
private:
    json_t       *current_node;

    int parse_node(Value &v);
    int get_type_node(Value &v);
    json_t *set_basic_node(Value &v);
    json_t *serialize_node(Value &v);

    Json(const Json&);
    Json& operator=(const Json&);
};

}

#endif
