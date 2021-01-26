/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Public DLL interface implementation
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "Lib.h"
#include "MediaConchDLL.h"

#include <ZenLib/CriticalSection.h>
#include <map>

//TEMP
#include <iostream>

using namespace std;
using namespace ZenLib;
using namespace MediaConch;
//---------------------------------------------------------------------------

//***************************************************************************
// Strings helper
//***************************************************************************

//---------------------------------------------------------------------------
typedef std::map<void*, std::string> outputs;
outputs output_strings;

static CriticalSection critical;
//---------------------------------------------------------------------------

//***************************************************************************
// C interface
//***************************************************************************

//---------------------------------------------------------------------------
void* MediaConch_new()
{
    //New
    Lib* handle=NULL;

    try
    {
        handle=new Lib;
    }
    catch(...)
    {

        delete handle;
        return NULL;
    }

    return handle;
}

//---------------------------------------------------------------------------
void MediaConch_delete(void* handle)
{
    if (output_strings.find(handle)!=output_strings.end())
        output_strings.erase(handle);

    delete (Lib*)handle;
}

//---------------------------------------------------------------------------
const char* MediaConch_get_last_error(void* handle)
{
    critical.Enter();
    output_strings[handle] = ((Lib*)handle)->get_last_error();
    critical.Leave();

    return output_strings[handle].c_str();
}

//---------------------------------------------------------------------------
long MediaConch_add_file(void* handle, const char* filename)
{
    return ((Lib*)handle)->add_file(filename);
}

//---------------------------------------------------------------------------
void MediaConch_add_report(void* handle, MediaConch_report_c report)
{
    ((Lib*)handle)->add_report((MediaConchLib::report)report);
}

//---------------------------------------------------------------------------
void MediaConch_add_policy(void* handle, const char* filename)
{
    ((Lib*)handle)->add_policy(filename);
}

//---------------------------------------------------------------------------
void MediaConch_set_format(void* handle, MediaConch_format_c format)
{
    ((Lib*)handle)->set_format((MediaConchLib::format) format);
}

//---------------------------------------------------------------------------
void MediaConch_set_display(void* handle, const char* filename)
{
    ((Lib*)handle)->set_display(filename);
}

//---------------------------------------------------------------------------
void MediaConch_set_mil_option(void* handle, const char* name, const char* value)
{
    ((Lib*)handle)->set_mil_option(name, value);
}

//---------------------------------------------------------------------------
void MediaConch_set_force_analyze(void* handle, bool force)
{
    ((Lib*)handle)->set_force_analyze(force);
}


//---------------------------------------------------------------------------
void MediaConch_set_policy_verbosity(void* handle, int verbosity)
{
    ((Lib*)handle)->set_policy_verbosity(verbosity);
}

//---------------------------------------------------------------------------
void MediaConch_set_implementation_verbosity(void* handle, int verbosity)
{
    ((Lib*)handle)->set_implementation_verbosity(verbosity);
}

//---------------------------------------------------------------------------
const char* MediaConch_get_report(void* handle, long file_id)
{
    critical.Enter();
    output_strings[handle] = ((Lib*)handle)->get_report(file_id);
    critical.Leave();

    return output_strings[handle].c_str();
}

