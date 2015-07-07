/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "policymenu.h"
#include "ui_policymenu.h"

#if QT_VERSION >= 0x050200
    #include <QFontDatabase>
#endif
#include <QLineEdit>

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

PolicyMenu::PolicyMenu(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::PolicyMenu)
{
    ui->setupUi(this);
}

PolicyMenu::~PolicyMenu()
{
    delete ui;
}

//***************************************************************************
// Functions
//***************************************************************************

//***************************************************************************
// Visual element
//***************************************************************************

//---------------------------------------------------------------------------
QLineEdit *PolicyMenu::get_title_line()
{
    return ui->title;
}

//---------------------------------------------------------------------------
QPushButton *PolicyMenu::get_exportPolicy_button()
{
    return ui->exportPolicy;
}

//---------------------------------------------------------------------------
QPushButton *PolicyMenu::get_addNewGor_button()
{
    return ui->addNewGor;
}

//---------------------------------------------------------------------------
QPushButton *PolicyMenu::get_deletePolicy_button()
{
    return ui->deletePolicy;
}

//---------------------------------------------------------------------------
QPushButton *PolicyMenu::get_duplicatePolicy_button()
{
    return ui->duplicatePolicy;
}

//***************************************************************************
// Slots
//***************************************************************************
