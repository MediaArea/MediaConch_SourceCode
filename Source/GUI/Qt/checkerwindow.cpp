/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "Common/Policy.h"
#include "Common/ImplementationReportDisplayHtmlXsl.h"
#include "mainwindow.h"
#include "checkerwindow.h"
#include "ui_mainwindow.h"
#include "policiestree.h"
#include "policiesmenu.h"
#include "policymenu.h"
#include "groupofrules.h"
#include "rulemenu.h"
#include "ruleedit.h"
#include "WebPage.h"
#include "WebView.h"
#include "progressbar.h"

#include <QTextEdit>
#include <QWebView>
#include <QWebFrame>
#include <QWebElement>
#include <QFrame>
#include <QProgressBar>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QUrl>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif
#if QT_VERSION >= 0x050200
    #include <QFontDatabase>
#endif

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

CheckerWindow::CheckerWindow(MainWindow *parent) : mainwindow(parent)
{
    // Visual elements
    progressBar=NULL;
    MainView=NULL;
    analyse=false;
}

CheckerWindow::~CheckerWindow()
{
    clearVisualElements();
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
void CheckerWindow::checker_add_file(QString& file, int policy)
{
    mainwindow->addFileToList(file);
    updateWebView(file.toStdString(), policy);
}

//---------------------------------------------------------------------------
void CheckerWindow::checker_add_files(QFileInfoList& list, int policy)
{
    for (int i = 0; i < list.count(); ++i)
    {
        QString file = list[i].absoluteFilePath();
        mainwindow->addFileToList(file);
    }
    updateWebView(list, policy);
}

//---------------------------------------------------------------------------
void CheckerWindow::checker_add_policy_file(QString& file, QString& policy)
{
    mainwindow->addFileToList(file);

    mainwindow->addPolicyToList(policy);
    updateWebView(file.toStdString(), -1);
    mainwindow->clearPolicyList();
}

//---------------------------------------------------------------------------
void CheckerWindow::checker_add_policy_files(QFileInfoList& list, QString& policy)
{
    for (int i = 0; i < list.count(); ++i)
    {
        QString file = list[i].absoluteFilePath();
        mainwindow->addFileToList(file);
    }
    mainwindow->addPolicyToList(policy);
    updateWebView(list, -1);
    mainwindow->clearPolicyList();
}

//---------------------------------------------------------------------------
bool CheckerWindow::is_analyzes_done()
{
    return analyse;
}

//***************************************************************************
// Slots
//***************************************************************************

//---------------------------------------------------------------------------
void CheckerWindow::actionCloseAllTriggered()
{
    mainwindow->clearFileList();
    clearVisualElements();
    createWebView();
    analyse = false;
}

//***************************************************************************
// Visual elements
//***************************************************************************

//---------------------------------------------------------------------------
void CheckerWindow::clearVisualElements()
{
    if (MainView)
    {
        mainwindow->remove_widget_from_layout(MainView);
        delete MainView;
        MainView=NULL;
    }

    if (progressBar)
    {
        mainwindow->remove_widget_from_layout(progressBar);
        delete progressBar;
        progressBar=NULL;
    }
}

//---------------------------------------------------------------------------
void CheckerWindow::createWebViewFinished(bool ok)
{
    if (!MainView || !ok)
        return; //TODO: Error

    if (progressBar)
    {
        mainwindow->remove_widget_from_layout(progressBar);
        delete progressBar;
        progressBar=NULL;
    }
    mainwindow->set_widget_to_layout(MainView);
}

//---------------------------------------------------------------------------
void CheckerWindow::setWebViewContent(QString& html)
{
    MainView=new WebView(mainwindow);

    WebPage* page = new WebPage(mainwindow, MainView);
    MainView->setPage(page);

    QObject::connect(MainView, SIGNAL(loadProgress(int)), progressBar->get_progress_bar(), SLOT(setValue(int)));
    QObject::connect(MainView, SIGNAL(loadFinished(bool)), this, SLOT(createWebViewFinished(bool)));

    QUrl url = QUrl("qrc:/html");
    if (!url.isValid())
        return;

    MainView->setContent(html.toUtf8(), "text/html", url);
}

//---------------------------------------------------------------------------
void CheckerWindow::createWebView()
{
    if (MainView)
        return;

    clearVisualElements();

    progressBar = new ProgressBar(mainwindow);
    mainwindow->set_widget_to_layout(progressBar);
    progressBar->get_progress_bar()->setValue(0);
    progressBar->show();

    QString html = create_html();
    setWebViewContent(html);
}

//---------------------------------------------------------------------------
void CheckerWindow::updateWebView(std::string file, int policy)
{
    if (!MainView)
        return;

    QString html = MainView->page()->currentFrame()->toHtml();

    //Load the new page
    clearVisualElements();

    progressBar = new ProgressBar(mainwindow);
    mainwindow->set_widget_to_layout(progressBar);
    progressBar->show();
    progressBar->get_progress_bar()->setValue(1);
    progressBar->get_progress_bar()->setMaximum(0);
    progressBar->get_progress_bar()->setMaximum(100);

    // Analyze
    mainwindow->analyze(file);

    // TODO: do not wait until the end of the analyzed
    mainwindow->wait_analyze_finished();

    //Add the file detail to the web page
    add_file_detail_to_html(html, file, policy);

    setWebViewContent(html);
    analyse = true;
}

//---------------------------------------------------------------------------
void CheckerWindow::updateWebView(QList<QFileInfo>& files, int policy)
{
    if (!MainView)
        return;

    QString html = MainView->page()->currentFrame()->toHtml();

    //Load the new page
    clearVisualElements();
    progressBar = new ProgressBar(mainwindow);
    mainwindow->set_widget_to_layout(progressBar);
    progressBar->show();
    progressBar->get_progress_bar()->setValue(1);
    progressBar->get_progress_bar()->setMaximum(0);

    // Analyze
    for (int i = 0; i < files.count(); ++i)
    {
        std::string file = files[i].absoluteFilePath().toStdString();
        mainwindow->analyze(file);
    }

    progressBar->get_progress_bar()->setMaximum(100);
    progressBar->get_progress_bar()->setValue(1);
    // TODO: do not wait until the end of the analyzed
    mainwindow->wait_analyze_finished();

    //Add the files details to the web page
    QString displayXsltRetain = displayXslt;
    for (int i = 0; i < files.count(); ++i)
    {
        displayXslt = displayXsltRetain;
        std::string file = files[i].absoluteFilePath().toStdString();
        add_file_detail_to_html(html, file, policy);
        progressBar->get_progress_bar()->setValue(((i + 1) * 100) / files.count());
    }
    resetDisplayXslt();

    setWebViewContent(html);
    analyse = true;
}

//---------------------------------------------------------------------------
void CheckerWindow::changeLocalFiles(QStringList& files)
{
    if (!MainView || !MainView->page())
        return;

    WebPage* p = (WebPage*)MainView->page();
    p->changeLocalFiles(files);
}

//***************************************************************************
// HELPER
//***************************************************************************

//---------------------------------------------------------------------------
void CheckerWindow::load_include_in_template(QString& html)
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
void CheckerWindow::remove_element_in_template(QString& html)
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
void CheckerWindow::change_collapse_form(QString& html)
{
    QRegExp reg("class=\"panel-collapse collapse in\"");
    int pos = 0;

    while ((pos = reg.indexIn(html, pos)) != -1)
    {
        html.replace(pos, reg.matchedLength(), "class=\"panel-collapse collapse\"");
    }
}

//---------------------------------------------------------------------------
void CheckerWindow::load_form_in_template(QString& html)
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
void CheckerWindow::create_policy_options(QString& policies)
{
    const std::vector<Policy *>& list = mainwindow->get_all_policies();

    QString system_policy;
    QString user_policy;
    for (size_t i = 0; i < list.size(); ++i)
    {
        if (list[i]->filename.length() && list[i]->filename.find(":/") == 0)
            system_policy += QString("<option value=\"%1\">%2</option>")
                .arg((int)i).arg(QString().fromUtf8(list[i]->title.c_str(), list[i]->title.length()));
        else
            user_policy += QString("<option value=\"%1\">%2</option>")
                .arg((int)i).arg(QString().fromUtf8(list[i]->title.c_str(), list[i]->title.length()));
    }

    // Create default policy opt-group
    if (system_policy.length())
        policies += QString("<optgroup label=\"System policies\">%1</optgroup>").arg(system_policy);

    // Create default policy opt-group
    if (user_policy.length())
        policies += QString("<optgroup label=\"User policies\">%1</optgroup>").arg(user_policy);
}

//---------------------------------------------------------------------------
void CheckerWindow::create_displays_options(QString& displays)
{
    std::vector<QString>& list = mainwindow->get_displays();

    QString system_display;
    QString user_display;
    for (size_t i = 0; i < list.size(); ++i)
    {
        QFileInfo file(list[i]);
        if (list[i].startsWith(":/"))
            system_display += QString("<option value=\"%1\">%2</option>")
                .arg((int)i).arg(file.baseName());
        else
            user_display += QString("<option value=\"%1\">%2</option>")
                .arg((int)i).arg(file.baseName());
    }

    // Create default display opt-group
    if (system_display.length())
        displays += QString("<optgroup label=\"System displays\">%1</optgroup>").arg(system_display);

    // Create user display opt-group
    if (user_display.length())
        displays += QString("<optgroup label=\"User displays\">%1</optgroup>").arg(user_display);
}

//---------------------------------------------------------------------------
void CheckerWindow::add_policy_to_form_selection(QString& policies, QString& form, const char *selector)
{
    QRegExp reg("<option selected=\"selected\" value=\"-1\">[\\n\\r\\t\\s]*Choose a policy[\\n\\r\\t\\s]*</option>");
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
void CheckerWindow::add_display_to_form_selection(QString& displays, QString& form, const char *selector)
{
    QRegExp reg("<option selected=\"selected\" value=\"-1\">[\\n\\r\\t\\s]*Choose a Display[\\n\\r\\t\\s]*</option>");
    reg.setMinimal(true);

    int pos = form.indexOf(selector);
    if (pos == -1)
        return;

    if ((pos = reg.indexIn(form, pos)) != -1)
    {
        pos += reg.matchedLength();
        form.insert(pos, displays);
    }
}

//---------------------------------------------------------------------------
QString CheckerWindow::create_form_upload()
{
    QFile template_html(":/formUpload.html");

    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray html = template_html.readAll();
    template_html.close();

    QString ret(html);
    QString policies;
    create_policy_options(policies);
    add_policy_to_form_selection(policies, ret, "checkerUpload_step1_policy");

    QString displays;
    create_displays_options(displays);
    add_display_to_form_selection(displays, ret, "checkerUpload_step1_display_selector");
    return ret;
}

//---------------------------------------------------------------------------
QString CheckerWindow::create_form_online()
{
    QFile template_html(":/formOnline.html");

    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray html = template_html.readAll();
    template_html.close();

    QString ret(html);
    QString policies;
    create_policy_options(policies);
    add_policy_to_form_selection(policies, ret, "checkerOnline_step1_policy");

    QString displays;
    create_displays_options(displays);
    add_display_to_form_selection(displays, ret, "checkerOnline_step1_display_selector");
    return ret;
}

//---------------------------------------------------------------------------
void CheckerWindow::remove_form_online(int pos, QString& html)
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
QString CheckerWindow::create_form_repository()
{
    QFile template_html(":/formRepository.html");

    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray html = template_html.readAll();
    template_html.close();

    QString ret(html);
    QString policies;
    create_policy_options(policies);
    add_policy_to_form_selection(policies, ret, "checkerRepository_step1_policy");

    QString displays;
    create_displays_options(displays);
    add_display_to_form_selection(displays, ret, "checkerRepository_step1_display_selector");
    return ret;
}

//---------------------------------------------------------------------------
void CheckerWindow::remove_template_tags(QString& data)
{
    load_include_in_template(data);
    remove_element_in_template(data);
    load_form_in_template(data);
}

//---------------------------------------------------------------------------
QString CheckerWindow::create_html_body()
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
void CheckerWindow::change_body_in_template(QString& body, QString& html)
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
QString CheckerWindow::create_html_base(QString& body)
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
QString CheckerWindow::create_html()
{
    QString body = create_html_body();
    QString base = create_html_base(body);
    return base;
}

//---------------------------------------------------------------------------
void CheckerWindow::change_html_file_detail_inform_xml(QString& html, std::string& file)
{
    QRegExp reg("data-filename=\"\\{\\{ check\\.getInfo \\}\\}\"");
    reg.setMinimal(true);

    int pos = 0;
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), QString("data-filename=\"%1\"").arg(QString().fromUtf8(file.c_str(), file.length())));

    reg = QRegExp("\\{\\{ check\\.getInfo\\.jstree\\|raw \\}\\}");
    reg.setMinimal(true);

    QString report = mainwindow->get_mediainfo_jstree(file);
    pos = 0;
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), report);

    reg = QRegExp("data-save-name=\"MediaInfo.xml\"");
    reg.setMinimal(true);

    while ((pos = reg.indexIn(html, 0)) != -1)
        html.replace(pos, reg.matchedLength(), QString("data-save-name=\"%1.MediaInfo.xml\"")
                     .arg(QString().fromUtf8(file.c_str(), file.length())));

    reg = QRegExp("<div id=\"infoXml\\d+\" class=\"hidden\">");
    if ((pos = reg.indexIn(html, 0)) != -1)
    {
        reg = QRegExp("<p class=\"modal-body\">");
        if ((pos = reg.indexIn(html, pos)) != -1)
        {
            report = mainwindow->get_mediainfo_xml(file);
#if QT_VERSION >= 0x050200
            report = report.toHtmlEscaped();
#else
            report = Qt::escape(report);
#endif
            report.replace(' ', "&nbsp;");
            report.replace('\n', "<br/>");
            html.insert(pos + reg.matchedLength(), report);
        }
    }
}

