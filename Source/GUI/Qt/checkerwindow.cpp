/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "Common/Policy.h"
#include "Common/generated/ImplementationReportDisplayHtmlXsl.h"
#include "mainwindow.h"
#include "checkerwindow.h"
#include "ui_mainwindow.h"
#include "WebPage.h"
#include "WebView.h"
#include "progressbar.h"
#include "WebView.h"
#include "resulttable.h"

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

CheckerWindow::CheckerWindow(MainWindow *parent) : mainwindow(parent), result_table(NULL)
{
    // Visual elements
    progress_bar = NULL;
    main_view = NULL;
    result_index = 0;
    is_finished = false;
}

CheckerWindow::~CheckerWindow()
{
    if (result_table)
    {
        delete result_table;
        result_table = NULL;
    }
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

    if (result_table)
    {
        delete result_table;
        result_table = NULL;
    }
    is_finished = true;

    result_table = new ResultTable(mainwindow, (WebPage*)main_view->page());
    if (files.size())
    {
        for (size_t i = 0; i < files.size(); ++i)
            result_table->add_file_to_result_table(files[i]);
        files.clear();
        page_start_waiting_loop();
    }

    if (progress_bar)
    {
        mainwindow->remove_widget_from_layout(progress_bar);
        delete progress_bar;
        progress_bar = NULL;
    }

    main_view->show();
    main_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainwindow->set_widget_to_layout(main_view);
}

//---------------------------------------------------------------------------
void CheckerWindow::set_web_view_content(QString& html)
{
    if (!main_view)
        main_view = new WebView(mainwindow);
    main_view->hide();

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
    main_view->setHtml(html.toUtf8(), url);
#endif
#if defined(WEB_MACHINE_KIT)
    main_view->setContent(html.toUtf8(), "text/html", url);
#endif
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
void CheckerWindow::change_local_files(const QStringList& files)
{
    if (!main_view || !main_view->page())
        return;

    WebPage* p = (WebPage*)main_view->page();
    p->change_local_files(files);
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
void CheckerWindow::create_policy_options(QString& policies)
{
    MediaConchLib::Get_Policies list;
    mainwindow->get_policies("JSON", list);

    QString system_policy;
    QString user_policy;
    int selected_policy = mainwindow->select_correct_policy();
    for (size_t i = 0; list.policies && i < list.policies->size(); ++i)
    {
        if (!list.policies->at(i))
            continue;

        if (list.policies->at(i)->is_system)
        {
            system_policy += QString("<option ");
            if ((int)list.policies->at(i)->id == selected_policy)
                system_policy += QString("selected=\"selected\" ");
            system_policy += QString("value=\"%1\">%2</option>")
                .arg(list.policies->at(i)->id).arg(QString().fromUtf8(list.policies->at(i)->name.c_str(), list.policies->at(i)->name.length()));
        }
        else
        {
            user_policy += QString("<option ");
            if ((int)list.policies->at(i)->id == selected_policy)
                user_policy += QString("selected=\"selected\" ");
            user_policy += QString("value=\"%1\">%2</option>")
                .arg(list.policies->at(i)->id).arg(QString().fromUtf8(list.policies->at(i)->name.c_str(), list.policies->at(i)->name.length()));
        }
    }

    if (selected_policy == -1)
        policies += QString("<option selected=\"selected\" value=\"-1\">No policy</optgroup>");
    else
        policies += QString("<option value=\"-1\">No policy</optgroup>");

    // Create default policy opt-group
    if (user_policy.length())
        policies += QString("<optgroup label=\"User policies\">%1</optgroup>").arg(user_policy);

    // Create default policy opt-group
    if (system_policy.length())
        policies += QString("<optgroup label=\"System policies\">%1</optgroup>").arg(system_policy);
}

//---------------------------------------------------------------------------
void CheckerWindow::create_displays_options(QString& displays)
{
    QString system_display;
    QString user_display;
    int selected_display = mainwindow->select_correct_display();
    const std::vector<QString>& displays_list = mainwindow->get_displays();
    for (size_t i = 0; i < displays_list.size(); ++i)
    {
        QFileInfo file(displays_list[i]);
        if (displays_list[i].startsWith(":/"))
        {
            system_display += QString("<option ");
            if ((int)i == selected_display)
                system_display += QString("selected=\"selected\" ");
            system_display += QString("value=\"%1\">%2</option>")
                .arg((int)i).arg(file.baseName());
        }
        else
        {
            user_display += QString("<option ");
            if ((int)i == selected_display)
                user_display += QString("selected=\"selected\" ");
            user_display += QString("value=\"%1\">%2</option>")
                .arg((int)i).arg(file.baseName());
        }
    }

    // Create user display opt-group
    if (user_display.length())
        displays += QString("<optgroup label=\"User displays\">%1</optgroup>").arg(user_display);

    // Create default display opt-group
    if (system_display.length())
        displays += QString("<optgroup label=\"System displays\">%1</optgroup>").arg(system_display);
}

//---------------------------------------------------------------------------
void CheckerWindow::create_verbosity_options(QString& verbosity)
{
    int selected_verbosity = mainwindow->select_correct_verbosity();
    for (int i = 0; i < 6; ++i)
    {
        verbosity += QString("<option ");
        if ((int)i == selected_verbosity)
            verbosity += QString("selected=\"selected\" ");
        verbosity += QString("value=\"%1\">%1</option>").arg(i);
    }
}

//---------------------------------------------------------------------------
void CheckerWindow::add_policy_to_html_selection(QString& policies, QString& html, const QString& selector)
{
    QRegExp reg("class=\"policyList form-control\">");
    int pos = html.indexOf(selector);

    reg.setMinimal(true);

    if (pos == -1)
        return;

    if ((pos = reg.indexIn(html, pos)) != -1)
    {
        pos += reg.matchedLength();
        html.insert(pos, policies);
    }
}

//---------------------------------------------------------------------------
void CheckerWindow::add_display_to_html_selection(QString& displays, QString& html, const QString& selector)
{
    QRegExp reg("class=\"displayList form-control\">");
    reg.setMinimal(true);

    int pos = html.indexOf(selector);
    if (pos == -1)
        return;

    if ((pos = reg.indexIn(html, pos)) != -1)
    {
        pos += reg.matchedLength();
        html.insert(pos, displays);
    }
}

//---------------------------------------------------------------------------
void CheckerWindow::add_verbosity_to_html_selection(QString& verbosity, QString& html, const QString& selector)
{
    QRegExp reg("class=\"verbosityList form-control\">");
    reg.setMinimal(true);

    int pos = html.indexOf(selector);
    if (pos == -1)
        return;

    if ((pos = reg.indexIn(html, pos)) != -1)
    {
        pos += reg.matchedLength();
        html.insert(pos, verbosity);
    }
}

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
    create_policy_options(policies);
    add_policy_to_html_selection(policies, ret, "checkerUpload_policy");

    QString displays;
    create_displays_options(displays);
    add_display_to_html_selection(displays, ret, "checkerUpload_display_selector");

    QString verbosity;
    create_verbosity_options(verbosity);
    add_verbosity_to_html_selection(verbosity, ret, "checkerUpload_verbosity_selector");

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
    add_policy_to_html_selection(policies, ret, "checkerOnline_policy");

    QString displays;
    create_displays_options(displays);
    add_display_to_html_selection(displays, ret, "checkerOnline_display_selector");

    QString verbosity;
    create_verbosity_options(verbosity);
    add_verbosity_to_html_selection(verbosity, ret, "checkerOnline_verbosity_selector");

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
    add_policy_to_html_selection(policies, ret, "checkerRepository_policy");

    QString displays;
    create_displays_options(displays);
    add_display_to_html_selection(displays, ret, "checkerRepository_display_selector");

    QString verbosity;
    create_verbosity_options(verbosity);
    add_verbosity_to_html_selection(verbosity, ret, "checkerRepository_display_selector");

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
void CheckerWindow::create_html_checker(QString& checker)
{
    QFile template_html(":/checker.html");

    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray html = template_html.readAll();
    template_html.close();

    checker = QString(html);
    remove_template_tags(checker);
}

//---------------------------------------------------------------------------
void CheckerWindow::create_html_result(QString& result)
{
    QFile template_html(":/result.html");

    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray html = template_html.readAll();
    template_html.close();

    result = QString(html);
    remove_template_tags(result);
}

//---------------------------------------------------------------------------
void CheckerWindow::change_checker_in_template(const QString& checker, QString& html)
{
    QRegExp reg("\\{% block checker %\\}\\{% endblock %\\}");
    int pos = 0;

    reg.setMinimal(true);
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), checker);
}

