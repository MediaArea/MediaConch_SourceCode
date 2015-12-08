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
    void addFileToList(const QString& file);
    void policy_to_delete(int row);
    int  exporting_to_schematron_file(int pos);
    int  exporting_to_xslt_file(int pos);
    void exporting_policy(int pos);

    // UI
    void                        Ui_Init                     ();

    // Helpers
    void                        Run();
    void                        set_widget_to_layout(QWidget* w);
    void                        remove_widget_from_layout(QWidget* w);
    std::string                 transformWithXsltFile(std::string& report, std::string trans);
    std::string                 transformWithXsltMemory(std::string& report, std::string memory);
    void                        checker_add_file(QString& file, int policy);
    void                        checker_add_files(QList<QFileInfo>& file, int policy);
    void                        checker_add_policy_file(QString& file, QString& policy);
    void                        checker_add_policy_files(QList<QFileInfo>& file, QString& policy);
    void                        analyze(std::string& file);
    void                        wait_analyze_finished();
    QString                     get_implementationreport_xml(const std::string& file);
    QString                     get_mediainfo_and_mediatrace_xml(const std::string& file);
    QString                     get_mediainfo_xml(const std::string& file);
    QString                     get_mediainfo_jstree(const std::string& file);
    QString                     get_mediatrace_xml(const std::string& file);
    QString                     get_mediatrace_jstree(const std::string& file);
    QString                     ask_for_schema_file();
    void                        checker_selected();
    void                        policies_selected();
    void                        display_selected();
    void                        add_default_policy();
    void                        add_default_displays();
    bool                        ValidatePolicy(const std::string& file, int policy, bool& valid, std::string& report);
    bool                        is_all_policies_saved();

    void                        addPolicyToList(QString& policy);
    void                        clearPolicyList();
    void                        clearFileList();
    void                        addXsltDisplay(QString& display_xslt);
    void                        removeXsltDisplay();

    const std::vector<Policy *>&    get_all_policies() const;
    std::vector<QString>&           get_displays();
    const std::vector<std::string>& policy_file_registered();
    QString                         get_local_folder() const;
    Policies&                       get_policies() { return C.policies; }

    const map<string, list<string> >* providePolicyExistingType() const {return &C.policies.existing_type; }

    const list<Policies::validatorType>* providePolicyExistingValidator() const {return &C.policies.existing_validator; }
    const list<string>* providePolicyExistingXsltOperator() const {return &C.policies.existing_xsltOperator; }

private:
    Ui::MainWindow *ui;

    // Internal
    Core C;

    // Visual elements
    QVBoxLayout*                Layout;
    CheckerWindow*              MainView;
    PoliciesWindow*             policiesView;
    DisplayWindow*              displayView;
    MenuMainWindow*             MenuView;

    std::vector<QString>        displaysList;

    int                         clearVisualElements();
    void                        clearPoliciesElements();
    void                        createWebView();
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
};

}

#endif // MAINWINDOW_H
