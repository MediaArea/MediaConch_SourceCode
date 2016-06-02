/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "policieswindow.h"
#include "policywindow.h"
#include "schematronwindow.h"
#include "unknownwindow.h"
#include "xsltwindow.h"
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
void PoliciesWindow::import_schema()
{
    QString file = mainwindow->ask_for_schema_file();
    if (!file.length())
        return;

    std::string err;
    int ret = mainwindow->import_policy(file, err);

    displayPoliciesTree();
    if (ret < 0)
        mainwindow->set_msg_to_status_bar(QString().fromStdString(err));
    else
    {
        mainwindow->clear_msg_in_status_bar();
        int row = (int)mainwindow->get_policies_count() - 1;
        QTreeWidgetItem* parent = policiesTree->get_policies_tree()->topLevelItem(0);
        if (row < 0 || !parent)
            return;

        QTreeWidgetItem *item = parent->child(row);
        if (!item)
            return;
        new_policy_filename(mainwindow->get_policy(row));

        parent->setExpanded(true);
        parent->setSelected(false);
        item->setSelected(true);
    }
}

//---------------------------------------------------------------------------
int PoliciesWindow::save_policy_to(Policies::PolicyType type)
{
    int  row = get_index_in_tree();
    if (row < 0)
        return -1;

    QTreeWidgetItem* item = get_item_in_tree();
    int ret = 0;
    if (type == Policies::POLICY_SCHEMATRON)
        ret = mainwindow->exporting_to_schematron_file(row);
    else if (type == Policies::POLICY_UNKNOWN)
        ret = mainwindow->exporting_to_unknown_file(row);
    else
        ret = mainwindow->exporting_to_xslt_file(row);

    unemphasis_policy_name_in_tree(item);
    return ret;
}

//---------------------------------------------------------------------------
void PoliciesWindow::save_policy()
{
    int  row = get_index_in_tree();
    if (row < 0)
        return;

    mainwindow->exporting_policy(row);
    QTreeWidgetItem* item = get_item_in_tree();
    unemphasis_policy_name_in_tree(item);
}

//---------------------------------------------------------------------------
void PoliciesWindow::new_policy_filename(Policy* p)
{
    QString path = mainwindow->get_local_folder();
    path += "/policies";

    QDir dir(path);
    if (!dir.exists())
        dir.mkpath(path);

    QString ext;
    if (p->type == Policies::POLICY_SCHEMATRON)
        ext = "sch";
    else
        ext = "xsl";
    for (int i = 0; 1; ++i)
    {
        QString tmp;
        if (!i)
            tmp = QString("%1/%2.%3").arg(path).arg(QString().fromStdString(p->title)).arg(ext);
        else
            tmp = QString("%1/%2_%4.%3").arg(path).arg(QString().fromStdString(p->title)).arg(ext).arg(i);

        QFileInfo file(tmp);
        if (file.exists())
            continue;
        p->filename = file.absoluteFilePath().toStdString();
        p->saved = false;
        break;
    }
}

//---------------------------------------------------------------------------
void PoliciesWindow::add_new_schematron_policy(QTreeWidgetItem* parent)
{
    Policy *p = new SchematronPolicy(false);

    p->title = string("New policy");
    if (mainwindow->policy_exists(p->title))
    {
        int i = 1;
        while (1)
        {
            std::stringstream ss;
            ss << p->title << " " << i;
            if (!mainwindow->policy_exists(ss.str()))
            {
                p->title = ss.str();
                break;
            }
            ++i;
        }
    }
    new_policy_filename(p);

    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    QString title = QString().fromStdString(p->title);
    item->setText(0, title);
    emphasis_policy_name_in_tree(item);

    mainwindow->add_policy(p);
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
    Policy *p = new XsltPolicy(false);

    p->title = string("New policy");
    if (mainwindow->policy_exists(p->title))
    {
        int i = 1;
        while (1)
        {
            std::stringstream ss;
            ss << p->title << " " << i;
            if (!mainwindow->policy_exists(ss.str()))
            {
                p->title = ss.str();
                break;
            }
            ++i;
        }
    }
    new_policy_filename(p);

    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    QString title = QString().fromStdString(p->title);
    item->setText(0, title);
    emphasis_policy_name_in_tree(item);

    mainwindow->add_policy(p);
    policywindow = new XsltWindow(this, mainwindow);
    policywindow->displayPolicyMenu(title);
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
    msgBox.setStandardButtons(QMessageBox::Cancel);
    QPushButton *xsltButton = msgBox.addButton(tr("&XSL"), QMessageBox::ActionRole);
    QPushButton *schematronButton = msgBox.addButton(tr("&Schematron"), QMessageBox::ActionRole);

    msgBox.exec();

    if (msgBox.clickedButton() == schematronButton)
        add_new_schematron_policy(parent);
    else if (msgBox.clickedButton() == xsltButton)
        add_new_xslt_policy(parent);
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
    for (size_t i = 0; i < mainwindow->get_policies_count(); ++i)
    {
        QString filename = QString().fromStdString(mainwindow->get_policy(i)->filename);
        if (filename.startsWith(":/"))
            continue;
        QFile file(filename);
        file.remove();
    }
    mainwindow->clear_policies();
    policiesMenu->get_deletePolicies_button()->setEnabled(false);
    unemphasis_policy_name_in_tree(policies);
}

