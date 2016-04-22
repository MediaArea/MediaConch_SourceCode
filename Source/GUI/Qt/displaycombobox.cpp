/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "displaycombobox.h"
#include "ui_displaycombobox.h"
#include <QComboBox>
#include <QFileInfo>

namespace MediaConch {

//***************************************************************************
// DisplayCombobox
//***************************************************************************

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

//---------------------------------------------------------------------------
DisplayCombobox::DisplayCombobox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DisplayFrame)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------
DisplayCombobox::~DisplayCombobox()
{
    delete ui;
}

//---------------------------------------------------------------------------
void DisplayCombobox::fill_display_box(const std::vector<QString>& list)
{
    ui->display->addItem("Last used display", QVariant("last"));
    for (size_t i = 0; i < list.size(); ++i)
    {
        QFileInfo file(list[i]);
        ui->display->addItem(file.baseName(), QVariant(list[i]));
    }
}

//***************************************************************************
// Visual element
//***************************************************************************

//---------------------------------------------------------------------------
QComboBox *DisplayCombobox::get_display_combo()
{
    return ui->display;
}

//---------------------------------------------------------------------------
QDialogButtonBox *DisplayCombobox::get_buttons_box()
{
    return ui->buttons;
}

}
