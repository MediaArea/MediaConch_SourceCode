/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "policieswindow.h"
#include "mainwindow.h"
#include "policiestree.h"
#include "policiesmenu.h"
#include "policymenu.h"
#include "groupofrules.h"
#include "rulemenu.h"
#include "ruleedit.h"
#include "WebPage.h"
#include "WebView.h"

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
#include <sstream>

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

PoliciesWindow::PoliciesWindow(MainWindow *parent) : mainwindow(parent)
{
    policiesTree = NULL;
    policiesMenu = NULL;
    policyMenu = NULL;
    groupOfRules = NULL;
    ruleMenu = NULL;
    ruleEdit = NULL;
}

PoliciesWindow::~PoliciesWindow()
{
    if (policiesTree)
    {
        clearPoliciesElements();
        mainwindow->remove_widget_from_layout(policiesTree);
        delete policiesTree;
        policiesTree=NULL;
    }
}

//***************************************************************************
// Slots
//***************************************************************************

//---------------------------------------------------------------------------
void PoliciesWindow::import_schematron()
{
    QString file = mainwindow->ask_for_schematron_file();
    if (!file.length())
        return;

    String ret = mainwindow->get_policies().import_schema(Policies::POLICY_SCHEMATRON, file.toStdString().c_str());

    displayPoliciesTree();
    if (ret.length())
            policiesTree->get_error_bar()->showMessage(QString().fromStdWString(ret));
    else
    {
        policiesTree->get_error_bar()->clearMessage();
        int row = mainwindow->get_policies().policies.size() - 1;
        QTreeWidgetItem* parent = policiesTree->get_policies_tree()->topLevelItem(0);
        if (row < 0 || !parent)
            return;

        QTreeWidgetItem *item = parent->child(row);
        if (!item)
            return;

        parent->setExpanded(true);
        parent->setSelected(false);
        item->setSelected(true);
    }
}

//---------------------------------------------------------------------------
void PoliciesWindow::save_policy_to()
{
    int  row = get_index_in_tree();

    if (row < 0)
        return;

    mainwindow->exporting_to_schematron_file(row);
    policyMenu->get_savePolicy_button()->setEnabled(true);
}

//---------------------------------------------------------------------------
void PoliciesWindow::save_policy()
{
    int  row = get_index_in_tree();

    if (row < 0)
        return;
    mainwindow->exporting_to_schematron(row);
}

//---------------------------------------------------------------------------
void PoliciesWindow::add_new_policy()
{
    QTreeWidget *tree = policiesTree->get_policies_tree();
    QTreeWidgetItem* parent = tree->topLevelItem(0);
    if (!parent)
        return;

    Policy *p = new SchematronPolicy;

    p->title = string("New policy");
    if (mainwindow->get_policies().policy_exists(p->title))
    {
        int i = 1;
        while (1)
        {
            std::stringstream ss;
            ss << p->title << " " << i;
            if (!mainwindow->get_policies().policy_exists(ss.str()))
            {
                p->title = ss.str();
                break;
            }
            ++i;
        }
    }

    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    QString title = QString().fromStdString(p->title);
    item->setText(0, title);

    mainwindow->get_policies().policies.push_back(p);
    displayPolicyMenu(title);
    parent->setExpanded(true);
    parent->setSelected(false);
    item->setSelected(true);
}

//---------------------------------------------------------------------------
void PoliciesWindow::duplicate_policy()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item)
        return;

    int row = get_index_in_tree();
    if (row < 0)
        return;

    Policy *p = new SchematronPolicy((SchematronPolicy*)mainwindow->get_policies().policies[row]);

    p->title = p->title + string(" (Copy)");

    mainwindow->get_policies().policies.push_back(p);
    QTreeWidgetItem* parent = item->parent();
    if (!parent)
        return;

    QTreeWidgetItem* new_item = new QTreeWidgetItem(parent);
    item->setSelected(false);
    new_item->setSelected(true);
    QString title = QString().fromStdString(p->title);
    new_item->setText(0, title);

    for (size_t i = 0; i < ((SchematronPolicy*)p)->patterns.size(); ++i)
        updatePoliciesTreePattern(((SchematronPolicy*)p)->patterns[i], new_item);
    displayPolicyMenu(title);
}

