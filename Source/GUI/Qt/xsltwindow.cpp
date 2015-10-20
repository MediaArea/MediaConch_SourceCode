/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "policieswindow.h"
#include "xsltwindow.h"
#include "mainwindow.h"
#include "xsltpolicymenu.h"
#include "xsltruleedit.h"

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
#include <QSpinBox>
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

XsltWindow::XsltWindow(PoliciesWindow *window, MainWindow *parent) : PolicyWindow(window, parent)
{
    policyMenu = NULL;
    ruleEdit = NULL;
}

XsltWindow::~XsltWindow()
{
    clearPolicyElements();
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
void XsltWindow::display_selection(int level, QTreeWidgetItem *item)
{
    switch (level)
    {
        case 1:
            displayPolicyMenu(item->text(0));
            break;
        case 2: {
            int rowPolicy = policieswindow->get_index_of_item_backXX(item, 1);
            int rowRule = policieswindow->get_index_of_item_backXX(item, 0);
            displayRuleEdit(rowPolicy, rowRule);
        } break;
        default:
            return;
    }
}

//***************************************************************************
// Slots
//***************************************************************************

//---------------------------------------------------------------------------
void XsltWindow::save_policy()
{
    policieswindow->save_policy();
}

//---------------------------------------------------------------------------
void XsltWindow::save_policy_to()
{
    if (policieswindow->save_policy_to(Policies::POLICY_XSLT) < 0)
        return;
    policyMenu->get_savePolicy_button()->setEnabled(true);
}

//---------------------------------------------------------------------------
void XsltWindow::delete_policy()
{
    clearPolicyElements();
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int row = policieswindow->get_index_in_tree();
    if (row < 0)
        return;

    // Internal data
    Policy *p = mainwindow->get_policies().policies[row];
    p->saved = true;
    for (size_t i = 0; i < ((XsltPolicy*)p)->rules.size(); ++i)
        delete ((XsltPolicy*)p)->rules[i];
    ((XsltPolicy*)p)->rules.clear();
    mainwindow->get_policies().policies.erase(mainwindow->get_policies().policies.begin() + row);

    // Visual
    policieswindow->policy_deleted(item, row);
}

//---------------------------------------------------------------------------
void XsltWindow::duplicate_policy()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item)
        return;

    int row = policieswindow->get_index_in_tree();
    if (row < 0)
        return;

    Policy *p = new XsltPolicy((XsltPolicy*)mainwindow->get_policies().policies[row]);

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
    for (size_t i = 0; i < ((XsltPolicy*)p)->rules.size(); ++i)
        policieswindow->updatePoliciesTreeXsltRule(((XsltPolicy*)p)->rules[i], new_item);
    displayPolicyMenu(title);
}

//---------------------------------------------------------------------------
void XsltWindow::add_new_rule()
{
    QTreeWidgetItem* parent = policieswindow->get_item_in_tree();
    if (!parent)
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(parent, 0);
    if (rowPolicy < 0)
        return;

    XsltRule *r = new XsltRule;
    r->title = string("New Rule");
    r->use_free_text = false;

    QString name = QString().fromStdString(r->title);
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, QStringList(name));

    mainwindow->get_policies().policies[rowPolicy]->saved = false;
    ((XsltPolicy*)mainwindow->get_policies().policies[rowPolicy])->rules.push_back(r);
    parent->setExpanded(true);
    parent->setSelected(false);
    item->setSelected(true);
    displayRuleEdit(rowPolicy, ((XsltPolicy*)mainwindow->get_policies().policies[rowPolicy])->rules.size() - 1);
}

//---------------------------------------------------------------------------
void XsltWindow::duplicate_rule()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item)
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (rowPolicy < 0 || row < 0)
        return;

    XsltPolicy* p = (XsltPolicy*)mainwindow->get_policies().policies[rowPolicy];
    XsltRule *r = new XsltRule(*p->rules[row]);

    r->title = r->title + string(" (Copy)");

    p->rules.push_back(r);
    //p->saved = false;

    QTreeWidgetItem* new_item = new QTreeWidgetItem(item->parent());
    new_item->setText(0, QString().fromStdString(r->title));
    item->setSelected(false);
    new_item->setSelected(true);
    displayRuleEdit(rowPolicy, p->rules.size() - 1);
}