//---------------------------------------------------------------------------
void CheckerWindow::change_html_file_detail_conformance(QString& html, std::string& file)
{
    QString report = mainwindow->get_implementationreport_xml(file);
    bool is_valid = true;

    if (report.indexOf(" outcome=\"fail\"") != -1)
        is_valid = false;

    // Apply HTML default display
    std::string r = report.toStdString();
    QString save_ext = "xml";
    bool is_html = false;
    if (displayXslt.length())
    {
        if (displayXslt.startsWith(":/displays/"))
        {
            QFile display_file(displayXslt);
            if (display_file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QByteArray data = display_file.readAll();
                display_file.close();
                r = mainwindow->transformWithXsltMemory(r, data.data());
            }
        }
        else
        {
            std::string trans = displayXslt.toStdString();
            r = mainwindow->transformWithXsltFile(r, trans);
        }

        report = QString().fromUtf8(r.c_str(), r.length());
        is_html = report_is_html(report);
        save_ext = is_html ? "html" : report_is_xml(report) ? "xml" : "txt";
    }
    else
    {
        r = mainwindow->transformWithXsltMemory(r, implementation_report_display_html_xsl);
        report = QString().fromUtf8(r.c_str(), r.length());
        is_html = report_is_html(report);
        save_ext = is_html ? "html" : report_is_xml(report) ? "xml" : "txt";
    }

    if (!is_html)
    {
#if QT_VERSION >= 0x050200
        report = report.toHtmlEscaped();
#else
        report = Qt::escape(report);
#endif
        report.replace(' ', "&nbsp;");
        report.replace('\n', "<br/>\n");
    }

    QRegExp reg("Implementation report");
    int pos = 0;
    reg.setMinimal(true);
    if ((pos = reg.indexIn(html, 0)) != -1)
    {
        reg = QRegExp("\\{\\{ check\\.getStatus \\? 'success' : 'danger' \\}\\}");
        reg.setMinimal(true);
        if ((pos = reg.indexIn(html, pos)) != -1)
            html.replace(pos, reg.matchedLength(), is_valid ? "success" : "danger");
        reg = QRegExp("\\{\\{ check\\.getStatus \\? '' : 'not ' \\}\\}");
        reg.setMinimal(true);
        if ((pos = reg.indexIn(html, pos)) != -1)
            html.replace(pos, reg.matchedLength(), is_valid ? "" : "not ");
    }

    reg = QRegExp("\\{\\{ check\\.getConformance\\|replace\\(\\{' ': '\\&nbsp;'\\}\\)\\|raw\\|nl2br \\}\\}");
    reg.setMinimal(true);
    if ((pos = reg.indexIn(html, 0)) != -1)
        html.replace(pos, reg.matchedLength(), report);

    reg = QRegExp("data-save-name=\"ImplementationReport.txt\"");
    reg.setMinimal(true);

    while ((pos = reg.indexIn(html, 0)) != -1)
        html.replace(pos, reg.matchedLength(), QString("data-save-name=\"%1.ImplementationReport.%2\"")
                     .arg(QString().fromUtf8(file.c_str(), file.length())).arg(save_ext));
}

