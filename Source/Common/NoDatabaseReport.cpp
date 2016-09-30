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
    std::map<long, std::vector<MC_Report*> >::iterator it_r = reports_saved.begin();
    for (; it_r != reports_saved.end(); ++it_r)
    {
        for (size_t i = 0; i < it_r->second.size(); ++i)
            delete it_r->second[i];
    }
    reports_saved.clear();

    for (size_t i = 0; i < files_saved.size(); ++i)
    {
        if (files_saved[i])
            delete files_saved[i];
    }
    files_saved.clear();
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
long NoDatabaseReport::add_file(int user, const std::string& filename, const std::string& file_last_modification,
                                std::string&, long generated_id,
                                long source_id, size_t generated_time,
                                const std::string& generated_log, const std::string& generated_error_log)
{
    MC_File* f = new MC_File;
    f->filename = filename;
    f->file_last_modification = file_last_modification;
    f->user = user;

    f->analyzed = false;

    f->source_id = source_id;
    f->generated_id = generated_id;

    f->generated_time = generated_time;
    f->generated_log = generated_log;
    f->generated_error_log = generated_error_log;

    long id = (long)files_saved.size();
    files_saved.push_back(f);
    return id;
}

//---------------------------------------------------------------------------
long NoDatabaseReport::update_file(int user, long file_id, const std::string& file_last_modification,
                                   std::string&, long generated_id,
                                   long source_id, size_t generated_time,
                                   const std::string& generated_log, const std::string& generated_error_log)
{
    if (file_id < 0 || file_id >= (long)files_saved.size() || files_saved[file_id]->user != user)
        return -1;

    MC_File* f = files_saved[file_id];

    f->file_last_modification = file_last_modification;

    f->analyzed = false;

    f->source_id = source_id;
    f->generated_id = generated_id;

    f->generated_time = generated_time;
    f->generated_log = generated_log;
    f->generated_error_log = generated_error_log;

    return file_id;
}

//---------------------------------------------------------------------------
long NoDatabaseReport::get_file_id(int user, const std::string& file, const std::string& file_last_modification)
{
    for (long id = 0; id < (long)files_saved.size(); ++id)
        if (files_saved[id] && file == files_saved[id]->filename &&
            (!file_last_modification.size() || file_last_modification == files_saved[id]->file_last_modification) &&
            files_saved[id]->user == user)
            return id;
    return -1;
}

//---------------------------------------------------------------------------
void NoDatabaseReport::get_file_name_from_id(int user, long id, std::string& file)
{
    if (id > 0 && id < (long)files_saved.size() && files_saved[id] &&
        files_saved[id]->user == user)
        file = files_saved[id]->filename;
    else
        file = std::string();
}

//---------------------------------------------------------------------------
void NoDatabaseReport::get_file_information_from_id(int user, long id, std::string& filename, std::string& file_last_modification,
                                                    long& generated_id, long& source_id, size_t& generated_time,
                                                    std::string& generated_log, std::string& generated_error_log, bool& analyzed)
{
    if (id > 0 && id < (long)files_saved.size() && files_saved[id] &&
        files_saved[id]->user == user)
    {
        filename = files_saved[id]->filename;
        file_last_modification = files_saved[id]->file_last_modification;
        generated_id = files_saved[id]->generated_id;
        source_id = files_saved[id]->source_id;
        generated_time = files_saved[id]->generated_time;
        generated_log = files_saved[id]->generated_log;
        generated_error_log = files_saved[id]->generated_error_log;
        analyzed = files_saved[id]->analyzed;
    }
    else
    {
        filename = std::string();
        file_last_modification = std::string();
        generated_id = -1;
        source_id = -1;
        generated_time = (size_t)-1;
        generated_log = std::string();
        generated_error_log = std::string();
        analyzed = false;
    }
}

//---------------------------------------------------------------------------
bool NoDatabaseReport::file_is_analyzed(int user, long id)
{
    if (id > 0 && id < (long)files_saved.size() && files_saved[id] &&
        files_saved[id]->user == user)
        return files_saved[id]->analyzed;

    return false;
}

//---------------------------------------------------------------------------
int NoDatabaseReport::update_file_generated_id(int user, long source_id, long generated_id)
{
    if (source_id > 0 && source_id < (long)files_saved.size() && files_saved[source_id] &&
        files_saved[source_id]->user == user)
    {
        files_saved[source_id]->generated_id = generated_id;
        return 0;
    }
    return -1;
}

//---------------------------------------------------------------------------
int NoDatabaseReport::update_file_analyzed(int user, long id, bool analyzed)
{
    if (id > 0 && id < (long)files_saved.size() && files_saved[id] &&
        files_saved[id]->user == user)
    {
        files_saved[id]->analyzed = analyzed;
        return 0;
    }
    return -1;
}

