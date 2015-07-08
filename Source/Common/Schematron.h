/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Schematron functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef SchematronH
#define SchematronH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <libxml/schematron.h>
#include <string>
#include <vector>
using namespace std;

//***************************************************************************
// Class Schematron
//***************************************************************************

class Schematron
{
public:
    //Constructor/Destructor
    Schematron();
    ~Schematron();
    bool             register_schema_from_file(const char* filename);
    bool             register_schema_from_memory();
    bool             register_schema_from_doc(xmlDocPtr doc);

    int              validate_xml(const char* xml, size_t len, bool silent=true);
    int              validate_xml_from_file(const char* file, bool silent=true);

    string           get_schema() const { return schema; }

    // HELPER
    string           read_file(const char* filename);
    static void      manage_generic_error(void *userData, const char* msg, ...);
    static void      manage_error(void *userData, xmlErrorPtr err);

    vector<string>   errors;
private:
    Schematron (const Schematron&);
    Schematron&      operator=(const Schematron&);

    string           schema;
    xmlSchematronPtr schematron_ctx;
};

#endif
