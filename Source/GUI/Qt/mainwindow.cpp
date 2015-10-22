/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menumainwindow.h"
#include "checkerwindow.h"
#include "policieswindow.h"
#include "displaywindow.h"
#include "helpwindow.h"

#include <QStringList>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QActionGroup>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMimeData>
#include <QLabel>
#include <QUrl>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>
#include <QRadioButton>
#include <QStatusBar>
#include <QMessageBox>
#include <QCloseEvent>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif
#if QT_VERSION >= 0x050200
    #include <QFontDatabase>
#endif
#include <sstream>

namespace MediaConch {

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
    ToolGroup->addAction(ui->actionChecker);
    ToolGroup->addAction(ui->actionPolicies);
    ToolGroup->addAction(ui->actionDisplay);
    
    // Visual elements
    Layout=(QVBoxLayout*)ui->centralWidget->layout();
    Layout->setContentsMargins(0, 0, 0, 0);
    MenuView = new MenuMainWindow(this);
    MainView=NULL;
    policiesView = NULL;
    displayView = NULL;

    // Window
    setWindowIcon(QIcon(":/icon/icon.png"));
    int left=70;
    int width=QApplication::desktop()->screenGeometry().width();
    if (width>1150)
    {
        left+=(width-1150)/2;
        width=1150;
    }
    move(left, 70);
    resize(width-140, QApplication::desktop()->screenGeometry().height()-140);
    setAcceptDrops(false);

