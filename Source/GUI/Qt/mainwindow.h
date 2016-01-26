/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Common/Core.h"

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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // Functions
    void add_file_to_list(const QString& file);
    void policy_to_delete(int row);
    int  exporting_to_schematron_file(size_t pos);
    int  exporting_to_xslt_file(size_t pos);
    void exporting_policy(size_t pos);

    // UI
    void                        Ui_Init();
    QStatusBar                 *get_status_bar();
    void                        set_msg_to_status_bar(const QString& message);
    void                        set_msg_error_to_status_bar(const QString& message);
    void                        clear_msg_in_status_bar();

    // Helpers
    void                        Run();
    void                        set_widget_to_layout(QWidget* w);
    void                        remove_widget_from_layout(QWidget* w);
    int                         transform_with_xslt_file(const std::string& report, const std::string& file, std::string& result);
    int                         transform_with_xslt_memory(const std::string& report, const std::string& memory, std::string& result);
    void                        checker_add_file(const QString& file, int policy);
    void                        checker_add_files(QList<QFileInfo>& file, int policy);
    void                        checker_add_policy_file(const QString& file, QString& policy);
    void                        checker_add_policy_files(QList<QFileInfo>& file, QString& policy);
    int                         analyze(const std::vector<std::string>& files);
    int                         is_analyze_finished(const std::vector<std::string>& files, double& percent_done);
    QString                     get_implementationreport_xml(const std::string& file, const std::string& display_name, const std::string& display_content, bool& is_valid);
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
    bool                        validate_policy(const std::string& file, int policy, const std::string& display_name, const std::string& display_content, std::string& report);
    bool                        validate_policies(const std::string& file, const std::string& display_name,
                                                  const std::string& display_content,
                                                  std::string& report);
    bool                        is_all_policies_saved();

    void                        add_policy_to_list(const QString& policy);
    void                        clear_policy_list();
    void                        clear_file_list();
    void                        add_xslt_display(const QString& display_xslt);
    void                        remove_xslt_display();
    int                         import_policy(const QString& file, std::string& err);
    bool                        policy_exists(const std::string& title);
    Policy                     *get_policy(size_t pos);
    void                        add_policy(Policy* policy);
    void                        remove_policy(size_t pos);
    void                        clear_policies();
    size_t                      get_policies_count() const;

    const std::vector<Policy *>&    get_all_policies() const;
    std::vector<QString>&           get_displays();
    const std::vector<std::string>& policy_file_registered();
    QString                         get_local_folder() const;

    const map<string, list<string> >* providePolicyExistingType() const { return &Policies::existing_type; }
    const list<Policies::validatorType>* providePolicyExistingValidator() const { return &Policies::existing_validator; }
    const list<string>* providePolicyExistingXsltOperator() const { return &Policies::existing_xsltOperator; }

private:
    Ui::MainWindow *ui;

    // Internal
    MediaConchLib            MCL;
    std::vector<std::string> files;
    std::vector<std::string> policies;

    // Visual elements
    QVBoxLayout*                Layout;
    CheckerWindow*              checkerView;
    PoliciesWindow*             policiesView;
    DisplayWindow*              displayView;
    MenuMainWindow*             MenuView;
    QLineEdit*                  status_msg;

    std::vector<QString>        displays_list;

    int                         clearVisualElements();
    void                        clearPoliciesElements();
    void                        createCheckerView();
    void                        createPoliciesView();
    void                        createDisplayView();
    void                        choose_schematron_file();
    void                        closeEvent(QCloseEvent *event);

    enum Run_View
    {
        RUN_CHECKER_VIEW,
        RUN_POLICIES_VIEW,
        RUN_DISPLAY_VIEW,
    };
    Run_View current_view;

private Q_SLOTS:
    void on_actionOpen_triggered();
    void on_actionChecker_triggered();
    void on_actionPolicies_triggered();
    void on_actionDisplay_triggered();
    void on_actionChooseSchema_triggered();

    //Help
    void on_actionAbout_triggered();
    void on_actionGettingStarted_triggered();
    void on_actionHowToUse_triggered();
    void on_actionDataFormat_triggered();

    void update_status_bar();
};

}

#endif // MAINWINDOW_H
