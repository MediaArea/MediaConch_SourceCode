/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef SCHEMATRONWINDOW_H
#define SCHEMATRONWINDOW_H

#include "policywindow.h"

#include <QObject>
#include <QString>

class QTreeWidgetItem;

namespace MediaConch {

class MainWindow;
class PoliciesWindow;
class PoliciesMenu;
class PolicyMenu;
class GroupOfRules;
class RuleMenu;
class RuleEdit;

class SchematronWindow : public PolicyWindow
{
    Q_OBJECT

public:
    explicit SchematronWindow(PoliciesWindow *window = 0, MainWindow *parent = 0);
    virtual ~SchematronWindow();

    // Visual elements
    void display_selection(int level, QTreeWidgetItem *item);
    void displayPolicyMenu(QString title);

private:
    PolicyMenu     *policyMenu;
    GroupOfRules   *groupOfRules;
    RuleMenu       *ruleMenu;
    RuleEdit       *ruleEdit;

    // Visual elements
    void                        clearPolicyElements();
    void                        displayGroupOfRules(QString title);
    void                        displayRuleMenu();
    void                        displayRuleEdit(QTreeWidgetItem *);
    void                        createPolicyMenu();
    void                        createGroupOfRules();
    void                        createRuleMenu();
    void                        createRuleEdit();

private Q_SLOTS:
    void save_policy();
    void save_policy_to();
    void add_new_gor();
    void add_new_rule();
    void add_new_assert();
    void duplicate_policy();
    void duplicate_gor();
    void duplicate_rule();
    void duplicate_assert();
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
    void assert_free_text_selected(bool);
    void assert_editor_selected(bool);
};

}

#endif // SCHEMATRONWINDOW_H
