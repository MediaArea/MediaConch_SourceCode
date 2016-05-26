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
#ifdef HAVE_JANSSON

//---------------------------------------------------------------------------
#ifndef JsonH
#define JsonH

//---------------------------------------------------------------------------
#include <string>
#include <vector>
#include <jansson.h>
#include "Container.h"

#ifndef json_object_foreach
# define json_object_foreach(obj, key, value)                                    \
    for (key = json_object_iter_key(json_object_iter(obj));                      \
         key && (value = json_object_iter_value(json_object_iter_at(obj, key))); \
         key = json_object_iter_key(json_object_iter_next(obj,                   \
                                                json_object_iter_at(obj, key))))
#endif

namespace MediaConch {

//***************************************************************************
// Class Json
//***************************************************************************


class Json : public Container
{
public:
    //Constructor/Destructor
    Json();
    ~Json();

    virtual int    parse(const std::string& data, Value &v);
    virtual int    serialize(Value &v, std::string&);
    virtual Value *get_value_by_key(Value& v, const std::string& key);
    virtual Value *get_value_in_array_by_key(Value& v, const std::string& key);
    
private:
    json_t        *current_node;

    int            parse_node(Value &v);
    int            get_type_node(Value &v);
    json_t        *set_basic_node(Value &v);
    json_t        *serialize_node(Value &v);

    Json(const Json&);
    Json& operator=(const Json&);
};

}

#endif // !JsonH

#endif // !HAVE_JANSSON
