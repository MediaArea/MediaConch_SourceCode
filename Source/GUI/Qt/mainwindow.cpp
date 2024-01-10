/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingswindow.h"
#include "checkerwindow.h"
#include "policieswindow.h"
#include "publicpolicieswindow.h"
#include "displaywindow.h"
#include "databasewindow.h"
#include "helpwindow.h"
#include "WebView.h"
#include "WebPage.h"
#include "Common/Version.h"
#include "Common/Reports.h"
#include "Common/generated/ImplementationReportDisplayHtmlXsl.h"
#include "Common/FileRegistered.h"
#include "DatabaseUi.h"
#include "SQLLiteUi.h"
#include "NoDatabaseUi.h"

#include <QStringList>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QActionGroup>
#include <QSpinBox>
#include <QApplication>
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
#include <QTextStream>
#include <QThread>
#include <QScreen>
#include <QStandardPaths>
#include <QFontDatabase>
#include <sstream>

namespace MediaConch {

//***************************************************************************
// Constant
//***************************************************************************

const std::string MainWindow::database_filename = std::string("MediaConchUi.db");
const std::string MainWindow::version = MEDIACONCH_VERSION;

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    web_view(NULL),
    ui(new Ui::MainWindow),
    db(NULL),
    workerfiles(this),
    user(-1)
{
    ui->setupUi(this);

    std::string err;
    MCL.init(err);

    // GUI database
    create_and_configure_ui_database();
    workerfiles.set_database(db);
    uisettings.set_database(db);
    uisettings.init();

    // Core configuration
    if (!MCL.get_implementation_schema_file().length())
    {
        if (MCL.create_default_implementation_schema(err) != 0)
        {
            if (err == "Unable to write implementation schema file.")
                err += "\nCheck write permissions on MediaConch data directory.";

            QMessageBox msgBox(QMessageBox::Critical, tr("Initialisation error"),
                       QString().fromStdString(err), QMessageBox::Abort, parent);

            msgBox.exec();
            close();
        }
    }

    int nb_threads=QThread::idealThreadCount();
    if (nb_threads!=-1)
        MCL.set_default_scheduler_max_threads(nb_threads);

    // Load policy
    if (!MCL.get_use_daemon())
    {
        MCL.load_system_policy();
        MCL.load_existing_policy();
    }

    // Groups
    QActionGroup* ToolGroup = new QActionGroup(this);
    ToolGroup->addAction(ui->actionChecker);
    ToolGroup->addAction(ui->actionPolicies);
    ToolGroup->addAction(ui->actionPublicPolicies);
    ToolGroup->addAction(ui->actionDisplay);
    ToolGroup->addAction(ui->actionDatabase);
    ToolGroup->addAction(ui->actionSettings);

    // Visual elements
    Layout=(QVBoxLayout*)ui->centralWidget->layout();
    Layout->setContentsMargins(0, 0, 0, 0);
    Layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    checkerView=NULL;
    policiesView = NULL;
    publicPoliciesView = NULL;
    displayView = NULL;
    databaseView = NULL;
    settingsView = NULL;

    // Window
    setWindowIcon(QIcon(":/icon/icon.png"));
    int left=70;
    int width=QGuiApplication::primaryScreen()->geometry().width();
    if (width>1366)
    {
        left+=(width-1366)/2;
        width=1366;
    }
    move(left, 70);
    resize(width-140, QGuiApplication::primaryScreen()->geometry().height()-140);
    setAcceptDrops(false);

    // Status bar
    statusBar()->show();
    clear_msg_in_status_bar();
    connect(this, SIGNAL(status_bar_show_message(const QString&, int)),
            statusBar(), SLOT(showMessage(const QString&, int)));
    connect(this, SIGNAL(status_bar_clear_message()),
            statusBar(), SLOT(clearMessage()));

    // worker load existing files
    workerfiles.fill_registered_files_from_db();
    workerfiles.start();

    // WebView
    web_view = new WebView(this);
    WebPage* page = new WebPage(this, web_view);
    web_view->setPage(page);
#if defined(WEB_MACHINE_ENGINE)
    QWebChannel *channel = new QWebChannel(page);
    page->setWebChannel(channel);
    channel->registerObject("webpage", page);
#endif
    connect(web_view, SIGNAL(loadProgress(int)), this, SLOT(loadProgress_Custom(int)));
    connect(web_view, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished_Custom(bool)));
    web_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    set_widget_to_layout(web_view);

    // Default
    add_default_displays();
    on_actionChecker_triggered();
}

MainWindow::~MainWindow()
{
    workerfiles.quit();
    workerfiles.wait();
    delete ui;
    clearVisualElements();

    if (web_view)
    {
        remove_widget_from_layout(web_view);
#if defined(WEB_MACHINE_ENGINE)
        WebPage* page = (WebPage*)web_view->page();
        QWebChannel *channel = page ? page->webChannel() : NULL;
        if (channel)
            channel->deregisterObject(page);
#endif
        delete web_view;
        web_view = NULL;
    }
}

//***************************************************************************
// Functions
//***************************************************************************
//---------------------------------------------------------------------------
bool MainWindow::mil_has_curl_enabled()
{
    return MCL.mil_has_curl_enabled();
}

//---------------------------------------------------------------------------
unsigned long long MainWindow::mil_version()
{
    return MCL.mil_version();
}

//---------------------------------------------------------------------------
bool MainWindow::close()
{
    clearVisualElements();
    return QWidget::close();
}


//---------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent *event)
{
    clearVisualElements();
    QWidget::closeEvent(event);
}

