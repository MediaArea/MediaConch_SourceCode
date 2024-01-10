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
#include <QFileDialog>
#include <QUrl>
#include <QRegularExpression>
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
void CheckerWindow::create_parsespeed_options(QString& parsespeed)
{
    std::string selected_parsespeed = main_window->select_correct_parsespeed();

    parsespeed +=  QString("<option ");
    if ("0" == selected_parsespeed)
        parsespeed += QString("selected=\"selected\" ");
    parsespeed += QString("value=\"0\">Very quick (0)</option>");

    parsespeed +=  QString("<option ");
    if ("0.3" == selected_parsespeed)
        parsespeed += QString("selected=\"selected\" ");
    parsespeed += QString("value=\"0.3\">Quick (0.3)</option>");

    parsespeed +=  QString("<option ");
    if ("0.5" == selected_parsespeed)
        parsespeed += QString("selected=\"selected\" ");
    parsespeed += QString("value=\"0.5\">Default (0.5)</option>");

    parsespeed +=  QString("<option ");
    if ("0.7" == selected_parsespeed)
        parsespeed += QString("selected=\"selected\" ");
    parsespeed += QString("value=\"0.7\">Advanced (0.7)</option>");

    parsespeed +=  QString("<option ");
    if ("1" == selected_parsespeed)
        parsespeed += QString("selected=\"selected\" ");
    parsespeed += QString("value=\"1\">Full (1)</option>");
}

//---------------------------------------------------------------------------
void CheckerWindow::add_policy_to_html_selection(QString& policies, QString& html, const QString& selector)
{
    QRegularExpression reg("class=\"policyList form-control\">", QRegularExpression::InvertedGreedinessOption);
    int pos = html.indexOf(selector);

    if (pos == -1)
        return;

    QRegularExpressionMatch match = reg.match(html, pos);
    if ((pos = match.capturedStart()) != -1)
    {
        pos += match.capturedLength();
        html.insert(pos, policies);
    }
}

//---------------------------------------------------------------------------
void CheckerWindow::add_display_to_html_selection(QString& displays, QString& html, const QString& selector)
{
    QRegularExpression reg("class=\"displayList form-control\">", QRegularExpression::InvertedGreedinessOption);

    int pos = html.indexOf(selector);
    if (pos == -1)
        return;

    QRegularExpressionMatch match = reg.match(html, pos);
    if ((pos = match.capturedStart()) != -1)
    {
        pos += match.capturedLength();
        html.insert(pos, displays);
    }
}

//---------------------------------------------------------------------------
void CheckerWindow::add_verbosity_to_html_selection(QString& verbosity, QString& html, const QString& selector)
{
    QRegularExpression reg("class=\"verbosityList form-control\">", QRegularExpression::InvertedGreedinessOption);

    int pos = html.indexOf(selector);
    if (pos == -1)
        return;

    QRegularExpressionMatch match = reg.match(html, pos);
    if ((pos = match.capturedStart()) != -1)
    {
        pos += match.capturedLength();
        html.insert(pos, verbosity);
    }
}

//---------------------------------------------------------------------------
void CheckerWindow::add_parsespeed_to_html_selection(QString& parsespeed, QString& html, const QString& selector)
{
    QRegularExpression reg("class=\"parsespeedList form-control\">", QRegularExpression::InvertedGreedinessOption);

    int pos = html.indexOf(selector);
    if (pos == -1)
        return;

    QRegularExpressionMatch match = reg.match(html, pos);
    if ((pos = match.capturedStart()) != -1)
    {
        pos += match.capturedLength();
        html.insert(pos, parsespeed);
    }
}

//---------------------------------------------------------------------------
void CheckerWindow::load_include_in_template(QString& html)
{
    QRegularExpression reg("\\{\\{[\\s]+include\\('AppBundle:(\\w+):(\\w+).html.twig'(,[\\s]*\\{ '\\w+':[\\s]*\\w+[\\s]*\\})?\\)[\\s]\\}\\}");
    QRegularExpressionMatch match;
    int pos = 0;

    while ((pos = (match = reg.match(html, pos)).capturedStart()) != -1)
    {
        QString app = match.captured(1);
        QString module = match.captured(2);
        if (app == "Default" && module == "quotaExceeded")
        {
            html.replace(match.capturedStart(), match.capturedLength(), "");
            continue;
        }
        html.replace(match.capturedStart(), match.capturedLength(), "");
        pos = 0;
    }
}

