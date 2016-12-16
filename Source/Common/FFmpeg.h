/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Scheduler functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef FFMPEGH
#define FFMPEGH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <map>
#include "MediaConchLib.h"
#include "PluginPreHook.h"

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Class FFmpeg
//***************************************************************************

class FFmpeg : public PluginPreHook
{
public:
    FFmpeg();
    virtual ~FFmpeg();
    FFmpeg(const FFmpeg&);

    virtual int load_plugin(const std::map<std::string, Container::Value>& obj, std::string& error);
    virtual int run(std::string& error);

private:
    FFmpeg&    operator=(const FFmpeg&);
};

}

#endif // !FFMPEGH
