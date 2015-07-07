/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "policiestree.h"
#include "policiesmenu.h"
#include "policymenu.h"
#include "groupofrules.h"
#include "rulemenu.h"
#include "ruleedit.h"

#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QActionGroup>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QLabel>
#include <QUrl>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>
#include <QRadioButton>
#if QT_VERSION >= 0x050200
    #include <QFontDatabase>
#endif
#include <sstream>

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Groups
    QActionGroup* ToolGroup = new QActionGroup(this);
    ToolGroup->addAction(ui->actionConch);
    ToolGroup->addAction(ui->actionInfo);
    ToolGroup->addAction(ui->actionTrace);
    ToolGroup->addAction(ui->actionSchematron);
    ToolGroup->addAction(ui->actionPolicies);
    QActionGroup* FormatGroup = new QActionGroup(this);
    FormatGroup->addAction(ui->actionText);
    FormatGroup->addAction(ui->actionXml);
    
    // Visual elements
    Layout=new QVBoxLayout(this);
    ui->centralWidget->setLayout(Layout);
    MainText=NULL;
    DragDrop_Image=NULL;
    DragDrop_Text=NULL;
    policiesTree = NULL;
    policiesMenu = NULL;
    policyMenu = NULL;
    groupOfRules = NULL;
    ruleMenu = NULL;
    ruleEdit = NULL;

    // Drag n drop
    setAcceptDrops(true);

    // Window
    setWindowIcon(QIcon(":/icon/icon.png"));
    move(QApplication::desktop()->screenGeometry().width()/4, QApplication::desktop()->screenGeometry().height()/4);
    resize(QApplication::desktop()->screenGeometry().width()/2, QApplication::desktop()->screenGeometry().height()/2);

    // Default
    on_actionConch_triggered();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//***************************************************************************
// Functions
//***************************************************************************

//---------------------------------------------------------------------------
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

//---------------------------------------------------------------------------
void MainWindow::dropEvent(QDropEvent *Event)
{
    if (Event->mimeData()->hasUrls())
    {
        QList<QUrl> urls=Event->mimeData()->urls();

        C.List.clear();
        for (int Pos=0; Pos<urls.size(); Pos++)
            C.List.push_back(urls[Pos].toLocalFile().toStdWString());
    }

    Run();
}

void MainWindow::policy_to_delete(int index)
{
    //Delete policy
    C.policies.erase_policy(index);
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
void MainWindow::Run()
{
    if (C.Tool == Core::tool_MediaPolicies)
    {
        displayPoliciesTree();
        return;
    }

    if (C.List.empty())
    {
        createDragDrop();
        return;
    }
    createMainText();

    MainText->setPlainText(QString().fromStdWString(C.Run().c_str()));
}

//---------------------------------------------------------------------------
QString MainWindow::ask_for_schematron_file()
{
    QString file=QFileDialog::getOpenFileName(this, "Open file", "", "Schematron file (*.sch);;All (*.*)", 0, QFileDialog::DontUseNativeDialog);
    return file;
}

void MainWindow::exporting_to_schematron_file(int pos)
{
    //TODO: -1: disable save or save all?
    if (pos < 0)
        return;
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Policy"),
                                                    "", tr("Schematron (*.sch)"));

    C.policies.export_schematron(filename.toStdString().c_str(), pos);
}

//***************************************************************************
// Slots
//***************************************************************************

//---------------------------------------------------------------------------
void MainWindow::on_actionOpen_triggered()
{
    QStringList List=QFileDialog::getOpenFileNames(this, "Open file", "", "Video files (*.avi *.mkv *.mov *.mxf *.mp4);;All (*.*)", 0, QFileDialog::DontUseNativeDialog);
    if (List.empty())
        return;

    C.List.clear();
    for (int Pos=0; Pos<List.size(); Pos++)
        C.List.push_back(List[Pos].toStdWString());

    Run();
}

//---------------------------------------------------------------------------
void MainWindow::on_actionCloseAll_triggered()
{
    C.List.clear();

    Run();
}

//---------------------------------------------------------------------------
void MainWindow::on_actionSavePolicies_triggered()
{
    exporting_to_schematron_file(-1);
}

//---------------------------------------------------------------------------
void MainWindow::on_actionConch_triggered()
{
    ui->menuFormat->setEnabled(false);
    ui->actionText->setEnabled(false);
    ui->actionXml->setEnabled(false);
    ui->actionText->setChecked(true);

    C.Tool=Core::tool_MediaConch;
    C.Format=Core::format_Text;
    Run();
}

//---------------------------------------------------------------------------
void MainWindow::on_actionInfo_triggered()
{
    ui->menuFormat->setEnabled(true);
    ui->actionText->setEnabled(true);
    ui->actionXml->setEnabled(true);

    C.Tool=Core::tool_MediaInfo;
    Run();
}

