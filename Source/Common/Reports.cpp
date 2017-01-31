/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#include <sstream>
#include <ZenLib/Dir.h>
#include <ZenLib/File.h>
#include <ZenLib/Ztring.h>

#include "Reports.h"
#include "Core.h"
#include "PluginLog.h"
#include "JS_Tree.h"
#include "Schema.h"
#include "Xslt.h"

#include "Common/generated/ImplementationReportXsl.h"
#include "Common/generated/ImplementationReportVeraPDFXsl.h"
#include "Common/generated/ImplementationReportDPFManagerXsl.h"
#include "Common/generated/ImplementationReportDisplayHtmlXsl.h"
#include "Common/generated/ImplementationReportDisplayCSVXsl.h"
#include "Common/generated/ImplementationReportDisplaySimpleXsl.h"
#if defined(_WIN32) || defined(WIN32)
#include "Common/generated/ImplementationReportDisplayTextXsl.h"
#else //defined(_WIN32) || defined(WIN32)
#include "Common/generated/ImplementationReportDisplayTextUnicodeXsl.h"
#endif //defined(_WIN32) || defined(WIN32)
#include "Common/generated/MediaTraceDisplayTextXsl.h"
#include "Common/generated/MediaTraceDisplayHtmlXsl.h"
#include "Common/generated/MicroMediaTraceToMediaTraceXsl.h"

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
Reports::Reports(Core *c) : core(c)
{
}

Reports::~Reports()
{
}

//***************************************************************************
// Reports
//***************************************************************************

//---------------------------------------------------------------------------
int Reports::checker_get_report(CheckerReport& cr, MediaConchLib::Checker_ReportRes* result, std::string& err)
{
    if (!cr.policies_ids.empty() || !cr.policies_contents.empty())
    {
        if (check_policies(cr.user, cr.files, cr.options, result, err,
                           cr.policies_ids.size() ? &cr.policies_ids : NULL,
                           cr.policies_contents.size() ? &cr.policies_contents : NULL) < 0)
            return -1;
        if (cr.format == MediaConchLib::format_Text)
            transform_with_xslt_text_memory(result->report, result->report);
        else if (cr.format == MediaConchLib::format_Html)
            transform_with_xslt_html_memory(result->report, result->report);
        else if (cr.format == MediaConchLib::format_Simple)
            transform_with_xslt_simple_memory(result->report, result->report);
        else if (cr.format == MediaConchLib::format_CSV)
            transform_with_xslt_csv_memory(result->report, result->report);
    }
    else
    {
        // For VeraPDF and DPFManager, to get the original XML
        if ((cr.report_set[MediaConchLib::report_MediaVeraPdf] || cr.report_set[MediaConchLib::report_MediaDpfManager]) &&
            cr.format == MediaConchLib::format_Xml &&
            (!cr.display_content || !cr.display_content->size()) &&
            (!cr.display_name || !cr.display_name->size()))
            cr.format = MediaConchLib::format_OrigXml;

        switch (cr.format)
        {
            case MediaConchLib::format_Text:
            case MediaConchLib::format_Xml:
            case MediaConchLib::format_MaXml:
            case MediaConchLib::format_Html:
            case MediaConchLib::format_OrigXml:
            case MediaConchLib::format_Simple:
            case MediaConchLib::format_CSV:
                if (get_reports_output(cr.user, cr.files, cr.options, cr.format, cr.report_set, result, err) < 0)
                    return -1;
                break;
            case MediaConchLib::format_JsTree:
                if (get_reports_output_JStree(cr.user, cr.files, cr.report_set, result->report, err) < 0)
                    return -1;
                break;
            default:
                return -1;
        }
    }

    if (cr.display_name)
        transform_with_xslt_file(result->report, *cr.display_name, cr.options, result->report);
    else if (cr.display_content)
        transform_with_xslt_memory(result->report, *cr.display_content, cr.options, result->report);

    if (result->has_valid && !result->valid)
        core->plugin_add_log(PluginLog::LOG_LEVEL_DEBUG, "Implementation report is not valid");
    return 0;
}

