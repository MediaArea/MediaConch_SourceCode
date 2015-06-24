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

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

PoliciesMenu::PoliciesMenu(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::PoliciesMenu)
{
    ui->setupUi(this);
    ui->errors->hide();
    ui->errors->setReadOnly(true);
}

PoliciesMenu::~PoliciesMenu()
{
    delete ui;
}

//***************************************************************************
// Functions
//***************************************************************************

//---------------------------------------------------------------------------
void PoliciesMenu::add_error(String error)
{
    errors.push_back(error);
}

void PoliciesMenu::show_errors()
{
    list<String>::iterator it = errors.begin();
    list<String>::iterator ite = errors.end();
    QString out;

    for (; it != ite; ++it)
    {
        out.append(QString().fromStdWString(*it));
        out.append("\n");
    }
    ui->errors->setPlainText(out);
    if (out.length() > 0)
    {
        ui->errors->show();
    }
}

void PoliciesMenu::clear()
{
    ui->errors->hide();
    errors.clear();
    ui->policies->clearContents();
    while (ui->policies->rowCount() > 0)
    {
        ui->policies->removeRow(0);
    }
}

void PoliciesMenu::add_policy(string name)
{
    int row = ui->policies->rowCount();
    ui->policies->insertRow(row);
    QTableWidgetItem *item = new QTableWidgetItem(QString().fromStdString(name));
    ui->policies->setItem(row, 0, item);
}

//***************************************************************************
// Visual element
//***************************************************************************

//---------------------------------------------------------------------------
const QPushButton *PoliciesMenu::get_schematron_button() const
{
    return ui->importSchematron;
}

//***************************************************************************
// Slots
//***************************************************************************

