/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menumainwindow.h"
#include "checkerwindow.h"
#include "resultwindow.h"
#include "policieswindow.h"
#include "displaywindow.h"
#include "helpwindow.h"
#include "Common/ImplementationReportDisplayHtmlXsl.h"

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
#include <QTimer>
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

    MCL.init();
    // Core configuration
    if (!MCL.get_implementation_schema_file().length())
        MCL.create_default_implementation_schema();
    // Verbosity option
    if (!MCL.get_implementation_verbosity().length())
        MCL.set_implementation_verbosity("5");

    // Groups
    QActionGroup* ToolGroup = new QActionGroup(this);
    ToolGroup->addAction(ui->actionChecker);
    ToolGroup->addAction(ui->actionResult);
    ToolGroup->addAction(ui->actionPolicies);
    ToolGroup->addAction(ui->actionDisplay);
    
    // Visual elements
    Layout=(QVBoxLayout*)ui->centralWidget->layout();
    Layout->setContentsMargins(0, 0, 0, 0);
    MenuView = new MenuMainWindow(this);
    checkerView=NULL;
    resultView=NULL;
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

    // Status bar
    statusBar()->show();
    status_msg = new QLineEdit(statusBar());
    status_msg->setReadOnly(true);
    statusBar()->addWidget(status_msg, 2);
    clear_msg_in_status_bar();

    // Connect the signal 
    connect(this, SIGNAL(setResultView()), this, SLOT(on_actionResult_triggered()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//***************************************************************************
// Functions
//***************************************************************************

//---------------------------------------------------------------------------
void MainWindow::add_file_to_list(const QString& file, const QString& path, const QString& policy, const QString& display)
{
    std::string full_file(path.toStdString());
    if (path.length())
        full_file += "/";
    full_file += file.toStdString();

    bool exists = false;
    FileRegistered *fr = get_file_registered_from_file(full_file);
    if (!fr)
        fr = new FileRegistered;
    else
        exists = true;

    fr->filename = file.toStdString();
    fr->filepath = path.toStdString();
    fr->policy = policy.toInt();
    fr->display = display.toInt();

    if (exists)
        return;

    registered_files.push_back(fr);
    std::vector<std::string> vec;
    vec.push_back(full_file);
    analyze(vec);
}

//---------------------------------------------------------------------------
void MainWindow::remove_file_to_list(const QString& file)
{
    std::string filename = file.toStdString();
    remove_file_registered_from_file(filename);
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
            createCheckerView();
            break;
        case RUN_RESULT_VIEW:
            //TODO: fill the view if file already here
            // if (!registered_files.empty())
            //     C.Run();
            createResultView();
            break;
        case RUN_POLICIES_VIEW:
            createPoliciesView();
            break;
        case RUN_DISPLAY_VIEW:
            createDisplayView();
            break;
        default:
            createCheckerView();
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
const std::vector<MainWindow::FileRegistered*>& MainWindow::get_registered_files()
{
    return registered_files;
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
int MainWindow::exporting_to_unknown_file(size_t pos)
{
    QString path = get_local_folder();
    path += "/policies";

    QDir dir(path);
    if (!dir.exists())
        dir.mkpath(dir.absolutePath());

    Policy* p = MCL.get_policy(pos);
    if (!p)
        return -1;

    path += "/" + QString().fromStdString(p->filename);
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Policy"),
                                              path, tr("XML (*.xml)"));

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
    checkerView->set_display_xslt(display_xslt);
}

//---------------------------------------------------------------------------
void MainWindow::remove_xslt_display()
{
    checkerView->reset_display_xslt();
}

//---------------------------------------------------------------------------
void MainWindow::clear_file_list()
{
    for (size_t i = 0; i < registered_files.size(); ++i)
        delete registered_files[i];
    registered_files.clear();
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
    QStringList list = QFileDialog::getOpenFileNames(this, "Open file", "", "Video files (*.avi *.mkv *.mov *.mxf *.mp4);;All (*.*)", 0, QFileDialog::DontUseNativeDialog);
    if (list.empty())
        return;

    for (int pos = 0; pos < list.size(); ++pos)
    {
        QFileInfo file(list[pos]);
        FileRegistered *fr = new FileRegistered;
        fr->filename = file.fileName().toStdString();
        fr->filepath = file.absolutePath().toStdString();
        registered_files.push_back(fr);
    }

    current_view = RUN_CHECKER_VIEW;
    Run();

    if (!checkerView)
        return;
    checkerView->change_local_files(list);
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
void MainWindow::on_actionResult_triggered()
{
    if (!ui->actionResult->isChecked())
        ui->actionResult->setChecked(true);
    if (clearVisualElements() < 0)
        return;
    current_view = RUN_RESULT_VIEW;
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
        set_msg_error_to_status_bar("Policy not valid");

    if (!ui->actionPolicies->isChecked())
        ui->actionPolicies->setChecked(true);
    current_view = RUN_POLICIES_VIEW;
    Run();
}

//---------------------------------------------------------------------------
void MainWindow::set_result_view()
{
#if defined(WEB_MACHINE_KIT)
    // Q_EMIT setResultView();
#endif
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
    if (checkerView)
    {
        delete checkerView;
        checkerView = NULL;
    }

    if (resultView)
    {
        delete resultView;
        resultView = NULL;
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
void MainWindow::createCheckerView()
{
    if (checkerView)
        return;

    if (clearVisualElements() < 0)
        return;
    checkerView = new CheckerWindow(this);
    checkerView->create_web_view();
}

//---------------------------------------------------------------------------
void MainWindow::createResultView()
{
    if (clearVisualElements() < 0)
        return;
    resultView = new ResultWindow(this);
    resultView->set_default_update_timer(MCL.get_ui_poll_request());
    resultView->display_results();
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

void MainWindow::set_msg_to_status_bar(const QString& message)
{
    statusBar()->clearMessage();
    status_msg->setText(message);
    QTimer::singleShot(5000, this, SLOT(update_status_bar()));
}

void MainWindow::set_msg_error_to_status_bar(const QString& message)
{
    statusBar()->clearMessage();

    QPalette pal = status_msg->palette();
    pal.setColor(QPalette::WindowText, Qt::red);
    pal.setColor(QPalette::Text, Qt::red);
    status_msg->setPalette(pal);
    status_msg->setText(message);

    QTimer::singleShot(5000, this, SLOT(update_status_bar()));
}

void MainWindow::clear_msg_in_status_bar()
{
    statusBar()->clearMessage();
    QPalette pal = status_msg->palette();
    pal.setColor(QPalette::WindowText, Qt::black);
    status_msg->setPalette(pal);
    status_msg->setText(QString());
}

QStatusBar *MainWindow::get_status_bar()
{
    return statusBar();
}

void MainWindow::update_status_bar()
{
    clear_msg_in_status_bar();
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
void MainWindow::result_selected()
{
    on_actionResult_triggered();
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
int MainWindow::analyze(const std::vector<std::string>& files)
{
    return MCL.analyze(files);
}

//---------------------------------------------------------------------------
int MainWindow::is_analyze_finished(const std::vector<std::string>& files, double& percent_done)
{
    return MCL.is_done(files, percent_done);
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
void MainWindow::get_implementation_report(const std::string& file, QString& report)
{
    FileRegistered *fr = get_file_registered_from_file(file);
    if (!fr || !fr->analyzed)
        return;

    std::string display_content;
    std::string display_name;
    const std::string* dname = NULL;
    const std::string* dcontent = NULL;
    fill_display_used(display_name, display_content, dname, dcontent, fr);

    std::bitset<MediaConchLib::report_Max> report_set;
    report_set.set(MediaConchLib::report_MediaConch);

    std::vector<std::string> files;
    files.push_back(file);

    MediaConchLib::ReportRes result;
    std::vector<std::string> vec;
    MCL.get_report(report_set, MediaConchLib::format_Xml, files,
                   vec, vec,
                   &result, dname, dcontent);

    report = QString().fromUtf8(result.report.c_str(), result.report.length());
}

//---------------------------------------------------------------------------
int MainWindow::validate_policy(const std::string& file, QString& report, int policy)
{
    FileRegistered* fr = get_file_registered_from_file(file);
    if (!fr || !fr->analyzed)
        return -1;

    if (policy == -1)
    {
        if (fr->policy == -1)
            return -1;
        policy = fr->policy;
    }

    Policy* p = get_policy((size_t)policy);
    if (!p)
    {
        report = QString("Policy not found");
        return 1;
    }
    std::string policy_content;
    p->dump_schema(policy_content);

    std::string display_content;
    std::string display_name;
    const std::string* dname = NULL;
    const std::string* dcontent = NULL;
    fill_display_used(display_name, display_content, dname, dcontent, fr);
    std::vector<std::string> files;
    files.push_back(file);

    MediaConchLib::ReportRes result;
    std::bitset<MediaConchLib::report_Max> report_set;
    std::vector<std::string> policies_contents;
    policies_contents.push_back(policy_content);

    std::vector<std::string> vec;

    if (MCL.get_report(report_set, MediaConchLib::format_Xml, files,
                       vec, policies_contents,
                       &result, dname, dcontent) < 0)
        return 0;

    report = QString().fromStdString(result.report);

    if (!result.has_valid)
        return 1;
    if (result.valid)
        return 1;
    return 0;
}

//---------------------------------------------------------------------------
MainWindow::FileRegistered* MainWindow::get_file_registered_from_file(const std::string& file)
{
    FileRegistered* fr = NULL;
    for (size_t i = 0; i < registered_files.size(); ++i)
    {
        std::string f = registered_files[i]->filepath + "/" + registered_files[i]->filename;
        if (f == file)
        {
            fr = registered_files[i];
            break;
        }
    }

    if (!fr)
        return fr;

    double percent;
    std::vector<std::string> files;
    files.push_back(file);
    int ret = is_analyze_finished(files, percent);
    if (ret < 0)
    {
        set_error_http((MediaConchLib::errorHttp)ret);
        return fr;
    }
    fr->analyzed = false;
    if (ret == MediaConchLib::errorHttp_TRUE)
    {
        fr->analyzed = true;
        std::vector<std::string> policies_names, policies_contents;
        std::vector<MediaConchLib::ValidateRes*> res;

        if (MCL.validate(MediaConchLib::report_MediaConch, files,
                     policies_names, policies_contents, res) == 0 && res.size() == 1)
            fr->implementation_valid = res[0]->valid;

        for (size_t i = 0; i < res.size() ; ++i)
            delete res[i];
        res.clear();
        if (fr->policy >= 0)
        {
            Policy *p = get_policy((size_t)fr->policy);
            if (p)
            {
                std::string policy_content;
                p->dump_schema(policy_content);
                policies_contents.push_back(policy_content);
            }

            if (p && MCL.validate(MediaConchLib::report_Max, files,
                         policies_names, policies_contents, res) == 0 && res.size() == 1)
                fr->policy_valid = res[0]->valid;
            for (size_t i = 0; i < res.size() ; ++i)
                delete res[i];
            res.clear();
        }
    }

    return fr;
}

//---------------------------------------------------------------------------
void MainWindow::remove_file_registered_from_file(const std::string& file)
{
    FileRegistered* fr = NULL;
    size_t pos = 0;
    for (size_t i = 0; i < registered_files.size(); ++i)
    {
        std::string f = registered_files[i]->filepath;
        if (f.length())
            f += "/";
        f += registered_files[i]->filename;

        if (f == file)
        {
            fr = registered_files[i];
            pos = i;
            break;
        }
    }

    if (!fr)
        return;

    registered_files[pos] = NULL;
    delete fr;
    registered_files.erase(registered_files.begin() + pos);
}

//---------------------------------------------------------------------------
void MainWindow::fill_display_used(std::string&, std::string& display_content,
                                   const std::string*& dname, const std::string*& dcontent,
                                   FileRegistered* fr)
{
    if (fr && fr->display > 0 && (size_t)fr->display < displays_list.size())
    {
        QFile display_xsl(displays_list[fr->display]);
        display_xsl.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray xsl = display_xsl.readAll();
        display_xsl.close();
        display_content = QString(xsl).toStdString();
    }
    else
        display_content = std::string(implementation_report_display_html_xsl);
    dcontent = &display_content;
    dname = NULL;
}

//---------------------------------------------------------------------------
void MainWindow::set_error_http(MediaConchLib::errorHttp code)
{
    QString error_msg;
    switch (code)
    {
        case MediaConchLib::errorHttp_INVALID_DATA:
            error_msg = "Data sent to the daemon is not correct";
            break;
        case MediaConchLib::errorHttp_INIT:
            error_msg = "Cannot initialize the HTTP connection";
            break;
        case MediaConchLib::errorHttp_CONNECT:
            error_msg = "Cannot connect to the daemon";
            break;
        default:
            error_msg = "Error not known";
            break;
    }
    set_msg_error_to_status_bar(error_msg);
}

}
