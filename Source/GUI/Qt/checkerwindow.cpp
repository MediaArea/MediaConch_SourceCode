/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
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

#if defined(WINDOWS)
    #include <windows.h>
#else
    #include <unistd.h>
#endif //!defined(WINDOWS)

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

CheckerWindow::CheckerWindow(MainWindow *parent) : CommonWebWindow(parent)
{
}

CheckerWindow::~CheckerWindow()
{
}

//***************************************************************************
// Visual elements
//***************************************************************************

//---------------------------------------------------------------------------
void CheckerWindow::create_web_view_finished()
{
}

//---------------------------------------------------------------------------
void CheckerWindow::create_web_view()
{
    display_html();
}

//---------------------------------------------------------------------------
void CheckerWindow::change_local_files(const QStringList& files)
{
    if (!main_window->web_view || !main_window->web_view->page())
        return;

    WebPage* p = (WebPage*)main_window->web_view->page();
    p->change_local_files(files);
}

//***************************************************************************
// HELPER
//***************************************************************************

//---------------------------------------------------------------------------
void CheckerWindow::create_policy_options(QString& policies)
{
    MediaConchLib::Get_Policies list;
    QString err;
    main_window->get_policies("JSON", list, err);

    QString system_policy;
    QString user_policy;
    int selected_policy = main_window->select_correct_policy();
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
        policies += QString("<optgroup class=\"userPolicy\" label=\"User policies\">%1</optgroup>").arg(user_policy);
    else
        policies += QString("<optgroup class=\"userPolicy hidden\" label=\"User policies\"></optgroup>");

    // Create default policy opt-group
    if (system_policy.length())
        policies += QString("<optgroup class=\"systemPolicy\" label=\"System policies\">%1</optgroup>").arg(system_policy);
    else
        policies += QString("<optgroup class=\"systemPolicy hidden\" label=\"System policies\"></optgroup>");
}

//---------------------------------------------------------------------------
void CheckerWindow::create_displays_options(QString& displays)
{
    QString system_display;
    QString user_display;
    int selected_display = main_window->select_correct_display();
    const std::vector<QString>& displays_list = main_window->get_displays();
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
    int selected_verbosity = main_window->select_correct_verbosity();
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

    bool has_libcurl = main_window->mil_has_curl_enabled();
    while ((pos = reg.indexIn(html, pos)) != -1)
    {
        QString value = reg.cap(1);
        if (value == "formUpload")
            html.replace(pos, reg.matchedLength(), create_form_upload());
        else if (value == "formOnline")
        {
            if (has_libcurl)
                html.replace(pos, reg.matchedLength(), create_form_online());
            else
            {
                remove_form_online(pos, html);
                remove_li_online(pos, html);
            }
        }
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
    QRegExp reg("<div role=\"tabpanel\" class=\"tab-pane panel col-md-12\" id=\"url\">");
    reg.setMinimal(true);
    start_div_pos = reg.lastIndexIn(html, start_div_pos);

    reg = QRegExp("</div>");
    reg.setMinimal(true);
    int end_div_pos = pos;
    if ((end_div_pos = reg.indexIn(html, end_div_pos)) != -1)
        end_div_pos += reg.matchedLength();

    if (end_div_pos != -1 && start_div_pos != -1)
        html.remove(start_div_pos, end_div_pos - start_div_pos);
}

//---------------------------------------------------------------------------
void CheckerWindow::remove_li_online(int& pos, QString& html)
{
    QRegExp reg("<li role=\"presentation\" class=\"\"><a href=\"#url\"");
    reg.setMinimal(true);
    int start = reg.lastIndexIn(html);
    if (start == -1)
        return;

    reg = QRegExp("</li>");
    reg.setMinimal(true);
    int end_pos = -1;
    if ((end_pos = reg.indexIn(html, start)) != -1)
        end_pos += reg.matchedLength();

    if (end_pos != -1 && start != -1)
    {
        int len = end_pos - start;
        html.remove(start, len);
        pos -= len;
    }
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
void CheckerWindow::change_checker_in_template(const QString& checker, QString& html)
{
    QRegExp reg("\\{% block checker %\\}\\{% endblock %\\}");
    int pos = 0;

    reg.setMinimal(true);
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), checker);
}

//---------------------------------------------------------------------------
void CheckerWindow::change_body_script_in_template(QString& html)
{
    QRegExp reg("\\{\\{ QT_SCRIPTS \\}\\}");
    QString script;
    int     pos = 0;

    reg.setMinimal(true);

#if defined(WEB_MACHINE_KIT)
    script += "        <script type=\"text/javascript\" src=\"qrc:/checker/webkit.js\"></script>\n";
#elif defined(WEB_MACHINE_ENGINE)
    script += "        <script type=\"text/javascript\" src=\"qrc:/qtwebchannel/qwebchannel.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/webengine.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/checker/webengine.js\"></script>\n";
#endif
    script += "        <script type=\"text/javascript\" src=\"qrc:/checker/base.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/checker/cellImplementation.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/checker/cellMediaInfo.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/checker/cellMediaTrace.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/checker/cellPolicy.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/checker/cellStatus.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/checker/table.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/utils/url.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/utils/size.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/utils/text.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/menu.js\"></script>\n";

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
void CheckerWindow::create_html_base(const QString& checker, QString& base)
{
    set_webmachine_script_in_template(base);
    change_body_script_in_template(base);
    change_checker_in_template(checker, base);
}

//---------------------------------------------------------------------------
void CheckerWindow::create_html(QString &html)
{
    QString checker;
    create_html_checker(checker);
    create_html_base(checker, html);
}

}
