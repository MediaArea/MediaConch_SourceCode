/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//---------------------------------------------------------------------------
#include "MediaConchLib.h"
#include "Core.h"
#include "Policy.h"

namespace MediaConch {

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
MediaConchLib::MediaConchLib()
{
    core = new Core;
}

MediaConchLib::~MediaConchLib()
{
    delete core;
}

//***************************************************************************
// Options
//***************************************************************************

//---------------------------------------------------------------------------
int MediaConchLib::add_option(const std::string& option)
{
    Options.push_back(option);
    return 0;
}

//---------------------------------------------------------------------------
bool MediaConchLib::ReportAndFormatCombination_IsValid(const std::vector<std::string>& files,
                                                       const std::bitset<MediaConchLib::report_Max>& reports,
                                                       const std::string& display, MediaConchLib::format& Format,
                                                       std::string& reason)
{
    // Forcing some formats
    if (files.size() > 1 && Format == MediaConchLib::format_Xml)
        Format = format_MaXml;

    if (reports.count() > 1 && Format == MediaConchLib::format_Xml)
        Format = MediaConchLib::format_MaXml;

    if (Format == MediaConchLib::format_Text && !display.empty())
        Format = format_Xml; //Forcing XML output in order to apply the Display XSL
    
    // Test of incompatibilities
    if (reports[MediaConchLib::report_MediaConch] && reports[MediaConchLib::report_MediaTrace]
        && Format == MediaConchLib::format_Text)
    {
        reason = "Combination of MediaConch and MediaTrace is currently not possible with Text output";
        return false;
    }

    return true;
}

//***************************************************************************
// Analyze
//***************************************************************************

//---------------------------------------------------------------------------
int MediaConchLib::analyze(const std::vector<std::string>& files)
{
    if (!files.size())
        return -1;

    for (size_t i = 0; i < files.size(); ++i)
        core->open_file(files[i]);
    return 0;
}

//---------------------------------------------------------------------------
bool MediaConchLib::is_done(const std::vector<std::string>& files, double& percent)
{
    if (!files.size())
        return true;

    bool done = true;
    percent = 0.0;
    double unit_percent = (1.0 / files.size()) * 100.0;
    for (size_t i = 0; i < files.size(); ++i)
    {
        double percent_done;
        if (core->is_done(files[i], percent_done))
            percent += unit_percent;
        else
        {
            percent += unit_percent * percent_done;
            done = false;
        }
    }
    return done;
}

//***************************************************************************
// Output
//***************************************************************************

//---------------------------------------------------------------------------
int MediaConchLib::get_report(const std::bitset<report_Max>& report_set, format f,
                              const std::vector<std::string>& files, const std::vector<std::string>& policies,
                              std::string& report)
{
    if (!files.size())
        return -1;

    core->GetOutput(report_set, f, files, policies, report);
    return 0;
}

//***************************************************************************
// Policy
//***************************************************************************

//---------------------------------------------------------------------------
bool MediaConchLib::validate_policy(const std::string& file, int policy, std::string& report)
{
    return core->validate_policy(file, policy, report);
}

//---------------------------------------------------------------------------
bool MediaConchLib::validate_policies(const std::string& file, std::vector<std::string>& policies,
                                      std::string& report)
{
    if (!policies.size())
        return false;

    bool ret = true;
    for (size_t i = 0; i < policies.size(); ++i)
    {
        if (!core->validate_policy(file, i, report))
            ret = false;
        report += "\r\n";
    }

    return ret;
}

//***************************************************************************
// XSL Transformation
//***************************************************************************

//---------------------------------------------------------------------------
int MediaConchLib::transform_with_xslt_file(const std::string& report, const std::string& file, std::string& result)
{
    result = core->transform_with_xslt_file(report, file);
    return 0;
}

//---------------------------------------------------------------------------
int MediaConchLib::transform_with_xslt_memory(const std::string& report, const std::string& memory, std::string& result)
{
    result = core->transform_with_xslt_memory(report, memory);
    return 0;
}

//***************************************************************************
// Database
//***************************************************************************

//---------------------------------------------------------------------------
void MediaConchLib::load_configuration()
{
    core->load_configuration();
}

//---------------------------------------------------------------------------
void MediaConchLib::set_configuration_path(const std::string& path)
{
    core->set_configuration_path(path);
}

//---------------------------------------------------------------------------
const std::string& MediaConchLib::get_configuration_path() const
{
    return core->get_configuration_path();
}
//***************************************************************************
// Database
//***************************************************************************

//---------------------------------------------------------------------------
void MediaConchLib::save_policies()
{
    for (size_t i = 0; i < core->policies.policies.size(); ++i)
        if (!core->policies.policies[i]->saved)
            core->policies.export_schema(NULL, i);
}

//---------------------------------------------------------------------------
void MediaConchLib::save_policy(size_t pos, const std::string* filename)
{
    if (pos > core->policies.policies.size())
        return;
    const char* path = NULL;
    if (filename)
        path = filename->c_str();
    core->policies.export_schema(path, pos);
}

//---------------------------------------------------------------------------
bool MediaConchLib::is_policies_saved() const
{
    for (size_t i = 0; i < core->policies.policies.size(); ++i)
        if (!core->policies.policies[i]->saved)
            return false;
    return true;
}

//---------------------------------------------------------------------------
bool MediaConchLib::is_policy_saved(size_t pos) const
{
    if (pos > core->policies.policies.size())
        return false;
    return core->policies.policies[pos]->saved;
}

//---------------------------------------------------------------------------
int MediaConchLib::import_policy_from_file(const std::string& filename, std::string& err)
{
    int ret = core->policies.import_schema(filename);
    if (ret < 0)
        err = core->policies.get_error();
    return ret;
}

//---------------------------------------------------------------------------
int MediaConchLib::import_policy_from_memory(const std::string& memory, const std::string& filename, std::string& err)
{
    int ret = core->policies.import_schema_from_memory(filename, memory.c_str(), memory.length());
    if (ret < 0)
        err = core->policies.get_error();
    return ret;
}

//---------------------------------------------------------------------------
bool MediaConchLib::policy_exists(const std::string& title)
{
    return core->policies.policy_exists(title);
}

//---------------------------------------------------------------------------
size_t MediaConchLib::get_policies_count() const
{
    return core->policies.policies.size();
}

//---------------------------------------------------------------------------
Policy* MediaConchLib::get_policy(size_t pos)
{
    if (pos > core->policies.policies.size())
        return NULL;
    return core->policies.policies[pos];
}

//---------------------------------------------------------------------------
const std::vector<Policy *>& MediaConchLib::get_policies() const
{
    return core->policies.policies;
}

//---------------------------------------------------------------------------
void MediaConchLib::add_policy(Policy* p)
{
    core->policies.policies.push_back(p);
}

//---------------------------------------------------------------------------
void MediaConchLib::remove_policy(size_t pos)
{
    core->policies.erase_policy(pos);
}

//---------------------------------------------------------------------------
void MediaConchLib::clear_policies()
{
    for (size_t i = 0; i < core->policies.policies.size(); ++i)
        delete core->policies.policies[i];
    core->policies.policies.clear();
}

}
