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
#ifndef DPFMANAGERH
#define DPFMANAGERH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <map>
#include "MediaConchLib.h"
#include "PluginFormat.h"

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Class DPFManager
//***************************************************************************

class DPFManager : public PluginFormat
{
public:
    DPFManager();
    virtual ~DPFManager();
    DPFManager(const DPFManager&);

    virtual int load_plugin(const std::map<std::string, Container::Value>& obj, std::string& error);
    virtual int run(std::string& error);

private:
    DPFManager&    operator=(const DPFManager&);

    int                       create_configuration_file(const std::string& report_dir, std::string& file);
    int                       delete_configuration_file(const std::string& file);
    int                       read_summary_report_for_new_report_file(const std::string& summary, std::string& file, std::string& error);
    static void               manage_xml_error(void *userData, const char* msg, ...);

    std::string               bin;
    std::vector<std::string>  params;
    std::vector<std::string>  isos;

    std::string               xml_error;

    static const char        *conf_version;
};

}

#endif // !DPFMANAGERH
