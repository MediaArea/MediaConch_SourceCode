/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include <QString>
#include <map>
#include <list>

#include "Common/Core.h"
#include "workerfiles.h"
#include "uisettings.h"
#include "DatabaseUi.h"


namespace Ui {
    class MainWindow;
}

class QVBoxLayout;
class QLabel;
class QFile;
class QStringList;
class QLineEdit;

namespace MediaConch {

class SettingsWindow;
class CheckerWindow;
class PoliciesWindow;
class PublicPoliciesWindow;
class DisplayWindow;
class DatabaseWindow;
class VerbositySpinbox;
class PolicyCombobox;
class DisplayCombobox;
class FileRegistered;
class WebView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // View selected
    enum Run_View
    {
        RUN_CHECKER_VIEW,
        RUN_POLICIES_VIEW,
        RUN_DISPLAY_VIEW,
        RUN_DATABASE_VIEW,
        RUN_SETTINGS_VIEW,
        RUN_PUBLIC_POLICIES_VIEW,
    };
    void                        Run();

    // Functions
    int                         add_file_to_list(const QString& file, const QString& path, const QString& policy,
                                                 const QString& display,
                                                 const QString& verbosity, bool fixer, bool,
                                                 const QStringList& options, std::string& err);
    int                         add_file_to_list(long id, std::string& err);
    int                         add_attachment_to_list(const QString& file, int policy, int display,
                                                       int verbosity, const QStringList& options,
                                                       std::string& err);
    void                        remove_file_to_list(long file_id);
    void                        remove_all_files_to_list();
    int                         update_policy_of_file_in_list(long file_id, long policy, std::string& error);
    int                         analyze_force_file_to_list(long id, std::string& err);
    void                        clear_file_list();

    // UI
    void                        Ui_Init();
    void                        set_msg_to_status_bar(const QString& message);
    void                        set_str_msg_to_status_bar(const std::string& message);
    void                        clear_msg_in_status_bar();
    void                        drag_and_drop_files_action(const QStringList& files);
    bool                        mil_has_curl_enabled();

    // Helpers
    void                        set_widget_to_layout(QWidget* w);
    void                        remove_widget_from_layout(QWidget* w);
    int                         analyze(const std::vector<std::string>& files, bool with_fixer, bool force,
                                        const std::vector<std::string>& options, std::vector<long>& files_id,
                                        std::string& err);
    int                         is_analyze_finished(const std::vector<std::string>& files,
                                                    std::vector<MediaConchLib::Checker_StatusRes>& res,std::string& err);
    int                         is_analyze_finished(const std::string& file, MediaConchLib::Checker_StatusRes& res, std::string& err);
    int                         validate(MediaConchLib::report report, const std::vector<std::string>& files,
                                         const std::vector<size_t>& policies_ids,
                                         const std::vector<std::string>& policies_contents,
                                         const std::map<std::string, std::string>& options,
                                         std::vector<MediaConchLib::Checker_ValidateRes*>& result, std::string& err);
    int                         validate(MediaConchLib::report report, const std::string& file,
                                         const std::vector<size_t>& policies_ids,
                                         const std::vector<std::string>& policies_contents,
                                         const std::map<std::string, std::string>& options,
                                         std::vector<MediaConchLib::Checker_ValidateRes*>& result, std::string& err);
    int                         validate_policy(long file_id, size_t policy_id,
                                                std::vector<MediaConchLib::Checker_ValidateRes*>& result, std::string& err);

    void                        checker_stop(const std::vector<long>& ids);
    QString                     get_mediainfo_and_mediatrace_xml(long file_id, const std::string& display_name,
                                                                 const std::string& display_content, std::string& err);
    QString                     get_mediainfo_xml(long file_id, const std::string& display_name,
                                                  const std::string& display_content, std::string& err);
    QString                     get_mediainfo_jstree(long file_id, std::string& err);
    QString                     get_mediatrace_xml(long file_id, const std::string& display_name,
                                                   const std::string& display_content, std::string& err);
    QString                     get_mediatrace_jstree(long file_id, std::string& err);
    QString                     ask_for_schema_file();
    void                        checker_selected();
    void                        policies_selected();
    void                        public_policies_selected();
    void                        display_selected();
    void                        database_selected();
    void                        settings_selected();
    void                        add_default_displays();
    void                        get_implementation_report(long file_id, QString& report, std::string& err,
                                                          int *display=NULL, int *verbosity=NULL);
    int                         validate_policy(long file_id, QString& report, std::string& err, int policy=-1, int *display=NULL);
    int                         checker_clear(long id, QString& err);
    int                         checker_clear(QString& err);
    int                         checker_file_from_id(long id, std::string& file, std::string& err);
    int                         checker_file_information(long id, MediaConchLib::Checker_FileInfo& file, std::string& err);

