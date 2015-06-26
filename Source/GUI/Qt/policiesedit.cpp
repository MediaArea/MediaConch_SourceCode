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
    add_values_to_selector();

    QObject::connect(ui->newRule, SIGNAL(clicked()), this, SLOT(on_addNewRule()));
    QObject::connect(ui->rules, SIGNAL(cellDoubleClicked(int, int)),
                     this, SLOT(cell_double_clicked(int, int)));
    QObject::connect(ui->deleteRule, SIGNAL(clicked()), this, SLOT(on_deleteRule()));
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
void PoliciesEdit::clear_errors()
{
    ui->errors->hide();
    errors.clear();
}

//---------------------------------------------------------------------------
void PoliciesEdit::clear()
{
    clear_errors();
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
    if (!r->description.length())
    {
        add_error(__T("Name of the rule must be set"));
        show_errors();
        return;
    }
    ui->ruleName->setText(QString());
    if (ui->freeTextSelector->isChecked())
    {
        r->use_free_text = true;
        r->text = ui->freeText->toPlainText().toStdString();
        ui->freeText->setText(QString());
    } else
    {
        r->type = ui->type->currentText().toStdString();
        r->field = ui->field->currentText().toStdString();
        r->validator = ui->validator->currentText().toStdString();
        r->value = ui->value->text().toStdString();
        ui->type->setCurrentIndex(0);
        ui->field->setCurrentIndex(0);
        ui->validator->setCurrentIndex(0);
        ui->value->setText(QString());
        r->use_free_text = false;
    }
    add_rule(r);
    mainwindow->rule_to_add(r);
}

//---------------------------------------------------------------------------
void PoliciesEdit::on_deleteRule()
{
    QList<QTableWidgetItem *> list = ui->rules->selectedItems();

    if (list.isEmpty())
    {
        return;
    }

    QTableWidgetItem *item = list.first();
    mainwindow->rule_to_delete(item->text().toStdString());
    ui->rules->removeRow(item->row());
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

    ui->ruleName->setText(item->text());
    if (r->use_free_text)
    {
        ui->freeText->setText(QString().fromStdString(r->text));
        ui->freeTextSelector->setChecked(true);
        Q_EMIT ui->freeTextSelector->clicked();
        return;
    }

    int pos = ui->type->findText(QString().fromStdString(r->type));
    if (pos != -1)
    {
        ui->type->setCurrentIndex(pos);
    }
    pos = ui->field->findText(QString().fromStdString(r->field));
    if (pos != -1)
    {
        ui->field->setCurrentIndex(pos);
    }
    pos = ui->validator->findText(QString().fromStdString(r->validator));
    if (pos != -1)
    {
        ui->validator->setCurrentIndex(pos);
    }
    ui->value->setText(QString().fromStdString(r->value));
    ui->editorSelector->setChecked(true);
    Q_EMIT ui->editorSelector->clicked();
}

//***************************************************************************
// HELPER
//***************************************************************************

//---------------------------------------------------------------------------
void PoliciesEdit::add_values_to_selector()
{
    const list<string> *existing_type = mainwindow->providePolicyExistingType();
    list<string>::const_iterator itType = existing_type->begin();
    list<string>::const_iterator iteType = existing_type->end();
    for (; itType != iteType; ++itType)
    {
        ui->type->addItem(QString().fromStdString(*itType));
    }

    const list<string> *existing_field = mainwindow->providePolicyExistingField();
    list<string>::const_iterator itField = existing_field->begin();
    list<string>::const_iterator iteField = existing_field->end();
    for (; itField != iteField; ++itField)
    {
        ui->field->addItem(QString().fromStdString(*itField));
    }

    const list<Policies::validatorType> *existing_validator = mainwindow->providePolicyExistingValidator();
    list<Policies::validatorType>::const_iterator itValidator = existing_validator->begin();
    list<Policies::validatorType>::const_iterator iteValidator = existing_validator->end();
    for (; itValidator != iteValidator; ++itValidator)
    {
        ui->validator->addItem(QString().fromStdString(itValidator->value));
    }
}
