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

    // Core configuration
    if (!MCL.get_implementation_schema_file().length())
        MCL.create_default_implementation_schema();
    MCL.init();

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
    if (width>1170)
    {
        left+=(width-1170)/2;
        width=1170;
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
void MainWindow::add_file_to_list(const QString& file)
{
    files.push_back(file.toStdString());
}

void MainWindow::policy_to_delete(int index)
{
    //Delete policy
    MCL.remove_policy((size_t)index);
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
            // if (!files.empty())
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
            // if (!files.empty())
            //     C.Run();
            createWebView();
            break;
    }
}

//---------------------------------------------------------------------------
int MainWindow::transform_with_xslt_file(const std::string& report, const std::string& file, std::string& result)
{
    return MCL.transform_with_xslt_file(report, file, result);
}

//---------------------------------------------------------------------------
int MainWindow::transform_with_xslt_memory(const std::string& report, const std::string& memory, std::string& result)
{
    return MCL.transform_with_xslt_memory(report, memory, result);
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
const std::vector<std::string>& MainWindow::policy_file_registered()
{
    return policies;
}

//---------------------------------------------------------------------------
QString MainWindow::get_local_folder() const
{
#if QT_VERSION >= 0x050400
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#elif QT_VERSION >= 0x050000
    QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
    QString path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif
    return path;
}

//---------------------------------------------------------------------------
QString MainWindow::ask_for_schema_file()
{
    QString file=QFileDialog::getOpenFileName(this, "Open file", "", "XSL file (*.xsl);;Schematron file (*.sch);;All (*.*)", 0, QFileDialog::DontUseNativeDialog);
    return file;
}

//---------------------------------------------------------------------------
int MainWindow::exporting_to_schematron_file(size_t pos)
{
    QString path = get_local_folder();
    path += "/policies";

    QDir dir(path);
    if (!dir.exists())
        dir.mkpath(dir.absolutePath());

    Policy* p = MCL.get_policy(pos);
    if (!p)
        return -1;

    path += "/" + QString().fromStdString(p->title) + ".sch";
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Policy"),
                                              path, tr("Schematron (*.sch)"));

    if (!filename.length())
        return -1;

    std::string f = filename.toStdString();
    MCL.save_policy(pos, &f);
    return 0;
}

//---------------------------------------------------------------------------
int MainWindow::exporting_to_xslt_file(size_t pos)
{
    QString path = get_local_folder();
    path += "/policies";

    QDir dir(path);
    if (!dir.exists())
        dir.mkpath(dir.absolutePath());

    Policy* p = MCL.get_policy(pos);
    if (!p)
        return -1;

    path += "/" + QString().fromStdString(p->title) + ".xsl";
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Policy"),
                                              path, tr("XSLT (*.xsl)"));

    if (!filename.length())
        return -1;

    std::string f = filename.toStdString();
    MCL.save_policy(pos, &f);
    return 0;
}

//---------------------------------------------------------------------------
void MainWindow::exporting_policy(size_t pos)
{
    MCL.save_policy(pos, NULL);
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
        std::string memory(schematron.constData(), schematron.length());
        std::string err;
        MCL.import_policy_from_memory(memory, file_str, err);
    }

    QString path = get_local_folder();
    path += "/policies";
    policies_dir = QDir(path);

    policies_dir.setFilter(QDir::Files);
    list = policies_dir.entryInfoList();
    for (int i = 0; i < list.count(); ++i)
    {
        QFile file(list[i].absoluteFilePath());

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;
        QByteArray data = file.readAll();
        std::string memory(data.constData(), data.length());
        std::string err;
        MCL.import_policy_from_memory(memory, list[i].absoluteFilePath().toStdString(),
                                      err);
    }
}

//---------------------------------------------------------------------------
void MainWindow::add_default_displays()
{
    QDir displays_dir(":/displays");

    displays_dir.setFilter(QDir::Files);
    QFileInfoList list = displays_dir.entryInfoList();
    for (int i = 0; i < list.count(); ++i)
        displays_list.push_back(list[i].absoluteFilePath());

    QString path = get_local_folder();
    path += "/displays";

    QDir dir(path);
    if (dir.exists())
    {
        dir.setFilter(QDir::Files);
        list = dir.entryInfoList();
        for (int i = 0; i < list.size(); ++i)
            displays_list.push_back(list[i].absoluteFilePath());
    }
}

//---------------------------------------------------------------------------
void MainWindow::add_xslt_display(const QString& display_xslt)
{
    MainView->set_display_xslt(display_xslt);
}

//---------------------------------------------------------------------------
void MainWindow::remove_xslt_display()
{
    MainView->reset_display_xslt();
}

