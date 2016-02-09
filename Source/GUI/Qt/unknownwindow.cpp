/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "policieswindow.h"
#include "unknownwindow.h"
#include "mainwindow.h"
#include "unknownpolicymenu.h"
#include "Common/UnknownPolicy.h"

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

UnknownWindow::UnknownWindow(PoliciesWindow *window, MainWindow *parent) : PolicyWindow(window, parent)
{
    policyMenu = NULL;
}

UnknownWindow::~UnknownWindow()
{
    clearPolicyElements();
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
void UnknownWindow::display_selection(int level, QTreeWidgetItem *item)
{
    switch (level)
    {
        case 1:
            displayPolicyMenu(item->text(0));
            break;
        default:
            return;
    }
}

//***************************************************************************
// Slots
//***************************************************************************

//---------------------------------------------------------------------------
void UnknownWindow::save_policy_to()
{
    if (policieswindow->save_policy_to(Policies::POLICY_UNKNOWN) < 0)
        return;
}

//---------------------------------------------------------------------------
void UnknownWindow::delete_policy()
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

    QFile file(QString().fromStdString(mainwindow->get_policy(row)->filename));
    file.remove();
    mainwindow->remove_policy(row);

    // Visual
    policieswindow->policy_deleted(item, row);
}

//---------------------------------------------------------------------------
void UnknownWindow::duplicate_policy()
{
    QTreeWidgetItem* item = policieswindow->get_item_in_tree();
    if (!item)
        return;

    int row = policieswindow->get_index_in_tree();
    if (row < 0)
        return;

    Policy *p = new UnknownPolicy((UnknownPolicy*)mainwindow->get_policy(row));

    mainwindow->add_policy(p);
    p->saved = true;
    QString new_name = QString().fromStdString(p->filename);
    for (;;)
    {
        if (new_name.endsWith(".xsl"))
            new_name.insert(new_name.length() - 4, "_copy");
        else
            new_name += "_copy";
        QFile file(new_name);
        if (!file.exists())
            break;
    }
    QFile old(QString().fromStdString(p->filename));
    old.copy(new_name);
    p->filename = new_name.toStdString();
    p->title = p->filename;

    QTreeWidgetItem* parent = item->parent();
    if (!parent)
        return;

    QTreeWidgetItem* new_item = new QTreeWidgetItem(parent);
    QString title = QString().fromStdString(p->filename);
    new_item->setText(0, title);
    item->setSelected(false);
    new_item->setSelected(true);
}

//***************************************************************************
// Visual elements
//***************************************************************************

//---------------------------------------------------------------------------
void UnknownWindow::clearPolicyElements()
{
    if (policyMenu)
    {
        policieswindow->remove_widget_from_tree_layout(policyMenu);
        delete policyMenu;
        policyMenu=NULL;
    }
}

//---------------------------------------------------------------------------
void UnknownWindow::createPolicyMenu()
{
    if (policyMenu)
        return;
    clearPolicyElements();
    policyMenu = new UnknownPolicyMenu(policieswindow->policyFrame());
    policieswindow->set_widget_to_tree_layout(policyMenu);
    QObject::connect(policyMenu->get_savePolicyTo_button(), SIGNAL(clicked()),
                     this, SLOT(save_policy_to()));
    QObject::connect(policyMenu->get_deletePolicy_button(), SIGNAL(clicked()),
                     this, SLOT(delete_policy()));
    QObject::connect(policyMenu->get_duplicatePolicy_button(), SIGNAL(clicked()),
                     this, SLOT(duplicate_policy()));
}

//---------------------------------------------------------------------------
void UnknownWindow::displayPolicyMenu(QString title)
{
    createPolicyMenu();
    QLineEdit* name = policyMenu->get_title_line();
    name->setText(title);
}

}