//---------------------------------------------------------------------------
void MainWindow::on_actionTrace_triggered()
{
    ui->menuFormat->setEnabled(true);
    ui->actionText->setEnabled(true);
    ui->actionXml->setEnabled(true);

    C.Tool=Core::tool_MediaTrace;
    Run();
}

//---------------------------------------------------------------------------
void MainWindow::on_actionSchematron_triggered()
{
    if (!C.policies.policies.size() && !C.SchematronFiles.size())
    {
        QString file = ask_for_schematron_file();
        if (file.length())
        {
            if (C.policies.import_schematron(file.toStdString().c_str()).length())
            {
                C.SchematronFiles.push_back(file.toStdWString());
            }
        }
    }
    ui->menuFormat->setEnabled(true);
    ui->actionText->setEnabled(false);
    ui->actionXml->setEnabled(true);
    ui->actionXml->setChecked(true);

    C.Tool=Core::tool_MediaSchematron;
    Run();
}

//---------------------------------------------------------------------------
void MainWindow::on_actionPolicies_triggered()
{
    ui->menuFormat->setEnabled(true);
    ui->actionText->setEnabled(false);
    ui->actionXml->setEnabled(true);
    ui->actionXml->setChecked(true);

    C.Tool=Core::tool_MediaPolicies;
    Run();
}

//---------------------------------------------------------------------------
void MainWindow::on_actionText_triggered()
{
    C.Format=Core::format_Text;
    Run();
}

//---------------------------------------------------------------------------
void MainWindow::on_actionXml_triggered()
{
    C.Format=Core::format_Xml;
    Run();
}

//---------------------------------------------------------------------------
void MainWindow::on_actionChooseSchematron_triggered()
{
    QString file = ask_for_schematron_file();
    if (file.length())
    {
        if (C.policies.import_schematron(file.toStdString().c_str()).length())
        {
            C.SchematronFiles.push_back(file.toStdWString());
        }
    }
    Run();
}

//---------------------------------------------------------------------------
void MainWindow::on_importSchematron()
{
    QString file = ask_for_schematron_file();
    String ret = C.policies.import_schematron(file.toStdString().c_str());

    displayPoliciesTree();
    if (ret.length())
        policiesTree->get_error_line()->setText(QString().fromStdWString(ret));
    else
    {
        int row = C.policies.policies.size() - 1;
        QTreeWidgetItem* parent = policiesTree->get_policies_tree()->topLevelItem(0);
        if (row < 0 || !parent)
            return;

        QTreeWidgetItem *item = parent->child(row);
        if (!item)
            return;

        parent->setExpanded(true);
        parent->setSelected(false);
        item->setSelected(true);
    }
}

//---------------------------------------------------------------------------
void MainWindow::on_exportSchematron()
{
    int  row = get_index_in_tree();

    if (row < 0)
        return;
    exporting_to_schematron_file(row);
    Run();
}

//---------------------------------------------------------------------------
void MainWindow::add_new_policy()
{
    QTreeWidget *tree = policiesTree->get_policies_tree();
    QTreeWidgetItem* parent = tree->topLevelItem(0);
    if (!parent)
        return;

    Policy *p = new Policy;

    p->title = string("New policy");

    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    QString title = QString().fromStdString(p->title);
    item->setText(0, title);

    C.policies.policies.push_back(p);
    displayPolicyMenu(title);
    parent->setExpanded(true);
    parent->setSelected(false);
    item->setSelected(true);
}

//---------------------------------------------------------------------------
void MainWindow::duplicate_policy()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item)
        return;

    int row = get_index_in_tree();
    if (row < 0)
        return;

    Policy *p = new Policy(*C.policies.policies[row]);

    p->title = p->title + string(" (Copy)");

    C.policies.policies.push_back(p);
    QTreeWidgetItem* parent = item->parent();
    if (!parent)
        return;

    QTreeWidgetItem* new_item = new QTreeWidgetItem(parent);
    item->setSelected(false);
    new_item->setSelected(true);
    QString title = QString().fromStdString(p->title);
    new_item->setText(0, title);

    for (size_t i = 0; i < p->patterns.size(); ++i)
        updatePoliciesTreePattern(p->patterns[i], new_item);
    displayPolicyMenu(title);
}

//---------------------------------------------------------------------------
void MainWindow::add_new_gor()
{
    QTreeWidgetItem* parent = get_item_in_tree();
    if (!parent)
        return;
    int rowPolicy = get_index_of_item_backXX(parent, 0);
    if (rowPolicy < 0)
        return;

    Pattern *p = new Pattern;

    p->name = string("New group of rules");

    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    QString name = QString().fromStdString(p->name);
    item->setText(0, name);

    C.policies.policies[rowPolicy]->patterns.push_back(p);
    displayGroupOfRules(name);
    parent->setExpanded(true);
    parent->setSelected(false);
    item->setSelected(true);
}

