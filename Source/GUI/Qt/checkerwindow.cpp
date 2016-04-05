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
#include "WebView.h"

#include <QTextEdit>
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

#if defined(WEB_MACHINE_ENGINE)
#include <QWebChannel>
#endif
#if defined(WEB_MACHINE_KIT)
#include <QWebFrame>
#endif

#if defined(WINDOWS)
    #include <windows.h>
#else
    #include <unistd.h>
#endif //!defined(WINDOWS)

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

CheckerWindow::CheckerWindow(MainWindow *parent) : mainwindow(parent)
{
    // Visual elements
    progress_bar = NULL;
    main_view = NULL;
    result_index = 0;
}

CheckerWindow::~CheckerWindow()
{
    if (main_view)
    {
        mainwindow->remove_widget_from_layout(main_view);
#if defined(WEB_MACHINE_ENGINE)
        WebPage* page = (WebPage*)main_view->page();
        QWebChannel *channel = page ? page->webChannel() : NULL;
        if (channel)
            channel->deregisterObject(page);
#endif
        delete main_view;
        main_view = NULL;
    }
    clear_visual_elements();
}

//***************************************************************************
// Visual elements
//***************************************************************************

//---------------------------------------------------------------------------
void CheckerWindow::clear_visual_elements()
{
    if (main_view)
        main_view->hide();

    if (progress_bar)
    {
        mainwindow->remove_widget_from_layout(progress_bar);
        delete progress_bar;
        progress_bar = NULL;
    }
}

//---------------------------------------------------------------------------
void CheckerWindow::create_web_view_finished(bool ok)
{
    if (!main_view || !ok)
    {
        create_web_view();
        mainwindow->set_msg_to_status_bar("Problem to load the checker page");
        return;
    }

    if (progress_bar)
    {
        mainwindow->remove_widget_from_layout(progress_bar);
        delete progress_bar;
        progress_bar = NULL;
    }
    mainwindow->set_widget_to_layout(main_view);
}

//---------------------------------------------------------------------------
void CheckerWindow::set_web_view_content(QString& html)
{
    if (!main_view)
        main_view = new WebView(mainwindow);

    WebPage* page = new WebPage(mainwindow, main_view);
    main_view->setPage(page);

    QObject::connect(main_view, SIGNAL(loadProgress(int)), progress_bar->get_progress_bar(), SLOT(setValue(int)));
    QObject::connect(main_view, SIGNAL(loadFinished(bool)), this, SLOT(create_web_view_finished(bool)));

    QUrl url = QUrl("qrc:/html");
    if (!url.isValid())
        return;

#if defined(WEB_MACHINE_ENGINE)
    QWebChannel *channel = new QWebChannel(page);
    page->setWebChannel(channel);
    channel->registerObject("webpage", page);
#endif
    main_view->setContent(html.toUtf8(), "text/html", url);
}

//---------------------------------------------------------------------------
void CheckerWindow::create_web_view()
{
    if (main_view)
    {
        mainwindow->remove_widget_from_layout(main_view);
#if defined(WEB_MACHINE_ENGINE)
        WebPage* page = (WebPage*)main_view->page();
        QWebChannel *channel = page ? page->webChannel() : NULL;
        if (channel)
            channel->deregisterObject(page);
#endif
        delete main_view;
        main_view = NULL;
    }

    clear_visual_elements();

    progress_bar = new ProgressBar(mainwindow);
    mainwindow->set_widget_to_layout(progress_bar);
    progress_bar->get_progress_bar()->setValue(0);
    progress_bar->show();

    QString html = create_html();
    set_web_view_content(html);
}

//---------------------------------------------------------------------------
void CheckerWindow::change_local_files(QStringList& files)
{
    if (!main_view || !main_view->page())
        return;

    WebPage* p = (WebPage*)main_view->page();
    p->changeLocalFiles(files);
}

//---------------------------------------------------------------------------
void CheckerWindow::hide()
{
    clear_visual_elements();
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
        html.replace(pos, reg.matchedLength(), "");
}

//---------------------------------------------------------------------------
void CheckerWindow::change_collapse_form(QString& html)
{
    QRegExp reg("class=\"panel-collapse collapse in\"");
    int pos = 0;

    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), "class=\"panel-collapse collapse\"");
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
QString CheckerWindow::create_form_upload()
{
    QFile template_html(":/formUpload.html");

    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray html = template_html.readAll();
    template_html.close();

    QString ret(html);
    QString policies;
    mainwindow->create_policy_options(policies);
    mainwindow->add_policy_to_html_selection(policies, ret, "checkerUpload_step1_policy");

    QString displays;
    mainwindow->create_displays_options(displays);
    mainwindow->add_display_to_html_selection(displays, ret, "checkerUpload_step1_display_selector");
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
    mainwindow->create_policy_options(policies);
    mainwindow->add_policy_to_html_selection(policies, ret, "checkerOnline_step1_policy");

    QString displays;
    mainwindow->create_displays_options(displays);
    mainwindow->add_display_to_html_selection(displays, ret, "checkerOnline_step1_display_selector");
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
    mainwindow->create_policy_options(policies);
    mainwindow->add_policy_to_html_selection(policies, ret, "checkerRepository_step1_policy");

    QString displays;
    mainwindow->create_displays_options(displays);
    mainwindow->add_display_to_html_selection(displays, ret, "checkerRepository_step1_display_selector");
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
        html.replace(pos, reg.matchedLength(), body);
}

//---------------------------------------------------------------------------
void CheckerWindow::change_body_script_in_template(QString& html)
{
    QRegExp reg("\\{% block bodyScript %\\}\\{% endblock %\\}");
    QString script;
    int     pos = 0;

    reg.setMinimal(true);
#if defined(WEB_MACHINE_KIT)
    script = "";
#elif defined(WEB_MACHINE_ENGINE)
    script = "<script>\n"
        "var webpage;\n"
        "$(document).ready(function()\n"
        "{\n"
        "// Register Qt WebPage object\n"
        "new QWebChannel(qt.webChannelTransport, function (channel) {\n"
        "webpage = channel.objects.webpage;\n"
        "});\n"
        "});\n"
        "</script>";
#endif
    if ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), script);
}

//---------------------------------------------------------------------------
QString CheckerWindow::create_html_base(QString& body)
{
    QFile template_html(":/base.html");
    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray html = template_html.readAll();
    template_html.close();

    QString base(html);

    change_body_script_in_template(base);
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

}
