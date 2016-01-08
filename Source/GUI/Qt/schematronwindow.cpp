/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "policieswindow.h"
#include "schematronwindow.h"
#include "mainwindow.h"
#include "policymenu.h"
#include "groupofrules.h"
#include "rulemenu.h"
#include "ruleedit.h"

#include <QTreeWidget>
#include <QTextEdit>
#include <QLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMimeData>
#include <QLabel>
#include <QUrl>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>
#include <QRadioButton>
#include <QStatusBar>
#include <QMessageBox>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif
#if QT_VERSION >= 0x050200
    #include <QFontDatabase>
#endif

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

SchematronWindow::SchematronWindow(PoliciesWindow *window, MainWindow *parent) : PolicyWindow(window, parent)
{
    policyMenu = NULL;
    groupOfRules = NULL;
    ruleMenu = NULL;
    ruleEdit = NULL;
}

SchematronWindow::~SchematronWindow()
{
    clearPolicyElements();
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
void SchematronWindow::display_selection(int level, QTreeWidgetItem *item)
{
    switch (level)
    {
        case 1:
            displayPolicyMenu(item->text(0));
            break;
        case 2:
            displayGroupOfRules(item->text(0));
            break;
        case 3:
            displayRuleMenu();
            break;
        case 4:
            displayRuleEdit(item);
            break;
        default:
            return;
    }
}

//***************************************************************************
// Slots
//***************************************************************************

//---------------------------------------------------------------------------
void SchematronWindow::save_policy()
{
    policieswindow->save_policy();
}

//---------------------------------------------------------------------------
void SchematronWindow::save_policy_to()
{
    if (policieswindow->save_policy_to(Policies::POLICY_SCHEMATRON) < 0)
        return;
    policyMenu->get_savePolicy_button()->setEnabled(true);
}

//---------------------------------------------------------------------------
void SchematronWindow::delete_policy()
{
    clearPolicyElements();

    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int row = policieswindow->get_index_in_tree();
    if (row < 0)
        return;

    // Internal data
    Policy *p = mainwindow->get_policy(row);
    for (size_t i = 0; i < ((SchematronPolicy*)p)->patterns.size(); ++i)
        delete ((SchematronPolicy*)p)->patterns[i];
    ((SchematronPolicy*)p)->patterns.clear();
    mainwindow->remove_policy(row);

    // Visual
    policieswindow->policy_deleted(item, row);
}

//---------------------------------------------------------------------------
void SchematronWindow::duplicate_policy()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item)
        return;

    int row = policieswindow->get_index_in_tree();
    if (row < 0)
        return;

    Policy *p = new SchematronPolicy((SchematronPolicy*)mainwindow->get_policy(row));

    p->title = p->title + string(" (Copy)");
    policieswindow->new_policy_filename(p);

    mainwindow->add_policy(p);
    QTreeWidgetItem* parent = item->parent();
    if (!parent)
        return;

    QTreeWidgetItem* new_item = new QTreeWidgetItem(parent);
    QString title = QString().fromStdString(p->title);
    new_item->setText(0, title);
    item->setSelected(false);
    for (size_t i = 0; i < ((SchematronPolicy*)p)->patterns.size(); ++i)
        policieswindow->updatePoliciesTreeSchematronPattern(((SchematronPolicy*)p)->patterns[i], new_item);
    new_item->setSelected(true);
    policieswindow->emphasis_policy_name_in_tree(new_item);
}

//---------------------------------------------------------------------------
void SchematronWindow::add_new_gor()
{
    QTreeWidgetItem* parent = policieswindow->get_item_in_tree();
    if (!parent)
        return;
    int rowPolicy = policieswindow->get_index_of_item_backXX(parent, 0);
    if (rowPolicy < 0)
        return;

    SchematronPattern *p = new SchematronPattern;

    p->name = string("New group of rules");

    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    QString name = QString().fromStdString(p->name);
    item->setText(0, name);
    policieswindow->emphasis_policy_name_in_tree(item);

    ((SchematronPolicy*)mainwindow->get_policy(rowPolicy))->patterns.push_back(p);
    mainwindow->get_policy(rowPolicy)->saved = false;
    parent->setExpanded(true);
    parent->setSelected(false);
    item->setSelected(true);
}

