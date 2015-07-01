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
PoliciesEdit::PoliciesEdit(QWidget *parent) :
    QFrame(parent),
    mainwindow((MainWindow *)parent),
    ui(new Ui::PoliciesEdit)
{
    ui->setupUi(this);
    ui->errors->hide();
    ui->errors->setReadOnly(true);
    clear_editor_fields();
    add_values_to_selector();

    ui->deleteRule->setEnabled(false);
    QObject::connect(ui->newRule, SIGNAL(clicked()), this, SLOT(on_addNewRule()));
    QObject::connect(ui->rules, SIGNAL(itemSelectionChanged()),
                     this, SLOT(rule_selected_changed()));
    QObject::connect(ui->rules, SIGNAL(cellClicked(int, int)),
                     this, SLOT(cell_clicked(int, int)));
    QObject::connect(ui->deleteRule, SIGNAL(clicked()), this, SLOT(on_deleteRule()));
    QObject::connect(ui->freeTextSelector, SIGNAL(clicked()), this, SLOT(free_text_selected()));
    QObject::connect(ui->editorSelector, SIGNAL(clicked()), this, SLOT(editor_selected()));
    QObject::connect(ui->type, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(editRule_type()));
    QObject::connect(ui->field, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(editRule_field()));
    QObject::connect(ui->validator, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(editRule_validator()));
    QObject::connect(ui->value, SIGNAL(textEdited(QString)),
                     this, SLOT(editRule_value()));
    QObject::connect(ui->freeText, SIGNAL(textChanged()),
                     this, SLOT(editRule_freeText()));
    QObject::connect(ui->ruleName, SIGNAL(textEdited(QString)),
                     this, SLOT(editRule_ruleName()));
}

//---------------------------------------------------------------------------
PoliciesEdit::~PoliciesEdit()
{
    delete ui;
    for (size_t i = 0; i < rules.size(); ++i)
    {
        delete rules[i];
    }
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
    clear_editor_fields();
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

    Rule rule;
    Policies p;
    Rule *new_rule = NULL;
    if (r->use_free_text && p.try_parsing_test(r->text, &rule))
    {
        rule.description = r->description;
        new_rule = new Rule(rule);
    } else {
        new_rule = new Rule(*r);
    }
    rules.push_back(new_rule);
}

void PoliciesEdit::set_name(string& policyName)
{
    ui->name->setText(QString().fromStdString(policyName));
}

const QPushButton *PoliciesEdit::get_newRule_button() const
{
    return ui->newRule;
}

const QDialogButtonBox *PoliciesEdit::get_validation_button() const
{
    return ui->validation;
}

string PoliciesEdit::get_new_name() const
{
    return ui->name->text().toStdString();
}

const vector<Rule *>& PoliciesEdit::get_rules() const
{
    return rules;
}

//***************************************************************************
// Visual element
//***************************************************************************

//***************************************************************************
// Slots
//***************************************************************************
void PoliciesEdit::copy_visual_to_rule(Rule *r)
{
    r->description = ui->ruleName->text().toStdString();
    if (ui->freeTextSelector->isChecked())
    {
        r->use_free_text = true;
        r->text = ui->freeText->toPlainText().toStdString();
    } else
    {
        r->type = ui->type->currentText().toStdString();
        r->field = ui->field->currentText().toStdString();
        r->validator = get_validator_value_from_pretty_name(ui->validator->currentText().toStdString());
        string value = ui->value->text().toStdString();
        value_to_quotted_value(value);
        r->value = value;
        r->use_free_text = false;
    }
}

//---------------------------------------------------------------------------
void PoliciesEdit::on_addNewRule()
{
    Rule *r = new Rule;

    copy_visual_to_rule(r);
    if (!r->description.length())
    {
        add_error(__T("Name of the rule must be set"));
        show_errors();
        delete r;
        return;
    }
    add_rule(r);
    clear_editor_fields();
    delete r;
}

//---------------------------------------------------------------------------
void PoliciesEdit::on_deleteRule()
{
    QList<QTableWidgetItem *> list = ui->rules->selectedItems();

    while (!list.isEmpty())
    {
        QTableWidgetItem *item = list.first();
        rules.erase(rules.begin() + item->row());
        ui->rules->removeRow(item->row());
        list.removeFirst();
    }
}

//---------------------------------------------------------------------------
void PoliciesEdit::editRule_type()
{
    QTableWidgetItem *item = NULL;

    if ((item = getSelectedRuleItem()) == NULL)
    {
        return;
    }
    Rule *r = rules[item->row()];
    r->type = ui->type->currentText().toStdString();
    r->use_free_text = false;
}

