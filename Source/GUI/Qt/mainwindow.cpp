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
#include "policycombobox.h"
#include "displaycombobox.h"
#include "verbosityspinbox.h"
#include "savereportpathbox.h"
#include "savepolicypathbox.h"
#include "savedisplaypathbox.h"
#include "loadfilespathbox.h"
#include "loadpolicypathbox.h"
#include "loaddisplaypathbox.h"
#include "Common/ImplementationReportDisplayHtmlXsl.h"
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
// Constant
//***************************************************************************

const std::string MainWindow::database_filename = std::string("MediaConchUi.db");

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    workerfiles(this)
{
    ui->setupUi(this);

    MCL.init();

    // GUI database
    create_and_configure_ui_database();
    workerfiles.set_database(db);
    uisettings.set_database(db);
    uisettings.init();

    // Core configuration
    if (!MCL.get_implementation_schema_file().length())
        MCL.create_default_implementation_schema();

    // Groups
    QActionGroup* ToolGroup = new QActionGroup(this);
    ToolGroup->addAction(ui->actionChecker);
    ToolGroup->addAction(ui->actionPolicies);
    ToolGroup->addAction(ui->actionDisplay);
    
    // Visual elements
    Layout=(QVBoxLayout*)ui->centralWidget->layout();
    Layout->setContentsMargins(0, 0, 0, 0);
    MenuView = new MenuMainWindow(this);
    checkerView=NULL;
    policiesView = NULL;
    displayView = NULL;
    default_policy_box = NULL;
    default_display_box = NULL;
    default_verbosity_box = NULL;

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

    // Status bar
    statusBar()->show();
    clear_msg_in_status_bar();
    connect(this, SIGNAL(status_bar_show_message(const QString&, int)),
            statusBar(), SLOT(showMessage(const QString&, int)));

    // worker load existing files
    workerfiles.fill_registered_files_from_db();
    workerfiles.start();

    // Default
    add_default_policy();
    add_default_displays();
    on_actionChecker_triggered();
}

MainWindow::~MainWindow()
{
    workerfiles.quit();
    workerfiles.wait();
    if (default_policy_box)
        delete default_policy_box;
    if (default_display_box)
        delete default_display_box;
    if (default_verbosity_box)
        delete default_verbosity_box;
    delete ui;
    if (checkerView)
        delete checkerView;
}

//***************************************************************************
// Functions
//***************************************************************************

//---------------------------------------------------------------------------
void MainWindow::add_file_to_list(const QString& file, const QString& path,
                                  const QString& policy, const QString& display,
                                  const QString& v)
{
    std::string filename = std::string(file.toUtf8().data(), file.toUtf8().length());
    std::string filepath = std::string(path.toUtf8().data(), path.toUtf8().length());
    int policy_i = policy.toInt();
    int display_i = display.toInt();
    int verbosity_i = v.toInt();

    // Save configuration used
    std::string policy_str;
    if (policy_i >= 0)
    {
        Policy *p = get_policy(policy_i);
        if (p)
            policy_str = p->filename;
    }
    uisettings.change_last_policy(policy_str);
    uisettings.change_last_display(displays_list[display_i].toUtf8().data());
    uisettings.change_last_verbosity(verbosity_i);

    std::string full_path = filepath;
    if (full_path.length())
        full_path += "/";
    full_path += filename;

    workerfiles.add_file_to_list(filename, filepath, policy_i, display_i, verbosity_i);
    checkerView->add_file_to_result_table(full_path);
}

