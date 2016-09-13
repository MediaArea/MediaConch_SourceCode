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

//---------------------------------------------------------------------------
#include "NoDatabaseReport.h"
#include <sstream>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// NoDatabaseReport
//***************************************************************************

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
NoDatabaseReport::NoDatabaseReport() : DatabaseReport()
{
}

//---------------------------------------------------------------------------
NoDatabaseReport::~NoDatabaseReport()
{
    std::map<std::string, std::vector<Report*> >::iterator it = reports_saved.begin();
    for (; it != reports_saved.end(); ++it)
    {
        for (size_t i = 0; i < it->second.size(); ++i)
            delete it->second[i];
    }
    reports_saved.clear();
}

//---------------------------------------------------------------------------
int NoDatabaseReport::init()
{
    return 0;
}

//---------------------------------------------------------------------------
int NoDatabaseReport::init_report()
{
    return init();
}

//---------------------------------------------------------------------------
int NoDatabaseReport::execute()
{
    return 0;
}

//---------------------------------------------------------------------------
int NoDatabaseReport::create_report_table()
{
    return 0;
}

//---------------------------------------------------------------------------
int NoDatabaseReport::update_report_table()
{
    return 0;
}

//---------------------------------------------------------------------------
int NoDatabaseReport::save_report(MediaConchLib::report reportKind, MediaConchLib::format format,
                                  const std::string& filename, const std::string& file_last_modification,
                                  const std::string& report, MediaConchLib::compression c,
                                  bool has_mil_version)
{
    Report* r = new Report;
    r->reportKind = reportKind;
    r->format = format;
    r->file_last_modification = file_last_modification;
    r->report = report;
    r->compression = c;
    r->has_mil_version = has_mil_version;
    std::map<std::string, std::vector<Report*> >::iterator it = reports_saved.find(filename);
    if (it != reports_saved.end())
    {
        for (size_t i = 0; i < it->second.size(); ++i)
        {
            Report *r = it->second[i];
            if (r->format == format && r->reportKind == reportKind)
            {
                delete r;
                it->second.erase(it->second.begin() + i);
                break;
            }
        }
    }
    reports_saved[filename].push_back(r);
    return 0;
}

//---------------------------------------------------------------------------
void NoDatabaseReport::get_report(MediaConchLib::report reportKind, MediaConchLib::format format,
                                  const std::string& filename, const std::string& file_last_modification,
                                  std::string& report, MediaConchLib::compression& c)
{
    std::map<std::string, std::vector<Report*> >::iterator it = reports_saved.find(filename);
    if (it == reports_saved.end())
        return;

    for (size_t i = 0; i < it->second.size(); ++i)
    {
        Report* r = it->second[i];
        if (r->format != format || r->reportKind != reportKind)
            continue;
        if (file_last_modification.length() && r->file_last_modification != file_last_modification)
            continue;

        report = r->report;
        c = r->compression;
        return;
    }
}

//---------------------------------------------------------------------------
int NoDatabaseReport::remove_report(const std::string& filename)
{
    std::map<std::string, std::vector<Report*> >::iterator it = reports_saved.find(filename);
    if (it == reports_saved.end())
        return -1;

    reports_saved.erase(it);
    return 0;
}

//---------------------------------------------------------------------------
bool NoDatabaseReport::file_is_registered(MediaConchLib::report reportKind, MediaConchLib::format format,
                                          const std::string& filename, const std::string& file_last_modification)
{
    std::map<std::string, std::vector<Report*> >::iterator it = reports_saved.find(filename);

    if (it == reports_saved.end())
        return false;

    for (size_t i = 0; i < it->second.size(); ++i)
    {
        Report* r = it->second[i];

        if (r->format == format && r->reportKind == reportKind
            && r->file_last_modification == file_last_modification)
            return true;
    }
    return false;
}

//---------------------------------------------------------------------------
bool NoDatabaseReport::file_is_registered(MediaConchLib::report reportKind, MediaConchLib::format format,
                                          const std::string& filename)
{
    std::map<std::string, std::vector<Report*> >::iterator it = reports_saved.find(filename);

    if (it == reports_saved.end())
        return false;

    for (size_t i = 0; i < it->second.size(); ++i)
    {
        Report* r = it->second[i];

        if (r->format == format && r->reportKind == reportKind)
            return true;
    }
    return false;
}

bool NoDatabaseReport::has_version_registered(const std::string& filename)
{
    std::map<std::string, std::vector<Report*> >::iterator it = reports_saved.find(filename);

    if (it == reports_saved.end())
        return false;

    for (size_t i = 0; i < it->second.size(); ++i)
    {
        if (it->second[i] && it->second[i]->has_mil_version)
            return true;
    }
    return false;
}

//---------------------------------------------------------------------------
void NoDatabaseReport::get_elements(std::vector<std::string>& vec)
{
    std::map<std::string, std::vector<Report*> >::iterator it = reports_saved.begin();

    for (; it != reports_saved.end(); ++it)
        vec.push_back(it->first);
}

//---------------------------------------------------------------------------
void NoDatabaseReport::get_element_report_kind(const std::string& file, MediaConchLib::report& report_kind)
{
    report_kind = MediaConchLib::report_MediaConch;
    if (reports_saved.find(file) != reports_saved.end() && reports_saved[file].size())
    {
        for (size_t i = 0; i < reports_saved[file].size(); ++i)
        {
            if (!reports_saved[file][i])
                continue;

            MediaConchLib::report tool_i = reports_saved[file][i]->reportKind;
            if (tool_i == MediaConchLib::report_MediaInfo || tool_i == MediaConchLib::report_MediaTrace || tool_i == MediaConchLib::report_MicroMediaTrace)
                tool_i = MediaConchLib::report_MediaConch;

            report_kind = tool_i;
        }
    }
}

}
