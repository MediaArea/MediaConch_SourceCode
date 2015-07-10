/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "policiesmenu.h"
#include "ui_policiesmenu.h"

#if QT_VERSION >= 0x050200
    #include <QFontDatabase>
#endif

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

PoliciesMenu::PoliciesMenu(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::PoliciesMenu)
{
    ui->setupUi(this);
}

PoliciesMenu::~PoliciesMenu()
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
QPushButton *PoliciesMenu::get_importPolicy_button()
{
    return ui->importPolicy;
}

//---------------------------------------------------------------------------
QPushButton *PoliciesMenu::get_deletePolicies_button()
{
    return ui->deletePolicies;
}

//---------------------------------------------------------------------------
QPushButton *PoliciesMenu::get_addNewPolicy_button()
{
    return ui->addNewPolicy;
}

//***************************************************************************
// Slots
//***************************************************************************

}