//---------------------------------------------------------------------------
void MainWindow::remove_file_to_list(const QString& file)
{
    std::string filename = std::string(file.toUtf8().data(), file.toUtf8().length());
    workerfiles.remove_file_registered_from_file(filename);
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
void MainWindow::get_registered_files(std::map<std::string, FileRegistered>& files)
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
    QString file = QFileDialog::getOpenFileName(this, "Open file", suggested, "XSL file (*.xsl);;Schematron file (*.sch);;All (*.*)", 0, QFileDialog::DontUseNativeDialog);

    if (file.length())
    {
        QDir info(QFileInfo(file).absoluteDir());
        set_last_load_policy_path(info.absolutePath().toUtf8().data());
    }

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

    QString suggested = QString().fromUtf8(select_correct_save_policy_path().c_str());
    suggested += "/" + QString().fromUtf8(p->title.c_str()) + ".sch";

    QString filename = QFileDialog::getSaveFileName(this, tr("Save Policy"),
                                                    suggested, tr("Schematron (*.sch)"));

    if (!filename.length())
        return -1;

    QDir info(QFileInfo(filename).absoluteDir());
    set_last_save_policy_path(info.absolutePath().toUtf8().data());

    std::string f(filename.toUtf8().data());
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

    QString suggested = QString().fromUtf8(select_correct_save_policy_path().c_str());
    suggested += "/" + QString().fromUtf8(p->title.c_str()) + ".xml";

    QString filename = QFileDialog::getSaveFileName(this, tr("Save Policy"),
                                                    suggested, tr("XML (*.xml)"));

    if (!filename.length())
        return -1;

    QDir info(QFileInfo(filename).absoluteDir());
    set_last_save_policy_path(info.absolutePath().toUtf8().data());

    std::string f(filename.toUtf8().data());
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

    QString suggested = QString().fromUtf8(select_correct_save_policy_path().c_str());
    suggested += "/" + QString().fromUtf8(p->title.c_str()) + ".xsl";

    QString filename = QFileDialog::getSaveFileName(this, tr("Save Policy"),
                                                    suggested, tr("XSLT (*.xsl)"));

    if (!filename.length())
        return -1;

    QDir info(QFileInfo(filename).absoluteDir());
    set_last_save_policy_path(info.absolutePath().toUtf8().data());

    std::string f(filename.toUtf8().data());
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
    workerfiles.clear_files();
}

//---------------------------------------------------------------------------
const std::vector<Policy *>& MainWindow::get_all_policies() const
{
    return MCL.get_policies();
}

//---------------------------------------------------------------------------
void MainWindow::get_policies(std::vector<std::pair<QString, QString> >& policies)
{
    const std::vector<Policy *>& list = get_all_policies();

    for (size_t i = 0; i < list.size(); ++i)
        policies.push_back(std::make_pair(QString().fromUtf8(list[i]->filename.c_str(), list[i]->filename.length()),
                                          QString().fromUtf8(list[i]->title.c_str(), list[i]->title.length())));
}

//---------------------------------------------------------------------------
std::vector<QString>& MainWindow::get_displays()
{
    return displays_list;
}

//---------------------------------------------------------------------------
int MainWindow::get_display_index_by_filename(const std::string& filename)
{
    for (size_t i = 0; i < displays_list.size(); ++i)
    {
        if (filename == displays_list[i].toUtf8().data())
            return i;
    }

    return 0;
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
        set_msg_to_status_bar("Policy not valid");

    if (!ui->actionPolicies->isChecked())
        ui->actionPolicies->setChecked(true);
    current_view = RUN_POLICIES_VIEW;
    Run();
}

//---------------------------------------------------------------------------
void MainWindow::on_actionDefaultVerbosity_triggered()
{
    if (!default_verbosity_box)
    {
        default_verbosity_box = new VerbositySpinbox(NULL);
        int w = width();
        int left = 0;
        if (w < 220)
            w = 220;
        else
        {
            w -= 220;
            left = w / 3;
            w = left + 220;
        }

        int h = height();
        int up = 0;
        if (h < 85)
            h = 85;
        else
        {
            h -= 85;
            up = h / 3;
            h = up + 85;
        }
        default_verbosity_box->move(left + x(), up + y());
        default_verbosity_box->resize(w, h);
    }
    QString value = QString("%1").arg(uisettings.get_default_verbosity());

    default_verbosity_box->get_verbosity_spin()->setValue(value.toInt());
    connect(default_verbosity_box->get_buttons_box(), SIGNAL(accepted()), this, SLOT(default_verbosity_accepted()));
    connect(default_verbosity_box->get_buttons_box(), SIGNAL(rejected()), this, SLOT(default_verbosity_rejected()));
    default_verbosity_box->show();
}

//---------------------------------------------------------------------------
void MainWindow::default_verbosity_accepted()
{
    if (!default_verbosity_box)
        return;

    uisettings.change_default_verbosity(default_verbosity_box->get_verbosity_spin()->value());

    delete default_verbosity_box;
    default_verbosity_box = NULL;
}

//---------------------------------------------------------------------------
void MainWindow::default_verbosity_rejected()
{
    if (!default_verbosity_box)
        return;

    delete default_verbosity_box;
    default_verbosity_box = NULL;
}

