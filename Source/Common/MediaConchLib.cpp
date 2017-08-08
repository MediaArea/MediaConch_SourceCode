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
#include <ZenLib/Dir.h>
#include <ZenLib/Ztring.h>
#include <ZenLib/ZtringList.h>
#include "MediaConchLib.h"
#include "Core.h"
#include "Reports.h"
#include "DaemonClient.h"
#include "Policy.h"
#include "Http.h"
#include "LibEventHttp.h"
#include "generated/PolicySample1.h"
#include "generated/PolicySample4.h"
#include "generated/PolicySample5.h"
#include "generated/PolicySample6.h"
#include "generated/PolicySample7.h"
#include "generated/PolicySample8.h"

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
const std::string MediaConchLib::display_simple_name = std::string("SIMPLE");
const std::string MediaConchLib::display_csv_name = std::string("CSV");

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
int MediaConchLib::init(std::string& err)
{
    load_configuration();
    if (!force_no_daemon)
    {
        use_daemon = core->is_using_daemon();
        if (use_daemon)
        {
            daemon_client = new DaemonClient(this);
            if (daemon_client->init(err) < 0)
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

//---------------------------------------------------------------------------
void MediaConchLib::reset_daemon_client()
{
    if (!daemon_client)
        return;

    daemon_client->reset();
}

//***************************************************************************
// Options
//***************************************************************************

//---------------------------------------------------------------------------
int MediaConchLib::test_mil_option(const std::string& key, std::string& value, std::string& report)
{
    if (!value.size())
        value = "1";

    MediaInfoNameSpace::String Option(ZenLib::Ztring().From_UTF8(key));
    MediaInfoNameSpace::String Value(ZenLib::Ztring().From_UTF8(value));

    String Result = core->Menu_Option_Preferences_Option(Option, Value);
    if (!Result.empty())
    {
        report = Ztring(Result).To_UTF8();
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
bool MediaConchLib::mil_has_curl_enabled()
{
    String Result = core->Menu_Option_Preferences_Option(__T("info_canhandleurls"), __T(""));
    if (Result.empty())
        return false;

    if (Result == __T("1"))
        return true;

    return false;
}

//---------------------------------------------------------------------------
bool MediaConchLib::ReportAndFormatCombination_IsValid(const std::vector<std::string>&,
                                                       const std::bitset<MediaConchLib::report_Max>& reports,
                                                       const std::string& display, MediaConchLib::format& Format,
                                                       std::string& reason)
{
    if (reports[MediaConchLib::report_MicroMediaTrace] && Format != MediaConchLib::format_Xml)
    {
        reason = "MicroMediaTrace requires an XML output.";
        return false;
    }

    if (Format != MediaConchLib::format_Max && !display.empty())
    {
        reason = "If a display is used, no other output format can be used.";
        return false;
    }

    if (reports[MediaConchLib::report_MediaInfo] && (Format == MediaConchLib::format_Simple))
    {
        reason = "Simple output is not currently supported for MediaInfo reports.";
        return false;
    }

    if (reports[MediaConchLib::report_MediaInfo] && (Format == MediaConchLib::format_Html))
    {
        reason = "HTML output is not currently supported for MediaInfo reports.";
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
// MediaConch
//***************************************************************************

//---------------------------------------------------------------------------
int MediaConchLib::mediaconch_get_plugins(std::vector<std::string>& plugins, std::string& error)
{
    if (use_daemon)
        return daemon_client->mediaconch_get_plugins(plugins, error);

    return core->mediaconch_get_plugins(plugins, error);
}

//---------------------------------------------------------------------------
int MediaConchLib::mediaconch_list_watch_folders(std::vector<std::string>& folders, std::string& error)
{
    if (use_daemon)
        return daemon_client->mediaconch_list_watch_folders(folders, error);

    return core->mediaconch_list_watch_folders(folders, error);
}

//---------------------------------------------------------------------------
int MediaConchLib::mediaconch_watch_folder(const std::string& folder, const std::string& folder_reports,
                                           const std::vector<std::string>& plugins, const std::vector<std::string>& policies,
                                           long *in_user, bool recursive,
                                           const std::vector<std::pair<std::string,std::string> >& options,
                                           long& user_id, std::string& error)
{
    if (use_daemon)
        return daemon_client->mediaconch_watch_folder(folder, folder_reports, plugins, policies,
                                                      in_user, recursive, options, user_id, error);

    return core->mediaconch_watch_folder(folder, folder_reports, plugins, policies,
                                         in_user, recursive, options, user_id, error);
}

//---------------------------------------------------------------------------
int MediaConchLib::mediaconch_edit_watch_folder(const std::string& folder, const std::string& folder_reports,
                                                std::string& error)
{
    if (use_daemon)
        return daemon_client->mediaconch_edit_watch_folder(folder, folder_reports, error);

    return core->mediaconch_edit_watch_folder(folder, folder_reports, error);
}

//---------------------------------------------------------------------------
int MediaConchLib::mediaconch_remove_watch_folder(const std::string& folder, std::string& error)
{
    if (use_daemon)
        return daemon_client->mediaconch_remove_watch_folder(folder, error);

    return core->mediaconch_remove_watch_folder(folder, error);
}

//***************************************************************************
// Analyze
//***************************************************************************

//---------------------------------------------------------------------------
int MediaConchLib::checker_analyze(int user, const std::vector<std::string>& files,
                                   const std::vector<std::string>& plugins,
                                   const std::vector<std::pair<std::string,std::string> >& options,
                                   std::vector<long>& files_id, std::string& error, bool force_analyze, bool mil_analyze)
{
    bool registered = false;
    for (size_t i = 0; i < files.size(); ++i)
    {
        long file_id;
        int ret = checker_analyze(user, files[i], plugins, options, registered, file_id, error, force_analyze, mil_analyze);
        if (ret < 0)
            return ret;
        files_id.push_back(file_id);
    }

    return errorHttp_NONE;
}

//---------------------------------------------------------------------------
int MediaConchLib::checker_analyze(int user, const std::string& file, const std::vector<std::string>& plugins,
                                   const std::vector<std::pair<std::string,std::string> >& options,
                                   bool& registered, long& file_id, std::string& error, bool force_analyze, bool mil_analyze)
{
    if (!file.length())
        return errorHttp_INVALID_DATA;

    if (use_daemon)
        return daemon_client->checker_analyze(user, file, plugins, options, registered, force_analyze, mil_analyze, file_id, error);

    long id = core->checker_analyze(user, file, registered, options, plugins, error, force_analyze, mil_analyze);
    if (id < 0)
        return -1;

    file_id = id;
    return 0;
}

//---------------------------------------------------------------------------
int MediaConchLib::checker_status(int user, const std::vector<long>& files_id, std::vector<Checker_StatusRes>& res,
                                  std::string& error)
{
    int done = errorHttp_TRUE;

    for (size_t i = 0; i < files_id.size(); ++i)
    {
        Checker_StatusRes r;
        int ret = checker_status(user, files_id[i], r, error);

        if (ret < 0)
            return ret;

        res.push_back(r);
    }

    return done;
}

//---------------------------------------------------------------------------
int MediaConchLib::checker_status(int user, long file_id, Checker_StatusRes& res, std::string& error)
{
    if (file_id < 0)
        return errorHttp_NONE;

    if (use_daemon)
        return daemon_client->checker_status(user, file_id, res, error);
    return core->checker_status(user, file_id, res, error);
}

//---------------------------------------------------------------------------
int MediaConchLib::checker_list(int user, std::vector<std::string>& vec, std::string& error)
{
    if (use_daemon)
        return daemon_client->checker_list(user, vec, error);

    return core->checker_list(user, vec, error);
}

//---------------------------------------------------------------------------
int MediaConchLib::checker_list(int user, std::vector<long>& vec, std::string& error)
{
    //TODO?
    // if (use_daemon)
    //     return daemon_client->checker_list_ids(user, vec, error);
    return core->checker_list(user, vec, error);
}

//---------------------------------------------------------------------------
int MediaConchLib::checker_file_from_id(int user, long id, std::string& filename, std::string& error)
{
    if (use_daemon)
        return daemon_client->checker_file_from_id(user, id, filename, error);

    return core->checker_file_from_id(user, id, filename, error);
}

//---------------------------------------------------------------------------
long MediaConchLib::checker_id_from_filename(int user, const std::string& filename,
                                             const std::vector<std::pair<std::string,std::string> >& options,
                                             std::string& error)
{
    if (use_daemon)
        return daemon_client->checker_id_from_filename(user, filename, options, error);

    return core->checker_id_from_filename(user, filename, options, error);
}

//---------------------------------------------------------------------------
int MediaConchLib::checker_file_information(int user, long id, MediaConchLib::Checker_FileInfo& info,
                                            std::string& error)
{
    if (use_daemon)
        return daemon_client->checker_file_information(user, id, info, error);

    return core->checker_file_information(user, id, info, error);
}

//***************************************************************************
// Output
//***************************************************************************

//---------------------------------------------------------------------------
int MediaConchLib::checker_get_report(CheckerReport& c_report, Checker_ReportRes* result, std::string& error)
{
    if (!c_report.files.size())
    {
        error = "No file given for report.";
        return -1;
    }

    if (use_daemon)
        return daemon_client->checker_get_report(c_report, result, error);
    return core->reports.checker_get_report(c_report, result, error);
}

//---------------------------------------------------------------------------
int MediaConchLib::checker_validate(int user, report report, const std::vector<long>& files,
                                    const std::vector<size_t>& policies_ids,
                                    const std::vector<std::string>& policies_contents,
                                    const std::map<std::string, std::string>& options,
                                    std::vector<Checker_ValidateRes*>& result, std::string& error)
{
    if (!files.size())
        return errorHttp_INVALID_DATA;

    if (report != report_MediaConch && !policies_ids.size() && !policies_contents.size() &&
        report != report_MediaVeraPdf && report != report_MediaDpfManager)
        return errorHttp_INVALID_DATA;

    if (use_daemon)
        return daemon_client->checker_validate(user, report, files,
                                               policies_ids, policies_contents,
                                               options,
                                               result, error);

    return core->reports.checker_validate(user, report, files,
                                          policies_ids, policies_contents,
                                          options,
                                          result, error);
}

//---------------------------------------------------------------------------
int MediaConchLib::checker_clear(int user, const std::vector<long>& files, std::string& error)
{
    if (use_daemon)
        return daemon_client->checker_clear(user, files, error);

    return core->checker_clear(user, files, error);
}

//---------------------------------------------------------------------------
int MediaConchLib::checker_stop(int user, const std::vector<long>& files, std::string& error)
{
    if (use_daemon)
        return daemon_client->checker_stop(user, files, error);

    return core->checker_stop(user, files, error);
}

//***************************************************************************
// Policy
//***************************************************************************

//---------------------------------------------------------------------------
int MediaConchLib::xslt_policy_create(int user, std::string& err, const std::string& type, int parent_id)
{
    if (use_daemon)
        return daemon_client->xslt_policy_create(user, type, parent_id, err);
    return core->policies.create_xslt_policy(user, type, parent_id, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_import(int user, const std::string& memory, std::string& err, const char* filename, bool is_system_policy)
{
    if (use_daemon)
        return daemon_client->policy_import(user, memory, err);

    return core->policies.import_policy_from_memory(user, memory, err, filename, is_system_policy);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_remove(int user, int pos, std::string& err)
{
    if (use_daemon)
        return daemon_client->policy_remove(user, pos, err);

    return core->policies.erase_policy(user, pos, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_dump(int user, int id, bool must_be_public, std::string& memory, std::string& err)
{
    if (use_daemon)
        return daemon_client->policy_dump(user, id, must_be_public, memory, err);

    return core->policies.dump_policy_to_memory(user, id, must_be_public, memory, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_duplicate(int user, int id, int dst_policy_id, int *dst_user, bool must_be_public, std::string& err)
{
    if (use_daemon)
        return daemon_client->policy_duplicate(user, id, dst_policy_id, dst_user, must_be_public, err);
    return core->policies.duplicate_policy(user, id, dst_policy_id, dst_user, must_be_public, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_move(int user, int id, int dst_policy_id, std::string& err)
{
    if (use_daemon)
        return daemon_client->policy_move(user, id, dst_policy_id, err);
    return core->policies.move_policy(user, id, dst_policy_id, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_save(int user, int pos, std::string& err)
{
    if (use_daemon)
        return daemon_client->policy_save(user, pos, err);

    return core->policies.save_policy(user, pos, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_change_info(int user, int id, const std::string& name, const std::string& description,
                                      const std::string& license, std::string& err)
{
    if (use_daemon)
        return daemon_client->policy_change_info(user, id, name, description, license, err);
    return core->policies.policy_change_info(user, id, name, description, license, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_change_type(int user, int id, const std::string& type, std::string& err)
{
    if (use_daemon)
        return daemon_client->policy_change_type(user, id, type, err);
    return core->policies.policy_change_type(user, id, type, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_change_is_public(int user, int id, bool is_public, std::string& err)
{
    if (use_daemon)
        return daemon_client->policy_change_is_public(user, id, is_public, err);
    return core->policies.policy_change_is_public(user, id, is_public, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_get(int user, int id, const std::string& format, bool must_be_public,
                              MediaConchLib::Get_Policy& policy, std::string& err)
{
    if (use_daemon)
    {
        int ret = daemon_client->policy_get(user, id, format, must_be_public, policy, err);
        return ret;
    }

    return core->policies.policy_get(user, id, format, must_be_public, policy, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_get_name(int user, int id, std::string& name, std::string& err)
{
    if (use_daemon)
        return daemon_client->policy_get_name(user, id, name, err);

    return core->policies.policy_get_name(user, id, name, err);
}

//---------------------------------------------------------------------------
size_t MediaConchLib::policy_get_policies_count(int user, std::string& err) const
{
    if (use_daemon)
        return daemon_client->policy_get_policies_count(user, err);

    return core->policies.get_policies_size(user, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_clear_policies(int user, std::string& err)
{
    if (use_daemon)
        return daemon_client->policy_clear_policies(user, err);

    return core->policies.clear_policies(user, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_get_policies(int user, const std::vector<int>& ids, const std::string& format, MediaConchLib::Get_Policies& policies, std::string& err)
{
    if (use_daemon)
        return daemon_client->policy_get_policies(user, ids, format, policies, err);
    else
        return core->policies.get_policies(user, ids, format, policies, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_get_public_policies(std::vector<Policy_Public_Policy*>& policies, std::string& err)
{
    if (use_daemon)
        return daemon_client->policy_get_public_policies(policies, err);

    return core->policies.get_public_policies(policies, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_get_policies_names_list(int user, std::vector<std::pair<int, std::string> >& policies, std::string& err)
{
    if (use_daemon)
        return daemon_client->policy_get_policies_names_list(user, policies, err);
    else
        return core->policies.get_policies_names_list(user, policies, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::xslt_policy_create_from_file(int user, long file, std::string& err)
{
    if (use_daemon)
        return daemon_client->xslt_policy_create_from_file(user, file, err);

    return core->policies.create_xslt_policy_from_file(user, file, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::xslt_policy_rule_create(int user, int policy_id, std::string& err)
{
    if (use_daemon)
        return daemon_client->xslt_policy_rule_create(user, policy_id, err);
    return core->policies.create_xslt_policy_rule(user, policy_id, err);
}

//---------------------------------------------------------------------------
XsltPolicyRule *MediaConchLib::xslt_policy_rule_get(int user, int policy_id, int id, std::string& err)
{
    if (use_daemon)
        return daemon_client->xslt_policy_rule_get(user, policy_id, id, err);
    return core->policies.get_xslt_policy_rule(user, policy_id, id, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::xslt_policy_rule_edit(int user, int policy_id, int rule_id, const XsltPolicyRule *rule, std::string& err)
{
    if (use_daemon)
        return daemon_client->xslt_policy_rule_edit(user, policy_id, rule_id, rule, err);
    return core->policies.edit_xslt_policy_rule(user, policy_id, rule_id, rule, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::xslt_policy_rule_duplicate(int user, int policy_id, int rule_id, int dst_policy_id, std::string& err)
{
    if (use_daemon)
        return daemon_client->xslt_policy_rule_duplicate(user, policy_id, rule_id, dst_policy_id, err);
    return core->policies.duplicate_xslt_policy_rule(user, policy_id, rule_id, dst_policy_id, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::xslt_policy_rule_move(int user, int policy_id, int rule_id, int dst_policy_id, std::string& err)
{
    if (use_daemon)
        return daemon_client->xslt_policy_rule_move(user, policy_id, rule_id, dst_policy_id, err);
    return core->policies.move_xslt_policy_rule(user, policy_id, rule_id, dst_policy_id, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::xslt_policy_rule_delete(int user, int policy_id, int rule_id, std::string& err)
{
    if (use_daemon)
        return daemon_client->xslt_policy_rule_delete(user, policy_id, rule_id, err);
    return core->policies.delete_xslt_policy_rule(user, policy_id, rule_id, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_get_values_for_type_field(const std::string& type, const std::string& field, std::vector<std::string>& values, std::string& err)
{
    return core->policy_get_values_for_type_field(type, field, values, err);
}

//---------------------------------------------------------------------------
int MediaConchLib::policy_get_fields_for_type(const std::string& type, std::vector<std::string>& fields, std::string& err)
{
    return core->policy_get_fields_for_type(type, fields, err);
}

//---------------------------------------------------------------------------
std::string MediaConchLib::XSLT_Policy_Rule::to_str() const
{
    std::stringstream out;

    out << "{";
    out << "id: " << id;
    if (name.size())
        out << ",\"name\":\"" << name << "\"";
    if (tracktype.size())
        out << ",\"tracktype\":\"" << tracktype << "\"";
    if (field.size())
        out << ",\"field\":\"" << field << "\"";
    if (scope.size())
        out << ",\"scope\":\"" << scope << "\"";
    out << ",occurrence:" << occurrence;
    if (ope.size())
        out << ",\"ope\":\"" << ope << "\"";
    if (value.size())
        out << ",\"value\":\"" << value << "\"";
    out << "}";
    return out.str();
}

//---------------------------------------------------------------------------
std::string MediaConchLib::Policy_Policy::to_str() const
{
    std::stringstream out;

    out << "{\"id\":" << id;
    out << ",\"parent_id\":" << parent_id;
    out << ",\"is_system\":" << std::boolalpha << is_system;
    if (is_public)
        out << ",\"is_public\":" << std::boolalpha << is_public;
    out << ",\"kind\":\"" << kind;
    out << "\",\"type\":\"" << type;
    out << "\",\"name\":\"" << name;
    out << "\",\"description\":\"" << description;
    out << "\",\"license\":\"" << license;
    out << "\",\"children\":[";
    for (size_t i = 0; i < children.size(); ++i)
    {
        if (i)
            out << ",";
        if (children[i].first == 0 && children[i].second.policy)
            out << children[i].second.policy->to_str();
        else if (children[i].first == 1 && children[i].second.rule)
            out << children[i].second.rule->to_str();
    }
    out << "]}";
    return out.str();
}

//***************************************************************************
// Implementation configuration
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
// Configuration
//***************************************************************************

//---------------------------------------------------------------------------
void MediaConchLib::load_configuration()
{
    core->load_configuration();
}

//---------------------------------------------------------------------------
void MediaConchLib::set_default_scheduler_max_threads(size_t nb)
{
    core->set_default_scheduler_max_threads(nb);
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
// Daemon
//***************************************************************************

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

//***************************************************************************
// Helper
//***************************************************************************

//---------------------------------------------------------------------------
int MediaConchLib::load_system_policy()
{
    std::string path = Core::get_local_data_path();
    path += "policies/";

    std::string err;

    std::string policy_path = path + "policy_sample_1.xml";
    std::string memory(policy_sample_1);
    core->policies.import_policy_from_memory(-1, memory, err, path.c_str(), true);

    policy_path = path + "policy_sample_4.xml";
    memory = std::string(policy_sample_4);
    core->policies.import_policy_from_memory(-1, memory, err, path.c_str(), true);

    policy_path = path + "policy_sample_5.xml";
    memory = std::string(policy_sample_5);
    core->policies.import_policy_from_memory(-1, memory, err, path.c_str(), true);

    policy_path = path + "policy_sample_6.xml";
    memory = std::string(policy_sample_6);
    core->policies.import_policy_from_memory(-1, memory, err, path.c_str(), true);

    policy_path = path + "policy_sample_7.xml";
    memory = std::string(policy_sample_7);
    core->policies.import_policy_from_memory(-1, memory, err, path.c_str(), true);

    policy_path = path + "policy_sample_8.xml";
    memory = std::string(policy_sample_8);
    core->policies.import_policy_from_memory(-1, memory, err, path.c_str(), true);

    return 0;
}

//---------------------------------------------------------------------------
int MediaConchLib::load_existing_policy()
{
    std::string path = Core::get_local_data_path();
    path += "policies/";

    ZenLib::Ztring Dir_Name = ZenLib::Ztring().From_UTF8(path);
    ZenLib::ZtringList list = ZenLib::Dir::GetAllFileNames(Dir_Name);

    for (size_t i = 0; i < list.size(); ++i)
    {
        std::string file = list[i].To_UTF8();
        size_t pos;
#if defined(_WIN32)
        while (1)
        {
            pos = file.find("\\");
            if (pos == std::string::npos)
                break;

            if (pos == 0 || file[pos - 1] != '/')
                file.replace(pos, 1, "/");
            else
                file.replace(pos, 1, "");
		}
#endif
        pos = file.find("/", path.size());
        std::string user_str = file.substr(path.size(), pos - path.size());
        int user = strtol(user_str.c_str(), NULL, 10);
        std::string err;

        core->policies.import_policy_from_file(user, file, err);
    }

    return 0;
}

//---------------------------------------------------------------------------
void MediaConchLib::register_log_callback(void (*log)(struct MediaInfo_Event_Log_0* Event))
{
    core->ecb.log = log;
}

}