//---------------------------------------------------------------------------
int Reports::checker_validate(int user, MediaConchLib::report report, const std::vector<long>& files,
                              const std::vector<size_t>& policies_ids,
                              const std::vector<std::string>& policies_contents,
                              const std::map<std::string, std::string>& options,
                              std::vector<MediaConchLib::Checker_ValidateRes*>& result, std::string& err)
{
    if (report != MediaConchLib::report_MediaConch && !policies_ids.size() && !policies_contents.size() &&
        report != MediaConchLib::report_MediaVeraPdf && report != MediaConchLib::report_MediaDpfManager)
        return -1;

    std::vector<long> file_tmp;
    for (size_t i = 0; i < files.size(); ++i)
    {
        file_tmp.clear();
        file_tmp.push_back(files[i]);

        MediaConchLib::Checker_ValidateRes* res = new MediaConchLib::Checker_ValidateRes;
        res->id = files[i];
        if (!policies_ids.empty() || !policies_contents.empty())
        {
            MediaConchLib::Checker_ReportRes tmp_res;
            if (check_policies(user, file_tmp, options, &tmp_res, err,
                                     policies_ids.size() ? &policies_ids : NULL,
                                     policies_contents.size() ? &policies_contents : NULL) < 0)
                continue;
            res->valid = tmp_res.has_valid ? tmp_res.valid : true;
        }
        else if (report == MediaConchLib::report_MediaConch)
        {
            //XXX
            std::string report;
            if (get_implementation_report(user, files[i], options, report, res->valid, err) < 0)
                return -1;
        }
        else if (report == MediaConchLib::report_MediaVeraPdf)
        {
            std::string report;
            if (get_verapdf_report(user, files[i], report, res->valid, err) < 0)
                return -1;
        }
        else if (report == MediaConchLib::report_MediaDpfManager)
        {
            std::string report;
            if (get_dpfmanager_report(user, files[i], report, res->valid, err) < 0)
                return -1;
        }

        result.push_back(res);
        if (!res->valid)
            core->plugin_add_log(PluginLog::LOG_LEVEL_DEBUG, "Implementation report is not valid");
    }

    return 0;
}

//***************************************************************************
// Reports Helpers
//***************************************************************************

//---------------------------------------------------------------------------
int Reports::get_reports_output(int user, const std::vector<long>& files,
                                const std::map<std::string, std::string>& options,
                                MediaConchLib::format f,
                                std::bitset<MediaConchLib::report_Max> report_set,
                                MediaConchLib::Checker_ReportRes* result, std::string& err)
{
    if (f == MediaConchLib::format_MaXml)
    {
        if (create_report_ma_xml(user, files, options, result->report, report_set, err) < 0)
            return -1;

        result->report += "\r\n";
    }
    else
    {
        if (report_set[MediaConchLib::report_MediaInfo])
        {
            std::string tmp;
            if (f == MediaConchLib::format_Xml)
            {
                if (create_report_mi_xml(user, files, tmp, err) < 0)
                    return -1;
            }
            else
            {
                if (core->get_report_saved(user, files, MediaConchLib::report_MediaInfo, f, "", tmp, err) < 0)
                    return -1;
            }

            result->report += tmp;
            result->report += "\r\n";
        }

        if (report_set[MediaConchLib::report_MediaTrace])
        {
            std::string tmp;
            if (create_report_mt_xml(user, files, tmp, err) < 0)
                tmp = std::string();

            if (f == MediaConchLib::format_Xml)
            {
            }
            else if (f == MediaConchLib::format_Html)
            {
                // Apply an XSLT to have HTML
                std::string memory(media_trace_display_html_xsl);
                std::map<std::string, std::string> opts;
                transform_with_xslt_memory(tmp, memory, opts, tmp);
            }
            else if (f == MediaConchLib::format_Text)
            {
                // Apply an XSLT to have Text
                std::string memory(media_trace_display_text_xsl);
                std::map<std::string, std::string> opts;
                transform_with_xslt_memory(tmp, memory, opts, tmp);
            }

            result->report += tmp;
            result->report += "\r\n";
        }

        if (report_set[MediaConchLib::report_MicroMediaTrace])
        {
            std::string tmp;
            if (create_report_mmt_xml(user, files, tmp, err) < 0)
                tmp = std::string();

            if (f == MediaConchLib::format_Xml)
                result->report += tmp;
            else
                result->report += "Format for MicroMediaTrace must be XML.";
        }

        if (report_set[MediaConchLib::report_MediaConch])
        {
            std::string tmp;
            std::string err;
            bool is_valid = false;
            if (get_implementation_reports(user, files, options, tmp, is_valid, err) < 0)
                tmp = std::string();

            if (f == MediaConchLib::format_Html)
                transform_with_xslt_html_memory(tmp, tmp);
            else if (f == MediaConchLib::format_Xml)
            {
                // No transformation for XML
            }
            else if (f == MediaConchLib::format_Simple)
                transform_with_xslt_simple_memory(tmp, tmp);
            else if (f == MediaConchLib::format_CSV)
                transform_with_xslt_csv_memory(tmp, tmp);
            else
                transform_with_xslt_text_memory(tmp, tmp);

            if (!result->has_valid)
                result->has_valid = true;
            if (!is_valid)
                result->valid = false;

            result->report += tmp;
            result->report += "\r\n";
        }

        if (report_set[MediaConchLib::report_MediaVeraPdf])
        {
            for (size_t i = 0; i < files.size(); ++i)
            {
                std::string tmp, transformed;
                bool valid = false;
                if (get_verapdf_report(user, files[i], tmp, valid, err) < 0)
                    tmp = "Not implemented.";

                std::string memory(implementation_report_vera_pdf_xsl);
                const std::map<std::string, std::string> opts;
                transform_with_xslt_memory(tmp, memory, opts, transformed);

                if (!policy_is_valid(transformed))
                    result->valid = false;
                else
                    result->valid = true;

                if (f == MediaConchLib::format_OrigXml)
                {
                    // No transformation, keep the original XML
                    result->report += tmp;
                }
                else if (f == MediaConchLib::format_Xml)
                {
                    // Get the transformed XML for applying display
                    result->report += transformed;
                }
                else
                {
                    if (f == MediaConchLib::format_Html)
                        transform_with_xslt_html_memory(transformed, transformed);
                    else if (f == MediaConchLib::format_Simple)
                        transform_with_xslt_simple_memory(transformed, transformed);
                    else if (f == MediaConchLib::format_CSV)
                        transform_with_xslt_csv_memory(transformed, transformed);
                    else
                        transform_with_xslt_text_memory(transformed, transformed);

                    result->report += transformed;
                }
                result->report += "\r\n";
            }
        }

        if (report_set[MediaConchLib::report_MediaDpfManager])
        {
            for (size_t i = 0; i < files.size(); ++i)
            {
                std::string tmp, transformed;
                bool valid = false;
                if (get_dpfmanager_report(user, files[i], tmp, valid, err) < 0)
                    return -1;

                std::string memory(implementation_report_dpf_manager_xsl);
                const std::map<std::string, std::string> opts;
                transform_with_xslt_memory(tmp, memory, opts, transformed);

                if (!policy_is_valid(transformed))
                    result->valid = false;
                else
                    result->valid = true;

                if (f == MediaConchLib::format_OrigXml)
                {
                    // No transformation, keep the original XML
                    result->report += tmp;
                }
                else if (f == MediaConchLib::format_Xml)
                {
                    // Get the transformed XML for applying display
                    result->report += transformed;
                }
                else
                {
                    if (f == MediaConchLib::format_Html)
                        transform_with_xslt_html_memory(transformed, transformed);
                    if (f == MediaConchLib::format_Simple)
                        transform_with_xslt_simple_memory(transformed, transformed);
                    else if (f == MediaConchLib::format_CSV)
                        transform_with_xslt_csv_memory(transformed, transformed);
                    else
                        transform_with_xslt_text_memory(transformed, transformed);

                    result->report += transformed;
                }
                result->report += "\r\n";
            }
        }
    }

    return 0;
}

