/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include "WebView.h"

class QProgressBar;

namespace MediaConch
{

class MainWindow;
class WebPage;

class SettingsWindow : QObject
{
    Q_OBJECT

public:
    explicit SettingsWindow(MainWindow* parent);
    ~SettingsWindow();

    void display_settings();
    void clear_visual_elements();

private:
    MainWindow*   parent;
    WebView*      settings_view;
    QProgressBar* progress_bar;

    void fill_html(QString& html);
    void add_policy_to_html_selection(QString& policies, QString& html, const QString& selector);
    void create_policy_options(QString& policies);
    void add_display_to_html_selection(QString& displays, QString& html, const QString& selector);
    void create_displays_options(QString& displays);
    void add_verbosity_to_html_selection(QString& verbosity, QString& html, const QString& selector);
    void create_verbosity_options(QString& verbosity);
    void add_save_report_path_to_js_input(WebPage *page);
    void add_load_files_path_to_js_input(WebPage *page);
    void add_save_policy_path_to_js_input(WebPage *page);
    void add_load_policy_path_to_js_input(WebPage *page);
    void add_save_display_path_to_js_input(WebPage *page);
    void add_load_display_path_to_js_input(WebPage *page);

private Q_SLOTS:
    void create_settings_finished(bool ok);
};

}

#endif // SETTINGSWINDOW_H