//---------------------------------------------------------------------------
void PoliciesWindow::policy_deleted(QTreeWidgetItem* item, int row)
{
    disconnectPoliciesTreeSelectionChanged();
    unemphasis_policy_name_in_tree(item);
    removeTreeChildren(item);
    QTreeWidgetItem* parent = item->parent();
    parent->takeChild(row);
    for (int i = 0; i < parent->childCount(); ++i)
    {
        item = parent->child(i);
        if (item && item->isSelected())
            item->setSelected(false);
    }
    connectPoliciesTreeSelectionChanged();
    parent->setSelected(false);
    parent->setSelected(true);
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

    int row = -1;
    switch (level)
    {
        case 0:
            displayPoliciesMenu();
            return;
        case 4:
            row = get_index_of_item_backXX(item, 3);
            break;
        case 3:
            row = get_index_of_item_backXX(item, 2);
            break;
        case 2:
            row = get_index_of_item_backXX(item, 1);
            break;
        case 1:
            row = get_index_of_item_backXX(item, 0);
            break;
        default:
            return;
    }
    clearPoliciesElements();
    if (row < 0 || mainwindow->get_policy(row)->type == Policies::POLICY_SCHEMATRON)
        policywindow = new SchematronWindow(this, mainwindow);
    else if (mainwindow->get_policy(row)->type == Policies::POLICY_UNKNOWN)
        policywindow = new UnknownWindow(this, mainwindow);
    else
        policywindow = new XsltWindow(this, mainwindow);
    policywindow->display_selection(level, item);
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
    connectPoliciesTreeSelectionChanged();
}

//---------------------------------------------------------------------------
void PoliciesWindow::updatePoliciesTreeSchematronRule(SchematronRule *rule, QTreeWidgetItem *parent)
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
void PoliciesWindow::updatePoliciesTreeSchematronPattern(SchematronPattern *pattern, QTreeWidgetItem *parent)
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
        updatePoliciesTreeSchematronRule(rule, item);
    }
}

//---------------------------------------------------------------------------
QTreeWidgetItem* PoliciesWindow::updatePoliciesTreeSchematronPolicy(SchematronPolicy* policy, QTreeWidgetItem *parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    QString title = QString().fromStdString(policy->title);
    if (!title.length())
        title = QString("New policy");
    item->setText(0, title);

    for (size_t i = 0; i < policy->patterns.size(); ++i)
    {
        SchematronPattern *pat = policy->patterns[i];
        if (!pat)
            continue;
        updatePoliciesTreeSchematronPattern(pat, item);
    }

    return item;
}

//---------------------------------------------------------------------------
QTreeWidgetItem* PoliciesWindow::updatePoliciesTreeUnknownPolicy(UnknownPolicy* policy, QTreeWidgetItem *parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    QString title = QString().fromStdString(policy->filename);
    item->setText(0, title);
    return item;
}

//---------------------------------------------------------------------------
void PoliciesWindow::updatePoliciesTreeXsltRule(XsltRule* rule, QTreeWidgetItem *parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    QString title = QString().fromStdString(rule->title);
    if (!title.length())
        title = QString("New Rule");
    item->setText(0, title);
}