//---------------------------------------------------------------------------
void MainWindow::add_policy_to_list(const QString& policy)
{
    policies.push_back(policy.toStdString());
}

//---------------------------------------------------------------------------
void MainWindow::clear_policy_list()
{
    policies.clear();
}

//---------------------------------------------------------------------------
void MainWindow::clear_file_list()
{
    files.clear();
}

//---------------------------------------------------------------------------
const std::vector<Policy *>& MainWindow::get_all_policies() const
{
    return MCL.get_policies();
}

//---------------------------------------------------------------------------
std::vector<QString>& MainWindow::get_displays()
{
    return displays_list;
}

//***************************************************************************
// Slots
//***************************************************************************

//---------------------------------------------------------------------------
void MainWindow::on_actionOpen_triggered()
{
    QStringList list=QFileDialog::getOpenFileNames(this, "Open file", "", "Video files (*.avi *.mkv *.mov *.mxf *.mp4);;All (*.*)", 0, QFileDialog::DontUseNativeDialog);
    if (list.empty())
        return;

    for (int pos = 0; pos < list.size(); ++pos)
        files.push_back(list[pos].toStdString());

    current_view = RUN_CHECKER_VIEW;
    Run();

    if (!MainView)
        return;
    MainView->change_local_files(list);
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

    std::string err;
    if (MCL.import_policy_from_file(file.toStdString(), err) < 0)
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
    if (!is_all_policies_saved())
    {
        QMessageBox msgBox(QMessageBox::Warning, tr("MediaConch"),
                           tr("All policy changes not saved will be discarded?"),
                           QMessageBox::Ok | QMessageBox::Save | QMessageBox::Cancel, this);
        QString info("Policies not saved:");
        for (size_t i = 0; i < MCL.get_policies_count(); ++i)
            if (!MCL.get_policy(i)->saved)
            {
                info += "\r\n\t";
                info += QString().fromStdString(MCL.get_policy(i)->title);
            }

        msgBox.setInformativeText(info);

        int ret = msgBox.exec();
        if (ret == QMessageBox::Save)
            MCL.save_policies();
        else if (ret == QMessageBox::Cancel)
        {
            event->ignore();
            return;
        }
    }
    QMainWindow::closeEvent(event);
}