//---------------------------------------------------------------------------
void XsltWindow::delete_rule()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (rowPolicy < 0 || row < 0)
        return;

    // Internal data
    XsltPolicy* p = (XsltPolicy*)mainwindow->get_policies().policies[rowPolicy];
    p->rules.erase(p->rules.begin() + row);
    p->saved = false;

    // Visual
    policieswindow->removeTreeChildren(item);
    QTreeWidgetItem* parent = item->parent();
    delete parent->takeChild(row);
    item = policieswindow->get_item_in_tree();
    if (item)
        item->setSelected(false);
    parent->setSelected(true);
    displayPolicyMenu(parent->text(0));
}

//---------------------------------------------------------------------------
void XsltWindow::edit_policy_title()
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

    Policy *p = mainwindow->get_policies().policies[row];
    if (p->title != qtitle.toStdString())
    {
        p->title = qtitle.toStdString();
        p->saved = false;

        QString title = QString().fromStdString(p->title);
        item->setText(0, title);
    }
}

//***************************************************************************
// Visual elements
//***************************************************************************

//---------------------------------------------------------------------------
void XsltWindow::clearPolicyElements()
{
    if (policyMenu)
    {
        policieswindow->remove_widget_from_tree_layout(policyMenu);
        delete policyMenu;
        policyMenu=NULL;
    }

    if (ruleEdit)
    {
        policieswindow->remove_widget_from_tree_layout(ruleEdit);
        delete ruleEdit;
        ruleEdit=NULL;
    }
}

//---------------------------------------------------------------------------
void XsltWindow::createPolicyMenu()
{
    if (policyMenu)
        return;
    clearPolicyElements();
    policyMenu = new XsltPolicyMenu(policieswindow->policyFrame());
    policieswindow->set_widget_to_tree_layout(policyMenu);
    QObject::connect(policyMenu->get_savePolicy_button(), SIGNAL(clicked()),
                     this, SLOT(save_policy()));
    QObject::connect(policyMenu->get_savePolicyTo_button(), SIGNAL(clicked()),
                     this, SLOT(save_policy_to()));
    QObject::connect(policyMenu->get_addNewRule_button(), SIGNAL(clicked()),
                     this, SLOT(add_new_rule()));
    QObject::connect(policyMenu->get_deletePolicy_button(), SIGNAL(clicked()),
                     this, SLOT(delete_policy()));
    QObject::connect(policyMenu->get_duplicatePolicy_button(), SIGNAL(clicked()),
                     this, SLOT(duplicate_policy()));
    QObject::connect(policyMenu->get_title_line(), SIGNAL(textChanged(QString)),
                     this, SLOT(edit_policy_title()));
}

//---------------------------------------------------------------------------
void XsltWindow::displayPolicyMenu(QString title)
{
    createPolicyMenu();
    QLineEdit* name = policyMenu->get_title_line();

    name->setText(title);

    policyMenu->get_savePolicy_button()->setEnabled(false);
    int index = policieswindow->get_index_in_tree();
    if (index >= 0 && index < (int)mainwindow->get_policies().policies.size())
    {
        if (mainwindow->get_policies().policies[index] && mainwindow->get_policies().policies[index]->filename.length())
            policyMenu->get_savePolicy_button()->setEnabled(true);
    }
}

//---------------------------------------------------------------------------
void XsltWindow::createRuleEdit()
{
    clearPolicyElements();
    ruleEdit = new XsltRuleEdit(mainwindow);
    policieswindow->set_widget_to_tree_layout(ruleEdit);
}

//---------------------------------------------------------------------------
void XsltWindow::edit_rule_name(QString new_name)
{
    if (!new_name.length())
    {
        policieswindow->get_error_bar()->showMessage(QString("Rule must have a name"));
        return;
    }
    policieswindow->get_error_bar()->clearMessage();

    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (row < 0 || rowPolicy < 0)
        return;

    XsltRule *r = ((XsltPolicy*)mainwindow->get_policies().policies[rowPolicy])->rules[row];
    if (r->title != new_name.toStdString())
    {
        r->title = new_name.toStdString();
        mainwindow->get_policies().policies[rowPolicy]->saved = false;
        QString name = QString().fromStdString(r->title);
        item->setText(0, name);
    }
}

//---------------------------------------------------------------------------
void XsltWindow::edit_rule_type()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (row < 0 || rowPolicy < 0)
        return;

    XsltRule *r = ((XsltPolicy*)mainwindow->get_policies().policies[rowPolicy])->rules[row];

    if (r->type != ruleEdit->get_type_select()->currentText().toStdString())
    {
        mainwindow->get_policies().policies[rowPolicy]->saved = false;
        r->type = ruleEdit->get_type_select()->currentText().toStdString();
        ruleEdit->change_values_of_field_selector();
    }
}

