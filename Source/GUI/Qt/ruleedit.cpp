/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "ruleedit.h"
#include "ui_ruleedit.h"
#include "Common/Policies.h"
#include "mainwindow.h"

#if QT_VERSION >= 0x050200
    #include <QFontDatabase>
#endif

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

//---------------------------------------------------------------------------
RuleEdit::RuleEdit(QWidget *parent) :
    QFrame(parent),
    mainwindow((MainWindow *)parent),
    ui(new Ui::RuleEdit)
{
    ui->setupUi(this);
    ui->errors->hide();
    ui->errors->setReadOnly(true);
    clear_editor_fields();
    add_values_to_selector();

    ui->deleteAssert->setEnabled(false);
    QObject::connect(ui->newAssert, SIGNAL(clicked()), this, SLOT(on_addNewAssert()));
    QObject::connect(ui->asserts, SIGNAL(itemSelectionChanged()),
                     this, SLOT(assert_selected_changed()));
    QObject::connect(ui->asserts, SIGNAL(cellClicked(int, int)),
                     this, SLOT(cell_clicked(int, int)));
    QObject::connect(ui->deleteAssert, SIGNAL(clicked()), this, SLOT(on_deleteAssert()));
    QObject::connect(ui->freeTextSelector, SIGNAL(clicked()), this, SLOT(free_text_selected()));
    QObject::connect(ui->editorSelector, SIGNAL(clicked()), this, SLOT(editor_selected()));
    QObject::connect(ui->type, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(editAssert_type()));
    QObject::connect(ui->field, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(editAssert_field()));
    QObject::connect(ui->validator, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(editAssert_validator()));
    QObject::connect(ui->value, SIGNAL(textEdited(QString)),
                     this, SLOT(editAssert_value()));
    QObject::connect(ui->freeText, SIGNAL(textChanged()),
                     this, SLOT(editAssert_freeText()));
    QObject::connect(ui->assertName, SIGNAL(textEdited(QString)),
                     this, SLOT(editAssert_assertName()));
}

//---------------------------------------------------------------------------
RuleEdit::~RuleEdit()
{
    delete ui;
    for (size_t i = 0; i < asserts.size(); ++i)
    {
        delete asserts[i];
    }
}

//***************************************************************************
// Functions
//***************************************************************************

//---------------------------------------------------------------------------
void RuleEdit::add_error(String error)
{
    errors.push_back(error);
}

//---------------------------------------------------------------------------
void RuleEdit::show_errors()
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
void RuleEdit::clear_errors()
{
    ui->errors->hide();
    errors.clear();
}

//---------------------------------------------------------------------------
void RuleEdit::clear()
{
    clear_errors();
    ui->asserts->clearContents();
    while (ui->asserts->rowCount() > 0)
    {
        ui->asserts->removeRow(0);
    }
    clear_editor_fields();
}

//---------------------------------------------------------------------------
void RuleEdit::add_assert(Assert *a)
{
    if (!a)
    {
        return;
    }

    int row = ui->asserts->rowCount();
    ui->asserts->insertRow(row);

    QTableWidgetItem *item;
    item = new QTableWidgetItem(QString().fromStdString(a->description));
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    ui->asserts->setItem(row, 0, item);

    Assert assert;
    Policies p;
    Assert *new_assert = NULL;
    if (a->use_free_text && p.try_parsing_test(a->text, &assert))
    {
        assert.description = a->description;
        new_assert = new Assert(assert);
    } else {
        new_assert = new Assert(*a);
    }
    asserts.push_back(new_assert);
}

void RuleEdit::set_name(string& policyName)
{
    ui->name->setText(QString().fromStdString(policyName));
}

const QPushButton *RuleEdit::get_newAssert_button() const
{
    return ui->newAssert;
}

const QDialogButtonBox *RuleEdit::get_validation_button() const
{
    return ui->validation;
}

string RuleEdit::get_new_name() const
{
    return ui->name->text().toStdString();
}

const vector<Assert *>& RuleEdit::get_asserts() const
{
    return asserts;
}

//***************************************************************************
// Visual element
//***************************************************************************

