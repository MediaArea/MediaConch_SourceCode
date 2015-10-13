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
#include "WebPage.h"
#include "WebView.h"

#include <QTextEdit>
#include <QWebView>
#include <QWebFrame>
#include <QWebElement>
#include <QProgressBar>
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
    
    // Visual elements
    Layout=(QVBoxLayout*)ui->centralWidget->layout();
    Layout->setContentsMargins(0, 0, 0, 0);
    createMenu();
    MainView=NULL;
    progressBar=NULL;
    policiesTree = NULL;
    policiesMenu = NULL;
    policyMenu = NULL;
    groupOfRules = NULL;
    ruleMenu = NULL;
    ruleEdit = NULL;

    // Window
    setWindowIcon(QIcon(":/icon/icon.png"));
    move(QApplication::desktop()->screenGeometry().width()/4-50, QApplication::desktop()->screenGeometry().height()/4);
    resize(QApplication::desktop()->screenGeometry().width()/2+100, QApplication::desktop()->screenGeometry().height()/2);
    setAcceptDrops(false);

    // Default
    add_default_policy();
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
    if (C.Tool == Core::tool_MediaPolicies)
    {
        displayPoliciesTree();
        return;
    }

    createWebView();
    //TODO: fill the view if file already here
    // if (!C.List.empty())
    //     C.Run();
}

//---------------------------------------------------------------------------
void MainWindow::checker_add_file(QString& file, QString& policy)
{
    if (C.Tool == Core::tool_MediaPolicies)
    {
        displayPoliciesTree();
        return;
    }

    addFileToList(file);
    updateWebView(file.toStdWString(), policy.toStdWString());
}

//---------------------------------------------------------------------------
void MainWindow::checker_add_files(QFileInfoList& list, QString& policy)
{
    if (C.Tool == Core::tool_MediaPolicies)
    {
        displayPoliciesTree();
        return;
    }

    for (int i = 0; i < list.count(); ++i)
    {
        QString file = list[i].absoluteFilePath();
        addFileToList(file);
    }
    updateWebView(list, policy.toStdWString());
}

//---------------------------------------------------------------------------
void MainWindow::checker_add_xslt_file(QString& file, QString& xslt)
{
    if (C.Tool == Core::tool_MediaPolicies)
    {
        displayPoliciesTree();
        return;
    }

    addFileToList(file);
    C.PoliciesFiles[Core::policyType_Xslt].push_back(xslt.toStdWString());
    updateWebView(file.toStdWString(), String());
    C.PoliciesFiles[Core::policyType_Xslt].clear();
}

//---------------------------------------------------------------------------
void MainWindow::checker_add_xslt_files(QFileInfoList& list, QString& xslt)
{
    if (C.Tool == Core::tool_MediaPolicies)
    {
        displayPoliciesTree();
        return;
    }

    for (int i = 0; i < list.count(); ++i)
    {
        QString file = list[i].absoluteFilePath();
        addFileToList(file);
    }
    C.PoliciesFiles[Core::policyType_Xslt].push_back(xslt.toStdWString());
    updateWebView(list, String());
    C.PoliciesFiles[Core::policyType_Xslt].clear();
}

//---------------------------------------------------------------------------
QString MainWindow::get_trace_for_file(const QString& filename)
{
    if (C.Tool == Core::tool_MediaPolicies)
    {
        displayPoliciesTree();
        return QString();
    }

    String file = filename.toStdWString();
    QString report = Run(Core::tool_MediaTrace, Core::format_Xml, file);
    return report;
}

//---------------------------------------------------------------------------
QString MainWindow::ask_for_schematron_file()
{
    QString file=QFileDialog::getOpenFileName(this, "Open file", "", "Schematron file (*.sch);;All (*.*)", 0, QFileDialog::DontUseNativeDialog);
    return file;
}

//---------------------------------------------------------------------------
void MainWindow::exporting_to_schematron_file(int pos)
{
    if (pos < 0)
        return;
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
        return;
    C.policies.export_schematron(filename.toStdString().c_str(), pos);

    C.policies.policies[pos]->filename = filename.toStdString();
    policyMenu->get_savePolicy_button()->setEnabled(true);
}