//---------------------------------------------------------------------------
void XsltWindow::edit_rule_field()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (row < 0 || rowPolicy < 0)
        return;

    XsltRule *r = ((XsltPolicy*)mainwindow->get_policies().policies[rowPolicy])->rules[row];

    if (r->field != ruleEdit->get_field_select()->currentText().toStdString())
    {
        mainwindow->get_policies().policies[rowPolicy]->saved = false;
        r->field = ruleEdit->get_field_select()->currentText().toStdString();
    }
}

//---------------------------------------------------------------------------
void XsltWindow::edit_rule_operator()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (row < 0 || rowPolicy < 0)
        return;

    XsltRule *r = ((XsltPolicy*)mainwindow->get_policies().policies[rowPolicy])->rules[row];

    if (r->ope != ruleEdit->get_operator_select()->currentText().toStdString())
    {
        mainwindow->get_policies().policies[rowPolicy]->saved = false;
        r->ope = ruleEdit->get_operator_select()->currentText().toStdString();
    }
}

//---------------------------------------------------------------------------
void XsltWindow::edit_rule_value()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (row < 0 || rowPolicy < 0)
        return;

    XsltRule *r = ((XsltPolicy*)mainwindow->get_policies().policies[rowPolicy])->rules[row];

    if (r->value != ruleEdit->get_value_line()->text().toStdString())
    {
        mainwindow->get_policies().policies[rowPolicy]->saved = false;
        r->value = ruleEdit->get_value_line()->text().toStdString();
    }
}

//---------------------------------------------------------------------------
void XsltWindow::edit_rule_occurrence()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (row < 0 || rowPolicy < 0)
        return;

    XsltRule *r = ((XsltPolicy*)mainwindow->get_policies().policies[rowPolicy])->rules[row];

    if (r->occurrence != ruleEdit->get_occurrence_box()->value())
    {
        mainwindow->get_policies().policies[rowPolicy]->saved = false;
        r->occurrence = ruleEdit->get_occurrence_box()->value();
    }
}

//---------------------------------------------------------------------------
void XsltWindow::displayRuleEdit(int rowPolicy, int rowRule)
{
    createRuleEdit();

    XsltRule *r = ((XsltPolicy*)mainwindow->get_policies().policies[rowPolicy])->rules[rowRule];
    ruleEdit->rule_clicked(r);

    QObject::connect(ruleEdit->get_name_line(), SIGNAL(textEdited(QString)),
                     this, SLOT(edit_rule_name(QString)));
    QObject::connect(ruleEdit->get_delRule_button(), SIGNAL(clicked()),
                     this, SLOT(delete_rule()));
    QObject::connect(ruleEdit->get_duplicateRule_button(), SIGNAL(clicked()),
                     this, SLOT(duplicate_rule()));
    QObject::connect(ruleEdit->get_type_select(), SIGNAL(currentIndexChanged(int)),
                     this, SLOT(edit_rule_type()));
    QObject::connect(ruleEdit->get_field_select(), SIGNAL(currentIndexChanged(int)),
                     this, SLOT(edit_rule_field()));
    QObject::connect(ruleEdit->get_operator_select(), SIGNAL(currentIndexChanged(int)),
                     this, SLOT(edit_rule_operator()));
    QObject::connect(ruleEdit->get_value_line(), SIGNAL(textEdited(QString)),
                     this, SLOT(edit_rule_value()));
    QObject::connect(ruleEdit->get_occurrence_box(), SIGNAL(valueChanged(int)),
                     this, SLOT(edit_rule_occurrence()));

    //TODO
    // QObject::connect(ruleEdit->get_editorSelector_radio(), SIGNAL(toggled(bool)),
    //                  this, SLOT(assert_editor_selected(bool)));
    // QObject::connect(ruleEdit->get_freeTextSelector_radio(), SIGNAL(toggled(bool)),
    //                  this, SLOT(assert_free_text_selected(bool)));
    // QObject::connect(ruleEdit->get_freeText_text(), SIGNAL(textChanged()),
    //                  this, SLOT(edit_assert_freeText()));
    // XsltRule test;
    // if (!a->use_free_text || mainwindow->get_policies().try_parsing_test(a->text, &test))
    //     ruleEdit->get_editorSelector_radio()->setEnabled(true);
    // else
    //     ruleEdit->get_editorSelector_radio()->setEnabled(false);
}

}
