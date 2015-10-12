/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "policieswindow.h"
#include "policywindow.h"
#include "schematronwindow.h"
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
#include <QFrame>
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
    policywindow = NULL;
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
void PoliciesWindow::add_new_schematron_policy(QTreeWidgetItem* parent)
{
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
    clearPoliciesElements();
    policywindow = new SchematronWindow(this, mainwindow);
    policywindow->displayPolicyMenu(title);
    parent->setExpanded(true);
    parent->setSelected(false);
    item->setSelected(true);
}

//---------------------------------------------------------------------------
void PoliciesWindow::add_new_xslt_policy(QTreeWidgetItem* parent)
{
    Policy *p = new XsltPolicy;

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
    // xsltwindow = new XsltWindow(this, mainwindow);
    // xsltwindow->displayPolicyMenu(title);
    parent->setExpanded(true);
    parent->setSelected(false);
    item->setSelected(true);
}

//---------------------------------------------------------------------------
void PoliciesWindow::add_new_policy()
{
    QTreeWidget *tree = policiesTree->get_policies_tree();
    QTreeWidgetItem* parent = tree->topLevelItem(0);
    if (!parent)
        return;

    QMessageBox msgBox;
    msgBox.setText("Which kind of policy do you want to use?");
    QPushButton *schematronButton = msgBox.addButton(tr("Schematron"), QMessageBox::ActionRole);
    QPushButton *xsltButton = msgBox.addButton(tr("XSL"), QMessageBox::ActionRole);

    msgBox.exec();

    if (msgBox.clickedButton() == schematronButton)
        add_new_schematron_policy(parent);
    else if (msgBox.clickedButton() == xsltButton)
        add_new_xslt_policy(parent);
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
    policywindow = new SchematronWindow(this, mainwindow);
    policywindow->displayPolicyMenu(title);
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
        default:
            clearPoliciesElements();
            if (1)
                policywindow = new SchematronWindow(this, mainwindow);
            //TODO XSLT
            policywindow->display_selection(level, item);
            break;
    }
}

//***************************************************************************
// Visual elements
//***************************************************************************

//---------------------------------------------------------------------------
void PoliciesWindow::set_widget_to_tree_layout(QWidget* w)
{
    policiesTree->get_menu_layout()->addWidget(w);
}

//---------------------------------------------------------------------------
void PoliciesWindow::remove_widget_from_tree_layout(QWidget* w)
{
    policiesTree->get_menu_layout()->removeWidget(w);
}

//---------------------------------------------------------------------------
QFrame* PoliciesWindow::policyFrame()
{
    if (!policiesTree)
        return NULL;
    return policiesTree->get_menu_frame();
}

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

    if (policywindow)
    {
        delete policywindow;
        policywindow=NULL;
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

//***************************************************************************
// HELPER
//***************************************************************************

//---------------------------------------------------------------------------
QStatusBar* PoliciesWindow::get_error_bar()
{
    if (!policiesTree)
        return NULL;
    return policiesTree->get_error_bar();
}

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
