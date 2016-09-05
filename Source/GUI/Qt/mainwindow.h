/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Common/Core.h"
#include "workerfiles.h"
#include "uisettings.h"
#include "DatabaseUi.h"

#include <QMainWindow>
#include <QFileInfo>
#include <QString>

namespace Ui {
    class MainWindow;
}

class QVBoxLayout;
class QLabel;
class QFile;
class QStringList;
class QLineEdit;

namespace MediaConch {

class MenuMainWindow;
class SettingsWindow;
class CheckerWindow;
class PoliciesWindow;
class DisplayWindow;
class VerbositySpinbox;
class PolicyCombobox;
class DisplayCombobox;
class FileRegistered;

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
        RUN_SETTINGS_VIEW,
    };
    void Run();

    // Functions
    void add_file_to_list(const QString& file, const QString& path, const QString& policy, const QString& display, const QString& verbosity);
    void remove_file_to_list(const QString& file);
    void update_policy_of_file_in_list(const QString& file, const QString& policy);
    void clear_file_list();
    void policy_to_delete(int row);
    int  xslt_policy_create_from_file(const QString& file);

    // UI
    void                        Ui_Init();
    void                        set_msg_to_status_bar(const QString& message);
    void                        clear_msg_in_status_bar();
    void                        drag_and_drop_files_action(const QStringList& files);

    // Helpers
    void                        set_widget_to_layout(QWidget* w);
    void                        remove_widget_from_layout(QWidget* w);
    int                         transform_with_xslt_file(const std::string& report, const std::string& file, std::string& result);
    int                         transform_with_xslt_memory(const std::string& report, const std::string& memory, std::string& result);
    int                         analyze(const std::vector<std::string>& files);
    int                         is_analyze_finished(const std::vector<std::string>& files, double& percent_done);
    int                         is_analyze_finished(const std::string& file, double& percent_done, MediaConchLib::report& report_kind);
    int                         validate(MediaConchLib::report report, const std::vector<std::string>& files,
                                         const std::vector<size_t>& policies_ids,
                                         const std::vector<std::string>& policies_contents,
                                         std::vector<MediaConchLib::Checker_ValidateRes*>& result);
    int                         validate(MediaConchLib::report report, const std::string& file,
                                         const std::vector<size_t>& policies_ids,
                                         const std::vector<std::string>& policies_contents,
                                         std::vector<MediaConchLib::Checker_ValidateRes*>& result);

    QString                     get_mediainfo_and_mediatrace_xml(const std::string& file, const std::string& display_name, const std::string& display_content);
    QString                     get_mediainfo_xml(const std::string& file, const std::string& display_name, const std::string& display_content);
    QString                     get_mediainfo_jstree(const std::string& file);
    QString                     get_mediatrace_xml(const std::string& file, const std::string& display_name, const std::string& display_content);
    QString                     get_mediatrace_jstree(const std::string& file);
    QString                     ask_for_schema_file();
    void                        checker_selected();
    void                        policies_selected();
    void                        display_selected();
    void                        settings_selected();
    void                        add_default_displays();
    void                        get_implementation_report(const std::string& file, QString& report, int *display=NULL, int *verbosity=NULL);
    int                         validate_policy(const std::string& file, QString& report, int policy=-1, int *display=NULL);

    void                        add_policy_to_list(const QString& policy);
    void                        clear_policy_list();
    void                        add_xslt_display(const QString& display_xslt);
    void                        remove_xslt_display();
    int                         policy_import(const QString& file, std::string& err);
    int                         xslt_policy_create(int parent_id, std::string& err);
    int                         policy_duplicate(int id, int dst_policy_id, std::string& err);
    int                         policy_move(int id, int dst_policy_id, std::string& err);
    int                         policy_change_info(int id, const std::string& name, const std::string& description, std::string& err);
    int                         policy_change_type(int id, const std::string& type, std::string& err);
    int                         policy_get(int pos, const std::string& format, MediaConchLib::Get_Policy& p);
    int                         policy_save(int pos, std::string& err);
    int                         policy_get_name(int pos, std::string& name, std::string& err);
    int                         policy_dump(int pos, std::string& memory, std::string& err);
    int                         policy_remove(int pos, std::string& err);
    int                         policy_export(int pos, std::string& err);
    int                         clear_policies(std::string& err);
    size_t                      get_policies_count() const;
    int                         xslt_policy_rule_create(int policy_id, std::string& err);
    int                         xslt_policy_rule_edit(int policy_id, int rule_id, const XsltPolicyRule *rule, std::string& err);
    int                         xslt_policy_rule_duplicate(int policy_id, int rule_id, int dst_policy_id, std::string& err);
    int                         xslt_policy_rule_move(int policy_id, int rule_id, int dst_policy_id, std::string& err);
    int                         xslt_policy_rule_delete(int policy_id, int rule_id, std::string& err);
    int                         get_values_for_type_field(const std::string& type, const std::string& field, std::vector<std::string>& values);
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
    void                        remove_file_registered_from_file(const std::string& file);

    int                         get_ui_database_path(std::string& path);
    void                        set_error_http(MediaConchLib::errorHttp code);

    void                          get_policies(const std::string&, MediaConchLib::Get_Policies&);
    QString                       get_local_folder() const;
    void                          get_registered_files(std::map<std::string, FileRegistered>& files);

    // Display related
    std::vector<QString>&         get_displays();
    int                           display_add_file(const QString& name, const QString& filename);
    void                          display_export_id(const QString& name);
    void                          display_delete_id(const QString& name);
    int                           get_display_index_by_filename(const std::string& filename);

    UiSettings&                 get_settings();

    const map<string, list<string> >* providePolicyExistingType() const { return &Policies::existing_type; }
    const list<Policies::validatorType>* providePolicyExistingValidator() const { return &Policies::existing_validator; }
    const list<string>* providePolicyExistingXsltOperator() const { return &Policies::existing_xsltOperator; }

private:
    Ui::MainWindow *ui;

    // Internal
    DatabaseUi                   *db;
    static const std::string      database_filename;
    MediaConchLib                 MCL;
    std::vector<QString>          displays_list;
    WorkerFiles                   workerfiles;
    UiSettings                    uisettings;
    Run_View                      current_view;

    // Visual elements
    QVBoxLayout*                Layout;
    CheckerWindow*              checkerView;
    PoliciesWindow*             policiesView;
    DisplayWindow*              displayView;
    MenuMainWindow*             MenuView;
    SettingsWindow*             settingsView;

    void                        create_and_configure_ui_database();
    int                         clearVisualElements();
    void                        clearPoliciesElements();
    void                        createCheckerView();
    void                        createPoliciesView();
    void                        createDisplayView();
    void                        createSettingsView();

    void                        fill_display_used(int *policy_i,
                                                  std::string& display_name, std::string& display_content,
                                                  const std::string*& dname, const std::string*& dcontent,
                                                  FileRegistered* fr);
    void                        fill_options_for_report(std::map<std::string, std::string>& opts, int *verbosity_p);

Q_SIGNALS:
    void status_bar_clear_message();
    void status_bar_show_message(const QString& message, int timeout);

private Q_SLOTS:
    void on_actionOpen_triggered();
    void on_actionChooseSchema_triggered();

    // View
    void on_actionChecker_triggered();
    void on_actionPolicies_triggered();
    void on_actionDisplay_triggered();
    void on_actionSettings_triggered();

public Q_SLOTS:
    //Help
    void on_actionAbout_triggered();
    void on_actionGettingStarted_triggered();
    void on_actionHowToUse_triggered();
    void on_actionDataFormat_triggered();
};

}

#endif // MAINWINDOW_H
