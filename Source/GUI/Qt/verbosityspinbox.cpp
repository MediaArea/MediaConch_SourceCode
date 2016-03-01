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
const QString VerbositySpinbox::VerbosityBox::default_text = QString("Default (5)");
const int     VerbositySpinbox::VerbosityBox::default_value = -1;

//---------------------------------------------------------------------------
QString VerbositySpinbox::VerbosityBox::textFromValue(int value) const
{
    if (value == default_value)
        return default_text;

    return QSpinBox::textFromValue(value);
}

//---------------------------------------------------------------------------
int VerbositySpinbox::VerbosityBox::valueFromText(const QString & text) const
{
    if (text == default_text)
        return default_value;

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
    ui->verbosity->setValue(-1);

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
