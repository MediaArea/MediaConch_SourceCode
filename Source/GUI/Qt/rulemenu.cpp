/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "rulemenu.h"
#include "ui_rulemenu.h"

#if QT_VERSION >= 0x050200
    #include <QFontDatabase>
#endif
#include <QLineEdit>

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

RuleMenu::RuleMenu(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::RuleMenu)
{
    ui->setupUi(this);
}

RuleMenu::~RuleMenu()
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
QPushButton *RuleMenu::get_addNewAssert_button()
{
    return ui->addNewAssert;
}

//---------------------------------------------------------------------------
QPushButton *RuleMenu::get_deleteRule_button()
{
    return ui->deleteRule;
}

//***************************************************************************
// Slots
//***************************************************************************
