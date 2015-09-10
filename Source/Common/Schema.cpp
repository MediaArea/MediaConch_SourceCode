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
}

//---------------------------------------------------------------------------
bool Schema::register_schema_from_file(const char* filename)
{
    schema = read_file(filename);
    if (!schema.length()) {
        std::stringstream error;

        error << "Schema file:'";
        error << filename;
        error << "' does not exist\n";
        errors.push_back(error.str());
        return false;
    }

    return register_schema_from_memory();
}

//---------------------------------------------------------------------------
std::string Schema::read_file(const char* filename)
{
    // open at the end (ate) to get the length of the file
    std::ifstream file_handler(filename, std::ios_base::ate);
    std::string buffer;

    if (!file_handler) {
        return buffer;
    }
    buffer.reserve(file_handler.tellg());
    file_handler.seekg(0, file_handler.beg);

    buffer.assign(std::istreambuf_iterator<char>(file_handler),
                  std::istreambuf_iterator<char>());
    file_handler.close();
    return buffer;
}

}