//---------------------------------------------------------------------------
void SchematronWindow::duplicate_gor()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item)
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (rowPolicy < 0 || row < 0)
        return;

    Policy *p = mainwindow->get_policy(rowPolicy);
    SchematronPattern *pat = new SchematronPattern(*((SchematronPolicy*)p)->patterns[row]);

    pat->name = pat->name + string(" (Copy)");
    ((SchematronPolicy*)p)->patterns.push_back(pat);
    p->saved = false;

    QTreeWidgetItem* parent = item->parent();
    if (!parent)
        return;

    QTreeWidgetItem* new_item = new QTreeWidgetItem(parent);
    item->setSelected(false);
    QString title = QString().fromStdString(pat->name);
    new_item->setText(0, title);
    for (size_t i = 0; i < pat->rules.size(); ++i)
        policieswindow->updatePoliciesTreeSchematronRule(pat->rules[i], new_item);
    policieswindow->emphasis_policy_name_in_tree(new_item);
    new_item->setSelected(true);
}

//---------------------------------------------------------------------------
void SchematronWindow::add_new_rule()
{
    QTreeWidgetItem* parent = policieswindow->get_item_in_tree();
    if (!parent)
        return;
    int rowPolicy = policieswindow->get_index_of_item_backXX(parent, 1);
    int rowGor = policieswindow->get_index_of_item_backXX(parent, 0);
    if (rowPolicy < 0 || rowGor < 0)
        return;

    SchematronRule *r = new SchematronRule;

    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    item->setText(0, QString("Rule"));
    policieswindow->emphasis_policy_name_in_tree(item);

    ((SchematronPolicy*)mainwindow->get_policy(rowPolicy))->patterns[rowGor]->rules.push_back(r);
    mainwindow->get_policy(rowPolicy)->saved = false;
    parent->setExpanded(true);
    parent->setSelected(false);
    item->setSelected(true);
}

//---------------------------------------------------------------------------
void SchematronWindow::duplicate_rule()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item)
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 2);
    int rowPattern = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (rowPattern < 0 || rowPolicy < 0 || row < 0)
        return;

    SchematronPattern *p = ((SchematronPolicy*)mainwindow->get_policy(rowPolicy))->patterns[rowPattern];
    SchematronRule *r = new SchematronRule(*p->rules[row]);

    p->rules.push_back(r);

    QTreeWidgetItem* parent = item->parent();
    if (!parent)
        return;

    mainwindow->get_policy(rowPolicy)->saved = false;
    QTreeWidgetItem* new_item = new QTreeWidgetItem(parent);
    new_item->setText(0, QString("Rule"));
    item->setSelected(false);
    for (size_t i = 0; i < r->asserts.size(); ++i)
    {
        SchematronAssert *assert = r->asserts[i];
        if (!assert)
            continue;

        QTreeWidgetItem* a = new QTreeWidgetItem(new_item);
        QString descr = QString().fromStdString(assert->description);
        a->setText(0, descr);
    }
    policieswindow->emphasis_policy_name_in_tree(new_item);
    new_item->setSelected(true);
}

//---------------------------------------------------------------------------
void SchematronWindow::add_new_assert()
{
    QTreeWidgetItem* parent = policieswindow->get_item_in_tree();
    if (!parent)
        return;
    int rowPolicy = policieswindow->get_index_of_item_backXX(parent, 2);
    int rowGor = policieswindow->get_index_of_item_backXX(parent, 1);
    int rowRule = policieswindow->get_index_of_item_backXX(parent, 0);
    if (rowPolicy < 0 || rowGor < 0 || rowRule < 0)
        return;

    mainwindow->get_policy(rowPolicy)->saved = false;
    SchematronAssert *a = new SchematronAssert;

    a->description = string("New Assert");
    a->use_free_text = false;
    QString name = QString().fromStdString(a->description);
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, QStringList(name));
    policieswindow->emphasis_policy_name_in_tree(item);

    ((SchematronPolicy*)mainwindow->get_policy(rowPolicy))->patterns[rowGor]->rules[rowRule]->asserts.push_back(a);
    parent->setExpanded(true);
    parent->setSelected(false);
    item->setSelected(true);
}