//***************************************************************************
// Slots
//***************************************************************************
void RuleEdit::copy_visual_to_assert(Assert *a)
{
    a->description = ui->assertName->text().toStdString();
    if (ui->freeTextSelector->isChecked())
    {
        a->use_free_text = true;
        a->text = ui->freeText->toPlainText().toStdString();
    } else
    {
        a->type = ui->type->currentText().toStdString();
        a->field = ui->field->currentText().toStdString();
        a->validator = get_validator_value_from_pretty_name(ui->validator->currentText().toStdString());
        string value = ui->value->text().toStdString();
        value_to_quotted_value(value);
        a->value = value;
        a->use_free_text = false;
    }
}

//---------------------------------------------------------------------------
void RuleEdit::on_addNewAssert()
{
    Assert a;

    copy_visual_to_assert(&a);
    if (!a.description.length())
    {
        add_error(__T("Name of the assert must be set"));
        show_errors();
        return;
    }
    add_assert(&a);
    clear_editor_fields();
}

//---------------------------------------------------------------------------
void RuleEdit::on_deleteAssert()
{
    QList<QTableWidgetItem *> list = ui->asserts->selectedItems();

    while (!list.isEmpty())
    {
        QTableWidgetItem *item = list.first();
        asserts.erase(asserts.begin() + item->row());
        ui->asserts->removeRow(item->row());
        list.removeFirst();
    }
}

//---------------------------------------------------------------------------
void RuleEdit::editAssert_type()
{
    QTableWidgetItem *item = NULL;

    if ((item = getSelectedAssertItem()) == NULL)
    {
        return;
    }
    Assert *r = asserts[item->row()];
    r->type = ui->type->currentText().toStdString();
    r->use_free_text = false;
}

//---------------------------------------------------------------------------
void RuleEdit::editAssert_field()
{
    QTableWidgetItem *item = NULL;

    if ((item = getSelectedAssertItem()) == NULL)
    {
        return;
    }
    Assert *r = asserts[item->row()];
    r->field = ui->field->currentText().toStdString();
    r->use_free_text = false;
}

//---------------------------------------------------------------------------
void RuleEdit::editAssert_validator()
{
    QTableWidgetItem *item = NULL;

    if ((item = getSelectedAssertItem()) == NULL)
    {
        return;
    }
    Assert *r = asserts[item->row()];
    r->validator = get_validator_value_from_pretty_name(ui->validator->currentText().toStdString());
    r->use_free_text = false;
}

//---------------------------------------------------------------------------
void RuleEdit::editAssert_value()
{
    QTableWidgetItem *item = NULL;

    if ((item = getSelectedAssertItem()) == NULL)
    {
        return;
    }
    Assert *r = asserts[item->row()];
    string value = ui->value->text().toStdString();
    value_to_quotted_value(value);
    r->value = value;
    r->use_free_text = false;
}

//---------------------------------------------------------------------------
void RuleEdit::editAssert_freeText()
{
    QTableWidgetItem *item = NULL;

    if ((item = getSelectedAssertItem()) == NULL)
    {
        return;
    }
    Assert *r = asserts[item->row()];
    r->text = ui->freeText->toPlainText().toStdString();
    r->use_free_text = true;
}

//---------------------------------------------------------------------------
void RuleEdit::editAssert_assertName()
{
    QTableWidgetItem *item = NULL;

    if ((item = getSelectedAssertItem()) == NULL)
    {
        return;
    }
    Assert *r = asserts[item->row()];
    r->description = ui->assertName->text().toStdString();
    item->setText(ui->assertName->text());
}

void RuleEdit::assert_selected_changed()
{
    if (ui->asserts->selectedItems().isEmpty())
    {
        ui->deleteAssert->setEnabled(false);
        clear_editor_fields();
    } else {
        ui->deleteAssert->setEnabled(true);
    }
}

//---------------------------------------------------------------------------
QTableWidgetItem* RuleEdit::getSelectedAssertItem()
{
    QList<QTableWidgetItem *> list = ui->asserts->selectedItems();

    if (list.isEmpty())
    {
        return NULL;
    }

    return list.first();
}

//---------------------------------------------------------------------------
QString RuleEdit::getSelectedAssertName()
{

    QTableWidgetItem *item = NULL;

    if ((item = getSelectedAssertItem()) == NULL)
    {
        return QString();
    }

    return item->text();
}

