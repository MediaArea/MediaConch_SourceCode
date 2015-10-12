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
    void exporting_to_schematron(int pos);
    int  exporting_to_xslt_file(int pos);
    void exporting_to_xslt(int pos);

    // UI
    void                        Ui_Init                     ();

    // Helpers
    void                        Run();
    QString                     Run(Core::tool, Core::format, String& file);
    void                        set_widget_to_layout(QWidget* w);
    void                        remove_widget_from_layout(QWidget* w);
    String                      transformWithXslt(String report, String trans);
    void                        checker_add_file(QString& file, QString& policy);
    void                        checker_add_files(QList<QFileInfo>& file, QString& policy);
    void                        checker_add_xslt_file(QString& file, QString& xslt, QString& display_xslt);
    void                        checker_add_xslt_files(QList<QFileInfo>& file, QString& xslt, QString& display_xslt);
    QString                     get_trace_for_file(const QString& filename);
    QString                     ask_for_schematron_file();
    void                        checker_selected();
    void                        policies_selected();
    void                        add_default_policy();
    bool                        ValidatePolicy(String& policy, bool& valid, String& report);

    void                        addXsltToList(QString& xslt, QString& display_xslt);
    void                        clearXsltList();
    void                        clearFileList();
    QStringList                 get_policy_titles();

    Policies&                   get_policies() { return C.policies; }

    const list<string>* providePolicyExistingType() const {return &C.policies.existing_type; }
    const list<string>* providePolicyExistingField() const {return &C.policies.existing_field; }
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
    MenuMainWindow*             MenuView;

    void                        clearVisualElements();
    void                        clearPoliciesElements();
    void                        createWebView();
    void                        updateWebView(String file, String policy);
    void                        updateWebView(QList<QFileInfo>& files, String policy);
    void                        createPoliciesView();
    void                        choose_schematron_file();

private Q_SLOTS:
    void on_actionOpen_triggered();
    void on_actionChecker_triggered();
    void on_actionPolicies_triggered();
    void on_actionChooseSchematron_triggered();
    void on_actionAbout_triggered();
};

}

#endif // MAINWINDOW_H
