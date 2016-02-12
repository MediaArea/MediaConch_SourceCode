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
    void                        change_local_files(QStringList& files);
    void                        hide();

private:
    // Visual elements
    MainWindow*                 mainwindow;
    WebView*                    MainView;
    ProgressBar*                progressBar;
    QString                     display_xslt;
    unsigned int                result_index;

    void                        clearVisualElements();
    void                        set_web_view_content(QString& html);

//***************************************************************************
// HELPER
//***************************************************************************

    void remove_template_tags(QString& data);
    void load_include_in_template(QString& html);
    void remove_element_in_template(QString& html);
    void load_form_in_template(QString& html);
    QString create_html();
    QString create_html_base(QString& body);
    QString create_html_body();
    QString create_form_upload();
    QString create_form_online();
    QString create_form_repository();
    void add_script_js_tree(std::string& file);
    void remove_form_online(int pos, QString& html);
    void change_collapse_form(QString& html);
    void change_body_in_template(QString& body, QString& html);
#if defined(WEB_MACHINE_ENGINE)
    void add_file_detail_to_html(std::string& file, int policy);
#endif
#if defined(WEB_MACHINE_KIT)
    void add_file_detail_to_html(std::string& file, int policy, QString& html);
#endif

private Q_SLOTS:
    void createWebViewFinished(bool ok);
};

}

#endif // MAINWINDOW_H