//---------------------------------------------------------------------------
void MainWindow::duplicate_gor()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item)
        return;

    int rowPolicy = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (rowPolicy < 0 || row < 0)
        return;

    Policy *p = C.policies.policies[rowPolicy];
    Pattern *pat = new Pattern(*p->patterns[row]);

    pat->name = pat->name + string(" (Copy)");
    p->patterns.push_back(pat);

    QTreeWidgetItem* parent = item->parent();
    if (!parent)
        return;

    QTreeWidgetItem* new_item = new QTreeWidgetItem(parent);
    item->setSelected(false);
    new_item->setSelected(true);
    QString title = QString().fromStdString(pat->name);
    new_item->setText(0, title);

    for (size_t i = 0; i < pat->rules.size(); ++i)
        updatePoliciesTreeRule(pat->rules[i], new_item);
    displayGroupOfRules(title);
}

//---------------------------------------------------------------------------
void MainWindow::add_new_rule()
{
    QTreeWidgetItem* parent = get_item_in_tree();
    if (!parent)
        return;
    int rowPolicy = get_index_of_item_backXX(parent, 1);
    int rowGor = get_index_of_item_backXX(parent, 0);
    if (rowPolicy < 0 || rowGor < 0)
        return;

    Rule *r = new Rule;

    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    item->setText(0, QString("Rule"));

    C.policies.policies[rowPolicy]->patterns[rowGor]->rules.push_back(r);
    displayRuleMenu();
    parent->setExpanded(true);
    parent->setSelected(false);
    item->setSelected(true);
}

//---------------------------------------------------------------------------
void MainWindow::duplicate_rule()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item)
        return;

    int rowPolicy = get_index_of_item_backXX(item, 2);
    int rowPattern = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (rowPattern < 0 || rowPolicy < 0 || row < 0)
        return;

    Pattern *p = C.policies.policies[rowPolicy]->patterns[rowPattern];
    Rule *r = new Rule(*p->rules[row]);

    p->rules.push_back(r);

    QTreeWidgetItem* parent = item->parent();
    if (!parent)
        return;

    QTreeWidgetItem* new_item = new QTreeWidgetItem(parent);
    new_item->setText(0, QString("Rule"));
    item->setSelected(false);
    new_item->setSelected(true);

    for (size_t i = 0; i < r->asserts.size(); ++i)
    {
        Assert *assert = r->asserts[i];
        if (!assert)
            continue;

        QTreeWidgetItem* a = new QTreeWidgetItem(new_item);
        QString descr = QString().fromStdString(assert->description);
        a->setText(0, descr);
    }
    displayRuleMenu();
}

//---------------------------------------------------------------------------
void MainWindow::add_new_assert()
{
    QTreeWidgetItem* parent = get_item_in_tree();
    if (!parent)
        return;
    int rowPolicy = get_index_of_item_backXX(parent, 2);
    int rowGor = get_index_of_item_backXX(parent, 1);
    int rowRule = get_index_of_item_backXX(parent, 0);
    if (rowPolicy < 0 || rowGor < 0 || rowRule < 0)
        return;

    Assert *a = new Assert;

    a->description = string("New Assert");
    a->use_free_text = false;
    QString name = QString().fromStdString(a->description);
    QTreeWidgetItem* item = new QTreeWidgetItem(parent, QStringList(name));

    C.policies.policies[rowPolicy]->patterns[rowGor]->rules[rowRule]->asserts.push_back(a);
    parent->setExpanded(true);
    parent->setSelected(false);
    item->setSelected(true);
    displayRuleEdit(item);
}

//---------------------------------------------------------------------------
void MainWindow::delete_all_policies()
{
    QTreeWidget *tree = policiesTree->get_policies_tree();
    QTreeWidgetItem* policies = tree->topLevelItem(0);
    if (!policies)
        return;
    removeTreeChildren(policies);
    for (size_t i = 0; i < C.policies.policies.size(); ++i)
        delete C.policies.policies[i];
    C.policies.policies.clear();
}

//---------------------------------------------------------------------------
void MainWindow::delete_policy()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int row = get_index_in_tree();
    if (row < 0)
        return;

    // Internal data
    Policy *p = C.policies.policies[row];
    for (size_t i = 0; i < p->patterns.size(); ++i)
        delete p->patterns[i];
    p->patterns.clear();
    C.policies.policies.erase(C.policies.policies.begin() + row);

    // Visual
    removeTreeChildren(item);
    QTreeWidgetItem* parent = item->parent();
    delete parent->takeChild(row);
    item = get_item_in_tree();
    if (item)
        item->setSelected(false);
    parent->setSelected(true);
    displayPoliciesMenu();
}

