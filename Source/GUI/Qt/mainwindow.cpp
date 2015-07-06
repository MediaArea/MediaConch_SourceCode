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

    //TODO: get policy number
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
    // TODO: Add error if ret.length() > 0
    // TODO: Display the policy imported

    displayPoliciesTree();
    Run();
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
    if (policiesTree) {
        return;
    }
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
    if (!policiesTree) {
        return;
    }
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
    if (policiesMenu) {
        return;
    }

    clearPoliciesElements();
    policiesMenu = new PoliciesMenu(policiesTree->get_menu_frame());
    policiesTree->get_menu_layout()->addWidget(policiesMenu);
    QObject::connect(policiesMenu->get_importPolicy_button(), SIGNAL(clicked()),
                     this, SLOT(on_importSchematron()));
    QObject::connect(policiesMenu->get_addNewPolicy_button(), SIGNAL(clicked()),
                     this, SLOT(add_new_policy()));
}

//---------------------------------------------------------------------------
void MainWindow::displayPoliciesMenu()
{
    createPoliciesMenu();
    updatePoliciesTree();
}

//---------------------------------------------------------------------------
void MainWindow::createPolicyMenu()
{
    if (policyMenu) {
        return;
    }
    clearPoliciesElements();
    policyMenu = new PolicyMenu(policiesTree->get_menu_frame());
    policiesTree->get_menu_layout()->addWidget(policyMenu);
    QObject::connect(policyMenu->get_exportPolicy_button(), SIGNAL(clicked()),
                     this, SLOT(on_exportSchematron()));
    QObject::connect(policyMenu->get_addNewGor_button(), SIGNAL(clicked()),
                     this, SLOT(add_new_gor()));
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
    if (groupOfRules) {
        return;
    }
    clearPoliciesElements();
    groupOfRules = new GroupOfRules(policiesTree->get_menu_frame());
    policiesTree->get_menu_layout()->addWidget(groupOfRules);
    // QObject::connect(groupOfRules->get_addNewRule_button(), SIGNAL(clicked()),
    //                  this, SLOT(on_addNewRule()));
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
    if (ruleMenu) {
        return;
    }
    clearPoliciesElements();
    ruleMenu = new RuleMenu(policiesTree->get_menu_frame());
    policiesTree->get_menu_layout()->addWidget(ruleMenu);
    // QObject::connect(groupOfRules->get_addNewAssert_button(), SIGNAL(clicked()),
    //                  this, SLOT(on_addNewAssert()));
}

//---------------------------------------------------------------------------
void MainWindow::displayRuleMenu()
{
    createRuleMenu();
}

//---------------------------------------------------------------------------
void MainWindow::createRuleEdit()
{
    if (ruleEdit) {
        ruleEdit->clear();
        return;
    }
    clearPoliciesElements();
    ruleEdit = new RuleEdit(this);
    policiesTree->get_menu_layout()->addWidget(ruleEdit);
}

//---------------------------------------------------------------------------
void MainWindow::displayRuleEdit(QTreeWidgetItem *item)
{
    createRuleEdit();

    if (!item)
        return;

    int rowPolicy = get_index_of_item_backXX(item, 3);
    int rowPattern = get_index_of_item_backXX(item, 2);
    int rowRule = get_index_of_item_backXX(item, 1);
    int rowAssert = get_index_of_item_backXX(item, 0);

    if (rowPolicy < 0 || rowPattern < 0 || rowRule < 0 || rowAssert < 0)
        return;
    ruleEdit->assert_clicked(C.policies.policies[rowPolicy]->patterns[rowPattern]->rules[rowRule]->asserts[rowAssert]);
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
