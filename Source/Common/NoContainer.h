/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// NOCONTAINER functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef NoContainerH
#define NoContainerH

//---------------------------------------------------------------------------
#include "Container.h"

namespace MediaConch {

//***************************************************************************
// Class NoContainer
//***************************************************************************


class NoContainer : public Container
{
public:
    //Constructor/Destructor
    NoContainer() {}
    virtual ~NoContainer() {}

    virtual int parse(std::string, Value&) { return -1; } //TODO
    virtual std::string serialize(Value &v) { return std::string(); }
    virtual Value* get_value_by_key(Value& v, std::string key) { return NULL; }
    virtual Value* get_value_in_array_by_key(Value& v, std::string key) { return NULL; }

private:
    NoContainer(const NoContainer&);
    NoContainer& operator=(const NoContainer&);
};

}

#endif // !NoContainerH