//---------------------------------------------------------------------------
void MainWindow::delete_gor()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int row = get_index_in_tree();
    int rowPolicy = get_index_of_item_backXX(item, 1);
    if (rowPolicy < 0 || row < 0)
        return;

    // Internal data
    Policy *p = C.policies.policies[rowPolicy];
    Pattern *pat = p->patterns[row];
    for (size_t i = 0; i < pat->rules.size(); ++i)
        delete pat->rules[i];
    pat->rules.clear();
    p->patterns.erase(p->patterns.begin() + row);

    // Visual
    removeTreeChildren(item);
    QTreeWidgetItem* parent = item->parent();
    delete parent->takeChild(row);
    item = get_item_in_tree();
    if (item)
        item->setSelected(false);
    parent->setSelected(true);
    displayPolicyMenu(parent->text(0));
}

//---------------------------------------------------------------------------
void MainWindow::delete_rule()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = get_index_of_item_backXX(item, 2);
    int rowGor = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (rowPolicy < 0 || rowGor < 0 || row < 0)
        return;

    // Internal data
    Pattern *p = C.policies.policies[rowPolicy]->patterns[rowGor];
    Rule *r = p->rules[row];
    for (size_t i = 0; i < r->asserts.size(); ++i)
        delete r->asserts[i];
    r->asserts.clear();
    p->rules.erase(p->rules.begin() + row);

    // Visual
    removeTreeChildren(item);
    QTreeWidgetItem* parent = item->parent();
    delete parent->takeChild(row);
    item = get_item_in_tree();
    if (item)
        item->setSelected(false);
    parent->setSelected(true);
    displayGroupOfRules(parent->text(0));
}

//---------------------------------------------------------------------------
void MainWindow::delete_assert()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = get_index_of_item_backXX(item, 3);
    int rowGor = get_index_of_item_backXX(item, 2);
    int rowRule = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (rowPolicy < 0 || rowGor < 0 || rowRule < 0 || row < 0)
        return;

    // Internal data
    Rule *r = C.policies.policies[rowPolicy]->patterns[rowGor]->rules[rowRule];
    Assert *a = r->asserts[row];
    r->asserts.erase(r->asserts.begin() + row);
    delete a;

    // Visual
    QTreeWidgetItem* parent = item->parent();
    delete parent->takeChild(row);
    item = get_item_in_tree();
    if (item)
        item->setSelected(false);
    parent->setSelected(true);
    displayRuleMenu();
}

//---------------------------------------------------------------------------
void MainWindow::edit_policy_title()
{
    QString qtitle = policyMenu->get_title_line()->text();
    if (!qtitle.length())
    {
        policiesTree->get_error_line()->setText(QString("Policy must have a title"));
        return;
    }
    policiesTree->get_error_line()->setText(QString());

    QTreeWidgetItem* item = get_item_in_tree();
    if (!item)
        return;

    int row = get_index_in_tree();;
    if (row < 0)
        return;

    Policy *p = C.policies.policies[row];
    p->title = qtitle.toStdString();

    QString title = QString().fromStdString(p->title);
    item->setText(0, title);
}

//---------------------------------------------------------------------------
void MainWindow::edit_gor_title()
{
    QString qname = groupOfRules->get_title_line()->text();
    if (!qname.length())
    {
        policiesTree->get_error_line()->setText(QString("Group of rules must have a title"));
        return;
    }
    policiesTree->get_error_line()->setText(QString());

    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int row = get_index_in_tree();
    int rowPolicy = get_index_of_item_backXX(item, 1);
    if (row < 0 || rowPolicy < 0)
        return;

    Pattern *p = C.policies.policies[rowPolicy]->patterns[row];
    p->name = qname.toStdString();

    QString title = QString().fromStdString(p->name);
    item->setText(0, title);
}

//---------------------------------------------------------------------------
void MainWindow::edit_assert_name(QString new_name)
{
    if (!new_name.length())
    {
        policiesTree->get_error_line()->setText(QString("Assert must have a name"));
        return;
    }
    policiesTree->get_error_line()->setText(QString());

    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = get_index_of_item_backXX(item, 3);
    int rowPattern = get_index_of_item_backXX(item, 2);
    int rowRule = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (row < 0 || rowPolicy < 0 || rowPattern < 0 || rowRule < 0)
        return;

    Assert *a = C.policies.policies[rowPolicy]->patterns[rowPattern]->rules[rowRule]->asserts[row];
    a->description = new_name.toStdString();

    QString name = QString().fromStdString(a->description);
    item->setText(0, name);
}

//---------------------------------------------------------------------------
void MainWindow::edit_assert_type()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = get_index_of_item_backXX(item, 3);
    int rowPattern = get_index_of_item_backXX(item, 2);
    int rowRule = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (row < 0 || rowPolicy < 0 || rowPattern < 0 || rowRule < 0)
        return;

    Assert *a = C.policies.policies[rowPolicy]->patterns[rowPattern]->rules[rowRule]->asserts[row];

    a->type = ruleEdit->get_type_select()->currentText().toStdString();
    a->use_free_text = false;
}