//---------------------------------------------------------------------------
int NoDatabaseReport::save_report(int user, long file_id, MediaConchLib::report reportKind, MediaConchLib::format format,
                                  const std::string& report, MediaConchLib::compression c,
                                  int mil_version)
{
    if (file_id < 0 || file_id > (long)files_saved.size() || !files_saved[file_id] ||
        files_saved[file_id]->user != user)
        return -1;

    MC_Report* r = new MC_Report;
    r->reportKind = reportKind;
    r->format = format;
    r->report = report;
    r->compression = c;
    r->mil_version = mil_version;

    std::map<long, std::vector<MC_Report*> >::iterator it = reports_saved.find(file_id);
    if (it != reports_saved.end())
    {
        for (size_t i = 0; i < reports_saved[file_id].size(); ++i)
        {
            MC_Report *r = reports_saved[file_id][i];
            if (r && r->format == format && r->reportKind == reportKind)
            {
                delete r;
                reports_saved[file_id].erase(reports_saved[file_id].begin() + i);
                break;
            }
        }
    }
    reports_saved[file_id].push_back(r);
    return 0;
}

//---------------------------------------------------------------------------
void NoDatabaseReport::get_report(int user, long file_id, MediaConchLib::report reportKind, MediaConchLib::format format,
                                  std::string& report, MediaConchLib::compression& c)
{
    if (file_id < 0 || file_id > (long)files_saved.size() || !files_saved[file_id] ||
        files_saved[file_id]->user != user)
        return;

    for (size_t i = 0; i < reports_saved[file_id].size(); ++i)
    {
        MC_Report* r = reports_saved[file_id][i];
        if (!r || r->format != format || r->reportKind != reportKind)
            continue;

        report = r->report;
        c = r->compression;
        return;
    }
}

//---------------------------------------------------------------------------
int NoDatabaseReport::remove_report(int user, long file_id)
{
    if (file_id < 0 || file_id > (long)files_saved.size() || !files_saved[file_id] ||
        files_saved[file_id]->user != user)
        return -1;

    std::map<long, std::vector<MC_Report*> >::iterator it = reports_saved.find(file_id);

    if (it == reports_saved.end())
        return -1;

    reports_saved.erase(it);
    return 0;
}

//---------------------------------------------------------------------------
bool NoDatabaseReport::report_is_registered(int user, long file_id, MediaConchLib::report reportKind, MediaConchLib::format format)
{
    if (file_id < 0 || file_id > (long)files_saved.size() || !files_saved[file_id] ||
        files_saved[file_id]->user != user)
        return false;

    for (size_t i = 0; i < reports_saved[file_id].size(); ++i)
    {
        MC_Report* r = reports_saved[file_id][i];

        if (r && r->format == format && r->reportKind == reportKind)
            return true;
    }
    return false;
}

int NoDatabaseReport::version_registered(int user, long file_id)
{
    if (file_id < 0 || file_id > (long)files_saved.size() || !files_saved[file_id] ||
        files_saved[file_id]->user != user)
        return -1;

    for (size_t i = 0; i < reports_saved[file_id].size(); ++i)
    {
        if (reports_saved[file_id][i] && reports_saved[file_id][i]->mil_version)
            return reports_saved[file_id][i]->mil_version;
    }

    return -1;
}

//---------------------------------------------------------------------------
void NoDatabaseReport::get_elements(int user, std::vector<std::string>& vec)
{
    for (size_t i = 0; i < files_saved.size(); ++i)
    {
        if (!files_saved[i] || files_saved[i]->user != user)
            continue;

        vec.push_back(files_saved[i]->filename);
    }
}

//---------------------------------------------------------------------------
void NoDatabaseReport::get_element_report_kind(int user, long file_id, MediaConchLib::report& report_kind)
{
    report_kind = MediaConchLib::report_MediaConch;
    if (file_id < 0 || file_id > (long)files_saved.size() || !files_saved[file_id] ||
        files_saved[file_id]->user != user)
        return;

    if (file_id >= 0 && reports_saved[file_id].size())
    {
        for (size_t i = 0; i < reports_saved[file_id].size(); ++i)
        {
            if (!reports_saved[file_id][i])
                continue;

            MediaConchLib::report tool_i = reports_saved[file_id][i]->reportKind;
            if (tool_i == MediaConchLib::report_MediaInfo || tool_i == MediaConchLib::report_MediaTrace || tool_i == MediaConchLib::report_MicroMediaTrace)
                tool_i = MediaConchLib::report_MediaConch;

            report_kind = tool_i;
        }
    }
}

}