//---------------------------------------------------------------------------
void PoliciesWindow::add_new_gor()
{
    QTreeWidgetItem* parent = get_item_in_tree();
    if (!parent)
        return;
    int rowPolicy = get_index_of_item_backXX(parent, 0);
    if (rowPolicy < 0)
        return;

    SchematronPattern *p = new SchematronPattern;

    p->name = string("New group of rules");

    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    QString name = QString().fromStdString(p->name);
    item->setText(0, name);

    ((SchematronPolicy*)mainwindow->get_policies().policies[rowPolicy])->patterns.push_back(p);
    displayGroupOfRules(name);
    parent->setExpanded(true);
    parent->setSelected(false);
    item->setSelected(true);
}

//---------------------------------------------------------------------------
void PoliciesWindow::duplicate_gor()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item)
        return;

    int rowPolicy = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (rowPolicy < 0 || row < 0)
        return;

    Policy *p = mainwindow->get_policies().policies[rowPolicy];
    SchematronPattern *pat = new SchematronPattern(*((SchematronPolicy*)p)->patterns[row]);

    pat->name = pat->name + string(" (Copy)");
    ((SchematronPolicy*)p)->patterns.push_back(pat);

    QTreeWidgetItem* parent = item->parent();
    if (!parent)
        return;

    QTreeWidgetItem* new_item = new QTreeWidgetItem(parent);
    item->setSelected(false);
    new_item->setSelected(true);
    QString title = QString().fromStdString(pat->name);
    new_item->setText(0, title);

    for (size_t i = 0; i < pat->rules.size(); ++i)
        updatePoliciesTreeRule(pat->rules[i], new_item);
    displayGroupOfRules(title);
}

//---------------------------------------------------------------------------
void PoliciesWindow::add_new_rule()
{
    QTreeWidgetItem* parent = get_item_in_tree();
    if (!parent)
        return;
    int rowPolicy = get_index_of_item_backXX(parent, 1);
    int rowGor = get_index_of_item_backXX(parent, 0);
    if (rowPolicy < 0 || rowGor < 0)
        return;

    SchematronRule *r = new SchematronRule;

    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    item->setText(0, QString("Rule"));

    ((SchematronPolicy*)mainwindow->get_policies().policies[rowPolicy])->patterns[rowGor]->rules.push_back(r);
    displayRuleMenu();
    parent->setExpanded(true);
    parent->setSelected(false);
    item->setSelected(true);
}

//---------------------------------------------------------------------------
void PoliciesWindow::duplicate_rule()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item)
        return;

    int rowPolicy = get_index_of_item_backXX(item, 2);
    int rowPattern = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (rowPattern < 0 || rowPolicy < 0 || row < 0)
        return;

    SchematronPattern *p = ((SchematronPolicy*)mainwindow->get_policies().policies[rowPolicy])->patterns[rowPattern];
    SchematronRule *r = new SchematronRule(*p->rules[row]);

    p->rules.push_back(r);

    QTreeWidgetItem* parent = item->parent();
    if (!parent)
        return;

    QTreeWidgetItem* new_item = new QTreeWidgetItem(parent);
    new_item->setText(0, QString("Rule"));
    item->setSelected(false);
    new_item->setSelected(true);

    for (size_t i = 0; i < r->asserts.size(); ++i)
    {
        SchematronAssert *assert = r->asserts[i];
        if (!assert)
            continue;

        QTreeWidgetItem* a = new QTreeWidgetItem(new_item);
        QString descr = QString().fromStdString(assert->description);
        a->setText(0, descr);
    }
    displayRuleMenu();
}

