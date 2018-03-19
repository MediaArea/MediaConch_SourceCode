/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// CONTAINER functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef ContainerH
#define ContainerH
//---------------------------------------------------------------------------

#include <string>
#include <map>
#include <vector>

namespace MediaConch {

//***************************************************************************
// Class Container
//***************************************************************************


class Container
{
public:
    struct Value
    {
        enum ContainerValueType
        {
            CONTAINER_TYPE_INTEGER,
            CONTAINER_TYPE_REAL,
            CONTAINER_TYPE_STRING,
            CONTAINER_TYPE_BOOL,
            CONTAINER_TYPE_NULL,
            CONTAINER_TYPE_ARRAY,
            CONTAINER_TYPE_OBJECT,
        };

        ContainerValueType                          type;
        long                                        l;
        double                                      d;
        bool                                        b;
        std::string                                 s;
        std::vector<Value>                          array;
        std::map<std::string, Value>                obj;
    };

public:
    //Constructor/Destructor
    Container() {}
    virtual ~Container() {}

    virtual int parse(const std::string& data, Value &v) = 0;
    virtual int serialize(Value &v, std::string&) = 0;
    virtual Value* get_value_by_key(Value& v, const std::string& key) = 0;
    virtual Value* get_value_in_array_by_key(Value& v, const std::string& key) = 0;

    std::string get_error() const { return error; }

protected:
    std::string error;

private:
    Container(const Container&);
    Container& operator=(const Container&);
};

}

#endif