//---------------------------------------------------------------------------
int Reports::get_reports_output_JStree(int user, const std::vector<long>& files,
                                       const std::bitset<MediaConchLib::report_Max>& report_set,
                                       std::string& report, std::string& err)
{
    std::vector<long> vec;
    for (size_t i = 0; i < files.size(); ++i)
    {
        vec.clear();
        vec.push_back(files[i]);
        JsTree js;
        if (i)
            report += "\n";
        if (report_set[MediaConchLib::report_MediaInfo])
        {
            std::string ret;
            if (create_report_mi_xml(user, vec, ret, err) < 0)
                return -1;
            report += js.format_from_inform_XML(ret);
        }

        if (report_set[MediaConchLib::report_MediaTrace])
        {
            std::string ret;
            if (create_report_mt_xml(user, vec, ret, err) < 0)
                return -1;
            report += js.format_from_trace_XML(ret);
        }
    }

    return 0;
}

//---------------------------------------------------------------------------
int Reports::get_implementation_reports(int user, const std::vector<long>& files,
                                        const std::map<std::string, std::string>& options,
                                        std::string& report, bool& valid, std::string& err)
{
    bool AcceptsHttps = core->accepts_https();
    valid = true;

    std::stringstream verbo;
    std::map<std::string, std::string>::const_iterator it = options.find("verbosity");
    if (it != options.end() && it->second.size())
        verbo << it->second;
    else
        verbo << "5";

    report = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    report += "<MediaConch xmlns=\"http" + (AcceptsHttps ? "s" : std::string()) +
        "://mediaarea.net/mediaconch\" version=\"0.2\" verbosity=\"" + verbo.str() + "\">\n";
    for (size_t i = 0; i < files.size(); ++i)
    {
        std::string file;
        if (core->checker_file_from_id(user, files[i], file, err) < 0)
            return -1;
        xml_escape_attributes(file);

        report += "  <media ref=\"" + file + "\">";
        std::string implem;
        bool v = false;
        if (get_implementation_report(user, files[i], options, implem, v, err) < 0)
            implem = std::string();
        else
            get_content_of_media_in_xml(implem);

        report += implem + "</media>\n";

        if (!v)
            valid = false;
    }

    report += "</MediaConch>\n";

    return 0;
}