//---------------------------------------------------------------------------
void PoliciesWindow::add_new_assert()
{
    QTreeWidgetItem* parent = get_item_in_tree();
    if (!parent)
        return;
    int rowPolicy = get_index_of_item_backXX(parent, 2);
    int rowGor = get_index_of_item_backXX(parent, 1);
    int rowRule = get_index_of_item_backXX(parent, 0);
    if (rowPolicy < 0 || rowGor < 0 || rowRule < 0)
        return;

    SchematronAssert *a = new SchematronAssert;

    a->description = string("New Assert");
    a->use_free_text = false;
    QString name = QString().fromStdString(a->description);
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, QStringList(name));

    ((SchematronPolicy*)mainwindow->get_policies().policies[rowPolicy])->patterns[rowGor]->rules[rowRule]->asserts.push_back(a);
    parent->setExpanded(true);
    parent->setSelected(false);
    item->setSelected(true);
    displayRuleEdit(item);
}

//---------------------------------------------------------------------------
void PoliciesWindow::duplicate_assert()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item)
        return;

    int rowPolicy = get_index_of_item_backXX(item, 3);
    int rowPattern = get_index_of_item_backXX(item, 2);
    int rowRule = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (rowPattern < 0 || rowPolicy < 0 || row < 0)
        return;

    SchematronRule *r = ((SchematronPolicy*)mainwindow->get_policies().policies[rowPolicy])->patterns[rowPattern]->rules[rowRule];
    SchematronAssert *a = new SchematronAssert(*r->asserts[row]);
    a->description = a->description + string(" (Copy)");
    r->asserts.push_back(a);

    QTreeWidgetItem* parent = item->parent();
    if (!parent)
        return;

    QTreeWidgetItem* new_item = new QTreeWidgetItem(parent);
    new_item->setText(0, QString().fromStdString(a->description));
    item->setSelected(false);
    new_item->setSelected(true);

    displayRuleEdit(new_item);
}

//---------------------------------------------------------------------------
void PoliciesWindow::delete_all_policies()
{
    int ret = QMessageBox::warning(mainwindow, tr("Delete all policies"),
                                   tr("Do you want to delete all policies?"),
                                   QMessageBox::Yes | QMessageBox::No);
    switch (ret)
    {
      case QMessageBox::Yes:
          break;
      case QMessageBox::No:
      default:
          return;
    }

    QTreeWidget *tree = policiesTree->get_policies_tree();
    QTreeWidgetItem* policies = tree->topLevelItem(0);
    if (!policies)
        return;
    removeTreeChildren(policies);
    for (size_t i = 0; i < mainwindow->get_policies().policies.size(); ++i)
        delete mainwindow->get_policies().policies[i];
    mainwindow->get_policies().policies.clear();
    policiesMenu->get_deletePolicies_button()->setEnabled(false);
}

//---------------------------------------------------------------------------
void PoliciesWindow::delete_policy()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int row = get_index_in_tree();
    if (row < 0)
        return;

    // Internal data
    Policy *p = mainwindow->get_policies().policies[row];
    for (size_t i = 0; i < ((SchematronPolicy*)p)->patterns.size(); ++i)
        delete ((SchematronPolicy*)p)->patterns[i];
    ((SchematronPolicy*)p)->patterns.clear();
    mainwindow->get_policies().policies.erase(mainwindow->get_policies().policies.begin() + row);

    // Visual
    removeTreeChildren(item);
    QTreeWidgetItem* parent = item->parent();
    delete parent->takeChild(row);
    item = get_item_in_tree();
    if (item)
        item->setSelected(false);
    parent->setSelected(true);
    displayPoliciesMenu();
}

