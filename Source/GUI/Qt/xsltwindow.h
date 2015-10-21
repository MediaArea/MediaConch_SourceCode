/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef XSLTWINDOW_H
#define XSLTWINDOW_H

#include "policywindow.h"

#include <QObject>
#include <QString>

class QTreeWidgetItem;

namespace MediaConch {

class MainWindow;
class PoliciesWindow;
class PoliciesMenu;
class XsltPolicyMenu;
class XsltRuleEdit;

class XsltWindow : public PolicyWindow
{
    Q_OBJECT

public:
    explicit XsltWindow(PoliciesWindow *window = 0, MainWindow *parent = 0);
    virtual ~XsltWindow();

    // Visual elements
    void display_selection(int level, QTreeWidgetItem *item);
    void displayPolicyMenu(QString title);

private:
    XsltPolicyMenu             *policyMenu;
    XsltRuleEdit               *ruleEdit;

    // Visual elements
    void                        clearPolicyElements();
    void                        displayRuleEdit(int, int);
    void                        createPolicyMenu();
    void                        createRuleEdit();

private Q_SLOTS:
    void save_policy();
    void save_policy_to();
    void duplicate_policy();
    void delete_policy();
    void edit_policy_title();
    void edit_policy_description();
    void add_new_rule();
    void duplicate_rule();
    void delete_rule();
    void rule_free_text_selected(bool checked);
    void rule_editor_selected(bool checked);

    void edit_rule_name(QString new_name);
    void edit_rule_type();
    void edit_rule_field();
    void edit_rule_operator();
    void edit_rule_value();
    void edit_rule_invalid();
    void edit_rule_occurrence();
    void edit_rule_freeText();
};

}

#endif // XSLTWINDOW_H
