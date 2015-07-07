/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Common/Core.h"

#include <QMainWindow>
#include <QString>

namespace Ui {
class MainWindow;
}

class QVBoxLayout;
class QPlainTextEdit;
class QLabel;
class PoliciesTree;
class PoliciesMenu;
class PolicyMenu;
class GroupOfRules;
class RuleMenu;
class RuleEdit;
class QTreeWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // Functions
    void dragEnterEvent         (QDragEnterEvent *event);
    void dropEvent              (QDropEvent *event);
    void policy_to_delete(int row);
    void exporting_to_schematron_file(int pos);

    // UI
    void                        Ui_Init                     ();

    // Helpers
    void                        Run();
    QString                     ask_for_schematron_file();

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
    QPlainTextEdit*             MainText;
    QLabel*                     DragDrop_Image;
    QLabel*                     DragDrop_Text;
    void                        clearVisualElements();
    void                        clearPoliciesElements();
    void                        createDragDrop();
    void                        createMainText();
    void                        removeTreeChildren(QTreeWidgetItem* item);
    void                        updatePoliciesTree();
    void                        updatePoliciesTreePolicy(Policy* p, QTreeWidgetItem *parent);
    void                        updatePoliciesTreePattern(Pattern *p, QTreeWidgetItem *parent);
    void                        updatePoliciesTreeRule(Rule *rule, QTreeWidgetItem *parent);
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
    void on_actionCloseAll_triggered();
    void on_actionConch_triggered();
    void on_actionInfo_triggered();
    void on_actionTrace_triggered();
    void on_actionSchematron_triggered();
    void on_actionPolicies_triggered();
    void on_actionText_triggered();
    void on_actionXml_triggered();
    void on_actionChooseSchematron_triggered();
    void on_actionSavePolicies_triggered();
    void on_importSchematron();
    void on_exportSchematron();
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

#endif // MAINWINDOW_H
