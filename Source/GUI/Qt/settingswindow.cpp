/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "settingswindow.h"
#include "mainwindow.h"
#include "WebPage.h"
#include "progressbar.h"
#if defined(WEB_MACHINE_ENGINE)
#include <QWebEnginePage>
#include <QWebChannel>
#endif
#if defined(WEB_MACHINE_KIT)
#include <QWebFrame>
#endif
#include <QFile>
#include <QProgressBar>

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

SettingsWindow::SettingsWindow(MainWindow *p) : CommonWebWindow(p)
{
}

SettingsWindow::~SettingsWindow()
{
    clear_visual_elements();
}

//***************************************************************************
// Visual elements
//***************************************************************************

//---------------------------------------------------------------------------
void SettingsWindow::create_web_view_finished(bool ok)
{
    if (!web_view || !ok)
        return;

    add_save_report_path_to_js_input((WebPage *)web_view->page());
    add_load_files_path_to_js_input((WebPage *)web_view->page());
    add_save_policy_path_to_js_input((WebPage *)web_view->page());
    add_load_policy_path_to_js_input((WebPage *)web_view->page());
    add_save_display_path_to_js_input((WebPage *)web_view->page());
    add_load_display_path_to_js_input((WebPage *)web_view->page());

    if (progress_bar)
    {
        main_window->remove_widget_from_layout(progress_bar);
        delete progress_bar;
        progress_bar = NULL;
    }

    web_view->show();
    main_window->set_widget_to_layout(web_view);
}

//---------------------------------------------------------------------------
void SettingsWindow::display_settings()
{
    clear_visual_elements();

    progress_bar = new ProgressBar(main_window);
    main_window->set_widget_to_layout(progress_bar);
    progress_bar->get_progress_bar()->setValue(0);
    progress_bar->show();

    QString html;
    create_html(html);

    QUrl url = QUrl("qrc:/html");
    if (!url.isValid())
        return;

    web_view = new WebView(main_window);
    web_view->hide();

    WebPage* page = new WebPage(main_window, web_view);
    web_view->setPage(page);

    QObject::connect(web_view, SIGNAL(loadProgress(int)), progress_bar->get_progress_bar(), SLOT(setValue(int)));
    QObject::connect(web_view, SIGNAL(loadFinished(bool)), this, SLOT(on_loadFinished(bool)));

#if defined(WEB_MACHINE_ENGINE)
    QWebChannel *channel = new QWebChannel(page);
    page->setWebChannel(channel);
    channel->registerObject("webpage", page);
    web_view->setHtml(html.toUtf8(), url);
#endif
#if defined(WEB_MACHINE_KIT)
    web_view->setContent(html.toUtf8(), "text/html", url);
#endif
}


//---------------------------------------------------------------------------
void SettingsWindow::create_html(QString& html)
{
    QString settings;
    create_html_settings(settings);

    QString base;
    create_html_base(base, settings);

    html = QString(base);
}

//---------------------------------------------------------------------------
void SettingsWindow::create_html_settings(QString& settings)
{
    QFile settings_file(":/settings.html");

    settings_file.open(QIODevice::ReadOnly | QIODevice::Text);
    settings = QString(settings_file.readAll());
    settings_file.close();

    fill_settings_html(settings);
}

//---------------------------------------------------------------------------
void SettingsWindow::create_html_base(QString& base, const QString& settings)
{
    QFile template_html(":/base.html");
    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    base = QString(template_html.readAll());
    template_html.close();

    set_webmachine_script_in_template(base);
    change_qt_scripts_in_template(base);
    change_checker_in_template(base, settings);
    remove_result_in_template(base);
}

//---------------------------------------------------------------------------
void SettingsWindow::clear_visual_elements()
{
    if (web_view)
    {
        main_window->remove_widget_from_layout(web_view);
#if defined(WEB_MACHINE_ENGINE)
        WebPage* page = (WebPage*)web_view->page();
        QWebChannel *channel = page ? page->webChannel() : NULL;
        if (channel)
            channel->deregisterObject(page);
#endif
        delete web_view;
        web_view = NULL;
    }

    if (progress_bar)
    {
        main_window->remove_widget_from_layout(progress_bar);
        delete progress_bar;
        progress_bar = NULL;
    }
}

//***************************************************************************
// HELPER
//***************************************************************************