//---------------------------------------------------------------------------
void MainWindow::edit_assert_field()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = get_index_of_item_backXX(item, 3);
    int rowPattern = get_index_of_item_backXX(item, 2);
    int rowRule = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (row < 0 || rowPolicy < 0 || rowPattern < 0 || rowRule < 0)
        return;

    Assert *a = C.policies.policies[rowPolicy]->patterns[rowPattern]->rules[rowRule]->asserts[row];

    a->field = ruleEdit->get_field_select()->currentText().toStdString();
    a->use_free_text = false;
}

//---------------------------------------------------------------------------
void MainWindow::edit_assert_validator()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = get_index_of_item_backXX(item, 3);
    int rowPattern = get_index_of_item_backXX(item, 2);
    int rowRule = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (row < 0 || rowPolicy < 0 || rowPattern < 0 || rowRule < 0)
        return;

    Assert *a = C.policies.policies[rowPolicy]->patterns[rowPattern]->rules[rowRule]->asserts[row];

    a->validator = ruleEdit->get_validator_value_from_pretty_name(ruleEdit->get_validator_select()->currentText().toStdString());
    a->use_free_text = false;
}

//---------------------------------------------------------------------------
void MainWindow::edit_assert_value()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = get_index_of_item_backXX(item, 3);
    int rowPattern = get_index_of_item_backXX(item, 2);
    int rowRule = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (row < 0 || rowPolicy < 0 || rowPattern < 0 || rowRule < 0)
        return;

    Assert *a = C.policies.policies[rowPolicy]->patterns[rowPattern]->rules[rowRule]->asserts[row];

    a->value = ruleEdit->get_value_line()->text().toStdString();
    ruleEdit->value_to_quotted_value(a->value);
    a->use_free_text = false;
}

//---------------------------------------------------------------------------
void MainWindow::edit_assert_freeText()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return;

    int rowPolicy = get_index_of_item_backXX(item, 3);
    int rowPattern = get_index_of_item_backXX(item, 2);
    int rowRule = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (row < 0 || rowPolicy < 0 || rowPattern < 0 || rowRule < 0)
        return;

    Assert *a = C.policies.policies[rowPolicy]->patterns[rowPattern]->rules[rowRule]->asserts[row];

    a->text = ruleEdit->get_freeText_text()->toPlainText().toStdString();
    a->use_free_text = true;

    Assert test;
    if (C.policies.try_parsing_test(a->text, &test))
        ruleEdit->get_editorSelector_radio()->setEnabled(true);
    else
        ruleEdit->get_editorSelector_radio()->setEnabled(false);
}

//---------------------------------------------------------------------------
void MainWindow::assert_free_text_selected(bool checked)
{
    if (!checked)
        return;

    Assert a;

    a.type = ruleEdit->get_type_select()->currentText().toStdString();
    a.field = ruleEdit->get_field_select()->currentText().toStdString();
    a.validator = ruleEdit->get_validator_value_from_pretty_name(ruleEdit->get_validator_select()->currentText().toStdString());
    string value = ruleEdit->get_value_line()->text().toStdString();
    ruleEdit->value_to_quotted_value(value);
    a.value = value;
    a.use_free_text = false;

    string assertStr = C.policies.serialize_assert_for_test(&a);
    ruleEdit->get_freeText_text()->setText(QString().fromStdString(assertStr));
    ruleEdit->get_freeText_text()->show();
    ruleEdit->get_editor_frame()->hide();
}

//---------------------------------------------------------------------------
void MainWindow::assert_editor_selected(bool checked)
{
    if (!checked)
        return;

    Assert a;
    if (C.policies.try_parsing_test(ruleEdit->get_freeText_text()->toPlainText().toStdString(), &a))
        ruleEdit->fill_editor_fields(&a);
    else
    {
        ruleEdit->get_type_select()->setCurrentIndex(0);
        ruleEdit->get_field_select()->setCurrentIndex(0);
        ruleEdit->get_validator_select()->setCurrentIndex(0);
        ruleEdit->get_value_line()->setText(QString());
    }
    ruleEdit->get_editor_frame()->show();
    ruleEdit->get_freeText_text()->hide();
}

