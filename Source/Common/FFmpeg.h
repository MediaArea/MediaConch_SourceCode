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

    virtual int load_plugin(const std::map<std::string, Container::Value>& obj, std::string& error);
    virtual int run(std::string& error);

private:
    FFmpeg(const FFmpeg&);
    FFmpeg&    operator=(const FFmpeg&);

    std::string               bin;
    std::string               outputDir;
    std::string               outputExt;
    std::vector<std::string>  inputParams;
    std::vector<std::string>  outputParams;
    std::vector<std::string>  params;

    void create_output_file_name();
};

}

#endif // !FFMPEGH