    void                        add_policy_to_list(const QString& policy);
    void                        clear_policy_list();
    void                        add_xslt_display(const QString& display_xslt);
    void                        remove_xslt_display();
    int                         policy_import(const QString& file, QString& err);
    int                         policy_import_data(const QString& data, QString& err);
    int                         xslt_policy_create(int parent_id, QString& err);
    int                         policy_duplicate(int id, int dst_policy_id, QString& err);
    int                         policy_move(int id, int dst_policy_id, QString& err);
    int                         policy_change_info(int id, const std::string& name, const std::string& description,
                                                   const std::string& license, QString& err);
    int                         policy_change_type(int id, const std::string& type, QString& err);
    int                         policy_change_is_public(int id, bool is_public, QString& err);
    int                         policy_get(int pos, const std::string& format, MediaConchLib::Get_Policy& p, QString& err);
    int                         policy_save(int pos, QString& err);
    int                         policy_get_name(int pos, std::string& name, QString& err);
    int                         policy_dump(int pos, std::string& memory, QString& err);
    int                         policy_remove(int pos, QString& err);
    int                         policy_export(int pos, QString& err);
    int                         policy_export_data(const QString& policy, const QString& p_name, QString& err);
    int                         clear_policies(QString& err);
    size_t                      get_policies_count(QString& err) const;
    long                        xslt_policy_create_from_file(long file_id, QString& err);
    int                         xslt_policy_rule_create(int policy_id, QString& err);
    XsltPolicyRule             *xslt_policy_rule_get(int policy_id, int rule_id, QString& err);
    int                         xslt_policy_rule_edit(int policy_id, int rule_id, const XsltPolicyRule *rule, QString& err);
    int                         xslt_policy_rule_duplicate(int policy_id, int rule_id, int dst_policy_id, QString& err);
    int                         xslt_policy_rule_move(int policy_id, int rule_id, int dst_policy_id, QString& err);
    int                         xslt_policy_rule_delete(int policy_id, int rule_id, QString& err);
    int                         get_values_for_type_field(const std::string& type, const std::string& field,
                                                          std::vector<std::string>& values);
    int                         get_fields_for_type(const std::string& type, std::vector<std::string>& fields);

    int                         select_correct_policy();
    int                         select_correct_display();
    int                         select_correct_verbosity();
    std::string                 select_correct_save_report_path();
    std::string                 select_correct_save_policy_path();
    std::string                 select_correct_save_display_path();
    std::string                 select_correct_load_files_path();
    std::string                 select_correct_load_policy_path();
    std::string                 select_correct_load_display_path();
    void                        set_last_save_report_path(const std::string& path);
    void                        set_last_save_policy_path(const std::string& path);
    void                        set_last_save_display_path(const std::string& path);
    void                        set_last_load_files_path(const std::string& path);
    void                        set_last_load_policy_path(const std::string& path);
    void                        set_last_load_display_path(const std::string& path);

    FileRegistered*             get_file_registered_from_file(const std::string& file);
    FileRegistered*             get_file_registered_from_id(long file_id);
    std::string                 get_filename_from_registered_file_id(long file_id);

    void                        create_policy_from_file(const FileRegistered* file);

    int                         get_ui_database_path(std::string& path);
    void                        get_version(std::string& version);

    int                         get_policies(const std::string&, MediaConchLib::Get_Policies&, QString& err);
    QString                     get_local_folder() const;
    void                        get_registered_files(std::map<std::string, FileRegistered*>& files);

    void                        checker_list(std::vector<long>& files, QString& error);

    // Display related
    std::vector<QString>&       get_displays();
    int                         display_add_file(const QString& name, const QString& filename);
    void                        display_export_id(const QString& name);
    void                        display_delete_id(const QString& name);
    int                         get_display_index_by_filename(const std::string& filename);

    UiSettings&                 get_settings();

    const std::map<std::string, std::list<std::string> >* providePolicyExistingType() const { return &Policies::existing_type; }
    const std::list<Policies::validatorType>* providePolicyExistingValidator() const { return &Policies::existing_validator; }
    const std::list<std::string>* providePolicyExistingXsltOperator() const { return &Policies::existing_xsltOperator; }

    WebView*                      web_view;

private:
    Ui::MainWindow *ui;

    // Internal
    DatabaseUi                   *db;
    static const std::string      database_filename;
    static const std::string      version;
    MediaConchLib                 MCL;
    std::vector<QString>          displays_list;
    WorkerFiles                   workerfiles;
    UiSettings                    uisettings;
    Run_View                      current_view;

    long                          user;

    // Visual elements
    QVBoxLayout*                Layout;
    CheckerWindow*              checkerView;
    PoliciesWindow*             policiesView;
    PublicPoliciesWindow*       publicPoliciesView;
    DisplayWindow*              displayView;
    DatabaseWindow*             databaseView;
    SettingsWindow*             settingsView;

    void                        create_and_configure_ui_database();
    int                         clearVisualElements();
    void                        clearPoliciesElements();
    void                        clearPublicPoliciesElements();
    void                        createCheckerView();
    void                        createPoliciesView();
    void                        createPublicPoliciesView();
    void                        createDisplayView();
    void                        createDatabaseView();
    void                        createSettingsView();

    void                        fill_display_used(int *policy_i,
                                                  std::string& display_name, std::string& display_content,
                                                  std::string*& dname, std::string*& dcontent,
                                                  FileRegistered* fr);
    void                        fill_options_for_report(std::map<std::string, std::string>& opts, int *verbosity_p);

protected:
    void closeEvent(QCloseEvent *event);

//***************************************************************************
// SIGNAL
//***************************************************************************
Q_SIGNALS:
    void status_bar_clear_message();
    void status_bar_show_message(const QString& message, int timeout);
    void execute_javascript(const QString& script);

private Q_SLOTS:
    void on_actionOpen_triggered();
    void on_actionChooseSchema_triggered();

public Q_SLOTS:
    // View
    void on_actionChecker_triggered();
    void on_actionPolicies_triggered();
    void on_actionPublicPolicies_triggered();
    void on_actionDisplay_triggered();
    void on_actionDatabase_triggered();
    void on_actionSettings_triggered();

    //Help
    void on_actionAbout_triggered();
    void on_actionGettingStarted_triggered();
    void on_actionHowToUse_triggered();
    void on_actionDataFormat_triggered();

    bool close();
    void loadFinished_Custom(bool ok);
    void loadProgress_Custom(int progress);
};

}

#endif // MAINWINDOW_H