//---------------------------------------------------------------------------
void MainWindow::exporting_to_schematron(int pos)
{
    C.policies.export_schematron(NULL, pos);
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
        String ret = C.policies.import_schematron_from_memory(list[i].absoluteFilePath().toStdString().c_str(),
                                                              schematron.constData(), schematron.length());
        (void)ret;
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
        QByteArray schematron = file.readAll();
        String ret = C.policies.import_schematron_from_memory(list[i].absoluteFilePath().toStdString().c_str(),
                                                              schematron.constData(), schematron.length());
        (void)ret;
    }
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
    clearVisualElements();

    Run();
}

//---------------------------------------------------------------------------
void MainWindow::on_actionChecker_triggered()
{
    C.Tool=Core::tool_MediaConch;
    C.Format=Core::format_Text;
    Run();
}

//---------------------------------------------------------------------------
void MainWindow::on_actionPolicies_triggered()
{
    C.Tool=Core::tool_MediaPolicies;
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
            C.PoliciesFiles[Core::policyType_Schematron].push_back(file.toStdWString());
        }
    }
    Run();
}

//---------------------------------------------------------------------------
void MainWindow::on_actionAbout_triggered()
{
    //Menu
    QString about("About MediaConch");
    QString descr(tr("MediaConch is an extensible, open source software project "
                     "consisting of an implementation checker, policy checker, "
                     "reporter, and fixer that targets preservation-level audiovisual "
                     "files (specifically Matroska, Linear Pulse Code Modulation (LPCM) "
                     "and FF Video Codec 1 (FFV1)) for use in memory institutions, "
                     "providing detailed and batch-level conformance checking "
                     "via an adaptable and flexible application program interface "
                     "accessible by the command line, a graphical user interface, "
                     "or a web-based shell. Conch is currently being developed by the MediaArea team, "
                     "notable for the creation of open source media checker software, MediaInfo. "
                     "Furthermore, the MediaArea team is dedicated to the further "
                     "development of the standardization of the Matroska and "
                     "FFV1 formats to ensure their longevity as a recommended digital "
                     "preservation file format."));
    QMessageBox::about(this, about, descr);
}

