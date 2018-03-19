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
#include "Schema.h"
#include <fstream>
#include <sstream>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Schema::Schema(bool n_https) : no_https(n_https)
{
}

//---------------------------------------------------------------------------
Schema::~Schema()
{
    schema.clear();
    report.clear();
}

//---------------------------------------------------------------------------
bool Schema::register_schema_from_file(const char* filename)
{
    if (read_file(filename, schema) < 0)
    {
        std::stringstream error;

        error << "Schema file:'";
        error << filename;
        error << "' does not exist\n";
        errors.push_back(error.str());
        return false;
    }

    return register_schema_from_memory(schema);
}

//---------------------------------------------------------------------------
bool Schema::register_schema_from_memory(const std::string&)
{
    return false;
}

//---------------------------------------------------------------------------
bool Schema::register_schema_from_doc(void*)
{
    return false;
}

//---------------------------------------------------------------------------
int Schema::read_file(const char* filename, std::string& buffer)
{
    // open at the end (ate) to get the length of the file
    std::ifstream file_handler(filename, std::ios_base::ate);

    buffer = std::string();
    if (!file_handler)
        return -1;

    int size = file_handler.tellg();
    if (size < 0)
        return -1;
    buffer.reserve(size);
    file_handler.seekg(0, file_handler.beg);

    buffer.assign(std::istreambuf_iterator<char>(file_handler),
                  std::istreambuf_iterator<char>());
    file_handler.close();
    return 0;
}

//---------------------------------------------------------------------------
int Schema::validate_xml_from_file(const char* filename, bool silent)
{
    std::string xml;

    if (read_file(filename, xml) < 0)
        return -1;

    return validate_xml(xml, silent);
}

//---------------------------------------------------------------------------
int Schema::validate_xml(const std::string&, bool)
{
    return -1;
}

//***************************************************************************
// Callbacks
//***************************************************************************

//---------------------------------------------------------------------------
void Schema::manage_error(void *userData, xmlErrorPtr err)
{
    Schema *obj = (Schema *)userData;
    if (!err || err->code == XML_ERR_OK)
        return;

    obj->errors.push_back(err->message);
}

//---------------------------------------------------------------------------
void Schema::manage_generic_error(void *userData, const char* msg, ...)
{
    Schema *obj = (Schema *)userData;
    va_list args;
    char buf[4096] = {0};

    va_start(args, msg);
#ifdef _MSC_VER
    int ret = vsnprintf_s(buf, sizeof(buf), _TRUNCATE, msg, args);
#else //_MSC_VER
    int ret = vsnprintf(buf, sizeof(buf), msg, args);
    if (ret < 0)
        ret = 0;
    buf[ret] = '\0';
#endif //_MSC_VER
    obj->errors.push_back(buf);
    va_end(args);
}

}
