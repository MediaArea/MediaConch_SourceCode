/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef POLICIESWINDOW_H
#define POLICIESWINDOW_H

#include "Common/Core.h"
#include "Common/SchematronPolicy.h"

#include <QFileInfo>
#include <QString>

class QVBoxLayout;
class QProgressBar;
class QLabel;
class QTreeWidgetItem;
class QFile;
class QStringList;

namespace MediaConch {

class MainWindow;
class PoliciesTree;
class PoliciesMenu;
class PolicyMenu;
class GroupOfRules;
class RuleMenu;
class RuleEdit;

class PoliciesWindow : public QObject
{
    Q_OBJECT

public:
    explicit PoliciesWindow(MainWindow *parent = 0);
    ~PoliciesWindow();

    // Helpers
    void                        displayPoliciesTree();

private:
    MainWindow     *mainwindow;
    PoliciesTree   *policiesTree;
    PoliciesMenu   *policiesMenu;
    PolicyMenu     *policyMenu;
    GroupOfRules   *groupOfRules;
    RuleMenu       *ruleMenu;
    RuleEdit       *ruleEdit;

    // Visual elements
    void                        clearVisualElements();
    void                        clearPoliciesElements();
    void                        removeTreeChildren(QTreeWidgetItem* item);
    void                        updatePoliciesTree();
    void                        updatePoliciesTreePolicy(Policy* p, QTreeWidgetItem *parent);
    void                        updatePoliciesTreePattern(SchematronPattern *p, QTreeWidgetItem *parent);
    void                        updatePoliciesTreeRule(SchematronRule *rule, QTreeWidgetItem *parent);
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

//***************************************************************************
// HELPER
//***************************************************************************

    int              get_index_in_tree();
    QTreeWidgetItem* get_item_in_tree();
    int              get_index_of_item_backXX(QTreeWidgetItem* item, size_t back);

private Q_SLOTS:
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