//---------------------------------------------------------------------------
void SettingsWindow::fill_settings_html(QString& html)
{
    QString policies;
    create_policy_options(policies);
    add_policy_to_html_selection(policies, html, "settings_policy");

    QString displays;
    create_displays_options(displays);
    add_display_to_html_selection(displays, html, "settings_display_selector");

    QString verbosity;
    create_verbosity_options(verbosity);
    add_verbosity_to_html_selection(verbosity, html, "settings_verbosity_selector");
}

//---------------------------------------------------------------------------
void SettingsWindow::create_policy_options(QString& policies)
{
    MediaConchLib::Get_Policies p;
    QString err;

    main_window->get_policies("JSON", p, err);

    QString system_policy;
    QString user_policy;

    std::string policy_str = main_window->get_settings().get_default_policy();
    int selected_policy;
    if (policy_str == "last")
        selected_policy = -2;
    else
        selected_policy = QString().fromUtf8(policy_str.c_str(), policy_str.length()).toInt();

    for (size_t i = 0; p.policies && i < p.policies->size(); ++i)
    {
        if (!p.policies->at(i))
            continue;

        if (p.policies->at(i)->is_system)
        {
            system_policy += QString("<option ");
            if ((int)p.policies->at(i)->id == selected_policy)
                system_policy += QString("selected=\"selected\" ");
            system_policy += QString("value=\"%1\">%2</option>")
                .arg(p.policies->at(i)->id).arg(QString().fromUtf8(p.policies->at(i)->name.c_str(), p.policies->at(i)->name.length()));
        }
        else
        {
            user_policy += QString("<option ");
            if ((int)p.policies->at(i)->id == selected_policy)
                user_policy += QString("selected=\"selected\" ");
            user_policy += QString("value=\"%1\">%2</option>")
                .arg(p.policies->at(i)->id).arg(QString().fromUtf8(p.policies->at(i)->name.c_str(), p.policies->at(i)->name.length()));
        }
    }

    if (selected_policy == -2)
        policies += QString("<option selected=\"selected\" value=\"-2\">Last policy used</option>");
    else
        policies += QString("<option value=\"-2\">Last policy used</option>");

    if (selected_policy == -1)
        policies += QString("<option selected=\"selected\" value=\"-1\">No policy</option>");
    else
        policies += QString("<option value=\"-1\">No policy</option>");

    // Create default policy opt-group
    if (user_policy.length())
        policies += QString("<optgroup label=\"User policies\">%1</optgroup>").arg(user_policy);

    // Create default policy opt-group
    if (system_policy.length())
        policies += QString("<optgroup label=\"System policies\">%1</optgroup>").arg(system_policy);
}

//---------------------------------------------------------------------------
void SettingsWindow::create_displays_options(QString& displays)
{
    QString system_display;
    QString user_display;

    std::string display_str = main_window->get_settings().get_default_display();
    int selected_display;
    if (display_str == "last")
        selected_display = -1;
    else
        selected_display = main_window->get_display_index_by_filename(display_str);

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

    if (selected_display == -1)
        displays += QString("<option selected=\"selected\" value=\"-1\">Last display used</option>");
    else
        displays += QString("<option value=\"-1\">Last display used</option>");

    // Create user display opt-group
    if (user_display.length())
        displays += QString("<optgroup label=\"User displays\">%1</optgroup>").arg(user_display);

    // Create default display opt-group
    if (system_display.length())
        displays += QString("<optgroup label=\"System displays\">%1</optgroup>").arg(system_display);
}

//---------------------------------------------------------------------------
void SettingsWindow::create_verbosity_options(QString& verbosity)
{
    int selected_verbosity = main_window->get_settings().get_default_verbosity();

    if (selected_verbosity == -1)
        verbosity += QString("<option selected=\"selected\" value=\"-1\">Last verbosity used</option>");
    else
        verbosity += QString("<option value=\"-1\">Last verbosity used</option>");

    for (int i = 0; i < 6; ++i)
    {
        verbosity += QString("<option ");
        if ((int)i == selected_verbosity)
            verbosity += QString("selected=\"selected\" ");
        verbosity += QString("value=\"%1\">%1</option>").arg(i);
    }
}

