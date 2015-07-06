/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "groupofrules.h"
#include "ui_groupofrules.h"

#if QT_VERSION >= 0x050200
    #include <QFontDatabase>
#endif
#include <QLineEdit>

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

GroupOfRules::GroupOfRules(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::GroupOfRules)
{
    ui->setupUi(this);
}

GroupOfRules::~GroupOfRules()
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
QLineEdit *GroupOfRules::get_title_line()
{
    return ui->title;
}

//---------------------------------------------------------------------------
QPushButton *GroupOfRules::get_addNewRule_button()
{
    return ui->addNewRule;
}

//---------------------------------------------------------------------------
QPushButton *GroupOfRules::get_deleteGor_button()
{
    return ui->deleteGor;
}

//***************************************************************************
// Slots
//***************************************************************************