//---------------------------------------------------------------------------
void MainWindow::on_actionDefaultPolicy_triggered()
{
    if (!default_policy_box)
    {
        default_policy_box = new PolicyCombobox(NULL);
        std::vector<std::pair<QString, QString> > policies;
        get_policies(policies);
        default_policy_box->fill_policy_box(policies);
        int w = width();
        int left = 0;
        if (w < 220)
            w = 220;
        else
        {
            w -= 220;
            left = w / 3;
            w = left + 220;
        }

        int h = height();
        int up = 0;
        if (h < 85)
            h = 85;
        else
        {
            h -= 85;
            up = h / 3;
            h = up + 85;
        }
        default_policy_box->move(left + x(), up + y());
        default_policy_box->resize(w, h);
    }
    QString value = QString().fromUtf8(uisettings.get_default_policy().c_str());

    QComboBox *box = default_policy_box->get_policy_combo();
    int pos = box->findData(QVariant(value));
    if (pos != -1)
        box->setCurrentIndex(pos);
    connect(default_policy_box->get_buttons_box(), SIGNAL(accepted()), this, SLOT(default_policy_accepted()));
    connect(default_policy_box->get_buttons_box(), SIGNAL(rejected()), this, SLOT(default_policy_rejected()));
    default_policy_box->show();
}

//---------------------------------------------------------------------------
void MainWindow::default_policy_accepted()
{
    if (!default_policy_box)
        return;

    int pos = default_policy_box->get_policy_combo()->currentIndex();
    if (pos >= 0)
    {
        QString policy_value = default_policy_box->get_policy_combo()->itemData(pos).toString();
        uisettings.change_default_policy(policy_value.toUtf8().data());
    }

    delete default_policy_box;
    default_policy_box = NULL;
}

//---------------------------------------------------------------------------
void MainWindow::default_policy_rejected()
{
    if (!default_policy_box)
        return;

    delete default_policy_box;
    default_policy_box = NULL;
}

//---------------------------------------------------------------------------
void MainWindow::on_actionDefaultDisplay_triggered()
{
    if (!default_display_box)
    {
        default_display_box = new DisplayCombobox(NULL);
        default_display_box->fill_display_box(get_displays());
        int w = width();
        int left = 0;
        if (w < 220)
            w = 220;
        else
        {
            w -= 220;
            left = w / 3;
            w = left + 220;
        }

        int h = height();
        int up = 0;
        if (h < 85)
            h = 85;
        else
        {
            h -= 85;
            up = h / 3;
            h = up + 85;
        }
        default_display_box->move(left + x(), up + y());
        default_display_box->resize(w, h);
    }
    QString value = QString().fromUtf8(uisettings.get_default_display().c_str());

    QComboBox *box = default_display_box->get_display_combo();
    int pos = box->findData(QVariant(value));
    if (pos != -1)
        box->setCurrentIndex(pos);
    connect(default_display_box->get_buttons_box(), SIGNAL(accepted()), this, SLOT(default_display_accepted()));
    connect(default_display_box->get_buttons_box(), SIGNAL(rejected()), this, SLOT(default_display_rejected()));
    default_display_box->show();
}

//---------------------------------------------------------------------------
void MainWindow::default_display_accepted()
{
    if (!default_display_box)
        return;

    int pos = default_display_box->get_display_combo()->currentIndex();
    if (pos >= 0)
    {
        QString display_value = default_display_box->get_display_combo()->itemData(pos).toString();
        uisettings.change_default_display(display_value.toUtf8().data());
    }

    delete default_display_box;
    default_display_box = NULL;
}

//---------------------------------------------------------------------------
void MainWindow::default_display_rejected()
{
    if (!default_display_box)
        return;

    delete default_display_box;
    default_display_box = NULL;
}

//---------------------------------------------------------------------------
void MainWindow::on_actionDefaultSaveReportPath_triggered()
{
    QString value = QString().fromUtf8(uisettings.get_default_save_report_path().c_str());
    SaveReportPathBox* box = new SaveReportPathBox(value);

    box->exec();
    const QString& val = box->get_path();
    if (val != value)
        uisettings.change_default_save_report_path(val.toUtf8().data());
    delete box;
    box = NULL;
}

//---------------------------------------------------------------------------
void MainWindow::on_actionDefaultSavePolicyPath_triggered()
{
    QString value = QString().fromUtf8(uisettings.get_default_save_policy_path().c_str());
    SavePolicyPathBox* box = new SavePolicyPathBox(value);

    box->exec();
    const QString& val = box->get_path();
    if (val != value)
        uisettings.change_default_save_policy_path(val.toUtf8().data());
    delete box;
    box = NULL;
}

//---------------------------------------------------------------------------
void MainWindow::on_actionDefaultSaveDisplayPath_triggered()
{
    QString value = QString().fromUtf8(uisettings.get_default_save_display_path().c_str());
    SaveDisplayPathBox* box = new SaveDisplayPathBox(value);

    box->exec();
    const QString& val = box->get_path();
    if (val != value)
        uisettings.change_default_save_display_path(val.toUtf8().data());
    delete box;
    box = NULL;
}