//---------------------------------------------------------------------------
int MainWindow::add_file_to_list(const QString& file, const QString& path,
                                 const QString& policy, const QString& display,
                                 const QString& v, bool fixer, bool create_policy,
                                 const QStringList& options, std::string& err)
{
    std::string filename = std::string(file.toUtf8().data(), file.toUtf8().length());
    std::string filepath = std::string(path.toUtf8().data(), path.toUtf8().length());
    int display_i = display.toInt();
    int verbosity_i = v.toInt();

    // Save configuration used
    std::string policy_str;
    uisettings.change_last_policy(policy.toUtf8().data());
    uisettings.change_last_display(displays_list[display_i].toUtf8().data());
    uisettings.change_last_verbosity(verbosity_i);

    int parsespeed_idx = options.lastIndexOf("file_parsespeed");
    if (parsespeed_idx != -1 && parsespeed_idx < options.size())
        uisettings.change_last_parsespeed(options[parsespeed_idx + 1].toStdString());
    else
        uisettings.change_last_parsespeed("");

    std::string full_path = filepath;
#ifdef WINDOWS
    if (full_path.length() && full_path[full_path.size() - 1] != '/' && full_path[full_path.size() - 1] != '\\')
        full_path += "/";
#else
    if (full_path.length())
        full_path += "/";
#endif
    full_path += filename;

    bool force = fixer;
    std::vector<std::string> opt_vec;
    for (int i = 0; i < options.size(); ++i)
        opt_vec.push_back(options[i].toUtf8().data());
    if (workerfiles.add_file_to_list(filename, filepath, policy.toInt(), display_i, verbosity_i, fixer, force, create_policy, opt_vec, err) < 0)
        return -1;
    return 0;
}

//---------------------------------------------------------------------------
int MainWindow::add_file_to_list(long id, std::string& err)
{
    std::string full_file;
    if (checker_file_from_id(id, full_file, err) < 0)
        return -1;

    QFileInfo info(QString().fromUtf8(full_file.c_str(), full_file.size()));
    return workerfiles.add_file_to_list(id, full_file, info.absolutePath().toUtf8().data(), info.fileName().toUtf8().data(), err);
}

//---------------------------------------------------------------------------
int MainWindow::add_attachment_to_list(const QString& file, int policy, int display,
                                       int verbosity, const QStringList& options, std::string& err)
{
    std::vector<std::string> opt_vec;
    for (int i = 0; i < options.size(); ++i)
        opt_vec.push_back(options[i].toUtf8().data());

    std::string filename = std::string(file.toUtf8().data(), file.toUtf8().length());
    if (workerfiles.add_attachment_to_list(filename, policy, display, verbosity, opt_vec, err) < 0)
        return -1;
    return 0;
}

//---------------------------------------------------------------------------
void MainWindow::remove_file_to_list(long file_id)
{
    workerfiles.remove_file_registered_from_id(file_id);
}

//---------------------------------------------------------------------------
void MainWindow::remove_all_files_to_list()
{
    workerfiles.remove_all_files_registered();
}

//---------------------------------------------------------------------------
int MainWindow::update_policy_of_file_in_list(long file_id, long policy, std::string& error)
{
    return workerfiles.update_policy_of_file_registered_from_file(file_id, policy, error);
}

//---------------------------------------------------------------------------
int MainWindow::analyze_force_file_to_list(long id, std::string& err)
{
    std::string full_path;
    if (checker_file_from_id(id, full_path, err) < 0)
        return -1;

    FileRegistered *fr = get_file_registered_from_file(full_path);
    if (!fr)
    {
        err = "File ID not registered";
        return -1;
    }
    if (db)
        db->ui_reset_file_for_reload(fr);

    if (workerfiles.add_file_to_list(fr->filename, fr->filepath, fr->policy, fr->display, fr->verbosity, false, true, false, fr->options, err) < 0)
        return -1;

    return 0;
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
        case RUN_POLICIES_VIEW:
            createPoliciesView();
            break;
        case RUN_PUBLIC_POLICIES_VIEW:
            createPublicPoliciesView();
            break;
        case RUN_DISPLAY_VIEW:
            createDisplayView();
            break;
        case RUN_DATABASE_VIEW:
            createDatabaseView();
            break;
        case RUN_SETTINGS_VIEW:
            createSettingsView();
            break;
        default:
            createCheckerView();
            break;
    }
}

//---------------------------------------------------------------------------
void MainWindow::get_registered_files(std::map<std::string, FileRegistered*>& files)
{
    workerfiles.get_registered_files(files);
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
    QString suggested = QString().fromUtf8(select_correct_load_policy_path().c_str());
    QString file = QFileDialog::getOpenFileName(this, "Open file", suggested, "XSL file (*.xsl);;All (*.*)", 0, QFileDialog::Option(0));

    if (file.length())
    {
        QDir info(QFileInfo(file).absoluteDir());
        set_last_load_policy_path(info.absolutePath().toUtf8().data());
    }

    return file;
}

//---------------------------------------------------------------------------
long MainWindow::xslt_policy_create_from_file(long file_id, QString& err)
{
    long pos = -1;

    std::string error;
    if (file_id >= 0)
        if ((pos = MCL.xslt_policy_create_from_file(user, file_id, error)) < 0)
            err = QString().fromUtf8(error.c_str(), error.size());

    QMessageBox::Icon icon;
    QString text;
    if (pos != -1)
    {
        icon = QMessageBox::Information;
        text = QString("Policy from %1 is created, you can find it in the \"Policies\" tab")
            .arg(QString().fromUtf8(get_filename_from_registered_file_id(file_id).c_str()));
    }
    else
    {
        icon = QMessageBox::Critical;
        text = err;
    }

    QMessageBox msgBox(icon, tr("Create policy"), text,
                       QMessageBox::NoButton, this);
    msgBox.exec();
    return pos;
}