//---------------------------------------------------------------------------
void MainWindow::import_schematron()
{
    QString file = ask_for_schematron_file();
    String ret = C.policies.import_schematron(file.toStdString().c_str());

    displayPoliciesTree();
    if (ret.length())
            policiesTree->get_error_bar()->showMessage(QString().fromStdWString(ret));
    else
    {
        policiesTree->get_error_bar()->clearMessage();
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
void MainWindow::save_policy_to()
{
    int  row = get_index_in_tree();

    if (row < 0)
        return;
    exporting_to_schematron_file(row);
}

//---------------------------------------------------------------------------
void MainWindow::save_policy()
{
    int  row = get_index_in_tree();

    if (row < 0)
        return;
    exporting_to_schematron(row);
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
    if (C.policies.policy_exists(p->title))
    {
        int i = 1;
        while (1)
        {
            std::stringstream ss;
            ss << p->title << " " << i;
            if (!C.policies.policy_exists(ss.str()))
            {
                p->title = ss.str();
                break;
            }
            ++i;
        }
    }

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
void MainWindow::duplicate_assert()
{
    QTreeWidgetItem* item = get_item_in_tree();
    if (!item)
        return;

    int rowPolicy = get_index_of_item_backXX(item, 3);
    int rowPattern = get_index_of_item_backXX(item, 2);
    int rowRule = get_index_of_item_backXX(item, 1);
    int row = get_index_in_tree();
    if (rowPattern < 0 || rowPolicy < 0 || row < 0)
        return;

    Rule *r = C.policies.policies[rowPolicy]->patterns[rowPattern]->rules[rowRule];
    Assert *a = new Assert(*r->asserts[row]);
    a->description = a->description + string(" (Copy)");
    r->asserts.push_back(a);

    QTreeWidgetItem* parent = item->parent();
    if (!parent)
        return;

    QTreeWidgetItem* new_item = new QTreeWidgetItem(parent);
    new_item->setText(0, QString().fromStdString(a->description));
    item->setSelected(false);
    new_item->setSelected(true);

    displayRuleEdit(new_item);
}

//---------------------------------------------------------------------------
void MainWindow::delete_all_policies()
{
    int ret = QMessageBox::warning(this, tr("Delete all policies"),
                                   tr("Do you want to delete all policies?"),
                                   QMessageBox::Yes | QMessageBox::No);
    switch (ret)
    {
      case QMessageBox::Yes:
          break;
      case QMessageBox::No:
      default:
          return;
    }

    QTreeWidget *tree = policiesTree->get_policies_tree();
    QTreeWidgetItem* policies = tree->topLevelItem(0);
    if (!policies)
        return;
    removeTreeChildren(policies);
    for (size_t i = 0; i < C.policies.policies.size(); ++i)
        delete C.policies.policies[i];
    C.policies.policies.clear();
    policiesMenu->get_deletePolicies_button()->setEnabled(false);
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
        policiesTree->get_error_bar()->showMessage(QString("Policy must have a title"));
        return;
    }
    policiesTree->get_error_bar()->clearMessage();

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
        policiesTree->get_error_bar()->showMessage(QString("Group of rules must have a title"));
        return;
    }
    policiesTree->get_error_bar()->clearMessage();

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
        policiesTree->get_error_bar()->showMessage(QString("Assert must have a name"));
        return;
    }
    policiesTree->get_error_bar()->clearMessage();

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
    if (MainView)
    {
        Layout->removeWidget(MainView);
        delete MainView;
        MainView=NULL;
    }

    if (progressBar)
    {
        Layout->removeWidget(progressBar);
        delete progressBar;
        progressBar=NULL;
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
void MainWindow::createMenuFinished(bool)
{
    if (!MenuView)
        return;

    QWebFrame *frame = MenuView->page()->currentFrame();
    frame->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    frame->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
}

//---------------------------------------------------------------------------
void MainWindow::createMenu()
{
    QFile menu_file(":/menu.html");

    MenuView = new QWebView(this);
    MenuView->setAcceptDrops(true);
    MenuView->setMaximumHeight(75);
    MenuView->setMinimumHeight(75);
    Layout->addWidget(MenuView);

    WebPage* page = new WebPage(this, MenuView);
    MenuView->setPage(page);

    menu_file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray html = menu_file.readAll();
    menu_file.close();

    QObject::connect(MenuView, SIGNAL(loadFinished(bool)), this, SLOT(createMenuFinished(bool)));

    QUrl url = QUrl("qrc:/html");
    if (!url.isValid())
        return;

    MenuView->setContent(QString(html).toUtf8(), "text/html", url);
}

//---------------------------------------------------------------------------
void MainWindow::createWebViewFinished(bool ok)
{
    if (!MainView)
        return;

    if (progressBar)
    {
        delete progressBar;
        progressBar = NULL;
    }
    if (!ok)
        return; //TODO: Error
    Layout->addWidget(MainView);
}

//---------------------------------------------------------------------------
void MainWindow::setWebViewContent(QString& html)
{
    MainView=new WebView(this);

    WebPage* page = new WebPage(this, MainView);
    MainView->setPage(page);

    QObject::connect(MainView, SIGNAL(loadProgress(int)), progressBar, SLOT(setValue(int)));
    QObject::connect(MainView, SIGNAL(loadFinished(bool)), this, SLOT(createWebViewFinished(bool)));

    QUrl url = QUrl("qrc:/html");
    if (!url.isValid())
        return;

    MainView->setContent(html.toUtf8(), "text/html", url);
}

//---------------------------------------------------------------------------
void MainWindow::createWebView()
{
    if (MainView)
        return;

    clearVisualElements();

    progressBar = new QProgressBar();
    Layout->addWidget(progressBar);

    QString html = create_html();
    setWebViewContent(html);
}

//---------------------------------------------------------------------------
void MainWindow::updateWebView(String file, String policy)
{
    if (!MainView)
        return;

    QString html = MainView->page()->currentFrame()->toHtml();

    //Load the new page
    clearVisualElements();
    progressBar = new QProgressBar();
    Layout->addWidget(progressBar);
    progressBar->setValue(0);
    progressBar->show();

    //Add the file detail to the web page
    add_file_detail_to_html(html, file, policy);

    setWebViewContent(html);
}

//---------------------------------------------------------------------------
void MainWindow::updateWebView(QList<QFileInfo>& files, String policy)
{
    if (!MainView)
        return;

    QString html = MainView->page()->currentFrame()->toHtml();

    //Load the new page
    clearVisualElements();
    progressBar = new QProgressBar();
    Layout->addWidget(progressBar);
    progressBar->show();

    //Add the files details to the web page
    for (int i = 0; i < files.count(); ++i)
    {
        String file = files[i].absoluteFilePath().toStdWString();
        add_file_detail_to_html(html, file, policy);
        progressBar->setValue((i * 100) / files.count());
    }

    setWebViewContent(html);
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
        policies = new QTreeWidgetItem(tree);
        policies->setText(0, tr("Policies"));
        policies->setSelected(true);
        createPoliciesMenu();
    }
    updatePoliciesTree();
    if (policies->childCount())
        policies->setExpanded(true);
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
                     this, SLOT(import_schematron()));
    QObject::connect(policiesMenu->get_addNewPolicy_button(), SIGNAL(clicked()),
                     this, SLOT(add_new_policy()));
    QObject::connect(policiesMenu->get_deletePolicies_button(), SIGNAL(clicked()),
                     this, SLOT(delete_all_policies()));

    if (!C.policies.policies.size())
        policiesMenu->get_deletePolicies_button()->setEnabled(false);
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
    QObject::connect(policyMenu->get_savePolicy_button(), SIGNAL(clicked()),
                     this, SLOT(save_policy()));
    QObject::connect(policyMenu->get_savePolicyTo_button(), SIGNAL(clicked()),
                     this, SLOT(save_policy_to()));
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

    policyMenu->get_savePolicy_button()->setEnabled(false);
    int index = get_index_in_tree();
    if (index >= 0 && index < (int)C.policies.policies.size())
    {
        if (C.policies.policies[index] && C.policies.policies[index]->filename.length())
            policyMenu->get_savePolicy_button()->setEnabled(true);
    }
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
    QObject::connect(ruleEdit->get_duplicateAssert_button(), SIGNAL(clicked()),
                     this, SLOT(duplicate_assert()));
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
QString MainWindow::Run(Core::tool tool, Core::format format, String& file)
{
    if (C.Tool == Core::tool_MediaPolicies || C.List.empty())
        return QString();

    C.Tool = tool;
    C.Format = format;
    return QString().fromStdWString(C.Run(file));
}

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

//---------------------------------------------------------------------------
void MainWindow::load_include_in_template(QString& html)
{
    QRegExp reg("\\{\\{[\\s]+include\\('AppBundle:(\\w+):(\\w+).html.twig'(,[\\s]*\\{ '\\w+':[\\s]*\\w+[\\s]*\\})?\\)[\\s]\\}\\}");
    int pos = 0;

    while ((pos = reg.indexIn(html, pos)) != -1)
    {
        QString app = reg.cap(1);
        QString module = reg.cap(2);
        if (app == "Default" && module == "quotaExceeded")
        {
            html.replace(pos, reg.matchedLength(), "");
            continue;
        }
        html.replace(pos, reg.matchedLength(), "");
        pos = 0;
    }
}

//---------------------------------------------------------------------------
void MainWindow::remove_element_in_template(QString& html)
{
    QRegExp reg("\\{% (.*) %\\}");
    int pos = 0;

    reg.setMinimal(true);
    while ((pos = reg.indexIn(html, pos)) != -1)
    {
        QString value = reg.cap(1);
        html.replace(pos, reg.matchedLength(), "");
    }
}

//---------------------------------------------------------------------------
void MainWindow::change_collapse_form(QString& html)
{
    QRegExp reg("class=\"panel-collapse collapse in\"");
    int pos = 0;

    while ((pos = reg.indexIn(html, pos)) != -1)
    {
        html.replace(pos, reg.matchedLength(), "class=\"panel-collapse collapse\"");
    }
}

//---------------------------------------------------------------------------
void MainWindow::load_form_in_template(QString& html)
{
    QRegExp reg("\\{\\{[\\s]+form\\((\\w+)\\)[\\s]\\}\\}");
    int pos = 0;

    while ((pos = reg.indexIn(html, pos)) != -1)
    {
        QString value = reg.cap(1);
        if (value == "formUpload")
            html.replace(pos, reg.matchedLength(), create_form_upload());
        else if (value == "formOnline")
#if defined(MEDIAINFO_LIBCURL_YES)
            html.replace(pos, reg.matchedLength(), create_form_online());
#else
        remove_form_online(pos, html);
#endif
        else if (value == "formRepository")
            html.replace(pos, reg.matchedLength(), create_form_repository());
        else
            html.replace(pos, reg.matchedLength(), "");
    }

    change_collapse_form(html);
}

//---------------------------------------------------------------------------
void MainWindow::create_policy_options(QString& policies)
{
    for (size_t i = 0; i < C.policies.policies.size(); ++i)
    {
        QString title = QString().fromStdString(C.policies.policies[i]->title);
        policies += QString("<option value=\"%1\">%1</option>").arg(title);
    }
}

//---------------------------------------------------------------------------
void MainWindow::add_policy_to_form_selection(QString& policies, QString& form, const char *selector)
{
    QRegExp reg("<option selected=\"selected\" value=\"\">[\\n\\r\\t\\s]*Choose a policy[\\n\\r\\t\\s]*</option>");
    int pos = form.indexOf(selector);

    reg.setMinimal(true);

    if (pos == -1)
        return;

    if ((pos = reg.indexIn(form, pos)) != -1)
    {
        pos += reg.matchedLength();
        form.insert(pos, policies);
    }
}

//---------------------------------------------------------------------------
QString MainWindow::create_form_upload()
{
    QFile template_html(":/formUpload.html");

    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray html = template_html.readAll();
    template_html.close();

    QString ret(html);
    QString policies;
    create_policy_options(policies);
    add_policy_to_form_selection(policies, ret, "checkerUpload_step1_policy");
    return ret;
}

//---------------------------------------------------------------------------
QString MainWindow::create_form_online()
{
    QFile template_html(":/formOnline.html");

    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray html = template_html.readAll();
    template_html.close();

    QString ret(html);
    QString policies;
    create_policy_options(policies);
    add_policy_to_form_selection(policies, ret, "checkerOnline_step1_policy");
    return ret;
}

//---------------------------------------------------------------------------
void MainWindow::remove_form_online(int pos, QString& html)
{
    int start_div_pos = pos;
    QRegExp reg("<div class=\"panel panel-default\">");
    reg.setMinimal(true);
    start_div_pos = reg.lastIndexIn(html, start_div_pos);

    reg = QRegExp("</div>");
    reg.setMinimal(true);
    int end_div_pos = pos;
    int nb_turn = 0;
    while ((end_div_pos = reg.indexIn(html, end_div_pos)) != -1)
    {
        ++nb_turn;
        end_div_pos += reg.matchedLength();
        if (nb_turn == 2)
            break;
    }
    if (end_div_pos != -1 && start_div_pos != -1)
        html.remove(start_div_pos, end_div_pos - start_div_pos);
}

//---------------------------------------------------------------------------
QString MainWindow::create_form_repository()
{
    QFile template_html(":/formRepository.html");

    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray html = template_html.readAll();
    template_html.close();

    QString ret(html);
    QString policies;
    create_policy_options(policies);
    add_policy_to_form_selection(policies, ret, "checkerRepository_step1_policy");
    return ret;
}

//---------------------------------------------------------------------------
void MainWindow::remove_template_tags(QString& data)
{
    load_include_in_template(data);
    remove_element_in_template(data);
    load_form_in_template(data);
}

//---------------------------------------------------------------------------
QString MainWindow::create_html_body()
{
    QFile template_html(":/checker.html");

    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray html = template_html.readAll();
    template_html.close();

    QString data(html);
    remove_template_tags(data);
    return data;
}

//---------------------------------------------------------------------------
void MainWindow::change_body_in_template(QString& body, QString& html)
{
    QRegExp reg("\\{% block body %\\}\\{% endblock %\\}");
    int pos = 0;

    reg.setMinimal(true);
    while ((pos = reg.indexIn(html, pos)) != -1)
    {
        html.replace(pos, reg.matchedLength(), body);
    }
}

//---------------------------------------------------------------------------
QString MainWindow::create_html_base(QString& body)
{
    QFile template_html(":/base.html");

    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray html = template_html.readAll();
    template_html.close();

    QString base(html);

    change_body_in_template(body, base);
    return base;
}

//---------------------------------------------------------------------------
QString MainWindow::create_html()
{
    QString body = create_html_body();
    QString base = create_html_base(body);
    return base;
}

//---------------------------------------------------------------------------
void MainWindow::change_html_file_detail_inform_xml(QString& html, String& file)
{
    QString report = Run(Core::tool_MediaConch, Core::format_Xml, file);
#if QT_VERSION >= 0x050200
    report = report.toHtmlEscaped();
#else
    report = Qt::escape(report);
#endif
    report.replace('\n', "<br/>\n");

    QRegExp reg("\\{\\{ check\\.getXml\\|nl2br \\}\\}");

    int pos = 0;

    reg.setMinimal(true);
    if ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), report);
}

