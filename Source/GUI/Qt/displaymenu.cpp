/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "displaymenu.h"
#include "ui_displaymenu.h"

#include <QTableWidget>
#include <QPushButton>

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

DisplayMenu::DisplayMenu(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::DisplayMenu)
{
    ui->setupUi(this);
}

DisplayMenu::~DisplayMenu()
{
    delete ui;
}

//***************************************************************************
// Functions
//***************************************************************************

//---------------------------------------------------------------------------
QTableWidget *DisplayMenu::get_display_table()
{
    return ui->table;
}

//---------------------------------------------------------------------------
QFrame *DisplayMenu::get_menu_frame()
{
    return ui->menu;
}

//---------------------------------------------------------------------------
QPushButton *DisplayMenu::get_addFile_button()
{
    return ui->addNewFile;
}

//---------------------------------------------------------------------------
QPushButton *DisplayMenu::get_exportFile_button()
{
    return ui->exportFile;
}

//---------------------------------------------------------------------------
QPushButton *DisplayMenu::get_delFile_button()
{
    return ui->deleteFile;
}

//***************************************************************************
// Visual element
//***************************************************************************

//***************************************************************************
// Slots
//***************************************************************************

}