//---------------------------------------------------------------------------
void SettingsWindow::add_policy_to_html_selection(QString& policies, QString& html, const QString& selector)
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
void SettingsWindow::add_display_to_html_selection(QString& displays, QString& html, const QString& selector)
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
void SettingsWindow::add_verbosity_to_html_selection(QString& verbosity, QString& html, const QString& selector)
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
void SettingsWindow::change_checker_in_template(QString& html, const QString& settings)
{
    QRegExp reg("\\{% block checker %\\}\\{% endblock %\\}");
    int pos = 0;

    reg.setMinimal(true);
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), settings);
}

//---------------------------------------------------------------------------
void SettingsWindow::remove_result_in_template(QString& html)
{
    QRegExp reg("\\{% block result %\\}\\{% endblock %\\}");
    int pos = 0;

    reg.setMinimal(true);
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), "");
}

//---------------------------------------------------------------------------
void SettingsWindow::change_qt_scripts_in_template(QString& html)
{
    QRegExp reg("\\{\\{ QT_SCRIPTS \\}\\}");
    QString script;
    int     pos = 0;

    reg.setMinimal(true);
#if defined(WEB_MACHINE_KIT)
    script = "        <script type=\"text/javascript\" src=\"qrc:/settings.js\"></script>\n";
#elif defined(WEB_MACHINE_ENGINE)
    script = "        <script type=\"text/javascript\" src=\"qrc:/qtwebchannel/qwebchannel.js\"></script>\n"
             "        <script type=\"text/javascript\" src=\"qrc:/webengine.js\"></script>\n"
             "        <script type=\"text/javascript\" src=\"qrc:/settings.js\"></script>\n";
#endif
    script += "        <script type=\"text/javascript\" src=\"qrc:/utils/url.js\"></script>\n";
    script += "        <script type=\"text/javascript\" src=\"qrc:/menu.js\"></script>\n";

    if ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), script);
}

//---------------------------------------------------------------------------
void SettingsWindow::set_webmachine_script_in_template(QString& html)
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
void SettingsWindow::add_save_report_path_to_js_input(WebPage *page)
{
    std::string selected_save_report_str = main_window->get_settings().get_default_save_report_path();
    QString selected_save_report = QString().fromUtf8(selected_save_report_str.c_str(), selected_save_report_str.length());
    page->use_javascript(QString("set_save_report_selected('%1');").arg(selected_save_report));
}

//---------------------------------------------------------------------------
void SettingsWindow::add_load_files_path_to_js_input(WebPage *page)
{
    std::string selected_load_files_str = main_window->get_settings().get_default_load_files_path();
    QString selected_load_files = QString().fromUtf8(selected_load_files_str.c_str(), selected_load_files_str.length());
    page->use_javascript(QString("set_load_files_selected('%1');").arg(selected_load_files));
}

//---------------------------------------------------------------------------
void SettingsWindow::add_save_policy_path_to_js_input(WebPage *page)
{
    std::string selected_save_policy_str = main_window->get_settings().get_default_save_policy_path();
    QString selected_save_policy = QString().fromUtf8(selected_save_policy_str.c_str(), selected_save_policy_str.length());
    page->use_javascript(QString("set_save_policy_selected('%1');").arg(selected_save_policy));
}

//---------------------------------------------------------------------------
void SettingsWindow::add_load_policy_path_to_js_input(WebPage *page)
{
    std::string selected_load_policy_str = main_window->get_settings().get_default_load_policy_path();
    QString selected_load_policy = QString().fromUtf8(selected_load_policy_str.c_str(), selected_load_policy_str.length());
    page->use_javascript(QString("set_load_policy_selected('%1');").arg(selected_load_policy));
}

//---------------------------------------------------------------------------
void SettingsWindow::add_save_display_path_to_js_input(WebPage *page)
{
    std::string selected_save_display_str = main_window->get_settings().get_default_save_display_path();
    QString selected_save_display = QString().fromUtf8(selected_save_display_str.c_str(), selected_save_display_str.length());
    page->use_javascript(QString("set_save_display_selected('%1');").arg(selected_save_display));
}

//---------------------------------------------------------------------------
void SettingsWindow::add_load_display_path_to_js_input(WebPage *page)
{
    std::string selected_load_display_str = main_window->get_settings().get_default_load_display_path();
    QString selected_load_display = QString().fromUtf8(selected_load_display_str.c_str(), selected_load_display_str.length());
    page->use_javascript(QString("set_load_display_selected('%1');").arg(selected_load_display));
}

}
