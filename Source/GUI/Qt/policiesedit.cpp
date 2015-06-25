/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "policiesedit.h"
#include "ui_policiesedit.h"
#include "Common/Policies.h"
#include "mainwindow.h"

#if QT_VERSION >= 0x050200
    #include <QFontDatabase>
#endif

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

//---------------------------------------------------------------------------
PoliciesEdit::PoliciesEdit(QWidget *parent, string name) :
    QFrame(parent),
    mainwindow((MainWindow *)parent),
    ui(new Ui::PoliciesEdit),
    policyName(name)
{
    ui->setupUi(this);
    ui->errors->hide();
    ui->errors->setReadOnly(true);
    ui->freeText->hide();
    ui->name->setText(QString().fromStdString(policyName));

    QObject::connect(ui->newRule, SIGNAL(clicked()), this, SLOT(on_addNewRule()));
    QObject::connect(ui->rules, SIGNAL(cellDoubleClicked(int, int)),
                     this, SLOT(cell_double_clicked(int, int)));
}

//---------------------------------------------------------------------------
PoliciesEdit::~PoliciesEdit()
{
    delete ui;
}

//***************************************************************************
// Functions
//***************************************************************************

//---------------------------------------------------------------------------
void PoliciesEdit::add_error(String error)
{
    errors.push_back(error);
}

//---------------------------------------------------------------------------
void PoliciesEdit::show_errors()
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

//---------------------------------------------------------------------------
void PoliciesEdit::clear()
{
    ui->errors->hide();
    errors.clear();
    ui->rules->clearContents();
    while (ui->rules->rowCount() > 0)
    {
        ui->rules->removeRow(0);
    }
}

//---------------------------------------------------------------------------
void PoliciesEdit::add_rule(Rule *r)
{
    if (!r)
    {
        return;
    }

    int row = ui->rules->rowCount();
    ui->rules->insertRow(row);

    QTableWidgetItem *item;
    item = new QTableWidgetItem(QString().fromStdString(r->description));
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    ui->rules->setItem(row, 0, item);
}

const QPushButton *PoliciesEdit::get_newRule_button() const
{
    return ui->newRule;
}

const QDialogButtonBox *PoliciesEdit::get_validation_button() const
{
    return ui->validation;
}

string PoliciesEdit::get_old_name() const
{
    return policyName;
}

string PoliciesEdit::get_new_name() const
{
    return ui->name->text().toStdString();
}

//***************************************************************************
// Visual element
//***************************************************************************

//***************************************************************************
// Slots
//***************************************************************************

//---------------------------------------------------------------------------
void PoliciesEdit::on_addNewRule()
{
    Rule *r = new Rule;

    r->description = ui->ruleName->text().toStdString();
    if (ui->freeTextSelector)
    {
        r->use_free_text = true;
        r->text = ui->freeText->toPlainText().toStdString();
    } else
    {
        r->use_free_text = false;
    }
    add_rule(r);
    mainwindow->rule_to_add(r);
}

//---------------------------------------------------------------------------
QString PoliciesEdit::getSelectedRuleName()
{
    QList<QTableWidgetItem *> list = ui->rules->selectedItems();

    if (list.isEmpty())
    {
        return QString();
    }

    QTableWidgetItem *item = list.first();

    return item->text();
}

//---------------------------------------------------------------------------
void PoliciesEdit::cell_double_clicked(int row, int column)
{
    QTableWidgetItem *item = ui->rules->item(row, column);
    string ruleName = item->text().toStdString();

    const Rule *r = mainwindow->get_rule_from_description(ruleName);

    if (r == NULL)
    {
        //Should not happened
        add_error(__T("Rule not found"));
        show_errors();
        return;
    }

    if (r->use_free_text)
    {
        ui->ruleName->setText(item->text());
        ui->freeText->setText(QString().fromStdString(r->text));
        ui->freeTextSelector->setChecked(true);
        Q_EMIT ui->freeTextSelector->clicked();
        return;
    }
}