//---------------------------------------------------------------------------
void MainWindow::change_html_file_detail_conformance(QString& html, String& file)
{
    QString report = Run(Core::tool_MediaConch, Core::format_Text, file);
#if QT_VERSION >= 0x050200
    report = report.toHtmlEscaped();
#else
    report = Qt::escape(report);
#endif
    report.replace(' ', "&nbsp;");
    report.replace('\n', "<br/>\n");

    QRegExp reg("\\{\\{ check\\.getConformance\\|replace\\(\\{' ': '\\&nbsp;'\\}\\)\\|raw\\|nl2br \\}\\}");

    int pos = 0;

    reg.setMinimal(true);
    if ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), report);
}

//---------------------------------------------------------------------------
void MainWindow::change_html_file_detail_policy_report(QString& html, String&, String& policy)
{
    //TODO: second parameter is the file, should do a Run() XML when database created
    bool valid;
    String r;
    if (!C.ValidatePolicy(policy, valid, r))
        valid = false;

    QString report = QString().fromStdWString(r);
#if QT_VERSION >= 0x050200
    report = report.toHtmlEscaped();
#else
    report = Qt::escape(report);
#endif
    report.replace('\n', "<br/>\n");

    QRegExp reg("\\{\\{ check\\.getStatus \\? 'success' : 'danger' \\}\\}");
    int pos = 0;

    reg.setMinimal(true);
    if ((pos = reg.indexIn(html, pos)) != -1)
    {
        if (!valid)
            html.replace(pos, reg.matchedLength(), "danger");
        else
            html.replace(pos, reg.matchedLength(), "success");
    }

    reg = QRegExp("\\{\\{ check\\.getStatus \\? '' : 'not ' \\}\\}");
    reg.setMinimal(true);
    if ((pos = reg.indexIn(html, pos)) != -1)
    {
        if (!valid)
            html.replace(pos, reg.matchedLength(), "not ");
        else
            html.replace(pos, reg.matchedLength(), QString());
    }

    reg = QRegExp("\\{% if check\\.getPolicy\\|length > 0 %\\}");
    reg.setMinimal(true);
    if ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), report);
}

