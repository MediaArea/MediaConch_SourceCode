/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "Common/XsltPolicy.h"
#include "Common/GeneratedCSVVideos.h"
#include "xsltruleedit.h"
#include "ui_xsltruleedit.h"
#include "mainwindow.h"
#include <QSizePolicy>

#if QT_VERSION >= 0x050200
    #include <QFontDatabase>
#endif

namespace MediaConch {

//***************************************************************************
// CustomSpinBox
//***************************************************************************

XsltRuleEdit::CustomSpinBox::CustomSpinBox(QWidget *parent) : QSpinBox(parent)
{
}

XsltRuleEdit::CustomSpinBox::~CustomSpinBox()
{
}

//---------------------------------------------------------------------------
QString XsltRuleEdit::CustomSpinBox::textFromValue(int value) const
{
    if (value == -1)
        return "All";
    return QSpinBox::textFromValue(value);
}

//---------------------------------------------------------------------------
int XsltRuleEdit::CustomSpinBox::valueFromText(QString& text) const
{
    if (text == "all" || text == "All")
        return -1;
    return QSpinBox::valueFromText(text);
}

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
    ui->test->setText(QString());
    add_values_to_selector();
    change_occurence_spin_box();
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

    ui->name->setText(QString().fromUtf8(r->title.c_str(), r->title.length()));

    // Set validator (operation to do)
    if (r->ope == "")
        r->ope = ui->ope->currentText().toUtf8().data();
    int pos = ui->ope->findText(QString().fromUtf8(r->ope.c_str(), r->ope.length()));
    if (pos != -1)
        ui->ope->setCurrentIndex(pos);

    // Set mode
    if (r->use_free_text)
        ui->freeTextSelector->setChecked(true);
    else
        ui->editorSelector->setChecked(true);

    // Set fields corresponding to the mode
    fill_mode_frame_fields(r);
    ui->modeFrame->show();
    check_editor_is_possible(r);
}

//---------------------------------------------------------------------------
void XsltRuleEdit::fill_mode_frame_fields(XsltRule *r)
{
    if (r->ope == "exists" || r->ope == "does_not_exist")
        fill_mode_frame_exists_fields(r);
    else if (r->ope == "is_true" || r->ope == "is_not_true")
        fill_mode_frame_is_true_fields(r);
    else
        fill_mode_frame_common_fields(r);
}

//---------------------------------------------------------------------------
void XsltRuleEdit::fill_mode_frame_exists_fields(XsltRule *r)
{
    int pos;

    // Hide everything
    ui->frameType->hide();
    ui->frameOccurrence->hide();
    ui->frameTest->hide();
    ui->frameValue->hide();

    if (r->use_free_text)
    {
        // Display Free Text test
        ui->frameTest->show();
        ui->test->setText(QString().fromUtf8(r->test.c_str(), r->test.length()));
    }
    else
    {
        // Display type && Occurrence
        ui->frameType->show();
        ui->frameOccurrence->show();

        // Set type
        if (r->type == "")
            r->type = ui->type->currentText().toUtf8().data();
        pos = ui->type->findText(QString().fromUtf8(r->type.c_str(), r->type.length()));
        if (pos != -1)
            ui->type->setCurrentIndex(pos);

        if (r->type == "General")
        {
            r->occurrence = -1;
            ui->occurrence->setReadOnly(true);
        }
        ui->occurrence->setValue(r->occurrence);
    }

    //Updating field selector makes r->field reseting
    std::string remain(r->field);
    change_values_of_field_selector(r->use_free_text, r->field, "");
    r->field = remain;
}

//---------------------------------------------------------------------------
void XsltRuleEdit::fill_mode_frame_is_true_fields(XsltRule *r)
{
    // Force to free text
    if (!r->use_free_text)
    {
        r->use_free_text = true;
        ui->freeTextSelector->setChecked(true);
    }

    // Hide everything
    ui->frameType->hide();
    ui->frameOccurrence->hide();
    ui->frameValue->hide();

    // Except Value && Field
    ui->frameTest->show();
    ui->test->setText(QString().fromUtf8(r->test.c_str(), r->test.length()));
    ui->frameField->show();

    change_values_of_field_selector(r->use_free_text, r->field, "");
}

//---------------------------------------------------------------------------
void XsltRuleEdit::fill_mode_frame_common_fields(XsltRule *r)
{
    int pos;

    // Hide everything
    ui->frameType->hide();
    ui->frameField->show();
    ui->frameOccurrence->hide();
    ui->frameTest->hide();
    ui->frameValue->show();

    if (r->use_free_text)
    {
        // Display Free Text test
        ui->frameTest->show();
        ui->test->setText(QString().fromUtf8(r->test.c_str(), r->test.length()));
    }
    else
    {
        // Display type && occurrence
        ui->frameType->show();
        ui->frameOccurrence->show();

        // Set type
        if (r->type == "")
            r->type = ui->type->currentText().toUtf8().data();
        pos = ui->type->findText(QString().fromUtf8(r->type.c_str(), r->type.length()));
        if (pos != -1)
            ui->type->setCurrentIndex(pos);

        if (r->type == "General")
        {
            r->occurrence = -1;
            ui->occurrence->setReadOnly(true);
        }
        ui->occurrence->setValue(r->occurrence);
    }

    std::string remain(r->field);
    std::string rvalue(r->value);

    change_values_of_field_selector(r->use_free_text, remain, rvalue);

    //Updating field selector makes r->field reseting
    r->field = remain;
    //Updating value selector makes r->value reseting
    r->value = rvalue;
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
QComboBox *XsltRuleEdit::get_value_select()
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
    return ui->test;
}