//---------------------------------------------------------------------------
void MainWindow::policiesTree_selectionChanged()
{
    QTreeWidget *tree = policiesTree->get_policies_tree();

    QList<QTreeWidgetItem *> list = tree->selectedItems();
    if (list.empty())
        return;

    QTreeWidgetItem *item = list.first();
    QTreeWidgetItem *tmp = item->parent();
    int level = 0;
    while (tmp)
    {
        tmp = tmp->parent();
        ++level;
    }
    switch (level)
    {
        case 0:
            displayPoliciesMenu();
            break;
        case 1:
            displayPolicyMenu(item->text(0));
            break;
        case 2:
            displayGroupOfRules(item->text(0));
            break;
        case 3:
            displayRuleMenu();
            break;
        case 4:
            displayRuleEdit(item);
            break;
        default:
            return;
    }
}

//***************************************************************************
// Visual elements
//***************************************************************************

//---------------------------------------------------------------------------
void MainWindow::clearVisualElements()
{
    if (MainText)
    {
        Layout->removeWidget(MainText);
        delete MainText; MainText=NULL;
    }

    if (policiesTree)
    {
        clearPoliciesElements();
        Layout->removeWidget(policiesTree);
        delete policiesTree;
        policiesTree=NULL;
    }

    if (ruleEdit)
    {
        Layout->removeWidget(ruleEdit);
        delete ruleEdit;
        ruleEdit=NULL;
    }

    if (DragDrop_Image)
    {
        Layout->removeWidget(DragDrop_Image);
        Layout->removeWidget(DragDrop_Text);
        delete DragDrop_Image; DragDrop_Image=NULL;
        delete DragDrop_Text; DragDrop_Text=NULL;
    }
}

//---------------------------------------------------------------------------
void MainWindow::clearPoliciesElements()
{
    if (!policiesTree)
        return;

    if (policiesMenu)
    {
        policiesTree->get_menu_layout()->removeWidget(policiesMenu);
        delete policiesMenu;
        policiesMenu=NULL;
    }

    if (policyMenu)
    {
        policiesTree->get_menu_layout()->removeWidget(policyMenu);
        delete policyMenu;
        policyMenu=NULL;
    }

    if (groupOfRules)
    {
        policiesTree->get_menu_layout()->removeWidget(groupOfRules);
        delete groupOfRules;
        groupOfRules = NULL;
    }

    if (ruleMenu)
    {
        policiesTree->get_menu_layout()->removeWidget(ruleMenu);
        delete ruleMenu;
        ruleMenu = NULL;
    }

    if (ruleEdit)
    {
        policiesTree->get_menu_layout()->removeWidget(ruleEdit);
        delete ruleEdit;
        ruleEdit=NULL;
    }
}

//---------------------------------------------------------------------------
void MainWindow::createMainText()
{
    if (MainText)
        return;

    clearVisualElements();

    MainText=new QPlainTextEdit(this);
    MainText->setReadOnly(true);
    #if QT_VERSION >= 0x050200
        MainText->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    #endif
    Layout->addWidget(MainText);
}

//---------------------------------------------------------------------------
void MainWindow::createDragDrop()
{
    if (DragDrop_Image)
        return;

    clearVisualElements();

    QFont Font;
    Font.setPointSize(Font.pointSize()*4);

    DragDrop_Image=new QLabel(this);
    DragDrop_Image->setAlignment(Qt::AlignCenter);
    DragDrop_Image->setPixmap(QPixmap(":/icon/dropfiles.png").scaled(256, 256));
    Layout->addWidget(DragDrop_Image);

    DragDrop_Text=new QLabel(this);
    DragDrop_Text->setAlignment(Qt::AlignCenter);
    DragDrop_Text->setFont(Font);
    QPalette Palette(DragDrop_Text->palette());
    Palette.setColor(QPalette::WindowText, Qt::darkGray);
    DragDrop_Text->setPalette(Palette);
    DragDrop_Text->setText("Drop video file(s) here");
    Layout->addWidget(DragDrop_Text);
}

//---------------------------------------------------------------------------
void MainWindow::createPoliciesTree()
{
    if (policiesTree)
        return;

    clearVisualElements();
    clearPoliciesElements();
    policiesTree = new PoliciesTree(this);
}

//---------------------------------------------------------------------------
void MainWindow::displayPoliciesTree()
{
    createPoliciesTree();

    Layout->addWidget(policiesTree);
    QTreeWidget *tree = policiesTree->get_policies_tree();

    QTreeWidgetItem* policies = tree->topLevelItem(0);
    if (!policies)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(tree);
        item->setText(0, tr("Policies"));
        item->setSelected(true);
        createPoliciesMenu();
    }
    updatePoliciesTree();
    QObject::connect(policiesTree->get_policies_tree(), SIGNAL(itemSelectionChanged()),
                     this, SLOT(policiesTree_selectionChanged()));
}

//---------------------------------------------------------------------------
void MainWindow::updatePoliciesTreeRule(Rule *rule, QTreeWidgetItem *parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    QString name = QString("Rule");
    item->setText(0, name);

    for (size_t i = 0; i < rule->asserts.size(); ++i)
    {
        Assert *assert = rule->asserts[i];
        if (!assert)
            continue;

        QTreeWidgetItem* a = new QTreeWidgetItem(item);
        QString descr = QString().fromStdString(assert->description);
        a->setText(0, descr);
    }
}