//---------------------------------------------------------------------------
void MainWindow::on_actionDefaultLoadFilesPath_triggered()
{
    QString value = QString().fromUtf8(uisettings.get_default_load_files_path().c_str());
    LoadFilesPathBox* box = new LoadFilesPathBox(value);

    box->exec();
    const QString& val = box->get_path();
    if (val != value)
        uisettings.change_default_load_files_path(val.toUtf8().data());
    delete box;
    box = NULL;
}

//---------------------------------------------------------------------------
void MainWindow::on_actionDefaultLoadPolicyPath_triggered()
{
    QString value = QString().fromUtf8(uisettings.get_default_load_policy_path().c_str());
    LoadPolicyPathBox* box = new LoadPolicyPathBox(value);

    box->exec();
    const QString& val = box->get_path();
    if (val != value)
        uisettings.change_default_load_policy_path(val.toUtf8().data());
    delete box;
    box = NULL;
}

//---------------------------------------------------------------------------
void MainWindow::on_actionDefaultLoadDisplayPath_triggered()
{
    QString value = QString().fromUtf8(uisettings.get_default_load_display_path().c_str());
    LoadDisplayPathBox* box = new LoadDisplayPathBox(value);

    box->exec();
    const QString& val = box->get_path();
    if (val != value)
        uisettings.change_default_load_display_path(val.toUtf8().data());
    delete box;
    box = NULL;
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

    if (policiesView)
    {
        delete policiesView;
        policiesView = NULL;
    }

    if (displayView)
    {
        delete displayView;
        displayView = NULL;
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
    std::map<std::string, FileRegistered> files;
    workerfiles.get_registered_files(files);

    std::map<std::string, FileRegistered>::iterator it = files.begin();
    for (; it != files.end(); ++it)
        checkerView->add_file_to_result_table(it->first);
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
    Q_EMIT status_bar_show_message(message, 5000);
}

void MainWindow::clear_msg_in_status_bar()
{
    Q_EMIT status_bar_clear_message();
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
    return MCL.import_policy_from_file(file.toUtf8().data(), err);
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
int MainWindow::get_policy_index_by_filename(const std::string& filename)
{
    const std::vector<Policy *>&  policies = get_all_policies();

    for (size_t i = 0; i < policies.size(); ++i)
    {
        if (policies[i] && policies[i]->filename == filename)
            return i;
    }

    return -1;
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
int MainWindow::select_correct_policy()
{
    // Policy
    std::string policy = uisettings.get_default_policy();
    if (policy == "last")
        policy = uisettings.get_last_policy();

    if (!policy.length())
        return -1;
    return get_policy_index_by_filename(policy);
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
int MainWindow::is_analyze_finished(const std::string& file, double& percent_done, MediaConchLib::report& report_kind)
{
    return MCL.is_done(file, percent_done, report_kind);
}

//---------------------------------------------------------------------------
int MainWindow::validate(MediaConchLib::report report, const std::vector<std::string>& files,
                         const std::vector<std::string>& policies_names,
                         const std::vector<std::string>& policies_contents,
                         std::vector<MediaConchLib::ValidateRes*>& result)
{
    return MCL.validate(report, files, policies_names, policies_contents, result);
}

//---------------------------------------------------------------------------
int MainWindow::validate(MediaConchLib::report report, const std::string& file,
                         const std::vector<std::string>& policies_names,
                         const std::vector<std::string>& policies_contents,
                         std::vector<MediaConchLib::ValidateRes*>& result)
{
    std::vector<std::string> files;
    files.push_back(file);

    return MCL.validate(report, files, policies_names, policies_contents, result);
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
    std::map<std::string, std::string> options;
    MCL.get_report(report_set, MediaConchLib::format_Xml, files,
                   vec, vec,
                   options,
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
    std::map<std::string, std::string> options;
    MCL.get_report(report_set, MediaConchLib::format_Xml, files,
                   vec, vec,
                   options,
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
    std::map<std::string, std::string> options;
    MCL.get_report(report_set, MediaConchLib::format_JsTree, files,
                   vec, vec, options, &result);
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
    std::map<std::string, std::string> options;
    MCL.get_report(report_set, MediaConchLib::format_Xml, files,
                   vec, vec,
                   options,
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
    std::map<std::string, std::string> options;
    MCL.get_report(report_set, MediaConchLib::format_JsTree, files,
                   vec, vec,
                   options,
                   &result);
    return QString().fromUtf8(result.report.c_str(), result.report.length());
}

//---------------------------------------------------------------------------
void MainWindow::get_implementation_report(const std::string& file, QString& report, int *display_p, int *verbosity)
{
    FileRegistered *fr = get_file_registered_from_file(file);
    if (!fr)
        return;

    if (!fr->analyzed)
    {
        delete fr;
        return;
    }

    std::string display_content;
    std::string display_name;
    const std::string* dname = NULL;
    const std::string* dcontent = NULL;
    fill_display_used(display_p, display_name, display_content, dname, dcontent, fr);

    std::bitset<MediaConchLib::report_Max> report_set;
    report_set.set(fr->report_kind);

    std::vector<std::string> files;
    files.push_back(file);

    MediaConchLib::ReportRes result;
    std::vector<std::string> vec;
    std::map<std::string, std::string> options;
    fill_options_for_report(options, verbosity);
    MCL.get_report(report_set, MediaConchLib::format_Xml, files,
                   vec, vec,
                   options,
                   &result, dname, dcontent);

    report = QString().fromUtf8(result.report.c_str(), result.report.length());
    delete fr;
}

//---------------------------------------------------------------------------
int MainWindow::validate_policy(const std::string& file, QString& report, int policy, int *display_p)
{
    FileRegistered* fr = get_file_registered_from_file(file);
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

    Policy* p = get_policy((size_t)policy);
    if (!p)
    {
        report = "Policy not found";
        delete fr;
        return 1;
    }
    std::string policy_content;
    p->dump_schema(policy_content);

    std::string display_content;
    std::string display_name;
    const std::string* dname = NULL;
    const std::string* dcontent = NULL;
    fill_display_used(display_p, display_name, display_content, dname, dcontent, fr);
    std::vector<std::string> files;
    files.push_back(file);
    delete fr;

    MediaConchLib::ReportRes result;
    std::bitset<MediaConchLib::report_Max> report_set;
    std::vector<std::string> policies_contents;
    policies_contents.push_back(policy_content);

    std::vector<std::string> vec;
    std::map<std::string, std::string> options;

    if (MCL.get_report(report_set, MediaConchLib::format_Xml, files,
                       vec, policies_contents,
                       options,
                       &result, dname, dcontent) < 0)
        return 0;

    report = QString().fromUtf8(result.report.c_str(), result.report.length());

    if (!result.has_valid)
        return 1;
    if (result.valid)
        return 1;
    return 0;
}

//---------------------------------------------------------------------------
FileRegistered* MainWindow::get_file_registered_from_file(const std::string& file)
{
    return workerfiles.get_file_registered_from_file(file);
}

//---------------------------------------------------------------------------
void MainWindow::remove_file_registered_from_file(const std::string& file)
{
    workerfiles.remove_file_registered_from_file(file);
}

//---------------------------------------------------------------------------
void MainWindow::fill_display_used(int *display_p, std::string&, std::string& display_content,
                                   const std::string*& dname, const std::string*& dcontent,
                                   FileRegistered* fr)
{
    if (fr && (fr->report_kind == MediaConchLib::report_MediaVeraPdf ||
               fr->report_kind == MediaConchLib::report_MediaDpfManager))
    {
        dname = NULL;
        dcontent = NULL;
        return;
    }

    if (display_p)
    {
        if (*display_p >= 0 && (size_t)*display_p < displays_list.size())
        {
            QFile display_xsl(displays_list[*display_p]);
            display_xsl.open(QIODevice::ReadOnly | QIODevice::Text);
            QByteArray xsl = display_xsl.readAll();
            display_xsl.close();
            display_content = QString(xsl).toStdString();
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
    set_msg_to_status_bar(error_msg);
}

//---------------------------------------------------------------------------
int MainWindow::get_ui_database_path(std::string& path)
{
    return MCL.get_ui_database_path(path);
}

//---------------------------------------------------------------------------
void MainWindow::create_and_configure_ui_database()
{
#ifdef HAVE_SQLITE
    std::string db_path;
    if (get_ui_database_path(db_path) < 0)
    {
        db_path = Core::get_local_data_path();
        QDir f(QString().fromStdString(db_path));
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
        QString msg = QString().fromStdString(error);
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
    std::string verbosity = QString().setNum(uisettings.get_default_verbosity()).toStdString();

    if (verbosity_p && *verbosity_p != -1)
        verbosity = QString().setNum(*verbosity_p).toStdString();

    if (verbosity.length())
    {
        if (verbosity == "-1")
            opts["verbosity"] = "5";
        else
            opts["verbosity"] = verbosity;
    }
}

}