//---------------------------------------------------------------------------
bool MainWindow::is_all_policies_saved()
{
    return MCL.is_policies_saved();
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
    MainView->create_web_view();
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
int MainWindow::import_policy(const QString& file, std::string& err)
{
    return MCL.import_policy_from_file(file.toStdString(), err);
}

//---------------------------------------------------------------------------
bool MainWindow::policy_exists(const std::string& title)
{
    return MCL.policy_exists(title);
}

//---------------------------------------------------------------------------
Policy* MainWindow::get_policy(size_t pos)
{
    return MCL.get_policy(pos);
}

//---------------------------------------------------------------------------
void MainWindow::remove_policy(size_t pos)
{
    return MCL.remove_policy(pos);
}

//---------------------------------------------------------------------------
void MainWindow::add_policy(Policy* policy)
{
    return MCL.add_policy(policy);
}

//---------------------------------------------------------------------------
void MainWindow::clear_policies()
{
    return MCL.clear_policies();
}

//---------------------------------------------------------------------------
size_t MainWindow::get_policies_count() const
{
    return MCL.get_policies_count();
}

//---------------------------------------------------------------------------
void MainWindow::analyze(const std::vector<std::string>& files)
{
    MCL.analyze(files);
}

//---------------------------------------------------------------------------
int MainWindow::is_analyze_finished(const std::vector<std::string>& files, double& percent_done)
{
    if (MCL.is_done(files, percent_done))
        return 0;
    return 1;
}

//---------------------------------------------------------------------------
QString MainWindow::get_implementationreport_xml(const std::string& file,
                                                 const std::string& display_name,
                                                 const std::string& display_content,
                                                 bool& is_valid)
{
    const std::string* dname = display_name.length() ? &display_name : NULL;
    const std::string* dcontent = display_content.length() ? &display_content : NULL;

    std::bitset<MediaConchLib::report_Max> report_set;
    report_set.set(MediaConchLib::report_MediaConch);
    std::vector<std::string> files;
    files.push_back(file);

    MediaConchLib::ReportRes result;
    std::vector<std::string> vec;
    MCL.get_report(report_set, MediaConchLib::format_Xml, files,
                   vec, vec,
                   &result, dname, dcontent);
    if (result.has_valid)
        is_valid = result.valid;
    else
        is_valid = false;

    return QString().fromUtf8(result.report.c_str(), result.report.length());
}

//---------------------------------------------------------------------------
QString MainWindow::get_mediainfo_and_mediatrace_xml(const std::string& file,
                                                     const std::string& display_name,
                                                     const std::string& display_content)
{
    std::bitset<MediaConchLib::report_Max> report_set;
    report_set.set(MediaConchLib::report_MediaInfo);
    report_set.set(MediaConchLib::report_MediaTrace);
    std::string report;
    std::vector<std::string> files;
    files.push_back(file);

    MediaConchLib::ReportRes result;
    std::vector<std::string> vec;
    MCL.get_report(report_set, MediaConchLib::format_Xml, files,
                   vec, vec,
                   &result, &display_name, &display_content);
    return QString().fromStdString(result.report);
}

//---------------------------------------------------------------------------
QString MainWindow::get_mediainfo_xml(const std::string& file,
                                      const std::string& display_name,
                                      const std::string& display_content)
{
    std::bitset<MediaConchLib::report_Max> report_set;
    report_set.set(MediaConchLib::report_MediaInfo);
    std::string report;
    std::vector<std::string> files;
    files.push_back(file);

    MediaConchLib::ReportRes result;
    std::vector<std::string> vec;
    MCL.get_report(report_set, MediaConchLib::format_Xml, files,
                   vec, vec,
                   &result, &display_name, &display_content);
    return QString().fromUtf8(result.report.c_str(), result.report.length());
}

//---------------------------------------------------------------------------
QString MainWindow::get_mediainfo_jstree(const std::string& file)
{
    std::bitset<MediaConchLib::report_Max> report_set;
    report_set.set(MediaConchLib::report_MediaInfo);
    std::string report;
    std::vector<std::string> files;
    files.push_back(file);

    MediaConchLib::ReportRes result;
    std::vector<std::string> vec;
    MCL.get_report(report_set, MediaConchLib::format_JsTree, files,
                   vec, vec, &result);
    return QString().fromUtf8(result.report.c_str(), result.report.length());
}

//---------------------------------------------------------------------------
QString MainWindow::get_mediatrace_xml(const std::string& file,
                                       const std::string& display_name,
                                       const std::string& display_content)
{
    std::bitset<MediaConchLib::report_Max> report_set;
    report_set.set(MediaConchLib::report_MediaTrace);
    std::string report;
    std::vector<std::string> files;
    files.push_back(file);

    MediaConchLib::ReportRes result;
    std::vector<std::string> vec;
    MCL.get_report(report_set, MediaConchLib::format_Xml, files,
                   vec, vec,
                   &result, &display_name, &display_content);
    return QString().fromUtf8(result.report.c_str(), result.report.length());
}

//---------------------------------------------------------------------------
QString MainWindow::get_mediatrace_jstree(const std::string& file)
{
    std::bitset<MediaConchLib::report_Max> report_set;
    report_set.set(MediaConchLib::report_MediaTrace);
    std::string report;
    std::vector<std::string> files;
    files.push_back(file);

    MediaConchLib::ReportRes result;
    std::vector<std::string> vec;
    MCL.get_report(report_set, MediaConchLib::format_JsTree, files,
                   vec, vec,
                   &result);
    return QString().fromUtf8(result.report.c_str(), result.report.length());
}

//---------------------------------------------------------------------------
bool MainWindow::validate_policy(const std::string& file, int policy,
                                 const std::string& display_name,
                                 const std::string& display_content,
                                 std::string& report)
{
    if (policy == -1)
        return validate_policies(file, display_name, display_content, report);

    MediaConchLib::ReportRes result;
    const std::string* dname = display_name.length() ? &display_name : NULL;
    const std::string* dcontent = display_content.length() ? &display_content : NULL;

    if (MCL.validate_policy(file, policy, &result, dname, dcontent) < 0)
        return false;
    report = result.report;
    if (!result.has_valid)
        return true;
    return result.valid;
}

//---------------------------------------------------------------------------
bool MainWindow::validate_policies(const std::string& file, const std::string& display_name,
                                   const std::string& display_content,
                                   std::string& report)
{
    const std::string* dname = display_name.length() ? &display_name : NULL;
    const std::string* dcontent = display_content.length() ? &display_content : NULL;

    std::vector<std::string> files;
    files.push_back(file);

    MediaConchLib::ReportRes result;
    std::bitset<MediaConchLib::report_Max> report_set;
    std::vector<std::string> vec;
    if (MCL.get_report(report_set, MediaConchLib::format_Xml, files,
                       policies, vec,
                       &result, dname, dcontent) < 0)
        return false;

    report = result.report;

    if (!result.has_valid)
        return true;
    return result.valid;
}

}