//---------------------------------------------------------------------------
void SchematronWindow::duplicate_assert()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item)
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 3);
    int rowPattern = policieswindow->get_index_of_item_backXX(item, 2);
    int rowRule = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (rowPattern < 0 || rowPolicy < 0 || row < 0)
        return;

    SchematronPolicy* p = (SchematronPolicy*)mainwindow->get_policy(rowPolicy);
    p->saved = false;
    SchematronRule *r = p->patterns[rowPattern]->rules[rowRule];
    SchematronAssert *a = new SchematronAssert(*r->asserts[row]);
    a->description = a->description + string(" (Copy)");
    r->asserts.push_back(a);

    QTreeWidgetItem* parent = item->parent();
    if (!parent)
        return;

    QTreeWidgetItem* new_item = new QTreeWidgetItem(parent);
    policieswindow->emphasis_policy_name_in_tree(new_item);
    new_item->setText(0, QString().fromStdString(a->description));
    item->setSelected(false);
    new_item->setSelected(true);
}

//---------------------------------------------------------------------------
void SchematronWindow::delete_gor()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int row = policieswindow->get_index_in_tree();
    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 1);
    if (rowPolicy < 0 || row < 0)
        return;

    policieswindow->disconnectPoliciesTreeSelectionChanged();

    // Internal data
    Policy *p = mainwindow->get_policy(rowPolicy);
    p->saved = false;
    SchematronPattern *pat = ((SchematronPolicy*)p)->patterns[row];
    for (size_t i = 0; i < pat->rules.size(); ++i)
        delete pat->rules[i];
    pat->rules.clear();
    ((SchematronPolicy*)p)->patterns.erase(((SchematronPolicy*)p)->patterns.begin() + row);

    // Visual
    policieswindow->emphasis_policy_name_in_tree(item);
    policieswindow->removeTreeChildren(item);
    QTreeWidgetItem* parent = item->parent();
    parent->takeChild(row);
    for (int i = 0; i < parent->childCount(); ++i)
    {
        item = parent->child(i);
        if (item && item->isSelected())
            item->setSelected(false);
    }
    policieswindow->connectPoliciesTreeSelectionChanged();
    parent->setSelected(false);
    parent->setSelected(true);
}

//---------------------------------------------------------------------------
void SchematronWindow::delete_rule()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 2);
    int rowGor = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (rowPolicy < 0 || rowGor < 0 || row < 0)
        return;

    policieswindow->disconnectPoliciesTreeSelectionChanged();

    // Internal data
    SchematronPolicy* po = (SchematronPolicy*)mainwindow->get_policy(rowPolicy);
    po->saved = false;
    SchematronPattern *p = po->patterns[rowGor];
    SchematronRule *r = p->rules[row];
    for (size_t i = 0; i < r->asserts.size(); ++i)
        delete r->asserts[i];
    r->asserts.clear();
    p->rules.erase(p->rules.begin() + row);

    // Visual
    policieswindow->emphasis_policy_name_in_tree(item);
    policieswindow->removeTreeChildren(item);
    QTreeWidgetItem* parent = item->parent();
    parent->takeChild(row);
    for (int i = 0; i < parent->childCount(); ++i)
    {
        item = parent->child(i);
        if (item && item->isSelected())
            item->setSelected(false);
    }

    policieswindow->connectPoliciesTreeSelectionChanged();
    parent->setSelected(false);
    parent->setSelected(true);
}

//---------------------------------------------------------------------------
void SchematronWindow::delete_assert()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 3);
    int rowGor = policieswindow->get_index_of_item_backXX(item, 2);
    int rowRule = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (rowPolicy < 0 || rowGor < 0 || rowRule < 0 || row < 0)
        return;

    policieswindow->disconnectPoliciesTreeSelectionChanged();

    // Internal data
    SchematronPolicy* po = (SchematronPolicy*)mainwindow->get_policy(rowPolicy);
    po->saved = false;
    SchematronRule *r = po->patterns[rowGor]->rules[rowRule];
    SchematronAssert *a = r->asserts[row];
    r->asserts.erase(r->asserts.begin() + row);
    delete a;

    // Visual
    QTreeWidgetItem* parent = item->parent();
    policieswindow->emphasis_policy_name_in_tree(item);
    parent->takeChild(row);
    for (int i = 0; i < parent->childCount(); ++i)
    {
        item = parent->child(i);
        if (item && item->isSelected())
            item->setSelected(false);
    }
    policieswindow->connectPoliciesTreeSelectionChanged();
    parent->setSelected(false);
    parent->setSelected(true);
}