//---------------------------------------------------------------------------
void MainWindow::change_html_file_detail_trace(QString& html, String& file)
{
    QRegExp reg("data-filename=\"\\{\\{ check\\.getTrace \\}\\}\"");
    reg.setMinimal(true);

    int pos = 0;
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), QString("data-filename=\"%1\"").arg(QString().fromStdWString(file)));

    reg = QRegExp("\\{\\{ check\\.getTrace\\.jstree\\|raw \\}\\}");
    reg.setMinimal(true);

    QString report = Run(Core::tool_MediaTrace, Core::format_JsTree, file);
    pos = 0;
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), report);
}

//---------------------------------------------------------------------------
void MainWindow::change_html_file_detail(QString& html, String& file)
{
    static int index = 0;
    ++index;

    QRegExp reg("\\{\\{ check.getSource \\}\\}");

    QFileInfo f(QString().fromStdWString(file));
    int pos = 0;

    reg.setMinimal(true);
    if ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), f.fileName());

    reg = QRegExp("\\{\\{ loop\\.index \\}\\}");
    pos = 0;
    reg.setMinimal(true);
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), QString("%1").arg(index));
}

//---------------------------------------------------------------------------
QString MainWindow::create_html_file_detail(String& file, String& policy)
{
    QFile template_html(":/fileDetailChecker.html");

    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray html = template_html.readAll();
    template_html.close();

    QString base(html);
    change_html_file_detail(base, file);
    change_html_file_detail_conformance(base, file);
    change_html_file_detail_inform_xml(base, file);
    // /!\: always has to be after inform_xml (need to be runned one time)
    change_html_file_detail_policy_report(base, file, policy);
    change_html_file_detail_trace(base, file);

    return base;
}

//---------------------------------------------------------------------------
void MainWindow::add_file_detail_to_html(QString& html, String& file, String& policy)
{
    QString new_html = create_html_file_detail(file, policy);

    QRegExp reg("<div class=\"col-md-6\">");
    reg.setMinimal(true);

    int pos = 0;
    int nb = 0;
    while ((pos = reg.indexIn(html, pos)) != -1)
    {
        ++nb;
        pos += reg.matchedLength();
        if (nb == 2)
        {
            html.insert(pos, new_html);
            break;
        }
    }
}

}