//---------------------------------------------------------------------------
void MainWindow::create_policy_from_file(const FileRegistered* file)
{
    if (!file)
        return;

    long id = -1;
    std::string error;
    if ((id = MCL.xslt_policy_create_from_file(user, file->file_id, error)) < 0)
        set_str_msg_to_status_bar(error);

    if (policiesView)
    {
        QString script = QString("policyTreeAjax.policyCreateFromFileCallback(%1);").arg(id);
        Q_EMIT execute_javascript(script);
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

    QString path = QString().fromUtf8(Core::get_local_data_path().c_str());
    path += "displays/";

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
    if (checkerView)
        checkerView->set_display_xslt(display_xslt);
}

//---------------------------------------------------------------------------
void MainWindow::remove_xslt_display()
{
    if (checkerView)
        checkerView->reset_display_xslt();
}

//---------------------------------------------------------------------------
void MainWindow::clear_file_list()
{
    workerfiles.clear_files();
}

//---------------------------------------------------------------------------
std::vector<QString>& MainWindow::get_displays()
{
    return displays_list;
}

//---------------------------------------------------------------------------
int MainWindow::display_add_file(const QString& name, const QString& filename)
{
    if (!displayView)
        return -1;

    return displayView->add_new_file(name, filename);
}

//---------------------------------------------------------------------------
void MainWindow::display_export_id(const QString& name)
{
    if (!displayView)
        return;

    displayView->export_file(name);
}

//---------------------------------------------------------------------------
void MainWindow::display_delete_id(const QString& name)
{
    if (!displayView)
        return;

    displayView->delete_file(name);
}

//---------------------------------------------------------------------------
int MainWindow::get_display_index_by_filename(const std::string& filename)
{
    std::string tmp(filename);
    if (!tmp.size())
        tmp = ":/displays/MediaConchHtml.xsl";

    for (size_t i = 0; i < displays_list.size(); ++i)
    {
        if (tmp == displays_list[i].toUtf8().data())
            return i;
    }

    return 0;
}

//---------------------------------------------------------------------------
UiSettings& MainWindow::get_settings()
{
    return uisettings;
}

int MainWindow::get_values_for_type_field(const std::string& type, const std::string& field, std::vector<std::string>& values)
{
    std::string err;
    return MCL.policy_get_values_for_type_field(type, field, values, err);
}

int MainWindow::get_fields_for_type(const std::string& type, std::vector<std::string>& fields)
{
    std::string err;
    return MCL.policy_get_fields_for_type(type, fields, err);
}

//***************************************************************************
// Slots
//***************************************************************************

//---------------------------------------------------------------------------
void MainWindow::on_actionOpen_triggered()
{
    QStringList list = QFileDialog::getOpenFileNames(this, "Open file", "", "Video files (*.avi *.mkv *.mov *.mxf *.mp4);;All (*.*)", 0, QFileDialog::Option(0));
    if (list.empty())
        return;

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
void MainWindow::on_actionPublicPolicies_triggered()
{
    if (!ui->actionPublicPolicies->isChecked())
        ui->actionPublicPolicies->setChecked(true);
    if (clearVisualElements() < 0)
        return;
    current_view = RUN_PUBLIC_POLICIES_VIEW;
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
void MainWindow::on_actionDatabase_triggered()
{
    if (!ui->actionDatabase->isChecked())
        ui->actionDatabase->setChecked(true);
    if (clearVisualElements() < 0)
        return;
    current_view = RUN_DATABASE_VIEW;
    Run();
}

//---------------------------------------------------------------------------
void MainWindow::on_actionSettings_triggered()
{
    if (!ui->actionSettings->isChecked())
        ui->actionSettings->setChecked(true);
    if (clearVisualElements() < 0)
        return;
    current_view = RUN_SETTINGS_VIEW;
    Run();
}

//---------------------------------------------------------------------------
void MainWindow::on_actionChooseSchema_triggered()
{
    QString file = ask_for_schema_file();
    if (!file.length())
        return;

    QString err;
    if (policy_import(file, err) < 0)
        set_msg_to_status_bar(err);

    if (!ui->actionPolicies->isChecked())
        ui->actionPolicies->setChecked(true);
    current_view = RUN_POLICIES_VIEW;
    Run();
}

//---------------------------------------------------------------------------
void MainWindow::loadFinished_Custom(bool ok)
{
    if (!ok)
    {
        set_msg_to_status_bar("Problem to load the checker page");
        return;
    }

    if (checkerView)
        checkerView->create_web_view_finished();
    if (policiesView)
        policiesView->create_web_view_finished();
    if (publicPoliciesView)
        publicPoliciesView->create_web_view_finished();
    if (displayView)
        displayView->create_web_view_finished();
    if (databaseView)
        databaseView->create_web_view_finished();
    if (settingsView)
        settingsView->create_web_view_finished();
}

//---------------------------------------------------------------------------
void MainWindow::loadProgress_Custom(int progress)
{
    if (progress == 100)
        status_bar_clear_message();
    else
        set_msg_to_status_bar(QString::number(progress) + "%");
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

    if (policiesView)
    {
        delete policiesView;
        policiesView = NULL;
    }

    if (publicPoliciesView)
    {
        delete publicPoliciesView;
        publicPoliciesView = NULL;
    }

    if (displayView)
    {
        delete displayView;
        displayView = NULL;
    }

    if (databaseView)
    {
        delete databaseView;
        databaseView = NULL;
    }

    if (settingsView)
    {
        delete settingsView;
        settingsView = NULL;
    }

    return 0;
}

//---------------------------------------------------------------------------
void MainWindow::createCheckerView()
{
    if (clearVisualElements() < 0)
        return;

    checkerView = new CheckerWindow(this);
    checkerView->create_web_view();
}

//---------------------------------------------------------------------------
void MainWindow::createPoliciesView()
{
    if (clearVisualElements() < 0)
        return;

    MCL.reset_daemon_client();
    policiesView = new PoliciesWindow(this);
    connect(this, SIGNAL(execute_javascript(const QString&)),
            policiesView, SLOT(use_javascript(const QString&)));
    policiesView->display_policies();
}

//---------------------------------------------------------------------------
void MainWindow::createPublicPoliciesView()
{
    if (clearVisualElements() < 0)
        return;

    MCL.reset_daemon_client();
    publicPoliciesView = new PublicPoliciesWindow(this);
    publicPoliciesView->display_public_policies();
}

//---------------------------------------------------------------------------
void MainWindow::createDisplayView()
{
    if (clearVisualElements() < 0)
        return;

    displayView = new DisplayWindow(this);
    displayView->display_display();
}

//---------------------------------------------------------------------------
void MainWindow::createDatabaseView()
{
    if (clearVisualElements() < 0)
        return;

    databaseView = new DatabaseWindow(this);
    databaseView->display_database();
}

//---------------------------------------------------------------------------
void MainWindow::createSettingsView()
{
    if (clearVisualElements() < 0)
        return;

    settingsView = new SettingsWindow(this);
    settingsView->display_settings();
}

//---------------------------------------------------------------------------
void MainWindow::set_msg_to_status_bar(const QString& message)
{
    if (message.isEmpty())
        clear_msg_in_status_bar();
    else
        Q_EMIT status_bar_show_message(message, 5000);
}

//---------------------------------------------------------------------------
void MainWindow::set_str_msg_to_status_bar(const std::string& message)
{
    set_msg_to_status_bar(QString::fromStdString(message));
}

//---------------------------------------------------------------------------
void MainWindow::clear_msg_in_status_bar()
{
    Q_EMIT status_bar_clear_message();
}

//---------------------------------------------------------------------------
void MainWindow::drag_and_drop_files_action(const QStringList& files)
{
    if (checkerView)
        checkerView->change_local_files(files);

    if (displayView)
        displayView->add_new_files(files);

    if (databaseView)
    {
        //nothing to do
    }

    if (settingsView)
    {
        //nothing to do
    }

    if (policiesView)
        policiesView->add_new_policies(files);

    if (publicPoliciesView)
    {
        //nothing to do
    }
}

//***************************************************************************
// HELPER
//***************************************************************************

//---------------------------------------------------------------------------
void MainWindow::set_widget_to_layout(QWidget* w)
{
    w->setSizePolicy(QSizePolicy::MinimumExpanding,
                     QSizePolicy::MinimumExpanding);
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
void MainWindow::public_policies_selected()
{
    on_actionPublicPolicies_triggered();
}

//---------------------------------------------------------------------------
void MainWindow::display_selected()
{
    on_actionDisplay_triggered();
}

//---------------------------------------------------------------------------
void MainWindow::database_selected()
{
    on_actionDatabase_triggered();
}

//---------------------------------------------------------------------------
void MainWindow::settings_selected()
{
    on_actionSettings_triggered();
}

//---------------------------------------------------------------------------
int MainWindow::get_policies(const std::string& format, MediaConchLib::Get_Policies& policies, QString& err)
{
    std::vector<int> ids;
    std::string error;
    int ret = MCL.policy_get_policies(user, ids, format, policies, error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::policy_import(const QString& filename, QString& err)
{
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        err = "Policy cannot be read";
        return -1;
    }

    QByteArray schema = file.readAll();
    file.close();

    std::string memory(schema.constData(), schema.length());
    std::string error;

    int ret = MCL.policy_import(user, memory, error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::policy_import_data(const QString& data, QString& err)
{
    std::string error;
    int ret = MCL.policy_import(user, data.toUtf8().data(), error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::xslt_policy_create(int parent_id, QString& err)
{
    std::string error;
    int ret = MCL.xslt_policy_create(user, error, "and", parent_id);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::policy_duplicate(int id, int dst_policy_id, QString& err)
{
    std::string error;
    int ret = MCL.policy_duplicate(user, id, dst_policy_id, NULL, false, error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::policy_move(int id, int dst_policy_id, QString& err)
{
    std::string error;
    int ret = MCL.policy_move(user, id, dst_policy_id, error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::policy_change_info(int id, const std::string& name, const std::string& description, const std::vector<std::string>& tags,
                                   const std::string& level, const std::string& license, QString& err)
{
    std::string error;
    int ret = MCL.policy_change_info(user, id, name, description, tags, level, license, error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::policy_change_type(int id, const std::string& type, QString& err)
{
    std::string error;
    int ret = MCL.policy_change_type(user, id, type, error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::policy_change_is_public(int id, bool is_public, QString& err)
{
    std::string error;
    int ret = MCL.policy_change_is_public(user, id, is_public, error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::xslt_policy_rule_create(int policy_id, QString& err)
{
    std::string error;
    int ret = MCL.xslt_policy_rule_create(user, policy_id, error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
XsltPolicyRule *MainWindow::xslt_policy_rule_get(int policy_id, int rule_id, QString& err)
{
    std::string error;
    XsltPolicyRule *ret = MCL.xslt_policy_rule_get(user, policy_id, rule_id, error);
    if (!ret)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::xslt_policy_rule_edit(int policy_id, int rule_id, const XsltPolicyRule *rule, QString& err)
{
    std::string error;
    int ret = MCL.xslt_policy_rule_edit(user, policy_id, rule_id, rule, error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::xslt_policy_rule_duplicate(int policy_id, int rule_id, int dst_policy_id, QString& err)
{
    std::string error;
    int ret = MCL.xslt_policy_rule_duplicate(user, policy_id, rule_id, dst_policy_id, error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::xslt_policy_rule_move(int policy_id, int rule_id, int dst_policy_id, QString& err)
{
    std::string error;
    int ret = MCL.xslt_policy_rule_move(user, policy_id, rule_id, dst_policy_id, error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::xslt_policy_rule_delete(int policy_id, int rule_id, QString& err)
{
    std::string error;
    int ret = MCL.xslt_policy_rule_delete(user, policy_id, rule_id, error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::policy_get(int pos, const std::string& format, MediaConchLib::Get_Policy& p,
                           QString& err)
{
    std::string error;
    int ret = MCL.policy_get(user, pos, format, false, p, error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::policy_remove(int id, QString& err)
{
    std::string error;
    int ret = MCL.policy_remove(user, id, error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::policy_save(int pos, QString& err)
{
    std::string error;
    int ret = MCL.policy_save(user, pos, error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::policy_dump(int pos, std::string& memory, QString& err)
{
    std::string error;
    int ret = MCL.policy_dump(user, pos, false, memory, error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::policy_get_name(int pos, std::string& name, QString& err)
{
    std::string error;
    int ret = MCL.policy_get_name(user, pos, name, error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::policy_export(int pos, QString& err)
{
    std::string error;
    std::string policy;
    if (MCL.policy_dump(user, pos, false, policy, error) < 0)
    {
        err = QString().fromUtf8(error.c_str(), error.size());
        return -1;
    }

    std::string p_name;
    if (MCL.policy_get_name(user, pos, p_name, error) < 0)
    {
        err = QString().fromUtf8(error.c_str(), error.size());
        return -1;
    }

    int ret = policy_export_data(QString().fromUtf8(policy.c_str(), policy.length()),
                                 QString().fromUtf8(p_name.c_str(), p_name.length()),
                                 err);
    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::policy_export_data(const QString& policy, const QString& p_name, QString& err)
{
    QString suggested = QString().fromUtf8(select_correct_save_policy_path().c_str());
    suggested += "/" + p_name + ".xml";

    QString filename = QFileDialog::getSaveFileName(this, tr("Save Policy"),
                                                    suggested, tr("XML (*.xml)"));

    if (!filename.length())
    {
        err = "No file name selected.";
        return -1;
    }

    QDir info(QFileInfo(filename).absoluteDir());
    set_last_save_policy_path(info.absolutePath().toUtf8().data());

    QFile file(filename);
    if (file.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        QTextStream stream(&file);
        stream << policy;
    }
    else
    {
        err = "Cannot write to file.";
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int MainWindow::clear_policies(QString& err)
{
    std::string error;
    int ret = MCL.policy_clear_policies(user, error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
size_t MainWindow::get_policies_count(QString& err) const
{
    std::string error;
    int ret = MCL.policy_get_policies_count(user, error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::select_correct_policy()
{
    // Policy
    std::string policy = uisettings.get_default_policy();
    if (policy == "last")
        policy = uisettings.get_last_policy();

    if (!policy.length())
        return -1;

    return QString().fromUtf8(policy.c_str(), policy.length()).toInt();
}

//---------------------------------------------------------------------------
int MainWindow::select_correct_display()
{
    // Display
    std::string display = uisettings.get_default_display();
    if (display == "last")
        display = uisettings.get_last_display();
    return get_display_index_by_filename(display);
}

//---------------------------------------------------------------------------
int MainWindow::select_correct_verbosity()
{
    // Verbosity
    int verbosity = uisettings.get_default_verbosity();
    if (verbosity == -1)
        verbosity = uisettings.get_last_verbosity();
    return verbosity;
}

//---------------------------------------------------------------------------
std::string MainWindow::select_correct_parsespeed()
{
    // Parsespeed
    std::string parsespeed = uisettings.get_default_parsespeed();
    if (parsespeed == "last")
        parsespeed = uisettings.get_last_parsespeed();
    if (parsespeed.empty())
        parsespeed = "0.5";
    return parsespeed;
}

//---------------------------------------------------------------------------
std::string MainWindow::select_correct_save_report_path()
{
    // Save report path
    std::string path = uisettings.get_default_save_report_path();
    if (!path.length() || path == "last")
        path = uisettings.get_last_save_report_path();
    return path;
}

//---------------------------------------------------------------------------
std::string MainWindow::select_correct_save_policy_path()
{
    // Save policy path
    std::string path = uisettings.get_default_save_policy_path();
    if (!path.length() || path == "last")
        path = uisettings.get_last_save_policy_path();
    return path;
}

//---------------------------------------------------------------------------
std::string MainWindow::select_correct_save_display_path()
{
    // Save display path
    std::string path = uisettings.get_default_save_display_path();
    if (!path.length() || path == "last")
        path = uisettings.get_last_save_display_path();
    return path;
}

//---------------------------------------------------------------------------
std::string MainWindow::select_correct_load_files_path()
{
    // Load files path
    std::string path = uisettings.get_default_load_files_path();
    if (!path.length() || path == "last")
        path = uisettings.get_last_load_files_path();
    return path;
}

//---------------------------------------------------------------------------
std::string MainWindow::select_correct_load_policy_path()
{
    // Load policy path
    std::string path = uisettings.get_default_load_policy_path();
    if (!path.length() || path == "last")
        path = uisettings.get_last_load_policy_path();
    return path;
}

//---------------------------------------------------------------------------
std::string MainWindow::select_correct_load_display_path()
{
    // Load display path
    std::string path = uisettings.get_default_load_display_path();
    if (!path.length() || path == "last")
        path = uisettings.get_last_load_display_path();
    return path;
}

//---------------------------------------------------------------------------
void MainWindow::set_last_save_report_path(const std::string& path)
{
    // Save report path
    uisettings.change_last_save_report_path(path);
}

//---------------------------------------------------------------------------
void MainWindow::set_last_save_policy_path(const std::string& path)
{
    // Save policy path
    uisettings.change_last_save_policy_path(path);
}

//---------------------------------------------------------------------------
void MainWindow::set_last_save_display_path(const std::string& path)
{
    // Save display path
    uisettings.change_last_save_display_path(path);
}

//---------------------------------------------------------------------------
void MainWindow::set_last_load_policy_path(const std::string& path)
{
    // Save report path
    uisettings.change_last_load_policy_path(path);
}

//---------------------------------------------------------------------------
void MainWindow::set_last_load_files_path(const std::string& path)
{
    // Save report path
    uisettings.change_last_load_files_path(path);
}

//---------------------------------------------------------------------------
void MainWindow::set_last_load_display_path(const std::string& path)
{
    // Save report path
    uisettings.change_last_load_display_path(path);
}

//---------------------------------------------------------------------------
int MainWindow::analyze(const std::vector<std::string>& files, bool with_fixer, bool force,
                        const std::vector<std::string>& opt, std::vector<long>& files_id,
                        std::string& err)
{
    std::vector<std::string> plugins;
    std::vector<std::pair<std::string, std::string> > options;
    if (with_fixer)
    {
        options.push_back(std::make_pair("File_TryToFix", "1"));
        force = true;
    }

    for (size_t i = 0; i + 1 < opt.size();)
    {
        if (opt[i] != "File_TryToFix")
            options.push_back(std::make_pair(opt[i], opt[i + 1]));
        i += 2;
    }

    return MCL.checker_analyze(user, files, plugins, options, files_id, err, force);
}

//---------------------------------------------------------------------------
int MainWindow::is_analyze_finished(const std::vector<std::string>& files,
                                    std::vector<MediaConchLib::Checker_StatusRes>& res, std::string& err)
{
    std::vector<long> files_id;
    for (size_t i = 0; ; )
    {
        long id = workerfiles.get_id_from_registered_file(files[i]);
        if (id < 0)
            return -1;

        files_id.push_back(id);
    }

    return MCL.checker_status(user, files_id, res, err);
}

//---------------------------------------------------------------------------
int MainWindow::is_analyze_finished(const std::string& file, MediaConchLib::Checker_StatusRes& res,
                                    std::string& err)
{
    long id = workerfiles.get_id_from_registered_file(file);
    if (id < 0)
        return -1;

    return MCL.checker_status(user, id, res, err);
}

//---------------------------------------------------------------------------
int MainWindow::validate(MediaConchLib::report report, const std::vector<std::string>& files,
                         const std::vector<size_t>& policies_ids,
                         const std::vector<std::string>& policies_contents,
                         const std::map<std::string, std::string>& options,
                         std::vector<MediaConchLib::Checker_ValidateRes*>& result, std::string& err)
{
    std::vector<long> files_id;
    for (size_t i = 0; ; )
    {
        long id = workerfiles.get_id_from_registered_file(files[i]);
        if (id < 0)
            return -1;

        files_id.push_back(id);
    }

    return MCL.checker_validate(user, report, files_id, policies_ids, policies_contents, options, result, err);
}

//---------------------------------------------------------------------------
int MainWindow::validate(MediaConchLib::report report, const std::string& file,
                         const std::vector<size_t>& policies_ids,
                         const std::vector<std::string>& policies_contents,
                         const std::map<std::string, std::string>& options,
                         std::vector<MediaConchLib::Checker_ValidateRes*>& result, std::string& err)
{
    std::vector<long> files_id;
    long id = workerfiles.get_id_from_registered_file(file);
    if (id < 0)
        return -1;

    files_id.push_back(id);

    return MCL.checker_validate(user, report, files_id, policies_ids, policies_contents, options, result, err);
}

//---------------------------------------------------------------------------
int MainWindow::validate_policy(long file_id, size_t policy_id,
                                std::vector<MediaConchLib::Checker_ValidateRes*>& result, std::string& err)
{
    std::vector<long> files_id;
    files_id.push_back(file_id);

    std::vector<size_t> policies_ids;
    policies_ids.push_back(policy_id);

    std::vector<std::string> policies_contents;
    std::map<std::string, std::string> options;

    return MCL.checker_validate(user, MediaConchLib::report_Max, files_id, policies_ids,
                                policies_contents, options, result, err);
}

//---------------------------------------------------------------------------
int MainWindow::checker_clear(long id, QString& err)
{
    std::string error;
    std::vector<long> files;
    files.push_back(id);

    int ret = MCL.checker_clear(user, files, error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    else
        remove_file_to_list(id);

    return ret;
}

//---------------------------------------------------------------------------
int MainWindow::checker_clear(QString& err)
{
    std::string error;
    std::vector<long> files;

    int ret = MCL.checker_clear(user, files, error);
    if (ret < 0)
        err = QString().fromUtf8(error.c_str(), error.size());
    else
        remove_all_files_to_list();

    return ret;
}

//---------------------------------------------------------------------------
void MainWindow::checker_stop(const std::vector<long>& ids)
{
    std::string error;
    MCL.checker_stop(user, ids, error);
}

//---------------------------------------------------------------------------
int MainWindow::checker_file_from_id(long id, std::string& file, std::string& err)
{
    return MCL.checker_file_from_id(user, id, file, err);
}

//---------------------------------------------------------------------------
long MainWindow::checker_id_from_filename(const std::string& filename, const std::vector<std::string>& options, std::string& err)
{
    std::vector<std::pair<std::string, std::string> > opts;
    for (size_t i = 0; i + 1 < options.size(); i += 2)
        opts.push_back(std::make_pair(options[i], options[i + 1]));

    return MCL.checker_id_from_filename(user, filename, opts, err);
}

//---------------------------------------------------------------------------
int MainWindow::checker_file_information(long id, MediaConchLib::Checker_FileInfo& file, std::string& err)
{
    return MCL.checker_file_information(user, id, file, err);
}

//---------------------------------------------------------------------------
int MainWindow::get_mediainfo_output_list(std::string& output, std::string& err)
{
    return MCL.mil_get_output_list(output, err);
}

//---------------------------------------------------------------------------
QString MainWindow::get_mediainfo_and_mediatrace_xml(long file_id,
                                                     const std::string& display_name,
                                                     const std::string& display_content,
                                                     std::string& err)
{
    CheckerReport cr;
    MediaConchLib::Checker_ReportRes result;

    cr.user = user;
    cr.files.push_back(file_id);
    cr.format = MediaConchLib::format_Xml;
    cr.report_set.set(MediaConchLib::report_MediaInfo);
    cr.report_set.set(MediaConchLib::report_MediaTrace);

    std::string d_name = display_name;
    std::string d_content = display_content;
    cr.display_name = d_name.size() ? &d_name : NULL;
    cr.display_content = d_content.size() ? &d_content : NULL;

    MCL.checker_get_report(cr, &result, err);
    return QString().fromUtf8(result.report.c_str(), result.report.length());
}

//---------------------------------------------------------------------------
QString MainWindow::get_mediainfo_report(long file_id,
                                      const std::string& display_name,
                                      const std::string& display_content,
                                      const std::string& mi_inform,
                                      std::string& err)
{
    CheckerReport cr;
    MediaConchLib::Checker_ReportRes result;

    cr.user = user;
    cr.files.push_back(file_id);
    cr.format = MediaConchLib::format_Xml;
    cr.report_set.set(MediaConchLib::report_MediaInfo);

    std::string d_name = display_name;
    std::string d_content = display_content;
    std::string m_inform = mi_inform;
    cr.display_name = d_name.size() ? &d_name : NULL;
    cr.display_content = d_content.size() ? &d_content : NULL;
    cr.mi_inform = m_inform.size() ? &m_inform : NULL;

    MCL.checker_get_report(cr, &result, err);
    return QString().fromUtf8(result.report.c_str(), result.report.length());
}

//---------------------------------------------------------------------------
QString MainWindow::get_mediainfo_jstree(long file_id, std::string& err)
{
    CheckerReport cr;
    MediaConchLib::Checker_ReportRes result;

    cr.user = user;
    cr.files.push_back(file_id);
    cr.format = MediaConchLib::format_JsTree;
    cr.report_set.set(MediaConchLib::report_MediaInfo);

    MCL.checker_get_report(cr, &result, err);
    return QString().fromUtf8(result.report.c_str(), result.report.length());
}

//---------------------------------------------------------------------------
QString MainWindow::get_mediatrace_xml(long file_id,
                                       const std::string& display_name,
                                       const std::string& display_content,
                                       std::string& err)
{
    CheckerReport cr;
    MediaConchLib::Checker_ReportRes result;

    cr.user = user;
    cr.files.push_back(file_id);
    cr.format = MediaConchLib::format_Xml;
    cr.report_set.set(MediaConchLib::report_MediaTrace);

    std::string d_name = display_name;
    std::string d_content = display_content;
    cr.display_name = d_name.size() ? &d_name : NULL;
    cr.display_content = d_content.size() ? &d_content : NULL;

    MCL.checker_get_report(cr, &result, err);
    return QString().fromUtf8(result.report.c_str(), result.report.length());
}

//---------------------------------------------------------------------------
QString MainWindow::get_mediatrace_jstree(long file_id, std::string& err)
{
    CheckerReport cr;
    MediaConchLib::Checker_ReportRes result;

    cr.user = user;
    cr.files.push_back(file_id);
    cr.format = MediaConchLib::format_JsTree;
    cr.report_set.set(MediaConchLib::report_MediaTrace);

    MCL.checker_get_report(cr, &result, err);
    return QString().fromUtf8(result.report.c_str(), result.report.length());
}

//---------------------------------------------------------------------------
void MainWindow::get_implementation_report(long file_id, QString& report, std::string& err,
                                           int *display_p, int *verbosity)
{
    FileRegistered *fr = get_file_registered_from_id(file_id);
    if (!fr)
        return;

    if (!fr->analyzed)
    {
        delete fr;
        return;
    }

    CheckerReport cr;
    MediaConchLib::Checker_ReportRes result;

    cr.user = user;
    cr.files.push_back(file_id);
    cr.format = MediaConchLib::format_Xml;
    cr.report_set.set(fr->report_kind);

    std::string display_content;
    std::string display_name;
    fill_display_used(display_p, display_name, display_content, cr.display_name, cr.display_content, fr);

    fill_options_for_report(cr.options, verbosity);

    MCL.checker_get_report(cr, &result, err);

    report = QString().fromUtf8(result.report.c_str(), result.report.length());
    delete fr;
}

//---------------------------------------------------------------------------
int MainWindow::validate_policy(long file_id, QString& report, std::string& err, int policy, int *display_p)
{
    FileRegistered* fr = get_file_registered_from_id(file_id);
    if (!fr)
        return -1;

    if (!fr->analyzed)
    {
        delete fr;
        return -1;
    }

    if (policy == -1)
    {
        if (fr->policy == -1)
        {
            delete fr;
            return -1;
        }
        policy = fr->policy;
    }

    CheckerReport cr;
    MediaConchLib::Checker_ReportRes result;

    cr.user = user;
    cr.files.push_back(file_id);
    cr.format = MediaConchLib::format_Xml;

    std::string display_content;
    std::string display_name;
    fill_display_used(display_p, display_name, display_content, cr.display_name, cr.display_content, fr);
    delete fr;

    cr.policies_ids.push_back(policy);

    if (MCL.checker_get_report(cr, &result, err) < 0)
        return 0;


    report = QString().fromUtf8(result.report.c_str(), result.report.length());

    if (!result.has_valid)
        return 1;
    if (result.valid)
        return 1;
    return 0;
}

//---------------------------------------------------------------------------
void MainWindow::checker_list(std::vector<long>& files, QString& error)
{
    std::string err;
    std::vector<long> list;
    if (MCL.checker_list(user, list, err) < 0)
        error = QString().fromUtf8(err.c_str(), err.size());
    else
    {
        for (size_t i = 0; i < list.size(); ++i)
            files.push_back(list[i]);
    }
}

//---------------------------------------------------------------------------
FileRegistered* MainWindow::get_file_registered_from_file(const std::string& file)
{
    return workerfiles.get_file_registered_from_file(file);
}

//---------------------------------------------------------------------------
FileRegistered* MainWindow::get_file_registered_from_id(long id)
{
    return workerfiles.get_file_registered_from_id(id);
}

//---------------------------------------------------------------------------
std::string MainWindow::get_filename_from_registered_file_id(long file_id)
{
    return workerfiles.get_filename_from_registered_file_id(file_id);
}

//---------------------------------------------------------------------------
void MainWindow::fill_display_used(int *display_p, std::string&, std::string& display_content,
                                   std::string*& dname, std::string*& dcontent,
                                   FileRegistered* fr)
{
    if (display_p)
    {
        if (*display_p >= 0 && (size_t)*display_p < displays_list.size())
        {
            QFile display_xsl(displays_list[*display_p]);
            display_xsl.open(QIODevice::ReadOnly | QIODevice::Text);
            QByteArray xsl = display_xsl.readAll();
            display_xsl.close();
            display_content = xsl.data();
        }
        else
            display_content = std::string(implementation_report_display_html_xsl);
    }
    else if (fr && fr->display > 0 && (size_t)fr->display < displays_list.size())
    {
        QFile display_xsl(displays_list[fr->display]);
        display_xsl.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray xsl = display_xsl.readAll();
        display_xsl.close();
        display_content = QString(xsl).toUtf8().data();
    }
    else
        display_content = std::string(implementation_report_display_html_xsl);
    dcontent = &display_content;
    dname = NULL;
}

//---------------------------------------------------------------------------
int MainWindow::get_ui_database_path(std::string& path)
{
    return MCL.get_ui_database_path(path);
}

//---------------------------------------------------------------------------
void MainWindow::get_version(std::string& v)
{
    v = version;
}

//---------------------------------------------------------------------------
void MainWindow::create_and_configure_ui_database()
{
#ifdef HAVE_SQLITE
    std::string db_path;
    if (get_ui_database_path(db_path) < 0)
    {
        db_path = Core::get_local_data_path();
        QDir f(QString().fromUtf8(db_path.c_str(), db_path.length()));
        if (!f.exists())
            db_path = ".";
    }

    db = new SQLLiteUi;

    db->set_database_directory(db_path);
    db->set_database_filename(database_filename);
    if (db->init_ui() < 0)
    {
        const std::vector<std::string>& errors = db->get_errors();
        std::string error;
        for (size_t i = 0; i < errors.size(); ++i)
        {
            if (i)
                error += " ";
            error += errors[i];
        }
        QString msg = QString().fromUtf8(error.c_str(), error.length());
        set_msg_to_status_bar(msg);
        delete db;
        db = NULL;
    }
#endif

    if (!db)
    {
        db = new NoDatabaseUi;
        db->init_ui();
    }
}

//---------------------------------------------------------------------------
void MainWindow::fill_options_for_report(std::map<std::string, std::string>& opts, int *verbosity_p)
{
    std::string verbosity = QString().setNum(uisettings.get_default_verbosity()).toUtf8().data();

    if (verbosity_p && *verbosity_p != -1)
        verbosity = QString().setNum(*verbosity_p).toUtf8().data();

    if (verbosity.length())
    {
        if (verbosity == "-1")
            opts["verbosity"] = "5";
        else
            opts["verbosity"] = verbosity;
    }
}

}
