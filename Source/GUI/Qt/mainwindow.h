/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Common/Core.h"
#include "Common/SchematronPolicy.h"

#include <QMainWindow>
#include <QFileInfo>
#include <QString>

namespace Ui {
class MainWindow;
}

class QVBoxLayout;
class QLabel;
class QTreeWidgetItem;
class QFile;
class QStringList;

namespace MediaConch {

class MenuMainWindow;
class CheckerWindow;
class PoliciesTree;
class PoliciesMenu;
class PolicyMenu;
class GroupOfRules;
class RuleMenu;
class RuleEdit;
class WebView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // Functions
    void addFileToList(const QString& file);
    void policy_to_delete(int row);
    void exporting_to_schematron_file(int pos);
    void exporting_to_schematron(int pos);

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

    const list<string>* providePolicyExistingType() const {return &C.policies.existing_type; }
    const list<string>* providePolicyExistingField() const {return &C.policies.existing_field; }
    const list<Policies::validatorType>* providePolicyExistingValidator() const {return &C.policies.existing_validator; }

private:
    Ui::MainWindow *ui;
    PoliciesTree   *policiesTree;
    PoliciesMenu   *policiesMenu;
    PolicyMenu     *policyMenu;
    GroupOfRules   *groupOfRules;
    RuleMenu       *ruleMenu;
    RuleEdit       *ruleEdit;

    // Internal
    Core C;

    // Visual elements
    QVBoxLayout*                Layout;
    CheckerWindow*              MainView;
    MenuMainWindow*             MenuView;
    void                        clearVisualElements();
    void                        clearPoliciesElements();
    void                        createWebView();
    void                        updateWebView(String file, String policy);
    void                        updateWebView(QList<QFileInfo>& files, String policy);
    void                        setWebViewContent(QString& html);
    void                        removeTreeChildren(QTreeWidgetItem* item);
    void                        updatePoliciesTree();
    void                        updatePoliciesTreePolicy(Policy* p, QTreeWidgetItem *parent);
    void                        updatePoliciesTreePattern(SchematronPattern *p, QTreeWidgetItem *parent);
    void                        updatePoliciesTreeRule(SchematronRule *rule, QTreeWidgetItem *parent);
    void                        displayPoliciesTree();
    void                        createPoliciesTree();
    void                        displayPoliciesMenu();
    void                        createPoliciesMenu();
    void                        displayPolicyMenu(QString title);
    void                        createPolicyMenu();
    void                        displayGroupOfRules(QString title);
    void                        createGroupOfRules();
    void                        displayRuleMenu();
    void                        createRuleMenu();
    void                        displayRuleEdit(QTreeWidgetItem *);
    void                        createRuleEdit();
    void                        choose_schematron_file();

//***************************************************************************
// HELPER
//***************************************************************************

    int get_index_in_tree();
    QTreeWidgetItem *get_item_in_tree();
    int get_index_of_item_backXX(QTreeWidgetItem* item, size_t back);

private Q_SLOTS:

    void on_actionOpen_triggered();
    void on_actionChecker_triggered();
    void on_actionPolicies_triggered();
    void on_actionChooseSchematron_triggered();
    void on_actionAbout_triggered();
    void import_schematron();
    void save_policy();
    void save_policy_to();
    void add_new_policy();
    void add_new_gor();
    void add_new_rule();
    void add_new_assert();
    void duplicate_policy();
    void duplicate_gor();
    void duplicate_rule();
    void duplicate_assert();
    void delete_all_policies();
    void delete_policy();
    void delete_gor();
    void delete_rule();
    void delete_assert();
    void edit_policy_title();
    void edit_gor_title();
    void edit_assert_name(QString new_name);
    void edit_assert_type();
    void edit_assert_field();
    void edit_assert_validator();
    void edit_assert_value();
    void edit_assert_freeText();
    void policiesTree_selectionChanged();
    void assert_free_text_selected(bool);
    void assert_editor_selected(bool);
};

}

#endif // MAINWINDOW_H