//---------------------------------------------------------------------------
int Reports::get_implementation_report(int user, long file, const std::map<std::string, std::string>& options,
                                       std::string& report, bool& valid, std::string& err)
{
    std::string options_str = Core::serialize_string_from_options_map(options);
    valid = true;
    std::string tmp_report;

    bool registered = false;
    if (core->implem_report_is_registered(user, file, options_str, registered, err) < 0)
        return -1;

    if (!registered)
    {
        std::string memory(implementation_report_xsl);
        std::vector<long> tocheck;
        tocheck.push_back(file);

        if (validate_xslt_from_memory(user, tocheck, options, memory, true, tmp_report, valid, err) < 0)
            return -1;

        if (core->register_mediaconch_to_database(user, file, options_str, tmp_report, err) < 0)
            return -1;
        tmp_report = std::string();
    }

    std::vector<long> files;
    files.push_back(file);
    if (core->get_report_saved(user, files, MediaConchLib::report_MediaConch, MediaConchLib::format_Xml,
                               options_str, tmp_report, err) < 0)
        return -1;

    if (valid)
        valid = implementation_is_valid(tmp_report);
    report += tmp_report;

    return 0;
}

//---------------------------------------------------------------------------
int Reports::get_verapdf_report(int user, long file, std::string& report, bool& valid, std::string& err)
{
    std::vector<long> files;
    files.push_back(file);

    if (core->get_report_saved(user, files, MediaConchLib::report_MediaVeraPdf, MediaConchLib::format_Xml,
                               "", report, err) < 0)
        return -1;

    valid = verapdf_report_is_valid(report);
    return 0;
}

//---------------------------------------------------------------------------
int Reports::get_dpfmanager_report(int user, long file, std::string& report, bool& valid, std::string& err)
{
    std::vector<long> files;
    files.push_back(file);

    if (core->get_report_saved(user, files, MediaConchLib::report_MediaDpfManager, MediaConchLib::format_Xml,
                               "", report, err) < 0)
        return -1;

    valid = dpfmanager_report_is_valid(report);
    return 0;
}

//***************************************************************************
// Reports create/change
//***************************************************************************

//---------------------------------------------------------------------------
int Reports::create_report_mi_xml(int user, const std::vector<long>& files, std::string& report, std::string& err)
{
    bool AcceptsHttps = core->accepts_https();

    std::string version = ZenLib::Ztring(core->Menu_Option_Preferences_Option (__T("Info_Version"), __T(""))).To_UTF8();
    std::string search(" - v");
    size_t pos = version.find(search);
    if (pos != std::string::npos)
        version = version.substr(pos + search.length());

    std::stringstream start;
    start << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    start << "<MediaInfo\n";
    start << "xmlns=\"http" << (AcceptsHttps ? "s" : std::string()) << "://mediaarea.net/mediainfo\"\n";
    start << "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
    start << "xsi:schemaLocation=\"http" << (AcceptsHttps ? "s" : std::string()) << "://mediaarea.net/mediainfo http" << (AcceptsHttps ? "s" : std::string()) << "://mediaarea.net/mediainfo/mediainfo_2_0.xsd\"\n";
    start << "version=\"2.0beta1\">\n";
    start << "<!-- Work in progress, not for production -->\n";
    start << "<creatingLibrary version=\"";
    start << version;
    start << "\" url=\"http" << (AcceptsHttps ? "s" : std::string()) << "://mediaarea.net/MediaInfo\">MediaInfoLib</creatingLibrary>\n";
    report += start.str();
    std::vector<long> vec;
    for (size_t i = 0; i < files.size(); ++i)
    {
        vec.clear();
        vec.push_back(files[i]);

        std::string file;
        if (core->checker_file_from_id(user, files[i], file, err) < 0)
            return -1;

        xml_escape_attributes(file);
        report += "<media ref=\"" + file + "\">\n";

        std::string info;
        if (core->get_report_saved(user, vec, MediaConchLib::report_MediaInfo,
                             MediaConchLib::format_Xml, "", info, err) < 0)
            return -1;

        get_content_of_media_in_xml(info);
        if (info.length())
            report += info;
        report += std::string("</media>\n");
    }
    report += std::string("</MediaInfo>");
    return 0;
}