//---------------------------------------------------------------------------
void RuleEdit::cell_clicked(int row, int column)
{
    QTableWidgetItem *item = ui->asserts->item(row, column);
    if (item->row() == -1)
    {
        //Should not happened
        add_error(__T("Assert not found"));
        show_errors();
        return;
    }
    item->setSelected(true);

    const Assert *r = asserts[item->row()];

    if (r == NULL)
    {
        //Should not happened
        add_error(__T("Assert not found"));
        show_errors();
        return;
    }

    ui->assertName->setText(item->text());
    if (r->use_free_text)
    {
        ui->freeText->setText(QString().fromStdString(r->text));
        ui->freeTextSelector->setChecked(true);
        ui->freeText->show();
        ui->editorFrame->hide();
        return;
    }
    fill_editor_fields(r);

    ui->editorSelector->setChecked(true);
    ui->editorFrame->show();
    ui->freeText->hide();
}

//---------------------------------------------------------------------------
void RuleEdit::free_text_selected()
{
    Assert a;

    a.type = ui->type->currentText().toStdString();
    a.field = ui->field->currentText().toStdString();
    a.validator = get_validator_value_from_pretty_name(ui->validator->currentText().toStdString());
    string value = ui->value->text().toStdString();
    value_to_quotted_value(value);
    a.value = value;
    a.use_free_text = false;

    Policies p;
    string assertStr = p.serialize_assert_for_test(&a);
    ui->freeText->setText(QString().fromStdString(assertStr));
    ui->freeText->show();
    ui->editorFrame->hide();
}

//---------------------------------------------------------------------------
void RuleEdit::editor_selected()
{
    Assert r;
    Policies p;
    if (p.try_parsing_test(ui->freeText->toPlainText().toStdString(), &r))
    {
        fill_editor_fields(&r);
    } else
    {
        ui->type->setCurrentIndex(0);
        ui->field->setCurrentIndex(0);
        ui->validator->setCurrentIndex(0);
        ui->value->setText(QString());
    }
    ui->editorFrame->show();
    ui->freeText->hide();
}

//***************************************************************************
// HELPER
//***************************************************************************

//---------------------------------------------------------------------------
void RuleEdit::add_values_to_selector()
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
        ui->validator->addItem(QString().fromStdString(itValidator->pretty_name));
    }
}

string RuleEdit::get_validator_value_from_pretty_name(string pretty_name)
{
    const list<Policies::validatorType> *existing_validator = mainwindow->providePolicyExistingValidator();
    list<Policies::validatorType>::const_iterator itValidator = existing_validator->begin();
    list<Policies::validatorType>::const_iterator iteValidator = existing_validator->end();
    for (; itValidator != iteValidator; ++itValidator)
    {
        if (itValidator->pretty_name == pretty_name)
        {
            return itValidator->value;
        }
    }
    return string();
}

string RuleEdit::get_validator_pretty_name_from_value(string value)
{
    const list<Policies::validatorType> *existing_validator = mainwindow->providePolicyExistingValidator();
    list<Policies::validatorType>::const_iterator itValidator = existing_validator->begin();
    list<Policies::validatorType>::const_iterator iteValidator = existing_validator->end();
    for (; itValidator != iteValidator; ++itValidator)
    {
        if (itValidator->value == value)
        {
            return itValidator->pretty_name;
        }
    }
    return string();
}

void RuleEdit::clear_editor_fields()
{
    ui->editorSelector->setChecked(true);
    ui->freeText->setText(QString());
    ui->assertName->setText(QString());
    ui->type->setCurrentIndex(0);
    ui->field->setCurrentIndex(0);
    ui->validator->setCurrentIndex(0);
    ui->value->setText(QString());
    ui->editorFrame->show();
    ui->freeText->hide();
}

void RuleEdit::fill_editor_fields(const Assert *r)
{
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
    pos = ui->validator->findText(QString().fromStdString(get_validator_pretty_name_from_value(r->validator)));
    if (pos != -1)
    {
        ui->validator->setCurrentIndex(pos);
    }

    string value = r->value;
    if (value.length() >= 2 && value[0] == '\'')
    {
        value = value.substr(1, value.length() - 2);
    }
    ui->value->setText(QString().fromStdString(value));
}

void RuleEdit::value_to_quotted_value(string& value)
{
    bool isNum = true;
    for (size_t i = 0; i < value.length(); ++i)
    {
        if ((value[i] > '9' || value[i] < '0') && value[i] != '.')
        {
            isNum = false;
            break;
        }
    }
    if (!isNum)
    {
        value = string("'") + value + string("'");
    }
}