//---------------------------------------------------------------------------
QTreeWidgetItem* PoliciesWindow::updatePoliciesTreeXsltPolicy(XsltPolicy* policy, QTreeWidgetItem *parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    QString title = QString().fromStdString(policy->title);
    if (!title.length())
        title = QString("New policy");
    item->setText(0, title);

    for (size_t i = 0; i < policy->rules.size(); ++i)
    {
        XsltRule *r = policy->rules[i];
        if (!r)
            continue;
        updatePoliciesTreeXsltRule(r, item);
    }
    return item;
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

    for (size_t i = 0; i < mainwindow->get_policies_count(); ++i)
    {
        Policy *policy = mainwindow->get_policy(i);
        if (!policy)
            continue;

        QTreeWidgetItem* item = NULL;
        if (policy->type == Policies::POLICY_SCHEMATRON)
            item = updatePoliciesTreeSchematronPolicy((SchematronPolicy*)policy, policies);
        else if (policy->type == Policies::POLICY_UNKNOWN)
            item = updatePoliciesTreeUnknownPolicy((UnknownPolicy*)policy, policies);
        else if (policy->type == Policies::POLICY_XSLT)
            item = updatePoliciesTreeXsltPolicy((XsltPolicy*)policy, policies);

        if (item && !policy->saved)
            emphasis_policy_name_in_tree(item);
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
                     this, SLOT(import_schema()));
    QObject::connect(policiesMenu->get_addNewPolicy_button(), SIGNAL(clicked()),
                     this, SLOT(add_new_policy()));
    QObject::connect(policiesMenu->get_deletePolicies_button(), SIGNAL(clicked()),
                     this, SLOT(delete_all_policies()));

    if (!mainwindow->get_policies_count())
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
void PoliciesWindow::set_message_to_status_bar(const QString& message)
{
    if (!mainwindow)
        return;
    return mainwindow->set_msg_to_status_bar(message);
}

//---------------------------------------------------------------------------
void PoliciesWindow::clear_message_in_status_bar()
{
    if (!mainwindow)
        return;
    return mainwindow->clear_msg_in_status_bar();
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

//---------------------------------------------------------------------------
void PoliciesWindow::connectPoliciesTreeSelectionChanged()
{
    if (!policiesTree)
        return;

    QObject::connect(policiesTree->get_policies_tree(), SIGNAL(itemSelectionChanged()),
                     this, SLOT(policiesTree_selectionChanged()));
}

//---------------------------------------------------------------------------
void PoliciesWindow::disconnectPoliciesTreeSelectionChanged()
{
    if (!policiesTree)
        return;

    QObject::disconnect(policiesTree->get_policies_tree(), SIGNAL(itemSelectionChanged()),
                     this, SLOT(policiesTree_selectionChanged()));
}

//---------------------------------------------------------------------------
void PoliciesWindow::emphasis_tree_widget_and_children(QTreeWidgetItem *item, bool do_parent)
{
    if (!item)
        return;

    QFont font = item->font(0);
    font.setBold(true);
    item->setFont(0, font);

    if (do_parent && item->parent())
    {
        QFont font = item->parent()->font(0);
        font.setBold(true);
        item->parent()->setFont(0, font);
    }

    for (int i = 0; i < item->childCount(); ++i)
        emphasis_tree_widget_and_children(item->child(i), false);
}

//---------------------------------------------------------------------------
void PoliciesWindow::emphasis_policy_name_in_tree(QTreeWidgetItem *item)
{
    if (!item || !item->parent())
        return;

    QTreeWidgetItem *tmp = item;
    for (; ; tmp = tmp->parent())
        if (!tmp->parent() || !tmp->parent()->parent())
            break;

    emphasis_tree_widget_and_children(tmp, true);
}

//---------------------------------------------------------------------------
void PoliciesWindow::unemphasis_policy_name_in_tree(QTreeWidgetItem *item)
{
    if (!item)
        return;

    QFont font = item->font(0);
    font.setBold(false);
    item->setFont(0, font);

    for (int i = 0; i < item->childCount(); ++i)
        unemphasis_policy_name_in_tree(item->child(i));

    if (mainwindow->is_all_policies_saved() && item->parent())
    {
        QFont font = item->parent()->font(0);
        font.setBold(false);
        item->parent()->setFont(0, font);
    }
}

}
