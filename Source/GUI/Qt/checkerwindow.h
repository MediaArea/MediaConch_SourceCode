/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef CHECKERWINDOW_H
#define CHECKERWINDOW_H

#include "mainwindow.h"

#include <QFileInfo>
#include <QString>
#include "WebChoose.h"

class QFrame;
class QFile;
class QWebView;

namespace MediaConch {

class MainWindow;
class WebView;
class ProgressBar;
class ResultTable;

class CheckerWindow : public QObject
{
    Q_OBJECT

public:
    explicit CheckerWindow(MainWindow *parent = 0);
    ~CheckerWindow();

    // Functions
    void                        create_web_view();
    void                        set_display_xslt(const QString& d) { display_xslt = d; }
    void                        reset_display_xslt() { display_xslt.clear(); }
    void                        change_local_files(const QStringList& files);
    void                        hide();
    void                        add_file_to_result_table(const std::string& full_path);
    void                        page_start_waiting_loop();

private:
    // Visual elements
    MainWindow*                 main_window;
    WebView*                    web_view;
    ProgressBar*                progress_bar;
    QString                     display_xslt;
    unsigned int                result_index;
    ResultTable                *result_table;
    bool                        is_finished;
    std::vector<std::string>    files;

    void                        clear_visual_elements();
    void                        set_web_view_content(QString& html);

//***************************************************************************
// HELPER
//***************************************************************************

    void add_policy_to_html_selection(QString& policies, QString& html, const QString& selector);
    void create_policy_options(QString& policies);
    void add_display_to_html_selection(QString& displays, QString& html, const QString& selector);
    void create_displays_options(QString& displays);
    void add_verbosity_to_html_selection(QString& verbosity, QString& html, const QString& selector);
    void create_verbosity_options(QString& verbosity);
    void remove_template_tags(QString& data);
    void load_include_in_template(QString& html);
    void remove_element_in_template(QString& html);
    void load_form_in_template(QString& html);
    QString create_html();
    void create_html_base(const QString& checker, const QString& result, QString& base);
    void create_html_checker(QString&);
    void create_html_result(QString&);
    QString create_form_upload();
    QString create_form_online();
    QString create_form_repository();
    void remove_form_online(int pos, QString& html);
    void remove_li_online(int& pos, QString& html);
    void change_collapse_form(QString& html);
    void change_checker_in_template(const QString& checker, QString& html);
    void change_result_in_template(const QString& result, QString& html);
    void change_body_script_in_template(QString& html);
    void set_webmachine_script_in_template(QString& html);

private Q_SLOTS:
    void create_web_view_finished(bool ok);
};

}

#endif // MAINWINDOW_H