    // Default
    add_default_policy();
    add_default_displays();
    on_actionChecker_triggered();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//***************************************************************************
// Functions
//***************************************************************************

//---------------------------------------------------------------------------
void MainWindow::addFileToList(const QString& file)
{
    C.List.push_back(file.toStdWString());
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
    switch (current_view)
    {
        case RUN_CHECKER_VIEW:
            //TODO: fill the view if file already here
            // if (!C.List.empty())
            //     C.Run();
            createWebView();
            break;
        case RUN_POLICIES_VIEW:
            createPoliciesView();
            break;
        case RUN_DISPLAY_VIEW:
            createDisplayView();
            break;
        default:
            //TODO: fill the view if file already here
            // if (!C.List.empty())
            //     C.Run();
            createWebView();
            break;
    }
}

//---------------------------------------------------------------------------
String MainWindow::transformWithXslt(String& report, String trans)
{
    return C.transformWithXslt(report, trans);
}

//---------------------------------------------------------------------------
void MainWindow::checker_add_file(QString& file, int policy)
{
    MainView->checker_add_file(file, policy);
}

//---------------------------------------------------------------------------
void MainWindow::checker_add_files(QFileInfoList& list, int policy)
{
    MainView->checker_add_files(list, policy);
}

//---------------------------------------------------------------------------
void MainWindow::checker_add_policy_file(QString& file, QString& policy)
{
    MainView->checker_add_policy_file(file, policy);
}

//---------------------------------------------------------------------------
void MainWindow::checker_add_policy_files(QFileInfoList& list, QString& policy)
{
    MainView->checker_add_policy_files(list, policy);
}

//---------------------------------------------------------------------------
const std::vector<String>& MainWindow::policy_file_registered()
{
    return C.PoliciesFiles;
}

//---------------------------------------------------------------------------
QString MainWindow::ask_for_schema_file()
{
    QString file=QFileDialog::getOpenFileName(this, "Open file", "", "XSL file (*.xsl);;Schematron file (*.sch);;All (*.*)", 0, QFileDialog::DontUseNativeDialog);
    return file;
}

//---------------------------------------------------------------------------
int MainWindow::exporting_to_schematron_file(int pos)
{
#if QT_VERSION >= 0x050400
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#elif QT_VERSION >= 0x050000
    QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
    QString path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif

    if (pos < (int)C.policies.policies.size() && pos >= 0 && C.policies.policies[pos])
        path += "/" + QString().fromStdString(C.policies.policies[pos]->title) + ".sch";

    QString filename = QFileDialog::getSaveFileName(this, tr("Save Policy"),
                                              path, tr("Schematron (*.sch)"));

    if (!filename.length())
        return -1;
    C.policies.export_schema(filename.toStdString().c_str(), pos);
    C.policies.policies[pos]->filename = filename.toStdString();
    return 0;
}

//---------------------------------------------------------------------------
int MainWindow::exporting_to_xslt_file(int pos)
{
#if QT_VERSION >= 0x050400
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#elif QT_VERSION >= 0x050000
    QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
    QString path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif

    if (pos < (int)C.policies.policies.size() && pos >= 0 && C.policies.policies[pos])
        path += "/" + QString().fromStdString(C.policies.policies[pos]->title) + ".xsl";

    QString filename = QFileDialog::getSaveFileName(this, tr("Save Policy"),
                                              path, tr("XSLT (*.xsl)"));

    if (!filename.length())
        return -1;
    C.policies.export_schema(filename.toStdString().c_str(), pos);
    C.policies.policies[pos]->filename = filename.toStdString();
    return 0;
}

//---------------------------------------------------------------------------
void MainWindow::exporting_policy(int pos)
{
    C.policies.export_schema(NULL, pos);
}

//---------------------------------------------------------------------------
bool MainWindow::ValidatePolicy(int policy, bool& valid, String& report)
{
    return C.ValidatePolicy(policy, valid, report);
}

//---------------------------------------------------------------------------
void MainWindow::add_default_policy()
{
    QDir policies_dir(":/policies");

    policies_dir.setFilter(QDir::Files);
    QFileInfoList list = policies_dir.entryInfoList();
    for (int i = 0; i < list.count(); ++i)
    {
        QFile file(list[i].absoluteFilePath());

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;
        QByteArray schematron = file.readAll();
        const std::string& file_str = list[i].absoluteFilePath().toStdString();
        C.policies.import_schema_from_memory(file_str, schematron.constData(), schematron.length());
    }

#if QT_VERSION >= 0x050400
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#elif QT_VERSION >= 0x050000
    QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
    QString path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif
    policies_dir = QDir(path);

    policies_dir.setFilter(QDir::Files);
    list = policies_dir.entryInfoList();
    for (int i = 0; i < list.count(); ++i)
    {
        QFile file(list[i].absoluteFilePath());

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;
        QByteArray data = file.readAll();
        C.policies.import_schema_from_memory(list[i].absoluteFilePath().toStdString(),
                                             data.constData(), data.length());
    }
}

//---------------------------------------------------------------------------
void MainWindow::add_default_displays()
{
#if QT_VERSION >= 0x050400
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#elif QT_VERSION >= 0x050000
    QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
    QString path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif

    QDir dir(path);
    dir.cd("Display");

    if (dir.exists())
    {
        dir.setFilter(QDir::Files);
        QFileInfoList list = dir.entryInfoList();
        for (int i = 0; i < list.size(); ++i)
            displaysList.push_back(list[i].absoluteFilePath());
    }
}

//---------------------------------------------------------------------------
void MainWindow::addXsltDisplay(QString& display_xslt)
{
    MainView->setDisplayXslt(display_xslt);
}

//---------------------------------------------------------------------------
void MainWindow::removeXsltDisplay()
{
    MainView->resetDisplayXslt();
}

//---------------------------------------------------------------------------
void MainWindow::addPolicyToList(QString& policy)
{
    C.PoliciesFiles.push_back(policy.toStdWString());
}

//---------------------------------------------------------------------------
void MainWindow::clearPolicyList()
{
    C.PoliciesFiles.clear();
}

//---------------------------------------------------------------------------
void MainWindow::clearFileList()
{
    C.List.clear();
}

//---------------------------------------------------------------------------
const std::vector<Policy *>& MainWindow::get_all_policies() const
{
    return C.policies.policies;
}

//---------------------------------------------------------------------------
std::vector<QString>& MainWindow::get_displays()
{
    return displaysList;
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
void MainWindow::on_actionChecker_triggered()
{
    if (!ui->actionChecker->isChecked())
        ui->actionChecker->setChecked(true);
    if (clearVisualElements() < 0)
        return;
    current_view = RUN_CHECKER_VIEW;
    Run();
}

//---------------------------------------------------------------------------
void MainWindow::on_actionPolicies_triggered()
{
    if (!ui->actionPolicies->isChecked())
        ui->actionPolicies->setChecked(true);
    if (clearVisualElements() < 0)
        return;
    current_view = RUN_POLICIES_VIEW;
    Run();
}

//---------------------------------------------------------------------------
void MainWindow::on_actionDisplay_triggered()
{
    if (!ui->actionDisplay->isChecked())
        ui->actionDisplay->setChecked(true);
    if (clearVisualElements() < 0)
        return;
    current_view = RUN_DISPLAY_VIEW;
    Run();
}

//---------------------------------------------------------------------------
void MainWindow::on_actionChooseSchema_triggered()
{
    QString file = ask_for_schema_file();
    if (!file.length())
        return;

    if (!C.policies.import_schema(file.toStdString()).length())
    {
        //TODO error
    }
    if (!ui->actionPolicies->isChecked())
        ui->actionPolicies->setChecked(true);
    current_view = RUN_POLICIES_VIEW;
    Run();
}

//---------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (C.Tool == Core::tool_MediaPolicies && policiesView && !policiesView->is_all_policies_saved())
    {
        QMessageBox msgBox(QMessageBox::Warning, tr("MediaConch"),
                           tr("All policy changes not saved will be discarded?"),
                           QMessageBox::Ok | QMessageBox::Save | QMessageBox::Cancel, this);
        QString info("Policies not saved:");
        for (size_t i = 0; i < C.policies.policies.size(); ++i)
            if (!C.policies.policies[i]->saved)
            {
                info += "\r\n\t";
                info += QString().fromStdString(C.policies.policies[i]->title);
            }

        msgBox.setInformativeText(info);

        int ret = msgBox.exec();
        if (ret == QMessageBox::Save)
        {
            for (size_t i = 0; i < C.policies.policies.size(); ++i)
                if (!C.policies.policies[i]->saved)
                    C.policies.export_schema(NULL, i);
        }
        else if (ret == QMessageBox::Cancel)
        {
            event->ignore();
            return;
        }
    }
    QMainWindow::closeEvent(event);
}

//***************************************************************************
// Help
//***************************************************************************

//---------------------------------------------------------------------------
void MainWindow::on_actionAbout_triggered()
{
    Help* Frame=new Help(this);
    Frame->About();
}

//---------------------------------------------------------------------------
void MainWindow::on_actionGettingStarted_triggered()
{
    Help* Frame=new Help(this);
    Frame->GettingStarted();
}

//---------------------------------------------------------------------------
void MainWindow::on_actionHowToUse_triggered()
{
    Help* Frame=new Help(this);
    Frame->HowToUse();
}

//---------------------------------------------------------------------------
void MainWindow::on_actionDataFormat_triggered()
{
    Help* Frame=new Help(this);
    Frame->DataFormat();
}

//***************************************************************************
// Visual elements
//***************************************************************************

//---------------------------------------------------------------------------
int MainWindow::clearVisualElements()
{
    if (MainView)
    {
        if (MainView->is_analyzes_done())
        {
            int ret = QMessageBox::warning(this, tr("MediaConch"),
                                           tr("All analysis results will be discarded?"),
                                           QMessageBox::Ok | QMessageBox::Cancel);
            if (ret == QMessageBox::Cancel)
                return -1;
        }
        delete MainView;
        MainView=NULL;
    }

    if (policiesView)
    {
        delete policiesView;
        policiesView=NULL;
    }

    if (displayView)
    {
        delete displayView;
        displayView=NULL;
    }

    return 0;
}

//---------------------------------------------------------------------------
void MainWindow::createWebView()
{
    if (MainView)
        return;

    if (clearVisualElements())
        return;
    MainView = new CheckerWindow(this);
    QObject::connect(ui->actionCloseAll, SIGNAL(triggered()),
                     MainView, SLOT(actionCloseAllTriggered()));
    MainView->createWebView();
}

//---------------------------------------------------------------------------
void MainWindow::createPoliciesView()
{
    if (clearVisualElements() < 0)
        return;
    policiesView = new PoliciesWindow(this);
    policiesView->displayPoliciesTree();
}

//---------------------------------------------------------------------------
void MainWindow::createDisplayView()
{
    if (clearVisualElements() < 0)
        return;
    displayView = new DisplayWindow(this);
    displayView->displayDisplay();
}

//***************************************************************************
// HELPER
//***************************************************************************

//---------------------------------------------------------------------------
void MainWindow::set_widget_to_layout(QWidget* w)
{
    Layout->addWidget(w);
}

//---------------------------------------------------------------------------
void MainWindow::remove_widget_from_layout(QWidget* w)
{
    Layout->removeWidget(w);
}

//---------------------------------------------------------------------------
void MainWindow::checker_selected()
{
    on_actionChecker_triggered();
}

//---------------------------------------------------------------------------
void MainWindow::policies_selected()
{
    on_actionPolicies_triggered();
}

//---------------------------------------------------------------------------
void MainWindow::display_selected()
{
    on_actionDisplay_triggered();
}

//---------------------------------------------------------------------------
void MainWindow::analyze(String& file)
{
    C.Tool = Core::tool_MediaConch;
    C.Run(file);
}

//---------------------------------------------------------------------------
QString MainWindow::get_implementationreport_text()
{
    C.Format = Core::format_Text;
    return QString().fromStdWString(C.MediaConch());
}

//---------------------------------------------------------------------------
QString MainWindow::get_mediainfo_and_mediatrace_xml()
{
    C.Format = Core::format_Xml;
    return QString().fromStdWString(C.MediaConch());
}

//---------------------------------------------------------------------------
QString MainWindow::get_mediainfo_xml()
{
    C.Format = Core::format_Xml;
    return QString().fromStdWString(C.MediaInfo());
}

//---------------------------------------------------------------------------
QString MainWindow::get_mediatrace_xml()
{
    C.Format = Core::format_Xml;
    return QString().fromStdWString(C.MediaTrace());
}

//---------------------------------------------------------------------------
QString MainWindow::get_mediatrace_jstree()
{
    C.Format = Core::format_JsTree;
    return QString().fromStdWString(C.MediaTrace());
}

}
