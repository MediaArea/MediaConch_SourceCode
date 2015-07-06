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
    add_values_to_selector();
    clear();

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
}

//***************************************************************************
// Functions
//***************************************************************************

//---------------------------------------------------------------------------
void RuleEdit::clear()
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

//---------------------------------------------------------------------------
void RuleEdit::assert_clicked(Assert *a)
{
    if (!a)
        return;

    ui->assertName->setText(QString().fromStdString(a->description));
    if (a->use_free_text)
    {
        ui->freeText->setText(QString().fromStdString(a->text));
        ui->freeTextSelector->setChecked(true);
        ui->freeText->show();
        ui->editorFrame->hide();
        return;
    }
    fill_editor_fields(a);

    ui->editorSelector->setChecked(true);
    ui->editorFrame->show();
    ui->freeText->hide();
}

//***************************************************************************
// Visual element
//***************************************************************************

//---------------------------------------------------------------------------
const QPushButton *RuleEdit::get_delAssert_button()
{
    return ui->deleteAssert;
}

//---------------------------------------------------------------------------
const QLineEdit *RuleEdit::get_assertName_line()
{
    return ui->assertName;
}

//***************************************************************************
// Slots
//***************************************************************************

//---------------------------------------------------------------------------
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
void RuleEdit::editAssert_type()
{
    // QTableWidgetItem *item = NULL;

    // if ((item = getSelectedAssertItem()) == NULL)
    //     return;
    // Assert *r = asserts[item->row()];
    // r->type = ui->type->currentText().toStdString();
    // r->use_free_text = false;
}

//---------------------------------------------------------------------------
void RuleEdit::editAssert_field()
{
    // QTableWidgetItem *item = NULL;

    // if ((item = getSelectedAssertItem()) == NULL)
    //     return;
    // Assert *r = asserts[item->row()];
    // r->field = ui->field->currentText().toStdString();
    // r->use_free_text = false;
}

//---------------------------------------------------------------------------
void RuleEdit::editAssert_validator()
{
    // QTableWidgetItem *item = NULL;

    // if ((item = getSelectedAssertItem()) == NULL)
    //     return;
    // Assert *r = asserts[item->row()];
    // r->validator = get_validator_value_from_pretty_name(ui->validator->currentText().toStdString());
    // r->use_free_text = false;
}

//---------------------------------------------------------------------------
void RuleEdit::editAssert_value()
{
    // QTableWidgetItem *item = NULL;

    // if ((item = getSelectedAssertItem()) == NULL)
    //     return;
    // Assert *r = asserts[item->row()];
    // string value = ui->value->text().toStdString();
    // value_to_quotted_value(value);
    // r->value = value;
    // r->use_free_text = false;
}

//---------------------------------------------------------------------------
void RuleEdit::editAssert_freeText()
{
    // QTableWidgetItem *item = NULL;

    // if ((item = getSelectedAssertItem()) == NULL)
    // {
    //     return;
    // }
    // Assert *r = asserts[item->row()];
    // r->text = ui->freeText->toPlainText().toStdString();
    // r->use_free_text = true;
}

//---------------------------------------------------------------------------
void RuleEdit::editAssert_assertName()
{
    // QTableWidgetItem *item = NULL;

    // if ((item = getSelectedAssertItem()) == NULL)
    // {
    //     return;
    // }
    // Assert *r = asserts[item->row()];
    // r->description = ui->assertName->text().toStdString();
    // item->setText(ui->assertName->text());
}

//---------------------------------------------------------------------------
void RuleEdit::free_text_selected()
{
    // Assert a;

    // a.type = ui->type->currentText().toStdString();
    // a.field = ui->field->currentText().toStdString();
    // a.validator = get_validator_value_from_pretty_name(ui->validator->currentText().toStdString());
    // string value = ui->value->text().toStdString();
    // value_to_quotted_value(value);
    // a.value = value;
    // a.use_free_text = false;

    // Policies p;
    // string assertStr = p.serialize_assert_for_test(&a);
    // ui->freeText->setText(QString().fromStdString(assertStr));
    // ui->freeText->show();
    // ui->editorFrame->hide();
}

//---------------------------------------------------------------------------
void RuleEdit::editor_selected()
{
    // Assert r;
    // Policies p;
    // if (p.try_parsing_test(ui->freeText->toPlainText().toStdString(), &r))
    // {
    //     fill_editor_fields(&r);
    // } else
    // {
    //     ui->type->setCurrentIndex(0);
    //     ui->field->setCurrentIndex(0);
    //     ui->validator->setCurrentIndex(0);
    //     ui->value->setText(QString());
    // }
    // ui->editorFrame->show();
    // ui->freeText->hide();
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

void RuleEdit::fill_editor_fields(const Assert *r)
{
    int pos = ui->type->findText(QString().fromStdString(r->type));
    if (pos != -1)
        ui->type->setCurrentIndex(pos);

    pos = ui->field->findText(QString().fromStdString(r->field));
    if (pos != -1)
        ui->field->setCurrentIndex(pos);

    pos = ui->validator->findText(QString().fromStdString(get_validator_pretty_name_from_value(r->validator)));
    if (pos != -1)
        ui->validator->setCurrentIndex(pos);

    string value = r->value;
    if (value.length() >= 2 && value[0] == '\'')
        value = value.substr(1, value.length() - 2);
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
