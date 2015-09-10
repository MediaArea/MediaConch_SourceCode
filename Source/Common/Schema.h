/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Schema functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef SchemaH
#define SchemaH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <string>
#include <vector>

namespace MediaConch {

//***************************************************************************
// Class Schema
//***************************************************************************

class Schema
{
public:
    //Constructor/Destructor
    Schema();
    virtual ~Schema();

    bool         register_schema_from_file(const char* filename);
    virtual bool register_schema_from_memory() = 0;
    virtual bool register_schema_from_doc(void* doc) = 0;

    virtual int  validate_xml(const char* xml, size_t len, bool silent=true) = 0;
    virtual int  validate_xml_from_file(const char* file, bool silent=true) = 0;

    std::string  get_schema() const { return schema; }
    std::vector<std::string> get_errors() const { return errors; }

protected:
    std::string              schema;
    std::vector<std::string> errors;

    // HELPER
    std::string  read_file(const char* filename);

private:
    Schema(const Schema&);
    Schema&     operator=(const Schema&);

};

}

#endif