//---------------------------------------------------------------------------
void SchematronWindow::edit_policy_title()
{
    QString qtitle = policyMenu->get_title_line()->text();
    if (!qtitle.length())
    {
        policieswindow->get_error_bar()->showMessage(QString("Policy must have a title"));
        return;
    }
    policieswindow->get_error_bar()->clearMessage();

    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item)
        return;

    int row = policieswindow->get_index_in_tree();;
    if (row < 0)
        return;

    Policy *p = mainwindow->get_policy(row);
    if (p->title != qtitle.toStdString())
    {
        p->saved = false;
        policieswindow->emphasis_policy_name_in_tree(item);
        p->title = qtitle.toStdString();
        QString title = QString().fromStdString(p->title);
        item->setText(0, title);
    }
}

//---------------------------------------------------------------------------
void SchematronWindow::edit_gor_title()
{
    QString qname = groupOfRules->get_title_line()->text();
    if (!qname.length())
    {
        policieswindow->get_error_bar()->showMessage(QString("Group of rules must have a title"));
        return;
    }
    policieswindow->get_error_bar()->clearMessage();

    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int row = policieswindow->get_index_in_tree();
    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 1);
    if (row < 0 || rowPolicy < 0)
        return;

    SchematronPolicy* po = (SchematronPolicy*)mainwindow->get_policy(rowPolicy);
    SchematronPattern *p = po->patterns[row];
    if (p->name != qname.toStdString())
    {
        policieswindow->emphasis_policy_name_in_tree(item);
        po->saved = false;
        p->name = qname.toStdString();
        QString title = QString().fromStdString(p->name);
        item->setText(0, title);
    }
}

//---------------------------------------------------------------------------
void SchematronWindow::edit_assert_name(QString new_name)
{
    if (!new_name.length())
    {
        policieswindow->get_error_bar()->showMessage(QString("Assert must have a name"));
        return;
    }
    policieswindow->get_error_bar()->clearMessage();

    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 3);
    int rowPattern = policieswindow->get_index_of_item_backXX(item, 2);
    int rowRule = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (row < 0 || rowPolicy < 0 || rowPattern < 0 || rowRule < 0)
        return;

    SchematronPolicy* po = (SchematronPolicy*)mainwindow->get_policy(rowPolicy);
    SchematronAssert *a = po->patterns[rowPattern]->rules[rowRule]->asserts[row];

    if (a->description != new_name.toStdString())
    {
        policieswindow->emphasis_policy_name_in_tree(item);
        po->saved = false;
        a->description = new_name.toStdString();
        QString name = QString().fromStdString(a->description);
        item->setText(0, name);
    }
}

//---------------------------------------------------------------------------
void SchematronWindow::edit_assert_type()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 3);
    int rowPattern = policieswindow->get_index_of_item_backXX(item, 2);
    int rowRule = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (row < 0 || rowPolicy < 0 || rowPattern < 0 || rowRule < 0)
        return;

    SchematronPolicy* po = (SchematronPolicy*)mainwindow->get_policy(rowPolicy);
    SchematronAssert *a = po->patterns[rowPattern]->rules[rowRule]->asserts[row];

    if (a->type != ruleEdit->get_type_select()->currentText().toStdString())
    {
        policieswindow->emphasis_policy_name_in_tree(item);
        po->saved = false;
        a->type = ruleEdit->get_type_select()->currentText().toStdString();
        a->use_free_text = false;
        ruleEdit->change_values_of_field_selector();
    }
}