//---------------------------------------------------------------------------
int Reports::create_report_mt_xml(int user, const std::vector<long>& files, std::string& report, std::string& err)
{
    bool AcceptsHttps = core->accepts_https();
    std::string version = ZenLib::Ztring(core->Menu_Option_Preferences_Option (__T("Info_Version"), __T(""))).To_UTF8();
    std::string search(" - v");
    size_t pos = version.find(search);
    if (pos != std::string::npos)
        version = version.substr(pos + search.length());

    std::stringstream start;
    start << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    start << "<MediaTrace\n";
    start << "xmlns=\"http" << (AcceptsHttps ? "s" : std::string()) << "://mediaarea.net/mediatrace\"\n";
    start << "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
    start << "xsi:schemaLocation=\"http" << (AcceptsHttps ? "s" : std::string()) << "://mediaarea.net/mediatrace http" << (AcceptsHttps ? "s" : std::string()) << "://mediaarea.net/mediatrace/mediatrace_0_1.xsd\"\n";
    start << "version=\"0.1\">\n";
    start << "<creatingLibrary version=\"";
    start << version;
    start << "\" url=\"http" << (AcceptsHttps ? "s" : std::string()) << "://mediaarea.net/MediaInfo\">MediaInfoLib</creatingLibrary>\n";

    report += start.str();

    std::vector<long> vec;
    for (size_t i = 0; i < files.size(); ++i)
    {
        vec.clear();
        vec.push_back(files[i]);

        std::string file;
        if (core->checker_file_from_id(user, files[i], file, err) < 0)
            return -1;

        xml_escape_attributes(file);
        report += "<media ref=\"" + file + "\">\n";

        std::string tmp;
        if (create_report_mmt_xml(user, vec, tmp, err) < 0)
            return -1;

        if (tmp.length())
        {
            std::map<std::string, std::string> opts;
            std::string memory(micromediatrace_to_mediatrace_xsl);
            transform_with_xslt_memory(tmp, memory, opts, tmp);
            get_content_of_media_in_xml(tmp);
            report += tmp;
        }
        report += std::string("</media>\n");
    }
    report += std::string("</MediaTrace>");
    return 0;
}

//---------------------------------------------------------------------------
int Reports::create_report_mmt_xml(int user, const std::vector<long>& files, std::string& report, std::string& err)
{
    bool AcceptsHttps = core->accepts_https();
    std::string version = ZenLib::Ztring(core->Menu_Option_Preferences_Option (__T("Info_Version"), __T(""))).To_UTF8();
    std::string search(" - v");
    size_t pos = version.find(search);
    if (pos != std::string::npos)
        version = version.substr(pos + search.length());

    std::stringstream start;
    start << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    start << "<MicroMediaTrace";
    start << " xmlns=\"http" << (AcceptsHttps ? "s" : std::string()) << "://mediaarea.net/micromediatrace\"";
    start << " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"";
    start << " mtsl=\"http" << (AcceptsHttps ? "s" : std::string()) << "://mediaarea.net/micromediatrace https://mediaarea.net/micromediatrace/micromediatrace.xsd\"";
    start << " version=\"0.1\">";
    start << "<creatingLibrary version=\"";
    start << version;
    start << "\" url=\"http" << (AcceptsHttps ? "s" : std::string()) << "://mediaarea.net/MediaInfo\">MediaInfoLib</creatingLibrary>";

    report += start.str();

    std::vector<long> vec;
    for (size_t i = 0; i < files.size(); ++i)
    {
        vec.clear();
        vec.push_back(files[i]);

        std::string file;
        if (core->checker_file_from_id(user, files[i], file, err) < 0)
            return -1;

        xml_escape_attributes(file);
        report += "<media ref=\"" + file + "\">";

        std::string trace;
        if (core->get_report_saved(user, vec, MediaConchLib::report_MicroMediaTrace,
                                   MediaConchLib::format_Xml, "", trace, err) < 0)
            return -1;

        if (trace.length())
            report += trace;
        report += std::string("</media>");
    }
    report += std::string("</MicroMediaTrace>");
    return 0;
}

