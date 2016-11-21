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
#ifndef PLUGINFORMATH
#define PLUGINFORMATH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "Plugin.h"

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Class Plugin
//***************************************************************************

class PluginFormat : public Plugin
{
public:
    PluginFormat() {}
    virtual ~PluginFormat() {}
    PluginFormat(const PluginFormat& p) : Plugin(p)
    {
        format = p.format;
        filename = p.filename;
        report_kind = p.report_kind;
    }

    std::string            get_format() const { return format; }
    MediaConchLib::report  get_report_kind() const { return report_kind; }
    const std::string&     get_file() const { return filename; }
    void                   set_file(const std::string& file) { filename = file; }

protected:
    std::string            format;
    std::string            filename;
    MediaConchLib::report  report_kind;

private:
    PluginFormat&          operator=(const PluginFormat&);
};

}

#endif // !PLUGINFORMATH