//---------------------------------------------------------------------------
void SchematronWindow::edit_assert_field()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 3);
    int rowPattern = policieswindow->get_index_of_item_backXX(item, 2);
    int rowRule = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (row < 0 || rowPolicy < 0 || rowPattern < 0 || rowRule < 0)
        return;

    SchematronPolicy* po = (SchematronPolicy*)mainwindow->get_policy(rowPolicy);
    SchematronAssert *a = po->patterns[rowPattern]->rules[rowRule]->asserts[row];

    if (a->field != ruleEdit->get_field_select()->currentText().toStdString())
    {
        policieswindow->emphasis_policy_name_in_tree(item);
        po->saved = false;
        a->field = ruleEdit->get_field_select()->currentText().toStdString();
        a->use_free_text = false;
    }
}

//---------------------------------------------------------------------------
void SchematronWindow::edit_assert_validator()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 3);
    int rowPattern = policieswindow->get_index_of_item_backXX(item, 2);
    int rowRule = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (row < 0 || rowPolicy < 0 || rowPattern < 0 || rowRule < 0)
        return;

    SchematronPolicy* po = (SchematronPolicy*)mainwindow->get_policy(rowPolicy);
    SchematronAssert *a = po->patterns[rowPattern]->rules[rowRule]->asserts[row];

    if (a->validator != ruleEdit->get_validator_value_from_pretty_name(ruleEdit->get_validator_select()->currentText().toStdString()))
    {
        policieswindow->emphasis_policy_name_in_tree(item);
        po->saved = false;
        a->validator = ruleEdit->get_validator_value_from_pretty_name(ruleEdit->get_validator_select()->currentText().toStdString());
        a->use_free_text = false;
    }
}

//---------------------------------------------------------------------------
void SchematronWindow::edit_assert_value()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 3);
    int rowPattern = policieswindow->get_index_of_item_backXX(item, 2);
    int rowRule = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (row < 0 || rowPolicy < 0 || rowPattern < 0 || rowRule < 0)
        return;

    SchematronPolicy* po = (SchematronPolicy*)mainwindow->get_policy(rowPolicy);
    SchematronAssert *a = po->patterns[rowPattern]->rules[rowRule]->asserts[row];

    if (a->value != ruleEdit->get_value_line()->text().toStdString())
    {
        policieswindow->emphasis_policy_name_in_tree(item);
        po->saved = false;
        a->value = ruleEdit->get_value_line()->text().toStdString();
        ruleEdit->value_to_quotted_value(a->value);
        a->use_free_text = false;
    }
}

//---------------------------------------------------------------------------
void SchematronWindow::edit_assert_freeText()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 3);
    int rowPattern = policieswindow->get_index_of_item_backXX(item, 2);
    int rowRule = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (row < 0 || rowPolicy < 0 || rowPattern < 0 || rowRule < 0)
        return;

    SchematronPolicy* po = (SchematronPolicy*)mainwindow->get_policy(rowPolicy);
    SchematronAssert *a = po->patterns[rowPattern]->rules[rowRule]->asserts[row];

    if (a->text != ruleEdit->get_freeText_text()->toPlainText().toStdString())
    {
        policieswindow->emphasis_policy_name_in_tree(item);
        po->saved = false;
        a->text = ruleEdit->get_freeText_text()->toPlainText().toStdString();
        a->use_free_text = true;

        SchematronAssert test;
        if (Policies::try_parsing_test(a->text, &test))
            ruleEdit->get_editorSelector_radio()->setEnabled(true);
        else
            ruleEdit->get_editorSelector_radio()->setEnabled(false);
    }
}

//---------------------------------------------------------------------------
void SchematronWindow::assert_free_text_selected(bool checked)
{
    if (!checked)
        return;

    SchematronAssert a;

    a.type = ruleEdit->get_type_select()->currentText().toStdString();
    a.field = ruleEdit->get_field_select()->currentText().toStdString();
    a.validator = ruleEdit->get_validator_value_from_pretty_name(ruleEdit->get_validator_select()->currentText().toStdString());
    string value = ruleEdit->get_value_line()->text().toStdString();
    ruleEdit->value_to_quotted_value(value);
    a.value = value;
    a.use_free_text = false;

    string assertStr = Policies::serialize_assert_for_test(&a);
    ruleEdit->get_freeText_text()->setText(QString().fromStdString(assertStr));
    ruleEdit->get_freeText_text()->show();
    ruleEdit->get_editor_frame()->hide();

    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    policieswindow->emphasis_policy_name_in_tree(item);
}