//---------------------------------------------------------------------------
void MainWindow::updatePoliciesTreePattern(Pattern *pattern, QTreeWidgetItem *parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    QString name = QString().fromStdString(pattern->name);
    if (!name.length())
        name = QString("New group of rules");
    item->setText(0, name);

    for (size_t i = 0; i < pattern->rules.size(); ++i)
    {
        Rule *rule =pattern->rules[i];
        if (!rule)
            continue;
        updatePoliciesTreeRule(rule, item);
    }
}

//---------------------------------------------------------------------------
void MainWindow::updatePoliciesTreePolicy(Policy* policy, QTreeWidgetItem *parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    QString title = QString().fromStdString(policy->title);
    if (!title.length())
        title = QString("New policy");
    item->setText(0, title);

    for (size_t i = 0; i < policy->patterns.size(); ++i)
    {
        Pattern *pat = policy->patterns[i];
        if (!pat)
            continue;
        updatePoliciesTreePattern(pat, item);
    }
}

void MainWindow::removeTreeChildren(QTreeWidgetItem* item)
{
    if (!item)
        return;
    QList<QTreeWidgetItem *> list = item->takeChildren();
    while (!list.isEmpty())
        removeTreeChildren(list.takeFirst());
}

//---------------------------------------------------------------------------
void MainWindow::updatePoliciesTree()
{
    if (!policiesTree)
        return;
    QTreeWidgetItem* policies = policiesTree->get_policies_tree()->topLevelItem(0);
    removeTreeChildren(policies);

    for (size_t i = 0; i < C.policies.policies.size(); ++i)
    {
        Policy *policy = C.policies.policies[i];
        if (!policy)
            continue;
        updatePoliciesTreePolicy(policy, policies);
    }
}

//---------------------------------------------------------------------------
void MainWindow::createPoliciesMenu()
{
    if (policiesMenu)
        return;

    clearPoliciesElements();
    policiesMenu = new PoliciesMenu(policiesTree->get_menu_frame());
    policiesTree->get_menu_layout()->addWidget(policiesMenu);
    QObject::connect(policiesMenu->get_importPolicy_button(), SIGNAL(clicked()),
                     this, SLOT(on_importSchematron()));
    QObject::connect(policiesMenu->get_addNewPolicy_button(), SIGNAL(clicked()),
                     this, SLOT(add_new_policy()));
    QObject::connect(policiesMenu->get_deletePolicies_button(), SIGNAL(clicked()),
                     this, SLOT(delete_all_policies()));
}

//---------------------------------------------------------------------------
void MainWindow::displayPoliciesMenu()
{
    createPoliciesMenu();
}

//---------------------------------------------------------------------------
void MainWindow::createPolicyMenu()
{
    if (policyMenu)
        return;
    clearPoliciesElements();
    policyMenu = new PolicyMenu(policiesTree->get_menu_frame());
    policiesTree->get_menu_layout()->addWidget(policyMenu);
    QObject::connect(policyMenu->get_exportPolicy_button(), SIGNAL(clicked()),
                     this, SLOT(on_exportSchematron()));
    QObject::connect(policyMenu->get_addNewGor_button(), SIGNAL(clicked()),
                     this, SLOT(add_new_gor()));
    QObject::connect(policyMenu->get_deletePolicy_button(), SIGNAL(clicked()),
                     this, SLOT(delete_policy()));
    QObject::connect(policyMenu->get_duplicatePolicy_button(), SIGNAL(clicked()),
                     this, SLOT(duplicate_policy()));
    QObject::connect(policyMenu->get_title_line(), SIGNAL(textChanged(QString)),
                     this, SLOT(edit_policy_title()));
}

//---------------------------------------------------------------------------
void MainWindow::displayPolicyMenu(QString title)
{
    createPolicyMenu();
    QLineEdit* name = policyMenu->get_title_line();

    name->setText(title);
}

//---------------------------------------------------------------------------
void MainWindow::createGroupOfRules()
{
    if (groupOfRules)
        return;
    clearPoliciesElements();
    groupOfRules = new GroupOfRules(policiesTree->get_menu_frame());
    policiesTree->get_menu_layout()->addWidget(groupOfRules);
    QObject::connect(groupOfRules->get_addNewRule_button(), SIGNAL(clicked()),
                     this, SLOT(add_new_rule()));
    QObject::connect(groupOfRules->get_title_line(), SIGNAL(textChanged(QString)),
                     this, SLOT(edit_gor_title()));
    QObject::connect(groupOfRules->get_deleteGor_button(), SIGNAL(clicked()),
                     this, SLOT(delete_gor()));
    QObject::connect(groupOfRules->get_duplicateGor_button(), SIGNAL(clicked()),
                     this, SLOT(duplicate_gor()));
}