//---------------------------------------------------------------------------
int Reports::create_report_ma_xml(int user, const std::vector<long>& files,
                                const std::map<std::string, std::string>& options,
                                std::string& report,
                                std::bitset<MediaConchLib::report_Max> reports,
                                std::string& err)
{
    bool AcceptsHttps = core->accepts_https();

    std::string version = ZenLib::Ztring(core->Menu_Option_Preferences_Option (__T("Info_Version"), __T(""))).To_UTF8();
    std::string search(" - v");
    size_t pos = version.find(search);
    if (pos != std::string::npos)
        version = version.substr(pos + search.length());

    std::stringstream start;
    start << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    start << "<MediaArea\n";
    start << "xmlns=\"http" << (AcceptsHttps ? "s" : std::string()) << "://mediaarea.net/mediaarea\"\n";
    start << "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
    start << "xsi:schemaLocation=\"http" << (AcceptsHttps ? "s" : std::string()) << "://mediaarea.net/mediaarea http" << (AcceptsHttps ? "s" : std::string()) << "://mediaarea.net/mediaarea/mediaarea_0_1.xsd\"\n";
    start << "version=\"0.1\">\n";
    start << "<!-- Work in progress, not for production -->\n";
    start << "<creatingLibrary version=\"";
    start << version;
    start << "\" url=\"http" << (AcceptsHttps ? "s" : std::string()) << "://mediaarea.net/MediaInfo\">MediaInfoLib</creatingLibrary>\n";
    report += start.str();

    std::vector<long> vec;
    for (size_t i = 0; i < files.size(); ++i)
    {
        vec.clear();
        vec.push_back(files[i]);

        std::string file;
        if (core->checker_file_from_id(user, files[i], file, err) < 0)
            return -1;

        xml_escape_attributes(file);
        report += "<media ref=\"" + file + "\">\n";

        if (reports[MediaConchLib::report_MediaInfo])
        {
            std::string info;
            if (core->get_report_saved(user, vec, MediaConchLib::report_MediaInfo, MediaConchLib::format_Xml, "", info, err) < 0)
                return -1;

            get_content_of_media_in_xml(info);
            if (info.length())
                report += "<MediaInfo xmlns=\"http" + (AcceptsHttps ? "s" : std::string()) + "://mediaarea.net/mediainfo\" version=\"2.0beta1\">" + info + "</MediaInfo>\n";
        }

        if (reports[MediaConchLib::report_MediaTrace])
        {
            std::string trace;
            if (create_report_mt_xml(user, vec, trace, err) < 0)
                return -1;

            get_content_of_media_in_xml(trace);
            report += "<MediaTrace xmlns=\"http" + (AcceptsHttps ? "s" : std::string()) + "://mediaarea.net/mediatrace\" version=\"0.1\">\n";
            if (trace.length())
                report += trace;
            report += "</MediaTrace>\n";
        }

        if (reports[MediaConchLib::report_MicroMediaTrace])
        {
            std::string trace;
            if (core->get_report_saved(user, vec, MediaConchLib::report_MicroMediaTrace,
                                       MediaConchLib::format_Xml, "", trace, err) < 0)
                return -1;

            report += "<MicroMediaTrace xmlns=\"http" + (AcceptsHttps ? "s" : std::string()) + "://mediaarea.net/micromediatrace\" version=\"0.1\">\n";
            if (trace.length())
                report += trace;

            report += "</MicroMediaTrace>\n";
        }

        if (reports[MediaConchLib::report_MediaConch])
        {
            std::string implem;
            bool valid;
            if (get_implementation_report(user, files[i], options, implem, valid, err) < 0)
                implem = std::string();
            else
                get_content_of_media_in_xml(implem);

            report += "<MediaConch xmlns=\"http" + (AcceptsHttps ? "s" : std::string()) +
                "://mediaarea.net/mediaconch\" version=\"0.2\">\n" + implem + "</MediaConch>\n";
        }
        report += std::string("</media>\n");
    }
    report += std::string("</MediaArea>");
    return 0;
}

//---------------------------------------------------------------------------
void Reports::get_content_of_media_in_xml(std::string& report)
{
    std::string media_start("<media ref");
    size_t start = report.find(media_start);

    if (start == std::string::npos)
    {
        report = std::string();
        return;
    }

    start += media_start.length();
    media_start = std::string(">");
    start = report.find(media_start, start);
    size_t end = report.rfind("</media>");
    if (start == std::string::npos || end == std::string::npos)
    {
        report = std::string();
        return;
    }

    report = report.substr(start + media_start.length(), end - start - media_start.length());
}

//---------------------------------------------------------------------------
void Reports::xml_escape_attributes(std::string& xml)
{
    size_t len = xml.length();
    for (size_t i = 0; i < len; ++i)
    {
        switch(xml[i])
        {
            case '&':
                xml.replace(i, 1, "&amp;");
                len += 4;
                i += 4;
                break;
            case '<':
                xml.replace(i, 1, "&lt;");
                len += 3;
                i += 3;
                break;
            case '>':
                xml.replace(i, 1, "&gt;");
                len += 3;
                i += 3;
                break;
            case '\'':
                xml.replace(i, 1, "&apos;");
                len += 5;
                i += 5;
                break;
            case '"':
                xml.replace(i, 1, "&quot;");
                len += 5;
                i += 5;
                break;
            default:
                break;
        }
    }
}

//***************************************************************************
// Reports validity
//***************************************************************************

//---------------------------------------------------------------------------
bool Reports::implementation_is_valid(const std::string& report)
{
    size_t pos = 0;
    std::string search("<implementationChecks ");
    while (1)
    {
        pos = report.find(search.c_str(), pos);
        if (pos == std::string::npos)
            return true;

        size_t end = report.find(">", pos);
        if (end == std::string::npos)
            return true;

        std::string search("fail_count=\"");
        size_t f = report.find(search, pos);

        if (f > end || f + search.size() + 2 > report.size())
            return true;

        if (report[f + search.size()] != '0')
            return false;

        pos += search.size();
    }

    return true;
}

//---------------------------------------------------------------------------
bool Reports::policy_is_valid(const std::string& report)
{
    size_t pos = report.find("<policy ");
    if (pos == std::string::npos)
        return true;

    size_t end = report.find(">", pos);
    if (end == std::string::npos)
        return true;

    std::string search("outcome=\"");
    size_t f = report.find(search, pos);

    if (f > end || f + search.size() + 4 > report.size())
        return true;

    if (report[f + search.size()] == 'f' && report[f + search.size() + 1] == 'a' && report[f + search.size() + 2] == 'i' && report[f + search.size() + 3] == 'l')
        return false;

    return true;
}

