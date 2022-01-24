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
#ifndef IMSC1PLUGINH
#define IMSC1PLUGINH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <string>

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Class VeraPDF
//***************************************************************************

class IMSC1Plugin
{
public:
    IMSC1Plugin(const std::string& wdir);
    ~IMSC1Plugin();

    std::string validate_file(const std::string& filename);
    std::string validate_buffer(const std::string& buffer, const std::string& filename);
    int         validate_ttml_file(const std::string& filename, bool& result, std::string& message, std::string& error);
    int         validate_ttml_buffer(const std::string& buffer, const std::string& filename, bool& result, std::string& message, std::string& error);
    int         create_report(std::string& report, const std::string& filename, bool result, const std::string& message, std::string& error);
    
private:
    int         write_shema(const std::string& path, const std::string& file, const char* schema, std::string& error);
    int         write_shemas(std::string& error);
    int         clean_shemas(std::string& error);
    static void xml_error_string(void* data, const char* format, ...);
    static void xml_error_discard(void*, const char* , ...);

    std::string wdir;
};

}

#endif // !IMSC1PLUGINH
