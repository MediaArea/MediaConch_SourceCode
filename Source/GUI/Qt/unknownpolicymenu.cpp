/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "unknownpolicymenu.h"
#include "ui_unknownpolicymenu.h"

#if QT_VERSION >= 0x050200
    #include <QFontDatabase>
#endif
#include <QLineEdit>

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

UnknownPolicyMenu::UnknownPolicyMenu(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::UnknownPolicyMenu)
{
    ui->setupUi(this);
    ui->savePolicy->setEnabled(false);
    ui->title->setReadOnly(true);
}

UnknownPolicyMenu::~UnknownPolicyMenu()
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
QLineEdit *UnknownPolicyMenu::get_title_line()
{
    return ui->title;
}

//---------------------------------------------------------------------------
QPushButton *UnknownPolicyMenu::get_savePolicy_button()
{
    return ui->savePolicy;
}

//---------------------------------------------------------------------------
QPushButton *UnknownPolicyMenu::get_savePolicyTo_button()
{
    return ui->savePolicyTo;
}

//---------------------------------------------------------------------------
QPushButton *UnknownPolicyMenu::get_deletePolicy_button()
{
    return ui->deletePolicy;
}

//---------------------------------------------------------------------------
QPushButton *UnknownPolicyMenu::get_duplicatePolicy_button()
{
    return ui->duplicatePolicy;
}

//***************************************************************************
// Slots
//***************************************************************************

}