//---------------------------------------------------------------------------
void PoliciesWindow::delete_gor()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int row = get_index_in_tree();
    int rowPolicy = get_index_of_item_backXX(item, 1);
    if (rowPolicy < 0 || row < 0)
        return;

    // Internal data
    Policy *p = mainwindow->get_policies().policies[rowPolicy];
    SchematronPattern *pat = ((SchematronPolicy*)p)->patterns[row];
    for (size_t i = 0; i < pat->rules.size(); ++i)
        delete pat->rules[i];
    pat->rules.clear();
    ((SchematronPolicy*)p)->patterns.erase(((SchematronPolicy*)p)->patterns.begin() + row);

    // Visual
    removeTreeChildren(item);
    QTreeWidgetItem* parent = item->parent();
    delete parent->takeChild(row);
    item = get_item_in_tree();
    if (item)
        item->setSelected(false);
    parent->setSelected(true);
    displayPolicyMenu(parent->text(0));
}

//---------------------------------------------------------------------------
void PoliciesWindow::delete_rule()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = get_index_of_item_backXX(item, 2);
    int rowGor = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (rowPolicy < 0 || rowGor < 0 || row < 0)
        return;

    // Internal data
    SchematronPattern *p = ((SchematronPolicy*)mainwindow->get_policies().policies[rowPolicy])->patterns[rowGor];
    SchematronRule *r = p->rules[row];
    for (size_t i = 0; i < r->asserts.size(); ++i)
        delete r->asserts[i];
    r->asserts.clear();
    p->rules.erase(p->rules.begin() + row);

    // Visual
    removeTreeChildren(item);
    QTreeWidgetItem* parent = item->parent();
    delete parent->takeChild(row);
    item = get_item_in_tree();
    if (item)
        item->setSelected(false);
    parent->setSelected(true);
    displayGroupOfRules(parent->text(0));
}

//---------------------------------------------------------------------------
void PoliciesWindow::delete_assert()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = get_index_of_item_backXX(item, 3);
    int rowGor = get_index_of_item_backXX(item, 2);
    int rowRule = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (rowPolicy < 0 || rowGor < 0 || rowRule < 0 || row < 0)
        return;

    // Internal data
    SchematronRule *r = ((SchematronPolicy*)mainwindow->get_policies().policies[rowPolicy])->patterns[rowGor]->rules[rowRule];
    SchematronAssert *a = r->asserts[row];
    r->asserts.erase(r->asserts.begin() + row);
    delete a;

    // Visual
    QTreeWidgetItem* parent = item->parent();
    delete parent->takeChild(row);
    item = get_item_in_tree();
    if (item)
        item->setSelected(false);
    parent->setSelected(true);
    displayRuleMenu();
}

//---------------------------------------------------------------------------
void PoliciesWindow::edit_policy_title()
{
    QString qtitle = policyMenu->get_title_line()->text();
    if (!qtitle.length())
    {
        policiesTree->get_error_bar()->showMessage(QString("Policy must have a title"));
        return;
    }
    policiesTree->get_error_bar()->clearMessage();

    QTreeWidgetItem* item = get_item_in_tree();
    if (!item)
        return;

    int row = get_index_in_tree();;
    if (row < 0)
        return;

    Policy *p = mainwindow->get_policies().policies[row];
    p->title = qtitle.toStdString();

    QString title = QString().fromStdString(p->title);
    item->setText(0, title);
}

//---------------------------------------------------------------------------
void PoliciesWindow::edit_gor_title()
{
    QString qname = groupOfRules->get_title_line()->text();
    if (!qname.length())
    {
        policiesTree->get_error_bar()->showMessage(QString("Group of rules must have a title"));
        return;
    }
    policiesTree->get_error_bar()->clearMessage();

    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int row = get_index_in_tree();
    int rowPolicy = get_index_of_item_backXX(item, 1);
    if (row < 0 || rowPolicy < 0)
        return;

    SchematronPattern *p = ((SchematronPolicy*)mainwindow->get_policies().policies[rowPolicy])->patterns[row];
    p->name = qname.toStdString();

    QString title = QString().fromStdString(p->name);
    item->setText(0, title);
}

