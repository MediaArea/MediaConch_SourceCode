/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "Common/XsltPolicy.h"
#include "xsltruleedit.h"
#include "ui_xsltruleedit.h"
#include "mainwindow.h"

#if QT_VERSION >= 0x050200
    #include <QFontDatabase>
#endif

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

//---------------------------------------------------------------------------
XsltRuleEdit::XsltRuleEdit(QWidget *parent) :
    QFrame(parent),
    mainwindow((MainWindow *)parent),
    ui(new Ui::XsltRuleEdit)
{
    ui->setupUi(this);
    ui->freeText->setText(QString());
    ui->freeText->hide();
    add_values_to_selector();
}

//---------------------------------------------------------------------------
XsltRuleEdit::~XsltRuleEdit()
{
    delete ui;
}

//***************************************************************************
// Visual element
//***************************************************************************

//---------------------------------------------------------------------------
void XsltRuleEdit::rule_clicked(XsltRule *r)
{
    if (!r)
        return;

    ui->name->setText(QString().fromStdString(r->title));
    if (r->use_free_text)
    {
        ui->freeText->setText(QString().fromStdString(r->text));
        ui->freeTextSelector->setChecked(true);
        ui->freeText->show();
        ui->editorFrame->hide();
        return;
    }
    if (r->type == "")
        r->type = ui->type->currentText().toStdString();
    if (r->ope == "")
        r->ope = ui->ope->currentText().toStdString();

    fill_editor_fields(r);

    ui->editorSelector->setChecked(true);
    ui->freeText->hide();
    ui->editorFrame->show();
}

//---------------------------------------------------------------------------
void XsltRuleEdit::fill_editor_fields(XsltRule *r)
{
    int pos = ui->type->findText(QString().fromStdString(r->type));
    if (pos != -1)
        ui->type->setCurrentIndex(pos);

    //Updating field selector makes r->field reseting
    std::string remain(r->field);
    change_values_of_field_selector();
    r->field = remain;

    pos = ui->field->findText(QString().fromStdString(r->field));
    if (pos != -1)
        ui->field->setCurrentIndex(pos);

    ui->occurrence->setValue(r->occurrence);
    pos = ui->ope->findText(QString().fromStdString(r->ope));
    if (pos != -1)
        ui->ope->setCurrentIndex(pos);

    string value = r->value;
    if (value.length() >= 2 && value[0] == '\'')
        value = value.substr(1, value.length() - 2);
    ui->value->setText(QString().fromStdString(value));
}

//---------------------------------------------------------------------------
const QPushButton *XsltRuleEdit::get_delRule_button()
{
    return ui->deleteRule;
}

//---------------------------------------------------------------------------
const QPushButton *XsltRuleEdit::get_duplicateRule_button()
{
    return ui->duplicateRule;
}

//---------------------------------------------------------------------------
const QLineEdit *XsltRuleEdit::get_name_line()
{
    return ui->name;
}

//---------------------------------------------------------------------------
QComboBox *XsltRuleEdit::get_type_select()
{
    return ui->type;
}

//---------------------------------------------------------------------------
QComboBox *XsltRuleEdit::get_field_select()
{
    return ui->field;
}

//---------------------------------------------------------------------------
QComboBox *XsltRuleEdit::get_operator_select()
{
    return ui->ope;
}

//---------------------------------------------------------------------------
QLineEdit *XsltRuleEdit::get_value_line()
{
    return ui->value;
}

//---------------------------------------------------------------------------
QSpinBox *XsltRuleEdit::get_occurrence_box()
{
    return ui->occurrence;
}

//---------------------------------------------------------------------------
QTextEdit *XsltRuleEdit::get_freeText_text()
{
    return ui->freeText;
}

//---------------------------------------------------------------------------
QFrame *XsltRuleEdit::get_editor_frame()
{
    return ui->editorFrame;
}

QRadioButton *XsltRuleEdit::get_freeTextSelector_radio()
{
    return ui->freeTextSelector;
}

QRadioButton *XsltRuleEdit::get_editorSelector_radio()
{
    return ui->editorSelector;
}

//***************************************************************************
// HELPER
//***************************************************************************

//---------------------------------------------------------------------------
void XsltRuleEdit::add_values_to_selector()
{
    const map<string, list<string> > *existing_type = mainwindow->providePolicyExistingType();
    map<string, list<string> >::const_iterator itType = existing_type->begin();
    map<string, list<string> >::const_iterator iteType = existing_type->end();
    for (; itType != iteType; ++itType)
        ui->type->addItem(QString().fromStdString(itType->first));
    ui->type->model()->sort(0);
    change_values_of_field_selector();

    const list<string> *existing_operator = mainwindow->providePolicyExistingXsltOperator();
    list<string>::const_iterator itOperator = existing_operator->begin();
    list<string>::const_iterator iteOperator = existing_operator->end();
    for (; itOperator != iteOperator; ++itOperator)
        ui->ope->addItem(QString().fromStdString(*itOperator));
}

//---------------------------------------------------------------------------
void XsltRuleEdit::change_values_of_field_selector()
{
    std::string type = ui->type->currentText().toStdString();
    ui->field->clear();

    const map<string, list<string> > *existing_type = mainwindow->providePolicyExistingType();
    map<string, list<string> >::const_iterator itType = existing_type->begin();
    map<string, list<string> >::const_iterator iteType = existing_type->end();
    for (; itType != iteType; ++itType)
    {
        if (itType->first.compare(type))
            continue;

        list<string>::const_iterator it = itType->second.begin();
        list<string>::const_iterator ite = itType->second.end();
        for (; it != ite; ++it)
            ui->field->addItem(QString().fromStdString(*it));
    }

    ui->field->model()->sort(0);
}

}
