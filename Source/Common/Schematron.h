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
#include "Schema.h"

namespace MediaConch {

//***************************************************************************
// Class Schematron
//***************************************************************************

class Schematron : public Schema
{
public:
    //Constructor/Destructor
    Schematron();
    virtual ~Schematron();

    virtual bool register_schema_from_memory(const std::string& schem);
    virtual bool register_schema_from_doc(void* doc);

    virtual int  validate_xml(std::string&, bool silent=true);

    // Callbacks
    static void  manage_generic_error(void *userData, const char* msg, ...);
    static void  manage_error(void *userData, xmlErrorPtr err);

private:
    Schematron(const Schematron&);
    Schematron&  operator=(const Schematron&);

    xmlSchematronPtr schematron_ctx;
};

}

#endif
