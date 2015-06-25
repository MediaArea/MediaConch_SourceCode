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
    ruleToAdd.push_back(r);
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

    string name = policiesEdit->get_old_name();
    map<string, vector<Rule *> >::const_iterator r = C.policies.rules.find(name);
    if (r == C.policies.rules.end())
    {
        return NULL;
    }
    it = r->second.begin();
    ite = r->second.end();

    for (; it != ite; ++it)
    {
        if ((*it)->description == description)
        {
            return *it;
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
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Policies"),
                                                    "", tr("Schematron (*.sch)"));

    C.policies.export_schematron(filename.toStdString().c_str());
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
    if (!C.SchematronFile.length())
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
    QString name = getSelectedPolicyName();
    displayPoliciesEdit(name.toStdString());
}

//---------------------------------------------------------------------------
void MainWindow::on_editPolicy(int row, int column)
{
    QTableWidgetItem *item = policiesMenu->get_policies_table()->item(row, column);
    string name = item->text().toStdString();
    displayPoliciesEdit(name);
}

//---------------------------------------------------------------------------
void MainWindow::on_addNewRuleRejected()
{
    ruleToAdd.clear();
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
    string old_name = policiesEdit->get_old_name();

    if (old_name.length() && old_name != new_name)
    {
        map<string, vector<Rule *> >::iterator oldIt = C.policies.rules.find(old_name);
        if (oldIt != C.policies.rules.end())
        {
            C.policies.rules[new_name] = C.policies.rules[old_name];
            C.policies.rules.erase(oldIt);
        }
    }

    vector<Rule *>::iterator it = ruleToAdd.begin();
    vector<Rule *>::iterator ite = ruleToAdd.end();

    for (; it != ite; ++it)
    {
        C.policies.rules[new_name].push_back(*it);
    }
    ruleToAdd.clear();
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
}

//---------------------------------------------------------------------------
void MainWindow::displayPoliciesMenu()
{
    createPoliciesMenu();

    Layout->addWidget(policiesMenu);
    policiesMenu->show_errors();

    map<string, vector<Rule *> >::iterator it = C.policies.rules.begin();
    map<string, vector<Rule *> >::iterator ite = C.policies.rules.end();
    for (; it != ite; ++it)
    {
        policiesMenu->add_policy(it->first);
    }
}

//---------------------------------------------------------------------------
void MainWindow::createPoliciesEdit(string name)
{
    if (policiesEdit) {
        policiesEdit->clear();
        return;
    }

    policiesMenu->hide();
    policiesEdit = new PoliciesEdit(this, name);
    QObject::connect(policiesEdit->get_validation_button(), SIGNAL(accepted()),
                     this, SLOT(on_addNewRuleAccepted()));
    QObject::connect(policiesEdit->get_validation_button(), SIGNAL(rejected()),
                     this, SLOT(on_addNewRuleRejected()));
}

//---------------------------------------------------------------------------
void MainWindow::displayPoliciesEdit(string name)
{
    createPoliciesEdit(name);
    Layout->addWidget(policiesEdit);
    policiesEdit->show_errors();

    if (name.length())
    {
        vector<Rule *>::iterator it = C.policies.rules[name.c_str()].begin();
        vector<Rule *>::iterator ite = C.policies.rules[name.c_str()].end();
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