//---------------------------------------------------------------------------
void CheckerWindow::change_html_file_detail_policy_report(QString& html, std::string& file, int policy)
{
    if (policy == -1 && !mainwindow->policy_file_registered().size())
    {
        remove_html_file_detail_policy_report(html);
        return;
    }

    bool valid;
    std::string r;
    if (!mainwindow->ValidatePolicy(file, policy, valid, r))
        valid = false;

    // Default without display
    QString save_ext("xml");
    bool is_html = false;

    QString report = QString().fromUtf8(r.c_str(), r.length());
    if (displayXslt.length())
    {
        if (displayXslt.startsWith(":/displays/"))
        {
            QFile display_file(displayXslt);
            if (display_file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QByteArray data = display_file.readAll();
                display_file.close();
                r = mainwindow->transformWithXsltMemory(r, data.data());
            }
        }
        else
        {
            std::string trans = displayXslt.toStdString();
            r = mainwindow->transformWithXsltFile(r, trans);
        }

        report = QString().fromUtf8(r.c_str(), r.length());
        is_html = report_is_html(report);
        save_ext = is_html ? "html" : report_is_xml(report) ? "xml" : "txt";
        resetDisplayXslt();
    }
    else
    {
        r = mainwindow->transformWithXsltMemory(r, implementation_report_display_html_xsl);
        report = QString().fromUtf8(r.c_str(), r.length());
        is_html = report_is_html(report);
        save_ext = is_html ? "html" : report_is_xml(report) ? "xml" : "txt";
    }

    QString policy_name;
    if (policy >= 0)
    {
        const std::string& title = mainwindow->get_all_policies()[policy]->title;
        policy_name = QString().fromUtf8(title.c_str(), title.length());
    }
    else
    {
        const std::vector<std::string>& policies_name = mainwindow->policy_file_registered();
        if (policies_name.size())
        {
            const std::string& name = policies_name.front();
            QFileInfo qfile(QString().fromUtf8(name.c_str(), name.length()));
            policy_name = qfile.baseName();
        }
    }

    if (!is_html)
    {
#if QT_VERSION >= 0x050200
        report = report.toHtmlEscaped();
#else
        report = Qt::escape(report);
#endif
        report.replace('\n', "<br/>\n");
    }
    change_report_policy_save_name(file, save_ext, html);

    QRegExp reg("\\{\\{ check\\.getPolicy \\}\\}");
    reg.setMinimal(true);
    int pos = 0;
    if ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), policy_name);

    reg = QRegExp("\\{\\{ check\\.getStatus \\? 'success' : 'danger' \\}\\}");
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
    if ((pos = reg.indexIn(html, 0)) != -1)
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
void CheckerWindow::change_html_file_detail_trace(QString& html, std::string& file)
{
    QRegExp reg("data-filename=\"\\{\\{ check\\.getTrace \\}\\}\"");
    reg.setMinimal(true);

    int pos = 0;
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), QString("data-filename=\"%1\"").arg(QString().fromUtf8(file.c_str(), file.length())));

    reg = QRegExp("\\{\\{ check\\.getTrace\\.jstree\\|raw \\}\\}");
    reg.setMinimal(true);

    QString report = mainwindow->get_mediatrace_jstree(file);
    pos = 0;
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), report);

    reg = QRegExp("data-save-name=\"MediaTrace.xml\"");
    reg.setMinimal(true);

    while ((pos = reg.indexIn(html, 0)) != -1)
        html.replace(pos, reg.matchedLength(), QString("data-save-name=\"%1.MediaTrace.xml\"")
                     .arg(QString().fromUtf8(file.c_str(), file.length())));

    reg = QRegExp("<div id=\"traceXml\\d+\" class=\"hidden\">");
    if ((pos = reg.indexIn(html, 0)) != -1)
    {
        reg = QRegExp("<p class=\"modal-body\">");
        if ((pos = reg.indexIn(html, pos)) != -1)
        {
            report = mainwindow->get_mediatrace_xml(file);
#if QT_VERSION >= 0x050200
            report = report.toHtmlEscaped();
#else
            report = Qt::escape(report);
#endif
            report.replace(' ', "&nbsp;");
            report.replace('\n', "<br/>");
            html.insert(pos + reg.matchedLength(), report);
        }
    }
}