//---------------------------------------------------------------------------
void CheckerWindow::change_result_in_template(const QString& result, QString& html)
{
    QRegExp reg("\\{% block result %\\}\\{% endblock %\\}");
    int pos = 0;

    reg.setMinimal(true);
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), result);
}

//---------------------------------------------------------------------------
void CheckerWindow::change_body_script_in_template(QString& html)
{
    QRegExp reg("\\{\\{ QT_SCRIPTS \\}\\}");
    QString script;
    int     pos = 0;

    reg.setMinimal(true);
#if defined(WEB_MACHINE_KIT)
    script = "        <script type=\"text/javascript\" src=\"qrc:/checker.js\"></script>\n";
#elif defined(WEB_MACHINE_ENGINE)
    script = "        <script type=\"text/javascript\" src=\"qrc:/qtwebchannel/qwebchannel.js\"></script>\n"
             "        <script type=\"text/javascript\" src=\"qrc:/webengine.js\"></script>\n"
             "        <script type=\"text/javascript\" src=\"qrc:/checker.js\"></script>\n";
#endif
    if ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), script);
}

//---------------------------------------------------------------------------
void CheckerWindow::set_webmachine_script_in_template(QString& html)
{
    QRegExp reg("\\{\\{[\\s]+webmachine[\\s]\\}\\}");
    QString machine;
    int     pos = 0;

    reg.setMinimal(true);
#if defined(WEB_MACHINE_KIT)
    machine = "WEB_MACHINE_KIT";
#elif defined(WEB_MACHINE_ENGINE)
    machine = "WEB_MACHINE_ENGINE";
#endif
    if ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), machine);
}

//---------------------------------------------------------------------------
void CheckerWindow::create_html_base(const QString& checker, const QString& result, QString& base)
{
    QFile template_html(":/base.html");
    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray html = template_html.readAll();
    template_html.close();

    base = QString(html);

    set_webmachine_script_in_template(base);
    change_body_script_in_template(base);
    change_checker_in_template(checker, base);
    change_result_in_template(result, base);
}

//---------------------------------------------------------------------------
QString CheckerWindow::create_html()
{
    QString checker;
    create_html_checker(checker);
    QString result;
    create_html_result(result);
    QString base;
    create_html_base(checker, result, base);
    return base;
}

//---------------------------------------------------------------------------
void CheckerWindow::add_file_to_result_table(const std::string& full_path)
{
    if (!result_table || !is_finished)
    {
        files.push_back(full_path);
        return;
    }

    result_table->add_file_to_result_table(full_path);
}

void CheckerWindow::page_start_waiting_loop()
{
    if (!main_view || !is_finished)
        return;

    WebPage* page = (WebPage*)main_view->page();
    if (!page)
        return;

    page->use_javascript("startWaitingLoop()");
}

}
