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
    Policy *p = mainwindow->get_policy(row);
    p->saved = true;
    policieswindow->emphasis_policy_name_in_tree(item);
    for (size_t i = 0; i < ((XsltPolicy*)p)->rules.size(); ++i)
        delete ((XsltPolicy*)p)->rules[i];
    ((XsltPolicy*)p)->rules.clear();

    QFile file(QString().fromStdString(mainwindow->get_policy(row)->filename));
    file.remove();
    mainwindow->remove_policy(row);

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

    Policy *p = new XsltPolicy((XsltPolicy*)mainwindow->get_policy(row));

    p->title = p->title + string(" (Copy)");

    mainwindow->add_policy(p);
    policieswindow->new_policy_filename(p);

    QTreeWidgetItem* parent = item->parent();
    if (!parent)
        return;

    QTreeWidgetItem* new_item = new QTreeWidgetItem(parent);
    QString title = QString().fromStdString(p->title);
    new_item->setText(0, title);
    item->setSelected(false);
    for (size_t i = 0; i < ((XsltPolicy*)p)->rules.size(); ++i)
        policieswindow->updatePoliciesTreeXsltRule(((XsltPolicy*)p)->rules[i], new_item);
    policieswindow->emphasis_policy_name_in_tree(new_item);
    new_item->setSelected(true);
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

    mainwindow->get_policy(rowPolicy)->saved = false;
    ((XsltPolicy*)mainwindow->get_policy(rowPolicy))->rules.push_back(r);
    parent->setExpanded(true);
    parent->setSelected(false);
    item->setSelected(true);
    policieswindow->emphasis_policy_name_in_tree(item);
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

    XsltPolicy* p = (XsltPolicy*)mainwindow->get_policy(rowPolicy);
    XsltRule *r = new XsltRule(*p->rules[row]);

    r->title = r->title + string(" (Copy)");

    p->rules.push_back(r);
    p->saved = false;
    policieswindow->emphasis_policy_name_in_tree(item);

    QTreeWidgetItem* new_item = new QTreeWidgetItem(item->parent());
    new_item->setText(0, QString().fromStdString(r->title));
    item->setSelected(false);
    new_item->setSelected(true);
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

    policieswindow->disconnectPoliciesTreeSelectionChanged();
    // Internal data
    XsltPolicy* p = (XsltPolicy*)mainwindow->get_policy(rowPolicy);
    p->rules.erase(p->rules.begin() + row);
    p->saved = false;
    policieswindow->emphasis_policy_name_in_tree(item);

    // Visual
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
void XsltWindow::edit_policy_title()
{
    QString qtitle = policyMenu->get_title_line()->text();
    if (!qtitle.length())
    {
        policieswindow->set_message_to_status_bar("Policy must have a title");
        return;
    }
    policieswindow->clear_message_in_status_bar();

    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item)
        return;

    int row = policieswindow->get_index_in_tree();;
    if (row < 0)
        return;

    Policy *p = mainwindow->get_policy(row);
    if (p->title != qtitle.toStdString())
    {
        p->title = qtitle.toStdString();
        p->saved = false;
        policieswindow->emphasis_policy_name_in_tree(item);

        QString title = QString().fromStdString(p->title);
        item->setText(0, title);
    }
}

//---------------------------------------------------------------------------
void XsltWindow::edit_policy_description()
{
    QString qdescription = policyMenu->get_description_line()->text();

    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item)
        return;

    int row = policieswindow->get_index_in_tree();;
    if (row < 0)
        return;

    Policy *p = mainwindow->get_policy(row);
    if (p->description != qdescription.toStdString())
    {
        p->description = qdescription.toStdString();
        p->saved = false;
        policieswindow->emphasis_policy_name_in_tree(item);
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
    QObject::connect(policyMenu->get_description_line(), SIGNAL(textChanged(QString)),
                     this, SLOT(edit_policy_description()));
}

