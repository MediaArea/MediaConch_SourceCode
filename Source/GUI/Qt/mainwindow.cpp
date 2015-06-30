/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "policiesmenu.h"
#include "policiesedit.h"

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
    policiesMenu = NULL;
    policiesEdit = NULL;

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

void MainWindow::rule_to_add(Rule *r)
{
    // If registered to delete, remove it
    vector<string>::iterator it = ruleToDelete.begin();
    vector<string>::iterator ite = ruleToDelete.end();

    for (; it != ite;)
    {
        if (*it == r->description)
        {
            it = ruleToDelete.erase(it);
            ite = ruleToDelete.end();
        } else {
            ++it;
        }
    }
    ruleToAdd.push_back(r);
}

void MainWindow::rule_to_delete(string description)
{
    //Remove temporary rules to add for this policy
    vector<Rule *>::iterator it = ruleToAdd.begin();
    vector<Rule *>::iterator ite = ruleToAdd.end();

    for (; it != ite;)
    {
        if ((*it)->description == description)
        {
            delete *it;
            it = ruleToAdd.erase(it);
            ite = ruleToAdd.end();
        } else {
            ++it;
        }
    }

    //Register rule to delete
    ruleToDelete.push_back(description);
}

void MainWindow::policy_to_delete(int index)
{
    //Remove temporary rules to add for this policy
    ruleToAdd.clear();

    //Delete policy
    C.policies.erase_policy(index);
}

