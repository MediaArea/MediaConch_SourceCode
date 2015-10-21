/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "xsltpolicymenu.h"
#include "ui_xsltpolicymenu.h"

#if QT_VERSION >= 0x050200
    #include <QFontDatabase>
#endif
#include <QLineEdit>

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

XsltPolicyMenu::XsltPolicyMenu(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::XsltPolicyMenu)
{
    ui->setupUi(this);
    ui->savePolicy->setEnabled(false);
}

XsltPolicyMenu::~XsltPolicyMenu()
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
QLineEdit *XsltPolicyMenu::get_title_line()
{
    return ui->title;
}

//---------------------------------------------------------------------------
QLineEdit *XsltPolicyMenu::get_description_line()
{
    return ui->description;
}

//---------------------------------------------------------------------------
QPushButton *XsltPolicyMenu::get_savePolicy_button()
{
    return ui->savePolicy;
}

//---------------------------------------------------------------------------
QPushButton *XsltPolicyMenu::get_savePolicyTo_button()
{
    return ui->savePolicyTo;
}

//---------------------------------------------------------------------------
QPushButton *XsltPolicyMenu::get_addNewRule_button()
{
    return ui->addNewRule;
}

//---------------------------------------------------------------------------
QPushButton *XsltPolicyMenu::get_deletePolicy_button()
{
    return ui->deletePolicy;
}

//---------------------------------------------------------------------------
QPushButton *XsltPolicyMenu::get_duplicatePolicy_button()
{
    return ui->duplicatePolicy;
}

//***************************************************************************
// Slots
//***************************************************************************

}
