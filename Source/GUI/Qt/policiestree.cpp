/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "policiestree.h"
#include "ui_policiestree.h"

#if QT_VERSION >= 0x050200
    #include <QFontDatabase>
#endif

#include <QStatusBar>
#include <QTreeWidget>
#include <QFrame>
#include <QVBoxLayout>

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

PoliciesTree::PoliciesTree(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::PoliciesTree)
{
    ui->setupUi(this);
    QVBoxLayout *Layout = new QVBoxLayout(ui->menu);
    ui->menu->setLayout(Layout);
    ui->policies->setColumnCount(1);
    ui->policies->header()->hide();
}

PoliciesTree::~PoliciesTree()
{
    delete ui;
}

//***************************************************************************
// Functions
//***************************************************************************

//---------------------------------------------------------------------------
QTreeWidget *PoliciesTree::get_policies_tree()
{
    return ui->policies;
}

//---------------------------------------------------------------------------
QFrame *PoliciesTree::get_menu_frame()
{
    return ui->menu;
}

//---------------------------------------------------------------------------
QLayout *PoliciesTree::get_menu_layout()
{
    return ui->menu->layout();
}

//***************************************************************************
// Visual element
//***************************************************************************

//***************************************************************************
// Slots
//***************************************************************************

}