//---------------------------------------------------------------------------
void MainWindow::displayGroupOfRules(QString title)
{
    createGroupOfRules();
    QLineEdit* name = groupOfRules->get_title_line();

    name->setText(title);
}

//---------------------------------------------------------------------------
void MainWindow::createRuleMenu()
{
    if (ruleMenu)
        return;
    clearPoliciesElements();
    ruleMenu = new RuleMenu(policiesTree->get_menu_frame());
    policiesTree->get_menu_layout()->addWidget(ruleMenu);
    QObject::connect(ruleMenu->get_addNewAssert_button(), SIGNAL(clicked()),
                     this, SLOT(add_new_assert()));
    QObject::connect(ruleMenu->get_deleteRule_button(), SIGNAL(clicked()),
                     this, SLOT(delete_rule()));
    QObject::connect(ruleMenu->get_duplicateRule_button(), SIGNAL(clicked()),
                     this, SLOT(duplicate_rule()));
}

//---------------------------------------------------------------------------
void MainWindow::displayRuleMenu()
{
    createRuleMenu();
}

//---------------------------------------------------------------------------
void MainWindow::createRuleEdit()
{
    clearPoliciesElements();
    ruleEdit = new RuleEdit(this);
    policiesTree->get_menu_layout()->addWidget(ruleEdit);
}

//---------------------------------------------------------------------------
void MainWindow::displayRuleEdit(QTreeWidgetItem *item)
{
    if (!item)
        return;
    createRuleEdit();

    int rowPolicy = get_index_of_item_backXX(item, 3);
    int rowPattern = get_index_of_item_backXX(item, 2);
    int rowRule = get_index_of_item_backXX(item, 1);
    int rowAssert = get_index_of_item_backXX(item, 0);

    if (rowPolicy < 0 || rowPattern < 0 || rowRule < 0 || rowAssert < 0)
        return;
    Assert *a = C.policies.policies[rowPolicy]->patterns[rowPattern]->rules[rowRule]->asserts[rowAssert];
    ruleEdit->assert_clicked(a);

    QObject::connect(ruleEdit->get_assertName_line(), SIGNAL(textEdited(QString)),
                     this, SLOT(edit_assert_name(QString)));
    QObject::connect(ruleEdit->get_delAssert_button(), SIGNAL(clicked()),
                     this, SLOT(delete_assert()));
    QObject::connect(ruleEdit->get_type_select(), SIGNAL(currentIndexChanged(int)),
                     this, SLOT(edit_assert_type()));
    QObject::connect(ruleEdit->get_field_select(), SIGNAL(currentIndexChanged(int)),
                     this, SLOT(edit_assert_field()));
    QObject::connect(ruleEdit->get_validator_select(), SIGNAL(currentIndexChanged(int)),
                     this, SLOT(edit_assert_validator()));
    QObject::connect(ruleEdit->get_value_line(), SIGNAL(textEdited(QString)),
                     this, SLOT(edit_assert_value()));
    QObject::connect(ruleEdit->get_freeText_text(), SIGNAL(textChanged()),
                     this, SLOT(edit_assert_freeText()));
    QObject::connect(ruleEdit->get_freeTextSelector_radio(), SIGNAL(toggled(bool)),
                     this, SLOT(assert_free_text_selected(bool)));
    QObject::connect(ruleEdit->get_editorSelector_radio(), SIGNAL(toggled(bool)),
                     this, SLOT(assert_editor_selected(bool)));

    Assert test;
    if (!a->use_free_text || C.policies.try_parsing_test(a->text, &test))
        ruleEdit->get_editorSelector_radio()->setEnabled(true);
    else
        ruleEdit->get_editorSelector_radio()->setEnabled(false);
}

//***************************************************************************
// HELPER
//***************************************************************************

//---------------------------------------------------------------------------
int MainWindow::get_index_in_tree()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item || !item->parent())
        return -1;

    return item->parent()->indexOfChild(item);
}

//---------------------------------------------------------------------------
int MainWindow::get_index_of_item_backXX(QTreeWidgetItem* item, size_t back)
{
    QTreeWidgetItem* tmp = item;
    for (size_t i = 0; i < back; ++i)
    {
        if (!tmp->parent())
            return -1;
        tmp = tmp->parent();
    }

    if (!tmp || !tmp->parent())
        return -1;
    return tmp->parent()->indexOfChild(tmp);
}

//---------------------------------------------------------------------------
QTreeWidgetItem *MainWindow::get_item_in_tree()
{
    QList<QTreeWidgetItem *> list = policiesTree->get_policies_tree()->selectedItems();
    if (list.empty())
        return NULL;

    return list.first();
}
