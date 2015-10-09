/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

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

#include <QTextEdit>
#include <QWebView>
#include <QWebFrame>
#include <QWebElement>
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
}

CheckerWindow::~CheckerWindow()
{
    clearVisualElements();
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
void CheckerWindow::checker_add_file(QString& file, QString& policy)
{
    mainwindow->addFileToList(file);
    updateWebView(file.toStdWString(), policy.toStdWString());
}

//---------------------------------------------------------------------------
void CheckerWindow::checker_add_files(QFileInfoList& list, QString& policy)
{
    for (int i = 0; i < list.count(); ++i)
    {
        QString file = list[i].absoluteFilePath();
        mainwindow->addFileToList(file);
    }
    updateWebView(list, policy.toStdWString());
}

//---------------------------------------------------------------------------
void CheckerWindow::checker_add_xslt_file(QString& file, QString& xslt, QString& display_xslt)
{
    mainwindow->addFileToList(file);

    mainwindow->addXsltToList(xslt, display_xslt);
    updateWebView(file.toStdWString(), String());
    mainwindow->clearXsltList();
}

//---------------------------------------------------------------------------
void CheckerWindow::checker_add_xslt_files(QFileInfoList& list, QString& xslt, QString& display_xslt)
{
    for (int i = 0; i < list.count(); ++i)
    {
        QString file = list[i].absoluteFilePath();
        mainwindow->addFileToList(file);
    }
    mainwindow->addXsltToList(xslt, display_xslt);
    updateWebView(list, String());
    mainwindow->clearXsltList();
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
    if (!MainView)
        return;

    if (progressBar)
    {
        mainwindow->remove_widget_from_layout(progressBar);
        delete progressBar;
        progressBar = NULL;
    }
    if (!ok)
        return; //TODO: Error
    mainwindow->set_widget_to_layout(MainView);
}

//---------------------------------------------------------------------------
void CheckerWindow::setWebViewContent(QString& html)
{
    MainView=new WebView(mainwindow);

    WebPage* page = new WebPage(mainwindow, MainView);
    MainView->setPage(page);

    QObject::connect(MainView, SIGNAL(loadProgress(int)), progressBar, SLOT(setValue(int)));
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

    progressBar = new QProgressBar();
    mainwindow->set_widget_to_layout(progressBar);

    QString html = create_html();
    setWebViewContent(html);
}

//---------------------------------------------------------------------------
void CheckerWindow::updateWebView(String file, String policy)
{
    if (!MainView)
        return;

    QString html = MainView->page()->currentFrame()->toHtml();

    //Load the new page
    clearVisualElements();
    progressBar = new QProgressBar();
    mainwindow->set_widget_to_layout(progressBar);
    progressBar->setValue(0);
    progressBar->show();

    //Add the file detail to the web page
    add_file_detail_to_html(html, file, policy);

    setWebViewContent(html);
}

//---------------------------------------------------------------------------
void CheckerWindow::updateWebView(QList<QFileInfo>& files, String policy)
{
    if (!MainView)
        return;

    QString html = MainView->page()->currentFrame()->toHtml();

    //Load the new page
    clearVisualElements();
    progressBar = new QProgressBar();
    mainwindow->set_widget_to_layout(progressBar);
    progressBar->setValue(0);
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
    QStringList list = mainwindow->get_policy_titles();

    for (int i = 0; i < list.count(); ++i)
        policies += QString("<option value=\"%1\">%1</option>").arg(list[i]);
}

//---------------------------------------------------------------------------
void CheckerWindow::add_policy_to_form_selection(QString& policies, QString& form, const char *selector)
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
void CheckerWindow::change_html_file_detail_inform_xml(QString& html, String& file)
{
    QString report = mainwindow->Run(Core::tool_MediaConch, Core::format_Xml, file);
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
void CheckerWindow::change_html_file_detail_conformance(QString& html, String& file)
{
    QString report = mainwindow->Run(Core::tool_MediaConch, Core::format_Text, file);
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
void CheckerWindow::change_html_file_detail_policy_report(QString& html, String&, String& policy)
{
    //TODO: second parameter is the file, should do a Run() XML when database created
    bool valid;
    String r;
    if (!mainwindow->ValidatePolicy(policy, valid, r))
        valid = false;

    if (displayXslt.length())
    {
        String trans = displayXslt.toStdWString();
        r = mainwindow->transformWithXslt(r, trans);
    }

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
void CheckerWindow::change_html_file_detail_trace(QString& html, String& file)
{
    QRegExp reg("data-filename=\"\\{\\{ check\\.getTrace \\}\\}\"");
    reg.setMinimal(true);

    int pos = 0;
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), QString("data-filename=\"%1\"").arg(QString().fromStdWString(file)));

    reg = QRegExp("\\{\\{ check\\.getTrace\\.jstree\\|raw \\}\\}");
    reg.setMinimal(true);

    QString report = mainwindow->Run(Core::tool_MediaTrace, Core::format_JsTree, file);
    pos = 0;
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), report);
}

//---------------------------------------------------------------------------
void CheckerWindow::change_html_file_detail(QString& html, String& file)
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
QString CheckerWindow::create_html_file_detail(String& file, String& policy)
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
void CheckerWindow::add_file_detail_to_html(QString& html, String& file, String& policy)
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
