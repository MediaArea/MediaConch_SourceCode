/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "verbosityspinbox.h"
#include "ui_verbosityspinbox.h"
#include <QSpinBox>

namespace MediaConch {

//***************************************************************************
// VerbosityBox
//***************************************************************************

//---------------------------------------------------------------------------
const QString VerbositySpinbox::VerbosityBox::last_used_text = QString("Last used verbosity");
const int     VerbositySpinbox::VerbosityBox::last_used_value = -1;

//---------------------------------------------------------------------------
QString VerbositySpinbox::VerbosityBox::textFromValue(int value) const
{
    if (value == last_used_value)
        return last_used_text;

    return QSpinBox::textFromValue(value);
}

//---------------------------------------------------------------------------
int VerbositySpinbox::VerbosityBox::valueFromText(const QString & text) const
{
    if (text == last_used_text)
        return last_used_value;

    return QSpinBox::valueFromText(text);
}

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

//---------------------------------------------------------------------------
VerbositySpinbox::VerbositySpinbox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VerbosityFrame)
{
    ui->setupUi(this);
    delete ui->verbosity;
    ui->verbosity = new VerbosityBox(this);
    ui->verbosity->setMinimum(-1);
    ui->verbosity->setMaximum(5);
    ui->verbosity->setValue(VerbosityBox::last_used_value);

    ui->gridLayout->addWidget(ui->verbosity, 0, 0, 1, 1);
}

//---------------------------------------------------------------------------
VerbositySpinbox::~VerbositySpinbox()
{
    delete ui;
}

//***************************************************************************
// Visual element
//***************************************************************************

//---------------------------------------------------------------------------
QSpinBox *VerbositySpinbox::get_verbosity_spin()
{
    return ui->verbosity;
}

//---------------------------------------------------------------------------
QDialogButtonBox *VerbositySpinbox::get_buttons_box()
{
    return ui->buttons;
}

}