//---------------------------------------------------------------------------
void SchematronWindow::assert_editor_selected(bool checked)
{
    if (!checked)
        return;

    SchematronAssert a;
    if (Policies::try_parsing_test(ruleEdit->get_freeText_text()->toPlainText().toStdString(), &a))
        ruleEdit->fill_editor_fields(&a);
    else
    {
        ruleEdit->get_type_select()->setCurrentIndex(0);
        ruleEdit->get_field_select()->setCurrentIndex(0);
        ruleEdit->get_validator_select()->setCurrentIndex(0);
        ruleEdit->get_value_line()->setText(QString());
    }
    ruleEdit->get_editor_frame()->show();
    ruleEdit->get_freeText_text()->hide();

    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    policieswindow->emphasis_policy_name_in_tree(item);
}

//***************************************************************************
// Visual elements
//***************************************************************************

//---------------------------------------------------------------------------
void SchematronWindow::clearPolicyElements()
{
    if (policyMenu)
    {
        policieswindow->remove_widget_from_tree_layout(policyMenu);
        delete policyMenu;
        policyMenu=NULL;
    }

    if (groupOfRules)
    {
        policieswindow->remove_widget_from_tree_layout(groupOfRules);
        delete groupOfRules;
        groupOfRules = NULL;
    }

    if (ruleMenu)
    {
        policieswindow->remove_widget_from_tree_layout(ruleMenu);
        delete ruleMenu;
        ruleMenu = NULL;
    }

    if (ruleEdit)
    {
        policieswindow->remove_widget_from_tree_layout(ruleEdit);
        delete ruleEdit;
        ruleEdit=NULL;
    }
}

//---------------------------------------------------------------------------
void SchematronWindow::createPolicyMenu()
{
    if (policyMenu)
        return;
    clearPolicyElements();
    policyMenu = new PolicyMenu(policieswindow->policyFrame());
    policieswindow->set_widget_to_tree_layout(policyMenu);
    QObject::connect(policyMenu->get_savePolicy_button(), SIGNAL(clicked()),
                     this, SLOT(save_policy()));
    QObject::connect(policyMenu->get_savePolicyTo_button(), SIGNAL(clicked()),
                     this, SLOT(save_policy_to()));
    QObject::connect(policyMenu->get_addNewGor_button(), SIGNAL(clicked()),
                     this, SLOT(add_new_gor()));
    QObject::connect(policyMenu->get_deletePolicy_button(), SIGNAL(clicked()),
                     this, SLOT(delete_policy()));
    QObject::connect(policyMenu->get_duplicatePolicy_button(), SIGNAL(clicked()),
                     this, SLOT(duplicate_policy()));
    QObject::connect(policyMenu->get_title_line(), SIGNAL(textChanged(QString)),
                     this, SLOT(edit_policy_title()));
}

//---------------------------------------------------------------------------
void SchematronWindow::displayPolicyMenu(QString title)
{
    createPolicyMenu();
    QLineEdit* name = policyMenu->get_title_line();

    name->setText(title);

    policyMenu->get_savePolicy_button()->setEnabled(false);
    int index = policieswindow->get_index_in_tree();
    Policy* p = mainwindow->get_policy(index);
    if (p && p->filename.length())
        policyMenu->get_savePolicy_button()->setEnabled(true);
}

//---------------------------------------------------------------------------
void SchematronWindow::createGroupOfRules()
{
    if (groupOfRules)
        return;
    clearPolicyElements();
    groupOfRules = new GroupOfRules(policieswindow->policyFrame());
    policieswindow->set_widget_to_tree_layout(groupOfRules);
    QObject::connect(groupOfRules->get_addNewRule_button(), SIGNAL(clicked()),
                     this, SLOT(add_new_rule()));
    QObject::connect(groupOfRules->get_title_line(), SIGNAL(textChanged(QString)),
                     this, SLOT(edit_gor_title()));
    QObject::connect(groupOfRules->get_deleteGor_button(), SIGNAL(clicked()),
                     this, SLOT(delete_gor()));
    QObject::connect(groupOfRules->get_duplicateGor_button(), SIGNAL(clicked()),
                     this, SLOT(duplicate_gor()));
}