//---------------------------------------------------------------------------
void XsltWindow::displayPolicyMenu(QString title)
{
    createPolicyMenu();
    QLineEdit* name = policyMenu->get_title_line();
    name->setText(title);

    policyMenu->get_savePolicy_button()->setEnabled(false);
    int index = policieswindow->get_index_in_tree();
    if (index >= 0 && index < (int)mainwindow->get_policies_count())
    {
        Policy* p = mainwindow->get_policy(index);
        if (p)
        {
            if (p->filename.length())
                policyMenu->get_savePolicy_button()->setEnabled(true);
            QLineEdit* descr = policyMenu->get_description_line();
            descr->setText(QString().fromStdString(p->description));
        }
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
        policieswindow->set_message_to_status_bar("Rule must have a name");
        return;
    }
    policieswindow->clear_message_in_status_bar();

    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (row < 0 || rowPolicy < 0)
        return;

    XsltRule *r = ((XsltPolicy*)mainwindow->get_policy(rowPolicy))->rules[row];
    if (r->title != new_name.toStdString())
    {
        r->title = new_name.toStdString();
        mainwindow->get_policy(rowPolicy)->saved = false;
        policieswindow->emphasis_policy_name_in_tree(item);
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

    XsltRule *r = ((XsltPolicy*)mainwindow->get_policy(rowPolicy))->rules[row];

    if (r->type != ruleEdit->get_type_select()->currentText().toStdString())
    {
        mainwindow->get_policy(rowPolicy)->saved = false;
        policieswindow->emphasis_policy_name_in_tree(item);
        r->type = ruleEdit->get_type_select()->currentText().toStdString();
        ruleEdit->change_values_of_field_selector(r->use_free_text, r->field);
        if (r->type == "General")
        {
            ruleEdit->get_occurrence_box()->setValue(-1);
            ruleEdit->get_occurrence_box()->setReadOnly(true);
        }
        else
            ruleEdit->get_occurrence_box()->setReadOnly(false);
        ruleEdit->check_editor_is_possible(r);
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

    XsltRule *r = ((XsltPolicy*)mainwindow->get_policy(rowPolicy))->rules[row];

    if (r->field != ruleEdit->get_field_select()->currentText().toStdString())
    {
        mainwindow->get_policy(rowPolicy)->saved = false;
        policieswindow->emphasis_policy_name_in_tree(item);
        r->field = ruleEdit->get_field_select()->currentText().toStdString();
        ruleEdit->check_editor_is_possible(r);
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

    XsltRule *r = ((XsltPolicy*)mainwindow->get_policy(rowPolicy))->rules[row];

    if (r->ope != ruleEdit->get_operator_select()->currentText().toStdString())
    {
        mainwindow->get_policy(rowPolicy)->saved = false;
        policieswindow->emphasis_policy_name_in_tree(item);
        r->ope = ruleEdit->get_operator_select()->currentText().toStdString();
        ruleEdit->rule_clicked(r);
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

    XsltRule *r = ((XsltPolicy*)mainwindow->get_policy(rowPolicy))->rules[row];

    if (r->value != ruleEdit->get_value_line()->text().toStdString())
    {
        mainwindow->get_policy(rowPolicy)->saved = false;
        policieswindow->emphasis_policy_name_in_tree(item);
        r->value = ruleEdit->get_value_line()->text().toStdString();
        ruleEdit->check_editor_is_possible(r);
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

    XsltRule *r = ((XsltPolicy*)mainwindow->get_policy(rowPolicy))->rules[row];

    if (r->occurrence != ruleEdit->get_occurrence_box()->value())
    {
        mainwindow->get_policy(rowPolicy)->saved = false;
        policieswindow->emphasis_policy_name_in_tree(item);
        r->occurrence = ruleEdit->get_occurrence_box()->value();
        ruleEdit->check_editor_is_possible(r);
    }
}

//---------------------------------------------------------------------------
void XsltWindow::edit_rule_freeText()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (row < 0 || rowPolicy < 0)
        return;

    XsltRule *r = ((XsltPolicy*)mainwindow->get_policy(rowPolicy))->rules[row];

    if (r->test != ruleEdit->get_freeText_text()->toPlainText().toStdString())
    {
        mainwindow->get_policy(rowPolicy)->saved = false;
        policieswindow->emphasis_policy_name_in_tree(item);
        r->test = ruleEdit->get_freeText_text()->toPlainText().toStdString();
        r->use_free_text = true;
        ruleEdit->check_editor_is_possible(r);
    }
}

//---------------------------------------------------------------------------
void XsltWindow::rule_free_text_selected(bool checked)
{
    if (!checked)
        return;

    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (row < 0 || rowPolicy < 0)
        return;

    policieswindow->emphasis_policy_name_in_tree(item);

    XsltPolicy* p = (XsltPolicy*)mainwindow->get_policy(rowPolicy);
    XsltRule *r = p->rules[row];

    p->create_test_from_rule(r, r->test);
    r->use_free_text = true;
    ruleEdit->rule_clicked(r);
}

//---------------------------------------------------------------------------
void XsltWindow::rule_editor_selected(bool checked)
{
    if (!checked)
        return;

    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = policieswindow->get_index_of_item_backXX(item, 1);
    int row = policieswindow->get_index_in_tree();
    if (row < 0 || rowPolicy < 0)
        return;

    policieswindow->emphasis_policy_name_in_tree(item);

    XsltPolicy* p = (XsltPolicy*)mainwindow->get_policy(rowPolicy);
    XsltRule *r = p->rules[row];
    p->parse_test_for_rule(r->test, r);
    if (r->use_free_text)
        r->use_free_text = false;

    ruleEdit->rule_clicked(r);
}

//---------------------------------------------------------------------------
void XsltWindow::displayRuleEdit(int rowPolicy, int rowRule)
{
    createRuleEdit();

    XsltRule *r = ((XsltPolicy*)mainwindow->get_policy(rowPolicy))->rules[rowRule];
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
    QObject::connect(ruleEdit->get_field_select(), SIGNAL(editTextChanged(const QString&)),
                     this, SLOT(edit_rule_field()));
    QObject::connect(ruleEdit->get_operator_select(), SIGNAL(currentIndexChanged(int)),
                     this, SLOT(edit_rule_operator()));
    QObject::connect(ruleEdit->get_value_line(), SIGNAL(textEdited(QString)),
                     this, SLOT(edit_rule_value()));
    QObject::connect(ruleEdit->get_occurrence_box(), SIGNAL(valueChanged(int)),
                     this, SLOT(edit_rule_occurrence()));
    QObject::connect(ruleEdit->get_editorSelector_radio(), SIGNAL(toggled(bool)),
                     this, SLOT(rule_editor_selected(bool)));
    QObject::connect(ruleEdit->get_freeTextSelector_radio(), SIGNAL(toggled(bool)),
                     this, SLOT(rule_free_text_selected(bool)));
    QObject::connect(ruleEdit->get_freeText_text(), SIGNAL(textChanged()),
                     this, SLOT(edit_rule_freeText()));
}

}
