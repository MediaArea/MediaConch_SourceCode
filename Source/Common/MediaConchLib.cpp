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
#include "ZenLib/Ztring.h"
#include "MediaConchLib.h"
#include "Core.h"
#include "DaemonClient.h"
#include "Policy.h"
#include "Http.h"
#include "LibEventHttp.h"

namespace MediaConch {

//***************************************************************************
// Statics
//***************************************************************************

//---------------------------------------------------------------------------
const std::string MediaConchLib::display_xml_name = std::string("XML");
const std::string MediaConchLib::display_maxml_name = std::string("MAXML");
const std::string MediaConchLib::display_text_name = std::string("TEXT");
const std::string MediaConchLib::display_html_name = std::string("HTML");
const std::string MediaConchLib::display_jstree_name = std::string("JSTREE");

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
MediaConchLib::MediaConchLib(bool no_daemon) : use_daemon(false), force_no_daemon(false), daemon_client(NULL)
{
    if (no_daemon)
        force_no_daemon = true;
    core = new Core;
}

MediaConchLib::~MediaConchLib()
{
    delete core;
    if (daemon_client)
    {
        delete daemon_client;
        daemon_client = NULL;
    }
}

//***************************************************************************
// General
//***************************************************************************

//---------------------------------------------------------------------------
int MediaConchLib::init()
{
    load_configuration();
    if (!force_no_daemon)
    {
        use_daemon = core->is_using_daemon();
        if (use_daemon)
        {
            daemon_client = new DaemonClient(this);
            if (daemon_client->init() < 0)
            {
                // Fallback to no daemon
                use_daemon = false;
            }
        }
    }
    load_plugins_configuration();
    return 0;
}

//---------------------------------------------------------------------------
int MediaConchLib::close()
{
    if (daemon_client)
    {
        daemon_client->close();
        delete daemon_client;
        daemon_client = NULL;
    }
    return 0;
}

//***************************************************************************
// Options
//***************************************************************************

//---------------------------------------------------------------------------
int MediaConchLib::add_option(const std::string& option, std::string& report)
{
    size_t egal_pos = option.find('=');
    if (egal_pos == std::string::npos || egal_pos > 2)
    {
        MediaInfoNameSpace::String Option(ZenLib::Ztring().From_UTF8(option), 2, egal_pos-2);
        MediaInfoNameSpace::String Value;
        if (egal_pos != std::string::npos)
            Value.assign(Ztring().From_UTF8(option), egal_pos + 1, std::string::npos);
        else
            Value=__T('1');

        String Result = core->Menu_Option_Preferences_Option(Option, Value);
        if (!Result.empty())
        {
            report = Ztring(Result).To_UTF8();
            return -1;
        }
    }
    Options.push_back(option);
    return 0;
}

//---------------------------------------------------------------------------
bool MediaConchLib::ReportAndFormatCombination_IsValid(const std::vector<std::string>&,
                                                       const std::bitset<MediaConchLib::report_Max>& reports,
                                                       const std::string& display, MediaConchLib::format& Format,
                                                       std::string& reason)
{
    if (reports[MediaConchLib::report_MicroMediaTrace])
    {
        reason = "MicroMediaTrace is for internal use only";
        return false;
    }

    // Forcing some formats
    if (Format == MediaConchLib::format_Text && !display.empty())
        Format = format_Xml; //Forcing Text (default) to XML

    if (Format != MediaConchLib::format_Xml && !display.empty())
    {
        reason = "If a display is used, no other output format can be used";
        return false;
    }

    if (reports.count() > 1 && Format == MediaConchLib::format_Xml)
        Format = MediaConchLib::format_MaXml;

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
int MediaConchLib::checker_analyze(const std::vector<std::string>& files, bool force_analyze)
{
    bool registered = false;
    for (size_t i = 0; i < files.size(); ++i)
    {
        int ret = checker_analyze(files[i], registered, force_analyze);
        if (ret < 0)
            return ret;
    }
    return errorHttp_NONE;
}

//---------------------------------------------------------------------------
int MediaConchLib::checker_analyze(const std::string& file, bool& registered, bool force_analyze)
{
    if (!file.length())
        return errorHttp_INVALID_DATA;

    // Send Options by API
    if (use_daemon)
        return daemon_client->checker_analyze(file, registered, force_analyze);
    return core->open_file(file, registered, Options, force_analyze);
}

//---------------------------------------------------------------------------
int MediaConchLib::checker_is_done(const std::vector<std::string>& files, double& percent)
{
    if (!files.size())
        return errorHttp_TRUE;

    MediaConchLib::report report_kind;
    int done = errorHttp_TRUE;
    percent = 0.0;
    double unit_percent = (1.0 / files.size()) * 100.0;
    for (size_t i = 0; i < files.size(); ++i)
    {
        double percent_done;
        int ret = checker_is_done(files[i], percent_done, report_kind);
        if (ret == errorHttp_TRUE)
            percent += unit_percent;
        else if (ret == errorHttp_NONE)
        {
            percent += unit_percent * percent_done;
            done = errorHttp_NONE;
        }
        else
            return ret;
    }
    return done;
}

//---------------------------------------------------------------------------
int MediaConchLib::checker_is_done(const std::string& file, double& percent, MediaConchLib::report& report_kind)
{
    if (!file.length())
        return errorHttp_NONE;

    if (use_daemon)
        return daemon_client->checker_is_done(file, percent, report_kind);
    return core->checker_is_done(file, percent, report_kind);
}

//---------------------------------------------------------------------------
void MediaConchLib::checker_list(std::vector<std::string>& vec)
{
    if (use_daemon)
    {
        daemon_client->checker_list(vec);
        return;
    }
    core->checker_list(vec);
}

//---------------------------------------------------------------------------
void MediaConchLib::checker_file_from_id(int id, std::string& filename)
{
    if (use_daemon)
    {
        daemon_client->checker_file_from_id(id, filename);
        return;
    }
}

//***************************************************************************
// Output
//***************************************************************************

//---------------------------------------------------------------------------
int MediaConchLib::checker_get_report(const std::bitset<report_Max>& report_set, format f,
                                      const std::vector<std::string>& files,
                                      const std::vector<size_t>& policies_ids,
                                      const std::vector<std::string>& policies_contents,
                                      const std::map<std::string, std::string>& options,
                                      Checker_ReportRes* result,
                                      const std::string* display_name,
                                      const std::string* display_content)
{
    if (!files.size())
        return errorHttp_INVALID_DATA;

    if (use_daemon)
        return daemon_client->checker_get_report(report_set, f, files,
                                                 policies_ids, policies_contents,
                                                 options, result,
                                                 display_name, display_content);
    return core->checker_get_report(report_set, f, files,
                                    policies_ids, policies_contents,
                                    options, result,
                                    display_name, display_content);
}

//---------------------------------------------------------------------------
int MediaConchLib::checker_validate(report report, const std::vector<std::string>& files,
                                    const std::vector<size_t>& policies_ids,
                                    const std::vector<std::string>& policies_contents,
                                    std::vector<Checker_ValidateRes*>& result)
{
    if (!files.size())
        return errorHttp_INVALID_DATA;

    if (report != report_MediaConch && !policies_ids.size() && !policies_contents.size() &&
        report != report_MediaVeraPdf && report != report_MediaDpfManager)
        return errorHttp_INVALID_DATA;

    if (use_daemon)
        return daemon_client->checker_validate(report, files, policies_ids, policies_contents,
                                               result);

    return core->checker_validate(report, files, policies_ids, policies_contents,
                                  result);
}

//---------------------------------------------------------------------------
int MediaConchLib::remove_report(const std::vector<std::string>& files)
{
    if (!files.size())
        return errorHttp_INVALID_DATA;

    return core->remove_report(files);
}

//***************************************************************************
// Policy
//***************************************************************************

//---------------------------------------------------------------------------
void MediaConchLib::set_implementation_schema_file(const std::string& file)
{
    core->set_implementation_schema_file(file);
}

//---------------------------------------------------------------------------
const std::string& MediaConchLib::get_implementation_schema_file()
{
    return core->get_implementation_schema_file();
}

//---------------------------------------------------------------------------
void MediaConchLib::create_default_implementation_schema()
{
    core->create_default_implementation_schema();
}

//---------------------------------------------------------------------------
void MediaConchLib::set_implementation_verbosity(const std::string& verbosity)
{
    core->set_implementation_verbosity(verbosity);
}

//---------------------------------------------------------------------------
const std::string& MediaConchLib::get_implementation_verbosity()
{
    return core->get_implementation_verbosity();
}

//***************************************************************************
// XSL Transformation
//***************************************************************************

//---------------------------------------------------------------------------
int MediaConchLib::transform_with_xslt_file(const std::string& report, const std::string& file, std::string& result)
{
    return core->transform_with_xslt_file(report, file, result);
}

//---------------------------------------------------------------------------
int MediaConchLib::transform_with_xslt_memory(const std::string& report, const std::string& memory, std::string& result)
{
    return core->transform_with_xslt_memory(report, memory, result);
}

//***************************************************************************
// Configuration
//***************************************************************************

//---------------------------------------------------------------------------
void MediaConchLib::load_configuration()
{
    core->load_configuration();
}

//---------------------------------------------------------------------------
void MediaConchLib::set_configuration_file(const std::string& file)
{
    core->set_configuration_file(file);
}

//---------------------------------------------------------------------------
void MediaConchLib::load_plugins_configuration()
{
    core->load_plugins_configuration();
}

//---------------------------------------------------------------------------
void MediaConchLib::set_plugins_configuration_file(const std::string& file)
{
    core->set_plugins_configuration_file(file);
}

//---------------------------------------------------------------------------
const std::string& MediaConchLib::get_configuration_file() const
{
    return core->get_configuration_file();
}

//---------------------------------------------------------------------------
void MediaConchLib::set_compression_mode(compression compress)
{
    if (core)
        core->set_compression_mode(compress);
}

//---------------------------------------------------------------------------
int MediaConchLib::get_ui_poll_request() const
{
    return core->get_ui_poll_request();
}

//---------------------------------------------------------------------------
int MediaConchLib::get_ui_database_path(std::string& path) const
{
    return core->get_ui_database_path(path);
}

//***************************************************************************
// Policy
//***************************************************************************

//---------------------------------------------------------------------------
int MediaConchLib::xslt_policy_create(std::string& err, int parent_id)
{
    if (use_daemon)
        return daemon_client->xslt_policy_create(parent_id, err);
    return core->policies.create_xslt_policy(parent_id, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_duplicate(int id, std::string& err)
{
    if (use_daemon)
        return daemon_client->policy_duplicate(id, err);
    return core->policies.duplicate_policy(id, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_change_name(int id, const std::string& name, const std::string& description, std::string& err)
{
    if (use_daemon)
        return daemon_client->policy_change_name(id, name, description, err);
    return core->policies.policy_change_name(id, name, description, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::xslt_policy_rule_create(int policy_id, std::string& err)
{
    if (use_daemon)
        return daemon_client->xslt_policy_create(policy_id, err);
    return core->policies.create_xslt_policy_rule(policy_id, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::xslt_policy_rule_edit(int policy_id, int rule_id, const XsltPolicyRule *rule, std::string& err)
{
    if (use_daemon)
        return daemon_client->xslt_policy_rule_edit(policy_id, rule_id, rule, err);
    return core->policies.edit_xslt_policy_rule(policy_id, rule_id, rule, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::xslt_policy_rule_duplicate(int policy_id, int rule_id, std::string& err)
{
    if (use_daemon)
        return daemon_client->xslt_policy_rule_duplicate(policy_id, rule_id, err);
    return core->policies.duplicate_xslt_policy_rule(policy_id, rule_id, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::xslt_policy_rule_delete(int policy_id, int rule_id, std::string& err)
{
    if (use_daemon)
        return daemon_client->xslt_policy_rule_delete(policy_id, rule_id, err);
    return core->policies.delete_xslt_policy_rule(policy_id, rule_id, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::xslt_policy_create_from_file(const std::string& file, std::string& err)
{
    if (use_daemon)
        return daemon_client->xslt_policy_create_from_file(file);

    return core->policies.create_xslt_policy_from_file(file, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_import(const std::string& memory, std::string& err, const char* filename, bool is_system_policy)
{
    if (use_daemon)
        return daemon_client->policy_import(memory, err);

    return core->policies.import_policy_from_memory(memory, err, filename, is_system_policy);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_remove(int pos, std::string& err)
{
    if (use_daemon)
        return daemon_client->policy_remove(pos, err);

    return core->policies.erase_policy(pos, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_dump(int id, std::string& memory, std::string& err)
{
    if (use_daemon)
        return daemon_client->policy_dump(id, memory, err);

    return core->policies.dump_policy_to_memory(id, memory, err);
}

//---------------------------------------------------------------------------
size_t MediaConchLib::policy_get_policies_count() const
{
    if (use_daemon)
        return daemon_client->policy_get_policies_count();

    return core->policies.get_policies_size();
}

//---------------------------------------------------------------------------
Policy* MediaConchLib::policy_get(int id, std::string& err)
{
    if (use_daemon)
        return daemon_client->policy_get(id, err);

    return core->policies.get_policy(id, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_get_name(int id, std::string& name, std::string& err)
{
    if (use_daemon)
        return daemon_client->policy_get_name(id, name, err);

    return core->policies.policy_get_name(id, name, err);
}

//---------------------------------------------------------------------------
void MediaConchLib::policy_get_policies(std::vector<std::pair<size_t, std::string> >& policies)
{
    if (use_daemon)
        daemon_client->policy_get_policies(policies);
    else
        core->policies.get_policies(policies);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_save(int pos, std::string& err)
{
    if (use_daemon)
        return daemon_client->policy_save(pos, err);

    return core->policies.save_policy(pos, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_clear_policies(std::string& err)
{
    if (use_daemon)
        return daemon_client->policy_clear_policies(err);

    return core->policies.clear_policies(err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_get_values_for_type_field(const std::string& type, const std::string& field, std::vector<std::string>& values)
{
    return core->policy_get_values_for_type_field(type, field, values);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_get_fields_for_type(const std::string& type, std::vector<std::string>& fields)
{
    return core->policy_get_fields_for_type(type, fields);
}

//---------------------------------------------------------------------------
void MediaConchLib::set_use_daemon(bool use)
{
    if (!use)
        force_no_daemon = true;
    else
        force_no_daemon = false;
    use_daemon = use;
}

//---------------------------------------------------------------------------
bool MediaConchLib::get_use_daemon() const
{
    return use_daemon;
}

//---------------------------------------------------------------------------
void MediaConchLib::get_daemon_address(std::string& addr, int& port) const
{
    core->get_daemon_address(addr, port);
}

}