//---------------------------------------------------------------------------
void SchematronWindow::displayGroupOfRules(QString title)
{
    createGroupOfRules();
    QLineEdit* name = groupOfRules->get_title_line();

    name->setText(title);
}

//---------------------------------------------------------------------------
void SchematronWindow::createRuleMenu()
{
    if (ruleMenu)
        return;
    clearPolicyElements();
    ruleMenu = new RuleMenu(policieswindow->policyFrame());
    policieswindow->set_widget_to_tree_layout(ruleMenu);
    QObject::connect(ruleMenu->get_addNewAssert_button(), SIGNAL(clicked()),
                     this, SLOT(add_new_assert()));
    QObject::connect(ruleMenu->get_deleteRule_button(), SIGNAL(clicked()),
                     this, SLOT(delete_rule()));
    QObject::connect(ruleMenu->get_duplicateRule_button(), SIGNAL(clicked()),
                     this, SLOT(duplicate_rule()));
}

//---------------------------------------------------------------------------
void SchematronWindow::displayRuleMenu()
{
    createRuleMenu();
}

//---------------------------------------------------------------------------
void SchematronWindow::createRuleEdit()
{
    clearPolicyElements();
    ruleEdit = new RuleEdit(mainwindow);
    policieswindow->set_widget_to_tree_layout(ruleEdit);
}

//---------------------------------------------------------------------------
void SchematronWindow::displayRuleEdit(QTreeWidgetItem *item)
{
    if (!item)
        return;
    createRuleEdit();

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 3);
    int rowPattern = policieswindow->get_index_of_item_backXX(item, 2);
    int rowRule = policieswindow->get_index_of_item_backXX(item, 1);
    int rowAssert = policieswindow->get_index_of_item_backXX(item, 0);

    if (rowPolicy < 0 || rowPattern < 0 || rowRule < 0 || rowAssert < 0)
        return;
    SchematronAssert *a = ((SchematronPolicy*)mainwindow->get_policy(rowPolicy))->patterns[rowPattern]->rules[rowRule]->asserts[rowAssert];
    ruleEdit->assert_clicked(a);

    QObject::connect(ruleEdit->get_assertName_line(), SIGNAL(textEdited(QString)),
                     this, SLOT(edit_assert_name(QString)));
    QObject::connect(ruleEdit->get_delAssert_button(), SIGNAL(clicked()),
                     this, SLOT(delete_assert()));
    QObject::connect(ruleEdit->get_duplicateAssert_button(), SIGNAL(clicked()),
                     this, SLOT(duplicate_assert()));
    QObject::connect(ruleEdit->get_type_select(), SIGNAL(currentIndexChanged(int)),
                     this, SLOT(edit_assert_type()));
    QObject::connect(ruleEdit->get_field_select(), SIGNAL(currentIndexChanged(int)),
                     this, SLOT(edit_assert_field()));
    QObject::connect(ruleEdit->get_validator_select(), SIGNAL(currentIndexChanged(int)),
                     this, SLOT(edit_assert_validator()));
    QObject::connect(ruleEdit->get_value_line(), SIGNAL(textEdited(QString)),
                     this, SLOT(edit_assert_value()));
    QObject::connect(ruleEdit->get_freeText_text(), SIGNAL(textChanged()),
                     this, SLOT(edit_assert_freeText()));
    QObject::connect(ruleEdit->get_freeTextSelector_radio(), SIGNAL(toggled(bool)),
                     this, SLOT(assert_free_text_selected(bool)));
    QObject::connect(ruleEdit->get_editorSelector_radio(), SIGNAL(toggled(bool)),
                     this, SLOT(assert_editor_selected(bool)));

    SchematronAssert test;
    if (!a->use_free_text || Policies::try_parsing_test(a->text, &test))
        ruleEdit->get_editorSelector_radio()->setEnabled(true);
    else
        ruleEdit->get_editorSelector_radio()->setEnabled(false);
}

}