//---------------------------------------------------------------------------
bool Reports::verapdf_report_is_valid(const std::string& report)
{
    size_t pos = report.find("<summary");
    if (pos == std::string::npos)
        return true;

    std::string invalid("inValid=\"");
    pos = report.find(invalid, pos);
    if (pos == std::string::npos)
        return true;

    if (report.size() > pos + invalid.size() && report[pos + invalid.size()] != '0')
        return false;

    return true;
}

//---------------------------------------------------------------------------
bool Reports::dpfmanager_report_is_valid(const std::string& report)
{
    size_t pos = report.find("<level>error</level>");
    if (pos != std::string::npos)
        return false;

    return true;
}

//***************************************************************************
// Policies
//***************************************************************************

//---------------------------------------------------------------------------
int Reports::check_policies(int user, const std::vector<long>& files,
                            const std::map<std::string, std::string>& opts,
                            MediaConchLib::Checker_ReportRes* result, std::string& err,
                            const std::vector<size_t>* policies_ids,
                            const std::vector<std::string>* policies_contents)
{
    if (!files.size())
    {
        err = "No file to validate";
        return -1;
    }

    std::map<std::string, std::string> options;
    std::map<std::string, std::string>::const_iterator it = opts.begin();
    for (; it != opts.end(); ++it)
        options[it->first] = it->second;

    unify_policy_options(user, options, err);

    std::vector<std::string> policies;
    if (core->policies.policy_get_policies(user, policies_ids, policies_contents, options, policies, err) < 0)
        return -1;

    std::stringstream Out;
    result->has_valid = true;
    if (check_policies_xslts(user, files, options, policies, Out, result->valid, err) < 0)
        return -1;

    result->report = Out.str();
    return 0;
}

//---------------------------------------------------------------------------
int Reports::check_policies_xslts(int user, const std::vector<long>& files,
                                   const std::map<std::string, std::string>& options,
                                   const std::vector<std::string>& policies,
                                   std::stringstream& Out, bool& valid, std::string& err)
{
    valid = true;
    for (size_t i = 0; i < policies.size(); ++i)
    {
        std::string tmp;
        if (validate_xslt_from_memory(user, files, options, policies[i], false,
                                       tmp, valid, err) < 0)
        {
            valid = false;
            Out << tmp;
        }
        else
        {
            Out << tmp;
            if (!policy_is_valid(tmp))
                valid = false;
        }
    }
    return 0;
}

//***************************************************************************
// Display
//***************************************************************************

//---------------------------------------------------------------------------
int Reports::transform_with_xslt_file(const std::string& report, const std::string& xslt,
                                   const std::map<std::string, std::string>& opts, std::string& result)
{
    Schema *S = new Xslt(!core->accepts_https());

    if (!S->register_schema_from_file(xslt.c_str()))
    {
        result = report;
        delete S;
        return -1;
    }

    S->set_options(opts);
    int valid = S->validate_xml(report);
    if (valid < 0)
    {
        result = report;
        delete S;
        return -1;
    }

    result = S->get_report();
    delete S;
    return 0;
}

//---------------------------------------------------------------------------
int Reports::transform_with_xslt_memory(const std::string& report, const std::string& memory,
                                        const std::map<std::string, std::string>& opts,
                                        std::string& result)
{
    Schema *S = new Xslt(!core->accepts_https());

    if (!S->register_schema_from_memory(memory))
    {
        result = report;
        return -1;
    }

    S->set_options(opts);
    int valid = S->validate_xml(report);
    if (valid < 0)
    {
        result = report;
        return -1;
    }

    result = S->get_report();
    delete S;
    return 0;
}

//---------------------------------------------------------------------------
int Reports::transform_with_xslt_text_memory(const std::string& report, std::string& result)
{
    const std::map<std::string, std::string> opts;
#if defined(_WIN32) || defined(WIN32)
    std::string memory(implementation_report_display_text_xsl);
#else //defined(_WIN32) || defined(WIN32)
    std::string memory(implementation_report_display_textunicode_xsl);
#endif //defined(_WIN32) || defined(WIN32)
    return transform_with_xslt_memory(report, memory, opts, result);
}

//---------------------------------------------------------------------------
int Reports::transform_with_xslt_html_memory(const std::string& report, std::string& result)
{
    std::map<std::string, std::string> opts;
    std::string memory(implementation_report_display_html_xsl);
    return transform_with_xslt_memory(report, memory, opts, result);
}

//---------------------------------------------------------------------------
int Reports::transform_with_xslt_simple_memory(const std::string& report, std::string& result)
{
    std::map<std::string, std::string> opts;
    std::string memory(implementation_report_display_simple_xsl);
    return transform_with_xslt_memory(report, memory, opts, result);
}

//---------------------------------------------------------------------------
int Reports::transform_with_xslt_csv_memory(const std::string& report, std::string& result)
{
    std::map<std::string, std::string> opts;
    std::string memory(implementation_report_display_csv_xsl);
    return transform_with_xslt_memory(report, memory, opts, result);
}

