/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Scheduler functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef IMSC1H
#define IMSC1H
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <map>
#include "MediaConchLib.h"
#include "PluginFormat.h"

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Class VeraPDF
//***************************************************************************

class IMSC1 : public PluginFormat
{
public:
    IMSC1();
    virtual ~IMSC1();
    IMSC1(const IMSC1&);

    virtual int load_plugin(const std::map<std::string, Container::Value>& obj, std::string& error);
    virtual int run(std::string& error);

private:
    IMSC1&    operator=(const IMSC1&);

    int         validate_ttml_file(bool& result, std::string& message, std::string& error);
    int         write_shema(const std::string& path, const std::string& file, const char* schema, std::string& error);
    static void xml_error_string(void* data, const char* format, ...);
    static void xml_error_discard(void*, const char* , ...);
};

}

#endif // !IMSC1H