//---------------------------------------------------------------------------
void PoliciesEdit::editRule_field()
{
    QTableWidgetItem *item = NULL;

    if ((item = getSelectedRuleItem()) == NULL)
    {
        return;
    }
    Rule *r = rules[item->row()];
    r->field = ui->field->currentText().toStdString();
    r->use_free_text = false;
}

//---------------------------------------------------------------------------
void PoliciesEdit::editRule_validator()
{
    QTableWidgetItem *item = NULL;

    if ((item = getSelectedRuleItem()) == NULL)
    {
        return;
    }
    Rule *r = rules[item->row()];
    r->validator = get_validator_value_from_pretty_name(ui->validator->currentText().toStdString());
    r->use_free_text = false;
}

//---------------------------------------------------------------------------
void PoliciesEdit::editRule_value()
{
    QTableWidgetItem *item = NULL;

    if ((item = getSelectedRuleItem()) == NULL)
    {
        return;
    }
    Rule *r = rules[item->row()];
    string value = ui->value->text().toStdString();
    value_to_quotted_value(value);
    r->value = value;
    r->use_free_text = false;
}

//---------------------------------------------------------------------------
void PoliciesEdit::editRule_freeText()
{
    QTableWidgetItem *item = NULL;

    if ((item = getSelectedRuleItem()) == NULL)
    {
        return;
    }
    Rule *r = rules[item->row()];
    r->text = ui->freeText->toPlainText().toStdString();
    r->use_free_text = true;
}

//---------------------------------------------------------------------------
void PoliciesEdit::editRule_ruleName()
{
    QTableWidgetItem *item = NULL;

    if ((item = getSelectedRuleItem()) == NULL)
    {
        return;
    }
    Rule *r = rules[item->row()];
    r->description = ui->ruleName->text().toStdString();
    item->setText(ui->ruleName->text());
}

void PoliciesEdit::rule_selected_changed()
{
    if (ui->rules->selectedItems().isEmpty())
    {
        ui->deleteRule->setEnabled(false);
        clear_editor_fields();
    } else {
        ui->deleteRule->setEnabled(true);
    }
}

//---------------------------------------------------------------------------
QTableWidgetItem* PoliciesEdit::getSelectedRuleItem()
{
    QList<QTableWidgetItem *> list = ui->rules->selectedItems();

    if (list.isEmpty())
    {
        return NULL;
    }

    return list.first();
}

//---------------------------------------------------------------------------
QString PoliciesEdit::getSelectedRuleName()
{

    QTableWidgetItem *item = NULL;

    if ((item = getSelectedRuleItem()) == NULL)
    {
        return QString();
    }

    return item->text();
}

//---------------------------------------------------------------------------
void PoliciesEdit::cell_clicked(int row, int column)
{
    QTableWidgetItem *item = ui->rules->item(row, column);
    if (item->row() == -1)
    {
        //Should not happened
        add_error(__T("Rule not found"));
        show_errors();
        return;
    }
    item->setSelected(true);

    const Rule *r = rules[item->row()];

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
void PoliciesEdit::free_text_selected()
{
    Rule r;

    r.type = ui->type->currentText().toStdString();
    r.field = ui->field->currentText().toStdString();
    r.validator = get_validator_value_from_pretty_name(ui->validator->currentText().toStdString());
    string value = ui->value->text().toStdString();
    value_to_quotted_value(value);
    r.value = value;
    r.use_free_text = false;

    Policies p;
    string ruleStr = p.serialize_rule_for_test(&r);
    ui->freeText->setText(QString().fromStdString(ruleStr));
    ui->freeText->show();
    ui->editorFrame->hide();
}

//---------------------------------------------------------------------------
void PoliciesEdit::editor_selected()
{
    Rule r;
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
        ui->validator->addItem(QString().fromStdString(itValidator->pretty_name));
    }
}

string PoliciesEdit::get_validator_value_from_pretty_name(string pretty_name)
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

string PoliciesEdit::get_validator_pretty_name_from_value(string value)
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

void PoliciesEdit::clear_editor_fields()
{
    ui->editorSelector->setChecked(true);
    ui->freeText->setText(QString());
    ui->ruleName->setText(QString());
    ui->type->setCurrentIndex(0);
    ui->field->setCurrentIndex(0);
    ui->validator->setCurrentIndex(0);
    ui->value->setText(QString());
    ui->editorFrame->show();
    ui->freeText->hide();
}

void PoliciesEdit::fill_editor_fields(const Rule *r)
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

void PoliciesEdit::value_to_quotted_value(string& value)
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