//***************************************************************************
// Validation
//***************************************************************************

//---------------------------------------------------------------------------
int Reports::validate_xslt_from_memory(int user, const std::vector<long>& files,
                                       const std::map<std::string, std::string>& opts,
                                       const std::string& memory, bool is_implem,
                                       std::string& report, bool& valid, std::string& err)
{
    valid = true;
    Schema *S = new Xslt(!core->accepts_https());

    if (is_implem)
    {
        std::map<std::string, std::string> options = core->get_implementation_options();

        // XXX: replace or add new XSL option for implementation
        std::map<std::string, std::string>::const_iterator it = opts.begin();
        for (; it != opts.end(); ++it)
            options[it->first] = it->second;

        unify_implementation_options(options);
        S->set_options(options);
    }

    int ret = 0;
    if (S->register_schema_from_memory(memory))
        ret = validation(user, files, S, report, valid, err);
    else
    {
        valid = false;

        std::stringstream Out;
        std::vector<std::string> errors = S->get_errors();

        Out << "Internal error for parsing Policy" << std::endl;
        for (size_t i = 0; i < errors.size(); i++)
            Out << "\t" << errors[i].c_str();
        report = Out.str();
    }

    delete S;
    return ret;
}

//---------------------------------------------------------------------------
int Reports::validation(int user, const std::vector<long>& files, Schema* S,
                        std::string& report, bool& valid, std::string& err)
{
    std::string xml;
    if (create_report_ma_xml(user, files, S->get_options(), xml, get_bitset_with_mi_mmt(), err) < 0)
        return -1;

    valid = true;

    int ret = S->validate_xml(xml);
    if (ret < 0)
    {
        valid = false;
        report = "Validation generated an internal error:";
        std::vector<std::string> errors = S->get_errors();
        for (size_t i = 0; i < errors.size(); ++i)
            report += "\t" + errors[i];
    }
    else
        report = S->get_report();

    return valid;
}

//***************************************************************************
// Helper
//***************************************************************************

//---------------------------------------------------------------------------
std::bitset<MediaConchLib::report_Max> Reports::get_bitset_with_mi_mt()
{
    std::bitset<MediaConchLib::report_Max> bits;

    bits.set(MediaConchLib::report_MediaInfo);
    bits.set(MediaConchLib::report_MediaTrace);
    return bits;
}

//---------------------------------------------------------------------------
std::bitset<MediaConchLib::report_Max> Reports::get_bitset_with_mi_mmt()
{
    std::bitset<MediaConchLib::report_Max> bits;

    bits.set(MediaConchLib::report_MediaInfo);
    bits.set(MediaConchLib::report_MicroMediaTrace);
    return bits;
}

//---------------------------------------------------------------------------
void Reports::unify_implementation_options(std::map<std::string, std::string>& opts)
{
    if (opts.find("verbosity") != opts.end() && opts["verbosity"].length() && opts["verbosity"] != "-1")
    {
        std::string& verbosity = opts["verbosity"];
        if (verbosity[0] == '"')
            verbosity = std::string("'") + verbosity + std::string("'");
        else
            verbosity = std::string("\"") + verbosity + std::string("\"");
    }
    else
        opts["verbosity"] = "\"5\"";
}

//---------------------------------------------------------------------------
void Reports::unify_policy_options(int user, std::map<std::string, std::string>& opts, std::string& err)
{
    std::map<std::string, std::string>::iterator it;
    if ((it = opts.find("policy_reference_id")) != opts.end())
    {
        std::string file = it->second;
        opts.erase(it);

        if (!file.length())
            return;

        std::vector<long> files;
        char *end = NULL;
        long file_id = strtol(file.c_str(), &end, 10);
        files.push_back(file_id);

        std::string report;
        if (create_report_ma_xml(user, files, opts, report, get_bitset_with_mi_mmt(), err) < 0)
            return;

        std::string path = Core::get_local_data_path();
        path += "policies_references_files/";
        if (!ZenLib::Dir::Exists(ZenLib::Ztring().From_UTF8(path)))
            if (!ZenLib::Dir::Create(ZenLib::Ztring().From_UTF8(path)))
                path = "./";

        for (size_t i = 0; ; ++i)
        {
            std::stringstream ss;
            ss << path << "reference";
            if (i)
                ss << i;
            ss << ".xml";
            if (ZenLib::File::Exists(ZenLib::Ztring().From_UTF8(ss.str())))
                continue;

            path = ss.str();
            break;
        }
        ZenLib::File fd;

        fd.Create(ZenLib::Ztring().From_UTF8(path));

        fd.Open(ZenLib::Ztring().From_UTF8(path), ZenLib::File::Access_Write);
        fd.Write(ZenLib::Ztring().From_UTF8(report));
        fd.Close();

        opts["compare"] = "\"" + path + "\"";
    }
}

}
