/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Xslt functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef XsltH
#define XsltH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <string>
#include "Schema.h"

namespace MediaConch {

//***************************************************************************
// Class Xslt
//***************************************************************************

class Xslt : public Schema
{
public:
    //Constructor/Destructor
    Xslt(bool no_https);
    virtual ~Xslt();

    virtual bool register_schema_from_memory(const std::string& schem);
    virtual bool register_schema_from_doc(void* doc);

    virtual int  validate_xml(const std::string& xml, bool silent=true);

    // Callbacks
    static void  manage_generic_error(void *userData, const char* msg, ...);
    static void  manage_error(void *userData, xmlErrorPtr err);

private:
    Xslt(const Xslt&);
    Xslt&  operator=(const Xslt&);

    xsltStylesheetPtr xslt_ctx;
    xmlDocPtr         doc_ctx;
};

}

#endif
