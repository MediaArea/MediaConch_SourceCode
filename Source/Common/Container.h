/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
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
        std::vector<std::pair<std::string, Value> > obj;
    };

public:
    //Constructor/Destructor
    Container() {}
    virtual ~Container() {}

    virtual int parse(std::string data, Value &v) = 0;
    virtual std::string serialize(Value &v) = 0;
    virtual Value* get_value_by_key(Value& v, std::string key) = 0;

    std::string get_error() const { return error; }

protected:
    std::string error;

private:
    Container(const Container&);
    Container& operator=(const Container&);
};

}

#endif
