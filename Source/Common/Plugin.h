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
#ifndef PLUGINH
#define PLUGINH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <map>
#include "MediaConchLib.h"
#include "Container.h"

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Class Plugin
//***************************************************************************

class Plugin
{
public:
    Plugin();
    virtual ~Plugin();

    virtual int load_plugin(const std::map<std::string, Container::Value>& obj, std::string& error) = 0;
    virtual int run(std::string& error) = 0;

    MediaConchLib::PluginType get_type() const { return type; }
    const std::string&        get_name() const { return name; }
    const std::string&        get_report() const { return report; }

protected:
    MediaConchLib::PluginType type;
    std::string               name;
    std::string               report;
    int                       exec_bin(const std::vector<std::string>& params, std::string& error);
    int                       read_report(const std::string& file, std::string& report);

    void                      unified_string(std::string& str);

private:
    Plugin(const Plugin&);
    Plugin&     operator=(const Plugin&);
};

}

#endif // !PLUGINH