//---------------------------------------------------------------------------
void PoliciesWindow::edit_assert_name(QString new_name)
{
    if (!new_name.length())
    {
        policiesTree->get_error_bar()->showMessage(QString("Assert must have a name"));
        return;
    }
    policiesTree->get_error_bar()->clearMessage();

    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = get_index_of_item_backXX(item, 3);
    int rowPattern = get_index_of_item_backXX(item, 2);
    int rowRule = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (row < 0 || rowPolicy < 0 || rowPattern < 0 || rowRule < 0)
        return;

    SchematronAssert *a = ((SchematronPolicy*)mainwindow->get_policies().policies[rowPolicy])->patterns[rowPattern]->rules[rowRule]->asserts[row];
    a->description = new_name.toStdString();

    QString name = QString().fromStdString(a->description);
    item->setText(0, name);
}

//---------------------------------------------------------------------------
void PoliciesWindow::edit_assert_type()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = get_index_of_item_backXX(item, 3);
    int rowPattern = get_index_of_item_backXX(item, 2);
    int rowRule = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (row < 0 || rowPolicy < 0 || rowPattern < 0 || rowRule < 0)
        return;

    SchematronAssert *a = ((SchematronPolicy*)mainwindow->get_policies().policies[rowPolicy])->patterns[rowPattern]->rules[rowRule]->asserts[row];

    a->type = ruleEdit->get_type_select()->currentText().toStdString();
    a->use_free_text = false;
}

//---------------------------------------------------------------------------
void PoliciesWindow::edit_assert_field()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = get_index_of_item_backXX(item, 3);
    int rowPattern = get_index_of_item_backXX(item, 2);
    int rowRule = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (row < 0 || rowPolicy < 0 || rowPattern < 0 || rowRule < 0)
        return;

    SchematronAssert *a = ((SchematronPolicy*)mainwindow->get_policies().policies[rowPolicy])->patterns[rowPattern]->rules[rowRule]->asserts[row];

    a->field = ruleEdit->get_field_select()->currentText().toStdString();
    a->use_free_text = false;
}

//---------------------------------------------------------------------------
void PoliciesWindow::edit_assert_validator()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = get_index_of_item_backXX(item, 3);
    int rowPattern = get_index_of_item_backXX(item, 2);
    int rowRule = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (row < 0 || rowPolicy < 0 || rowPattern < 0 || rowRule < 0)
        return;

    SchematronAssert *a = ((SchematronPolicy*)mainwindow->get_policies().policies[rowPolicy])->patterns[rowPattern]->rules[rowRule]->asserts[row];

    a->validator = ruleEdit->get_validator_value_from_pretty_name(ruleEdit->get_validator_select()->currentText().toStdString());
    a->use_free_text = false;
}

//---------------------------------------------------------------------------
void PoliciesWindow::edit_assert_value()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = get_index_of_item_backXX(item, 3);
    int rowPattern = get_index_of_item_backXX(item, 2);
    int rowRule = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (row < 0 || rowPolicy < 0 || rowPattern < 0 || rowRule < 0)
        return;

    SchematronAssert *a = ((SchematronPolicy*)mainwindow->get_policies().policies[rowPolicy])->patterns[rowPattern]->rules[rowRule]->asserts[row];

    a->value = ruleEdit->get_value_line()->text().toStdString();
    ruleEdit->value_to_quotted_value(a->value);
    a->use_free_text = false;
}

//---------------------------------------------------------------------------
void PoliciesWindow::edit_assert_freeText()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = get_index_of_item_backXX(item, 3);
    int rowPattern = get_index_of_item_backXX(item, 2);
    int rowRule = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (row < 0 || rowPolicy < 0 || rowPattern < 0 || rowRule < 0)
        return;

    SchematronAssert *a = ((SchematronPolicy*)mainwindow->get_policies().policies[rowPolicy])->patterns[rowPattern]->rules[rowRule]->asserts[row];

    a->text = ruleEdit->get_freeText_text()->toPlainText().toStdString();
    a->use_free_text = true;

    SchematronAssert test;
    if (mainwindow->get_policies().try_parsing_test(a->text, &test))
        ruleEdit->get_editorSelector_radio()->setEnabled(true);
    else
        ruleEdit->get_editorSelector_radio()->setEnabled(false);
}

