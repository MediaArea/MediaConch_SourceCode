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
Schema::Schema()
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

}