//---------------------------------------------------------------------------
void CheckerWindow::remove_element_in_template(QString& html)
{
    QRegularExpression reg("\\{% (.*) %\\}", QRegularExpression::InvertedGreedinessOption);
    html.replace(reg, "");
}

//---------------------------------------------------------------------------
void CheckerWindow::change_collapse_form(QString& html)
{
    QRegularExpression reg("class=\"panel-collapse collapse in\"");
    html.replace(reg, "class=\"panel-collapse collapse\"");
}

//---------------------------------------------------------------------------
void CheckerWindow::load_form_in_template(QString& html)
{
    QRegularExpression reg("\\{\\{[\\s]+form\\((\\w+)\\)[\\s]\\}\\}");
    QRegularExpressionMatch match;

    bool has_libcurl = main_window->mil_has_curl_enabled();

    int pos = 0;
    while ((pos = (match = reg.match(html)).capturedStart()) != -1)
    {
        QString value = match.captured(1);
        if (value == "formUpload")
        {
            html.replace(match.capturedStart(), match.capturedLength(), create_form_upload());
        }
        else if (value == "formOnline")
        {
            if (has_libcurl)
                html.replace(match.capturedStart(), match.capturedLength(), create_form_online());
            else
            {
                remove_form_online(pos, html);
                remove_li_online(html);
            }
        }
        else if (value == "formRepository")
            html.replace(match.capturedStart(), match.capturedLength(), create_form_repository());
        else
            html.replace(match.capturedStart(), match.capturedLength(), "");
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

    QString parsespeed;
    create_parsespeed_options(parsespeed);
    add_parsespeed_to_html_selection(parsespeed, ret, "checkerUpload_parsespeed_selector");

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

    QString parsespeed;
    create_parsespeed_options(parsespeed);
    add_parsespeed_to_html_selection(parsespeed, ret, "checkerOnline_parsespeed_selector");

    return ret;
}

//---------------------------------------------------------------------------
void CheckerWindow::remove_form_online(int pos, QString& html)
{
    int start_div_pos = pos;
    start_div_pos = html.lastIndexOf("<div role=\"tabpanel\" class=\"tab-pane panel col-md-12\" id=\"url\">", start_div_pos);

    int end_div_pos = html.indexOf("</div>", start_div_pos);
    if (end_div_pos != -1)
        end_div_pos += 6;

    if (end_div_pos != -1 && start_div_pos != -1)
        html.remove(start_div_pos, end_div_pos - start_div_pos);
}

//---------------------------------------------------------------------------
void CheckerWindow::remove_li_online(QString& html)
{
    int start = html.lastIndexOf("<li role=\"presentation\" class=\"\"><a href=\"#url\"");
    if (start == -1)
        return;

    int end_pos = html.indexOf("</li>", start);
    if (end_pos != -1)
        end_pos += 5;

    if (end_pos != -1 && start != -1)
    {
        int len = end_pos - start;
        html.remove(start, len);
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

    QString parsespeed;
    create_parsespeed_options(parsespeed);
    add_parsespeed_to_html_selection(parsespeed, ret, "checkerRepository_parsespeed_selector");

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
    QRegularExpression reg("\\{% block checker %\\}\\{% endblock %\\}", QRegularExpression::InvertedGreedinessOption);
    int pos = 0;

    QRegularExpressionMatch match;
    while ((match = reg.match(html, pos)).hasMatch())
    {
        pos = match.capturedStart();
        html.replace(pos, match.capturedLength(), checker);
    }
}

//---------------------------------------------------------------------------
void CheckerWindow::change_body_script_in_template(QString& html)
{
    QRegularExpression reg("\\{\\{ QT_SCRIPTS \\}\\}", QRegularExpression::InvertedGreedinessOption);
    QString script;
    int     pos = 0;

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

    QRegularExpressionMatch match = reg.match(html, pos);
    if ((pos = match.capturedStart()) != -1)
        html.replace(pos, match.capturedLength(), script);
}

//---------------------------------------------------------------------------
void CheckerWindow::set_webmachine_script_in_template(QString& html)
{
    QRegularExpression reg("\\{\\{[\\s]+webmachine[\\s]\\}\\}", QRegularExpression::InvertedGreedinessOption);
    QString machine;
    int     pos = 0;

#if defined(WEB_MACHINE_KIT)
    machine = "WEB_MACHINE_KIT";
#elif defined(WEB_MACHINE_ENGINE)
    machine = "WEB_MACHINE_ENGINE";
#endif
    QRegularExpressionMatch match = reg.match(html, pos);
    if ((pos = match.capturedStart()) != -1)
        html.replace(pos, match.capturedLength(), machine);
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