//---------------------------------------------------------------------------
void PoliciesWindow::assert_free_text_selected(bool checked)
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

    string assertStr = mainwindow->get_policies().serialize_assert_for_test(&a);
    ruleEdit->get_freeText_text()->setText(QString().fromStdString(assertStr));
    ruleEdit->get_freeText_text()->show();
    ruleEdit->get_editor_frame()->hide();
}

//---------------------------------------------------------------------------
void PoliciesWindow::assert_editor_selected(bool checked)
{
    if (!checked)
        return;

    SchematronAssert a;
    if (mainwindow->get_policies().try_parsing_test(ruleEdit->get_freeText_text()->toPlainText().toStdString(), &a))
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
}

//---------------------------------------------------------------------------
void PoliciesWindow::policiesTree_selectionChanged()
{
    QTreeWidget *tree = policiesTree->get_policies_tree();

    QList<QTreeWidgetItem *> list = tree->selectedItems();
    if (list.empty())
        return;

    QTreeWidgetItem *item = list.first();
    QTreeWidgetItem *tmp = item->parent();
    int level = 0;
    while (tmp)
    {
        tmp = tmp->parent();
        ++level;
    }
    switch (level)
    {
        case 0:
            displayPoliciesMenu();
            break;
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
// Visual elements
//***************************************************************************

//---------------------------------------------------------------------------
void PoliciesWindow::clearPoliciesElements()
{
    if (!policiesTree)
        return;

    if (policiesMenu)
    {
        policiesTree->get_menu_layout()->removeWidget(policiesMenu);
        delete policiesMenu;
        policiesMenu=NULL;
    }

    if (policyMenu)
    {
        policiesTree->get_menu_layout()->removeWidget(policyMenu);
        delete policyMenu;
        policyMenu=NULL;
    }

    if (groupOfRules)
    {
        policiesTree->get_menu_layout()->removeWidget(groupOfRules);
        delete groupOfRules;
        groupOfRules = NULL;
    }

    if (ruleMenu)
    {
        policiesTree->get_menu_layout()->removeWidget(ruleMenu);
        delete ruleMenu;
        ruleMenu = NULL;
    }

    if (ruleEdit)
    {
        policiesTree->get_menu_layout()->removeWidget(ruleEdit);
        delete ruleEdit;
        ruleEdit=NULL;
    }
}

//---------------------------------------------------------------------------
void PoliciesWindow::createPoliciesTree()
{
    if (policiesTree)
        return;

    clearPoliciesElements();
    policiesTree = new PoliciesTree(mainwindow);
}

//---------------------------------------------------------------------------
void PoliciesWindow::displayPoliciesTree()
{
    createPoliciesTree();

    mainwindow->set_widget_to_layout(policiesTree);
    QTreeWidget *tree = policiesTree->get_policies_tree();

    QTreeWidgetItem* policies = tree->topLevelItem(0);
    if (!policies)
    {
        policies = new QTreeWidgetItem(tree);
        policies->setText(0, tr("Policies"));
        policies->setSelected(true);
        createPoliciesMenu();
    }
    updatePoliciesTree();
    if (policies->childCount())
        policies->setExpanded(true);
    QObject::connect(policiesTree->get_policies_tree(), SIGNAL(itemSelectionChanged()),
                     this, SLOT(policiesTree_selectionChanged()));
}

//---------------------------------------------------------------------------
void PoliciesWindow::updatePoliciesTreeRule(SchematronRule *rule, QTreeWidgetItem *parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    QString name = QString("Rule");
    item->setText(0, name);

    for (size_t i = 0; i < rule->asserts.size(); ++i)
    {
        SchematronAssert *assert = rule->asserts[i];
        if (!assert)
            continue;

        QTreeWidgetItem* a = new QTreeWidgetItem(item);
        QString descr = QString().fromStdString(assert->description);
        a->setText(0, descr);
    }
}

//---------------------------------------------------------------------------
void PoliciesWindow::updatePoliciesTreePattern(SchematronPattern *pattern, QTreeWidgetItem *parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    QString name = QString().fromStdString(pattern->name);
    if (!name.length())
        name = QString("New group of rules");
    item->setText(0, name);

    for (size_t i = 0; i < pattern->rules.size(); ++i)
    {
        SchematronRule *rule =pattern->rules[i];
        if (!rule)
            continue;
        updatePoliciesTreeRule(rule, item);
    }
}

//---------------------------------------------------------------------------
void PoliciesWindow::updatePoliciesTreePolicy(Policy* policy, QTreeWidgetItem *parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    QString title = QString().fromStdString(policy->title);
    if (!title.length())
        title = QString("New policy");
    item->setText(0, title);

    for (size_t i = 0; i < ((SchematronPolicy*)policy)->patterns.size(); ++i)
    {
        SchematronPattern *pat = ((SchematronPolicy*)policy)->patterns[i];
        if (!pat)
            continue;
        updatePoliciesTreePattern(pat, item);
    }
}

void PoliciesWindow::removeTreeChildren(QTreeWidgetItem* item)
{
    if (!item)
        return;
    QList<QTreeWidgetItem *> list = item->takeChildren();
    while (!list.isEmpty())
        removeTreeChildren(list.takeFirst());
}

//---------------------------------------------------------------------------
void PoliciesWindow::updatePoliciesTree()
{
    if (!policiesTree)
        return;

    QTreeWidgetItem* policies = policiesTree->get_policies_tree()->topLevelItem(0);
    removeTreeChildren(policies);

    for (size_t i = 0; i < mainwindow->get_policies().policies.size(); ++i)
    {
        Policy *policy = mainwindow->get_policies().policies[i];
        if (!policy)
            continue;
        updatePoliciesTreePolicy(policy, policies);
    }
}

//---------------------------------------------------------------------------
void PoliciesWindow::createPoliciesMenu()
{
    if (policiesMenu)
        return;

    clearPoliciesElements();
    policiesMenu = new PoliciesMenu(policiesTree->get_menu_frame());
    policiesTree->get_menu_layout()->addWidget(policiesMenu);
    QObject::connect(policiesMenu->get_importPolicy_button(), SIGNAL(clicked()),
                     this, SLOT(import_schematron()));
    QObject::connect(policiesMenu->get_addNewPolicy_button(), SIGNAL(clicked()),
                     this, SLOT(add_new_policy()));
    QObject::connect(policiesMenu->get_deletePolicies_button(), SIGNAL(clicked()),
                     this, SLOT(delete_all_policies()));

    if (!mainwindow->get_policies().policies.size())
        policiesMenu->get_deletePolicies_button()->setEnabled(false);
}

//---------------------------------------------------------------------------
void PoliciesWindow::displayPoliciesMenu()
{
    createPoliciesMenu();
}

//---------------------------------------------------------------------------
void PoliciesWindow::createPolicyMenu()
{
    if (policyMenu)
        return;
    clearPoliciesElements();
    policyMenu = new PolicyMenu(policiesTree->get_menu_frame());
    policiesTree->get_menu_layout()->addWidget(policyMenu);
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
void PoliciesWindow::displayPolicyMenu(QString title)
{
    createPolicyMenu();
    QLineEdit* name = policyMenu->get_title_line();

    name->setText(title);

    policyMenu->get_savePolicy_button()->setEnabled(false);
    int index = get_index_in_tree();
    if (index >= 0 && index < (int)mainwindow->get_policies().policies.size())
    {
        if (mainwindow->get_policies().policies[index] && mainwindow->get_policies().policies[index]->filename.length())
            policyMenu->get_savePolicy_button()->setEnabled(true);
    }
}

//---------------------------------------------------------------------------
void PoliciesWindow::createGroupOfRules()
{
    if (groupOfRules)
        return;
    clearPoliciesElements();
    groupOfRules = new GroupOfRules(policiesTree->get_menu_frame());
    policiesTree->get_menu_layout()->addWidget(groupOfRules);
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
void PoliciesWindow::displayGroupOfRules(QString title)
{
    createGroupOfRules();
    QLineEdit* name = groupOfRules->get_title_line();

    name->setText(title);
}

//---------------------------------------------------------------------------
void PoliciesWindow::createRuleMenu()
{
    if (ruleMenu)
        return;
    clearPoliciesElements();
    ruleMenu = new RuleMenu(policiesTree->get_menu_frame());
    policiesTree->get_menu_layout()->addWidget(ruleMenu);
    QObject::connect(ruleMenu->get_addNewAssert_button(), SIGNAL(clicked()),
                     this, SLOT(add_new_assert()));
    QObject::connect(ruleMenu->get_deleteRule_button(), SIGNAL(clicked()),
                     this, SLOT(delete_rule()));
    QObject::connect(ruleMenu->get_duplicateRule_button(), SIGNAL(clicked()),
                     this, SLOT(duplicate_rule()));
}

//---------------------------------------------------------------------------
void PoliciesWindow::displayRuleMenu()
{
    createRuleMenu();
}

//---------------------------------------------------------------------------
void PoliciesWindow::createRuleEdit()
{
    clearPoliciesElements();
    ruleEdit = new RuleEdit(mainwindow);
    policiesTree->get_menu_layout()->addWidget(ruleEdit);
}

//---------------------------------------------------------------------------
void PoliciesWindow::displayRuleEdit(QTreeWidgetItem *item)
{
    if (!item)
        return;
    createRuleEdit();

    int rowPolicy = get_index_of_item_backXX(item, 3);
    int rowPattern = get_index_of_item_backXX(item, 2);
    int rowRule = get_index_of_item_backXX(item, 1);
    int rowAssert = get_index_of_item_backXX(item, 0);

    if (rowPolicy < 0 || rowPattern < 0 || rowRule < 0 || rowAssert < 0)
        return;
    SchematronAssert *a = ((SchematronPolicy*)mainwindow->get_policies().policies[rowPolicy])->patterns[rowPattern]->rules[rowRule]->asserts[rowAssert];
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
    if (!a->use_free_text || mainwindow->get_policies().try_parsing_test(a->text, &test))
        ruleEdit->get_editorSelector_radio()->setEnabled(true);
    else
        ruleEdit->get_editorSelector_radio()->setEnabled(false);
}

//***************************************************************************
// HELPER
//***************************************************************************

//---------------------------------------------------------------------------
int PoliciesWindow::get_index_in_tree()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return -1;

    return item->parent()->indexOfChild(item);
}

//---------------------------------------------------------------------------
int PoliciesWindow::get_index_of_item_backXX(QTreeWidgetItem* item, size_t back)
{
    QTreeWidgetItem* tmp = item;
    for (size_t i = 0; i < back; ++i)
    {
        if (!tmp->parent())
            return -1;
        tmp = tmp->parent();
    }

    if (!tmp || !tmp->parent())
        return -1;
    return tmp->parent()->indexOfChild(tmp);
}

//---------------------------------------------------------------------------
QTreeWidgetItem *PoliciesWindow::get_item_in_tree()
{
    QList<QTreeWidgetItem *> list = policiesTree->get_policies_tree()->selectedItems();
    if (list.empty())
        return NULL;

    return list.first();
}

}