const Rule *MainWindow::get_rule_from_description(string description) const
{
    vector<Rule *>::const_iterator it = ruleToAdd.begin();
    vector<Rule *>::const_iterator ite = ruleToAdd.end();

    for (; it != ite; ++it)
    {
        if ((*it)->description == description)
        {
            return *it;
        }
    }

    if (policiesMenu)
    {
        QList<QTableWidgetItem *> list = policiesMenu->get_policies_table()->selectedItems();
        if (list.isEmpty())
        {
            return NULL;
        }

        int row = list.first()->row();

        vector<Rule *>::const_iterator it = C.policies.rules[row].second.begin();
        vector<Rule *>::const_iterator ite = C.policies.rules[row].second.end();

        for (; it != ite; ++it)
        {
            if ((*it)->description == description)
            {
                return *it;
            }
        }
    }

    return NULL;
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
void MainWindow::Run()
{
    if (C.Tool == Core::tool_MediaPolicies)
    {
        displayPoliciesMenu();
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

void MainWindow::exporting_to_schematron_file()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Policies"),
                                                    "", tr("Schematron (*.sch)"));

    C.policies.export_schematron(filename.toStdString().c_str());
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
    exporting_to_schematron_file();
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
    if (!C.policies.rules.size() && !C.SchematronFile.length())
    {
        QString file = ask_for_schematron_file();
        if (file.length())
        {
            C.SchematronFile = file.toStdWString();
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
        C.SchematronFile = file.toStdWString();
    }
    Run();
}

//---------------------------------------------------------------------------
void MainWindow::on_importSchematron()
{
    QString file = ask_for_schematron_file();
    String ret = C.policies.import_schematron(file.toStdString().c_str());
    if (ret.length() && policiesMenu) {
        policiesMenu->add_error(ret);
    }

    Run();
}

//---------------------------------------------------------------------------
void MainWindow::on_addNewPolicy()
{
    displayPoliciesEdit(-1);
}

//---------------------------------------------------------------------------
void MainWindow::on_editPolicy(int row, int)
{
    displayPoliciesEdit(row);
}

//---------------------------------------------------------------------------
void MainWindow::on_editPolicy()
{
    QList<QTableWidgetItem *> list = policiesMenu->get_policies_table()->selectedItems();
    if (list.isEmpty())
    {
        return;
    }

    QTableWidgetItem *item = list.first();

    if (!item)
    {
        return;
    }
    string name = item->text().toStdString();
    displayPoliciesEdit(item->row());
}

//---------------------------------------------------------------------------
void MainWindow::on_addNewRuleRejected()
{
    ruleToAdd.clear();
    ruleToDelete.clear();
    clearVisualElements();
    displayPoliciesMenu();
}

//---------------------------------------------------------------------------
void MainWindow::on_addNewRuleAccepted()
{
    string new_name = policiesEdit->get_new_name();
    if (!new_name.length())
    {
        policiesEdit->add_error(__T("Policy must have a name"));
        policiesEdit->show_errors();
        return;
    }

    int row = -1;
    if (policiesMenu)
    {
        QList<QTableWidgetItem *> list = policiesMenu->get_policies_table()->selectedItems();
        if (list.isEmpty())
        {
            goto create_new_policy;
        }

        row = list.first()->row();
        if (new_name != list.first()->text().toStdString())
        {
            C.policies.rules[row].first = new_name;
        }
    } else {
    create_new_policy:
        vector<Rule *> v;
        C.policies.rules.push_back(make_pair(new_name, v));
        row = C.policies.rules.size() - 1;
    }

    vector<Rule *>::iterator it = ruleToAdd.begin();
    vector<Rule *>::iterator ite = ruleToAdd.end();

    for (; it != ite; ++it)
    {
        C.policies.rules[row].second.push_back(*it);
    }
    ruleToAdd.clear();

    vector<string>::iterator itDel = ruleToDelete.begin();
    vector<string>::iterator iteDel = ruleToDelete.end();

    for (; itDel != iteDel; ++itDel)
    {
        vector<Rule *>::iterator itRule = C.policies.rules[row].second.begin();
        vector<Rule *>::iterator iteRule = C.policies.rules[row].second.end();

        for (; itRule != iteRule;)
        {
            if (*itDel == (*itRule)->description)
            {
                delete *itRule;
                itRule = C.policies.rules[row].second.erase(itRule);
                iteRule = C.policies.rules[row].second.end();
            } else {
                ++itRule;
            }
        }
    }
    ruleToDelete.clear();

    clearVisualElements();
    displayPoliciesMenu();
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

    if (policiesMenu)
    {
        Layout->removeWidget(policiesMenu);
        delete policiesMenu; policiesMenu=NULL;
    }

    if (policiesEdit)
    {
        Layout->removeWidget(policiesEdit);
        delete policiesEdit; policiesEdit=NULL;
    }

    if (DragDrop_Image)
    {
        Layout->removeWidget(DragDrop_Image);
        Layout->removeWidget(DragDrop_Text);
        delete DragDrop_Image; DragDrop_Image=NULL;
        delete DragDrop_Text; DragDrop_Text=NULL;
    }

    ruleToAdd.clear();
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
void MainWindow::createPoliciesMenu()
{
    if (policiesMenu) {
        policiesMenu->clear();
        return;
    }
    clearVisualElements();
    policiesMenu = new PoliciesMenu(this);
    QObject::connect(policiesMenu->get_schematron_button(), SIGNAL(clicked()),
                     this, SLOT(on_importSchematron()));
    QObject::connect(policiesMenu->get_addNewPolicy_button(), SIGNAL(clicked()),
                     this, SLOT(on_addNewPolicy()));
    QObject::connect(policiesMenu->get_policies_table(), SIGNAL(cellDoubleClicked(int, int)),
                     this, SLOT(on_editPolicy(int, int)));
    QObject::connect(policiesMenu->get_editPolicy_button(), SIGNAL(clicked()),
                     this, SLOT(on_editPolicy()));
}

//---------------------------------------------------------------------------
void MainWindow::displayPoliciesMenu()
{
    createPoliciesMenu();

    Layout->addWidget(policiesMenu);
    policiesMenu->show_errors();

    vector<pair<string, vector<Rule *> > >::iterator it = C.policies.rules.begin();
    vector<pair<string, vector<Rule *> > >::iterator ite = C.policies.rules.end();
    for (; it != ite; ++it)
    {
        policiesMenu->add_policy(it->first);
    }
}

//---------------------------------------------------------------------------
void MainWindow::createPoliciesEdit()
{
    if (policiesEdit) {
        policiesEdit->clear();
        return;
    }

    policiesMenu->hide();
    policiesEdit = new PoliciesEdit(this);
    QObject::connect(policiesEdit->get_validation_button(), SIGNAL(accepted()),
                     this, SLOT(on_addNewRuleAccepted()));
    QObject::connect(policiesEdit->get_validation_button(), SIGNAL(rejected()),
                     this, SLOT(on_addNewRuleRejected()));
}

//---------------------------------------------------------------------------
void MainWindow::displayPoliciesEdit(int row)
{
    createPoliciesEdit();
    Layout->addWidget(policiesEdit);
    policiesEdit->show_errors();

    if (row != -1)
    {
        policiesEdit->set_name(C.policies.rules[row].first);
        vector<Rule *>::iterator it = C.policies.rules[row].second.begin();
        vector<Rule *>::iterator ite = C.policies.rules[row].second.end();
        for (; it != ite; ++it)
        {
            policiesEdit->add_rule(*it);
        }
    }
}

//***************************************************************************
// HELPER
//***************************************************************************

//---------------------------------------------------------------------------
QString MainWindow::getSelectedPolicyName()
{
    QList<QTableWidgetItem *> list = policiesMenu->get_policies_table()->selectedItems();

    if (list.isEmpty())
    {
        return QString();
    }

    QTableWidgetItem *item = list.first();

    return item->text();
}
