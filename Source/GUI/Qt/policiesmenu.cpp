/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "policiesmenu.h"
#include "ui_policiesmenu.h"
#include "mainwindow.h"

#if QT_VERSION >= 0x050200
    #include <QFontDatabase>
#endif

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

PoliciesMenu::PoliciesMenu(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::PoliciesMenu),
    mainwindow((MainWindow *)parent)
{
    ui->setupUi(this);
    ui->errors->hide();
    ui->errors->setReadOnly(true);

    QObject::connect(ui->deletePolicy, SIGNAL(clicked()),
                     this, SLOT(delete_clicked()));
    QObject::connect(ui->exportPolicies, SIGNAL(clicked()),
                     this, SLOT(export_clicked()));
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
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
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

//---------------------------------------------------------------------------
const QPushButton *PoliciesMenu::get_addNewPolicy_button() const
{
    return ui->addNewPolicy;
}

//---------------------------------------------------------------------------
const QPushButton *PoliciesMenu::get_editPolicy_button() const
{
    return ui->editPolicy;
}

//---------------------------------------------------------------------------
const QTableWidget *PoliciesMenu::get_policies_table() const
{
    return ui->policies;
}

//***************************************************************************
// Slots
//***************************************************************************

void PoliciesMenu::delete_clicked()
{
    QList<QTableWidgetItem *> list = ui->policies->selectedItems();
    if (list.isEmpty())
    {
        return;
    }

    QTableWidgetItem *item = list.first();

    if (!item)
    {
        return;
    }
    mainwindow->policy_to_delete(item->text().toStdString());
    ui->policies->removeRow(item->row());
}

void PoliciesMenu::export_clicked()
{
    mainwindow->exporting_to_schematron_file();
}
