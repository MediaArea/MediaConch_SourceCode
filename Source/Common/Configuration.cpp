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
#include <sstream>
#include <iostream>
#include <fstream>
#include "Core.h"
#include "Configuration.h"
#include "Json.h"
#include "NoContainer.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Configuration
//***************************************************************************

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Configuration::Configuration()
{
    parse_error = true;
#ifdef HAVE_JANSSON
    c = new Json;
#else
    c = new NoContainer;
#endif

}

//---------------------------------------------------------------------------
Configuration::~Configuration()
{
    if (c)
    {
        delete c;
        c = NULL;
    }
}

//---------------------------------------------------------------------------
void Configuration::set_file(const std::string& file)
{
    this->file = file;
}

//---------------------------------------------------------------------------
std::string Configuration::get_error() const
{
    return error;
}

//---------------------------------------------------------------------------
int Configuration::parse()
{
    parse_error = true;

    std::ifstream file_handler(file.c_str(), std::ios_base::ate);

    if (!file_handler)
    {
        std::stringstream streamError;
        streamError << "Cannot open file: " << file;
        error = streamError.str();
        return -1;
    }

    std::string buffer;
    buffer.reserve(file_handler.tellg());
    file_handler.seekg(0, file_handler.beg);
    buffer.assign(std::istreambuf_iterator<char>(file_handler),
                  std::istreambuf_iterator<char>());
    file_handler.close();

    if (c->parse(buffer.c_str(), values) < 0)
    {
        error = c->get_error();
        return -1;
    }

    parse_error = false;
    return 0;
}

//---------------------------------------------------------------------------
int Configuration::get(const std::string& key, std::string &val)
{
    Container::Value *v = get_value_for_key(key);

    if (!v)
        return -1;

    if (v->type != Container::Value::CONTAINER_TYPE_STRING)
        return -1;
    val = v->s;
    return 0;
}

//---------------------------------------------------------------------------
int Configuration::get(const std::string& key, long &val)
{
    Container::Value *v = get_value_for_key(key);

    if (!v)
        return -1;

    if (v->type != Container::Value::CONTAINER_TYPE_INTEGER)
        return -1;
    val = v->l;
    return 0;
}

//---------------------------------------------------------------------------
int Configuration::get(const std::string& key, double &val)
{
    Container::Value *v = get_value_for_key(key);

    if (!v)
        return -1;

    if (v->type != Container::Value::CONTAINER_TYPE_REAL)
        return -1;
    val = v->d;
    return 0;
}

//---------------------------------------------------------------------------
int Configuration::get(const std::string& key, bool &val)
{
    Container::Value *v = get_value_for_key(key);

    if (!v)
        return -1;

    if (v->type != Container::Value::CONTAINER_TYPE_BOOL)
        return -1;
    val = v->b;
    return 0;
}

//---------------------------------------------------------------------------
int Configuration::get(const std::string& key)
{
    Container::Value *v = get_value_for_key(key);

    if (!v)
        return -1;

    if (v->type != Container::Value::CONTAINER_TYPE_NULL)
        return -1;
    return 0;
}

//---------------------------------------------------------------------------
int Configuration::get(const std::string& key, std::vector<Container::Value>& val)
{
    Container::Value *v = get_value_for_key(key);

    if (!v)
        return -1;

    if (v->type != Container::Value::CONTAINER_TYPE_ARRAY)
        return -1;
    val = v->array;
    return 0;
}

//---------------------------------------------------------------------------
int Configuration::get(const std::string& key, std::vector<std::pair<std::string, Container::Value> >& val)
{
    Container::Value *v = get_value_for_key(key);

    if (!v)
        return -1;

    if (v->type != Container::Value::CONTAINER_TYPE_OBJECT)
        return -1;
    val = v->obj;
    return 0;
}

//---------------------------------------------------------------------------
Container::Value *Configuration::get_value_for_key(const std::string& key)
{
    if (parse_error)
        return NULL;
    return c->get_value_in_array_by_key(values, key);
}

}