//---------------------------------------------------------------------------
QFrame *XsltRuleEdit::get_editor_frame()
{
    return ui->modeFrame;
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
        ui->type->addItem(QString().fromUtf8(itType->first.c_str(), itType->first.length()));
    ui->type->model()->sort(0);
    change_values_of_field_selector(true, "", "");

    const list<string> *existing_operator = mainwindow->providePolicyExistingXsltOperator();
    list<string>::const_iterator itOperator = existing_operator->begin();
    list<string>::const_iterator iteOperator = existing_operator->end();
    for (; itOperator != iteOperator; ++itOperator)
        ui->ope->addItem(QString().fromUtf8(itOperator->c_str(), itOperator->length()));
}

//---------------------------------------------------------------------------
void XsltRuleEdit::change_values_of_field_selector(bool is_free_text, const std::string& new_field, const std::string& new_value)
{
    std::string new_field_remain(new_field);
    ui->field->clear();
    if (!is_free_text)
    {
        std::string type = ui->type->currentText().toUtf8().data();
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
                ui->field->addItem(QString().fromUtf8(it->c_str(), it->length()));
        }
    }
    ui->field->model()->sort(0);

    if (!new_field_remain.length())
        return;

    int pos = ui->field->findText(QString().fromUtf8(new_field_remain.c_str(), new_field_remain.length()));
    if (pos != -1)
        ui->field->setCurrentIndex(pos);
    else
    {
        ui->field->addItem(QString().fromUtf8(new_field_remain.c_str(), new_field_remain.length()));
        int pos = ui->field->findText(QString().fromUtf8(new_field_remain.c_str(), new_field_remain.length()));
        if (pos != -1)
            ui->field->setCurrentIndex(pos);
    }

    if (!is_free_text)
        change_values_of_value_selector(ui->type->currentText().toUtf8().data(),
                                        ui->field->currentText().toUtf8().data(),
                                        new_value);
}

//---------------------------------------------------------------------------
void XsltRuleEdit::change_values_of_value_selector(const std::string& type,
                                                   const std::string& field,
                                                   const std::string& new_value)
{
    std::string val(new_value);
    if (val.length() >= 2 && val[0] == '\'')
        val = val.substr(1, val.length() - 2);

    ui->value->clear();
    if (field.length())
    {
        std::map<std::string, std::map<std::string, std::vector<std::string> > > values;
        if (get_generated_values_from_csv(values) < 0)
            return;

        if (values.find(type) != values.end())
        {
            if (values[type].find(field) != values[type].end())
            {
                for (size_t i = 0; i < values[type][field].size(); ++i)
                    ui->value->addItem(QString().fromUtf8(values[type][field][i].c_str(), values[type][field][i].length()));
            }
        }
    }

    int pos = ui->value->findText(QString().fromUtf8(val.c_str(), val.length()));
    if (pos != -1)
        ui->value->setCurrentIndex(pos);
    else
    {
        ui->value->addItem(QString().fromUtf8(val.c_str(), val.length()));
        int pos = ui->value->findText(QString().fromUtf8(val.c_str(), val.length()));
        if (pos != -1)
            ui->value->setCurrentIndex(pos);
    }
}

//---------------------------------------------------------------------------
void XsltRuleEdit::change_occurence_spin_box()
{
    delete ui->occurrence;
    ui->occurrence = new CustomSpinBox(ui->frameOccurrence);
    ui->occurrence->setObjectName("occurrence");
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(ui->occurrence->sizePolicy().hasHeightForWidth());
    ui->occurrence->setSizePolicy(sizePolicy);
    ui->occurrence->setMinimum(-1);
    ui->occurrence->setMaximum(16777215);
    ui->occurrence->setValue(1);
    ui->horizontalLayout_3->addWidget(ui->occurrence);
}

//---------------------------------------------------------------------------
void XsltRuleEdit::check_editor_is_possible(XsltRule* r)
{
    if (r->ope == "is_true" || r->ope == "is_not_true")
    {
        ui->editorSelector->setDisabled(true);
        return;
    }

    if (!r->use_free_text)
    {
        ui->editorSelector->setEnabled(true);
        return;
    }

    XsltPolicy p(false);
    XsltRule* rule = new XsltRule(*r);
    rule->use_free_text = false;
    if (r->test.length() && (!p.parse_test_for_rule(r->test, rule) || rule->use_free_text))
        ui->editorSelector->setDisabled(true);
    else
        ui->editorSelector->setEnabled(true);

    delete rule;
}

}
