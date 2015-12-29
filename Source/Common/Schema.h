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
#include <map>

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
    virtual bool register_schema_from_memory(const std::string& schem) = 0;
    virtual bool register_schema_from_doc(void* doc) = 0;

    virtual int  validate_xml(const std::string& xml, bool silent=true) = 0;
    virtual int  validate_xml_from_file(const char* file, bool silent=true);

    std::string  get_schema() const { return schema; }
    std::string  get_report() const { return report; }
    std::vector<std::string> get_errors() const { return errors; }

    void set_options(const std::map<std::string, std::string>& opt) { options = opt; }

protected:
    std::string                        schema;
    std::string                        report;
    std::vector<std::string>           errors;
    std::map<std::string, std::string> options;

    // HELPER
    int          read_file(const char* filename, std::string& buffer);

private:
    Schema(const Schema&);
    Schema&     operator=(const Schema&);

};

}

#endif