//---------------------------------------------------------------------------
void CheckerWindow::change_html_file_detail(QString& html, std::string& file)
{
    static int index = 0;
    ++index;

    QRegExp reg("\\{\\{ check.getSource \\}\\}");

    QFileInfo f(QString().fromUtf8(file.c_str(), file.length()));
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
void CheckerWindow::remove_html_file_detail_policy_report(QString& html)
{
    QRegExp reg("<li class=\"list-group-item report policyResult\">");
    reg.setMinimal(true);
    int pos = 0;
    if ((pos = reg.indexIn(html, pos)) == -1)
        return;
    html.insert(pos + 26, " hidden");
}

//---------------------------------------------------------------------------
QString CheckerWindow::create_html_file_detail(std::string& file, int policy)
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
void CheckerWindow::add_file_detail_to_html(QString& html, std::string& file, int policy)
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

//---------------------------------------------------------------------------
bool CheckerWindow::report_is_html(QString& report)
{
    QRegExp reg("<\\!DOCTYPE.*html", Qt::CaseInsensitive);

    if (reg.indexIn(report, 0) != -1)
        return true;

    return false;
}

//---------------------------------------------------------------------------
bool CheckerWindow::report_is_xml(QString& report)
{
    QRegExp reg("<\\?xml ", Qt::CaseInsensitive);

    if (reg.indexIn(report, 0) != -1)
        return true;

    return false;
}

//---------------------------------------------------------------------------
bool CheckerWindow::implementationreport_is_valid(QString& report)
{
    QRegExp reg("Fail \\|", Qt::CaseInsensitive);

    if (reg.indexIn(report, 0) != -1)
        return false;

    return true;
}

//---------------------------------------------------------------------------
void CheckerWindow::change_report_policy_save_name(std::string& file, QString& ext, QString& html)
{
    QRegExp reg("data-save-name=\"PolicyReport.txt\"");
    reg.setMinimal(true);

    int pos = 0;
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), QString("data-save-name=\"%1.MediaConch.%2\"")
                     .arg(QString().fromUtf8(file.c_str(), file.length())).arg(ext));
}

//---------------------------------------------------------------------------
QString CheckerWindow::file_remove_ext(std::string& file)
{
    QFileInfo f(QString().fromUtf8(file.c_str(), file.length()));

    QString ret = f.baseName();
    int pos;
    if ((pos = ret.lastIndexOf('.')) != -1)
        ret.chop(ret.length() - pos);
    return ret;
}

}
