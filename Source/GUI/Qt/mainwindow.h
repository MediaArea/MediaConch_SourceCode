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
class CheckerWindow;
class PoliciesWindow;
class DisplayWindow;
class VerbositySpinbox;
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
    };
    void Run();

    // Functions
    void add_file_to_list(const QString& file, const QString& path, const QString& policy, const QString& display, const QString& verbosity);
    void remove_file_to_list(const QString& file);
    void clear_file_list();
    void policy_to_delete(int row);
    int  exporting_to_schematron_file(size_t pos);
    int  exporting_to_unknown_file(size_t pos);
    int  exporting_to_xslt_file(size_t pos);
    void exporting_policy(size_t pos);

    // UI
    void                        Ui_Init();
    void                        set_msg_to_status_bar(const QString& message);
    void                        clear_msg_in_status_bar();

    void                        add_policy_to_html_selection(QString& policies, QString& html, const QString& selector);
    void                        create_policy_options(QString& policies);
    void                        add_display_to_html_selection(QString& displays, QString& html, const QString& selector);
    void                        create_displays_options(QString& displays);

    // Helpers
    void                        set_widget_to_layout(QWidget* w);
    void                        remove_widget_from_layout(QWidget* w);
    int                         transform_with_xslt_file(const std::string& report, const std::string& file, std::string& result);
    int                         transform_with_xslt_memory(const std::string& report, const std::string& memory, std::string& result);
    int                         analyze(const std::vector<std::string>& files);
    int                         is_analyze_finished(const std::vector<std::string>& files, double& percent_done);
    int                         is_analyze_finished(const std::string& file, double& percent_done, MediaConchLib::report& report_kind);
    int                         validate(MediaConchLib::report report, const std::vector<std::string>& files,
                                         const std::vector<std::string>& policies_names,
                                         const std::vector<std::string>& policies_contents,
                                         std::vector<MediaConchLib::ValidateRes*>& result);
    int                         validate(MediaConchLib::report report, const std::string& file,
                                         const std::vector<std::string>& policies_names,
                                         const std::vector<std::string>& policies_contents,
                                         std::vector<MediaConchLib::ValidateRes*>& result);

    QString                     get_mediainfo_and_mediatrace_xml(const std::string& file, const std::string& display_name, const std::string& display_content);
    QString                     get_mediainfo_xml(const std::string& file, const std::string& display_name, const std::string& display_content);
    QString                     get_mediainfo_jstree(const std::string& file);
    QString                     get_mediatrace_xml(const std::string& file, const std::string& display_name, const std::string& display_content);
    QString                     get_mediatrace_jstree(const std::string& file);
    QString                     ask_for_schema_file();
    void                        checker_selected();
    void                        policies_selected();
    void                        display_selected();
    void                        add_default_policy();
    void                        add_default_displays();
    void                        get_implementation_report(const std::string& file, QString& report, int *display=NULL, int *verbosity=NULL);
    int                         validate_policy(const std::string& file, QString& report, int policy=-1, int *display=NULL);
    bool                        is_all_policies_saved();

    void                        add_policy_to_list(const QString& policy);
    void                        clear_policy_list();
    void                        add_xslt_display(const QString& display_xslt);
    void                        remove_xslt_display();
    int                         import_policy(const QString& file, std::string& err);
    bool                        policy_exists(const std::string& title);
    Policy                     *get_policy(size_t pos);
    void                        add_policy(Policy* policy);
    void                        remove_policy(size_t pos);
    void                        clear_policies();
    size_t                      get_policies_count() const;

    FileRegistered*             get_file_registered_from_file(const std::string& file);
    void                        remove_file_registered_from_file(const std::string& file);

    int                         get_ui_database_path(std::string& path);
    void                        set_error_http(MediaConchLib::errorHttp code);

    const std::vector<Policy *>&  get_all_policies() const;
    std::vector<QString>&         get_displays();
    QString                       get_local_folder() const;
    void                          get_registered_files(std::map<std::string, FileRegistered>& files);

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

    // Visual elements
    QVBoxLayout*                Layout;
    CheckerWindow*              checkerView;
    PoliciesWindow*             policiesView;
    DisplayWindow*              displayView;
    MenuMainWindow*             MenuView;
    VerbositySpinbox*           verbosity_box;

    void                        create_and_configure_ui_database();
    int                         clearVisualElements();
    void                        clearPoliciesElements();
    void                        createCheckerView();
    void                        createPoliciesView();
    void                        createDisplayView();
    void                        choose_schematron_file();
    void                        closeEvent(QCloseEvent *event);

    void                        fill_display_used(int *policy_i,
                                                  std::string& display_name, std::string& display_content,
                                                  const std::string*& dname, const std::string*& dcontent,
                                                  FileRegistered* fr);
    void                        fill_options_for_report(std::map<std::string, std::string>& opts, int *verbosity_p);

    Run_View current_view;

Q_SIGNALS:
    void status_bar_clear_message();
    void status_bar_show_message(const QString& message, int timeout);

private Q_SLOTS:
    void on_actionOpen_triggered();
    void on_actionChooseSchema_triggered();
    void on_actionVerbosity_triggered();

    // verbosity
    void verbosity_accepted();
    void verbosity_rejected();

    // View
    void on_actionChecker_triggered();
    void on_actionPolicies_triggered();
    void on_actionDisplay_triggered();

public Q_SLOTS:
    //Help
    void on_actionAbout_triggered();
    void on_actionGettingStarted_triggered();
    void on_actionHowToUse_triggered();
    void on_actionDataFormat_triggered();
};

}

#endif // MAINWINDOW_H
