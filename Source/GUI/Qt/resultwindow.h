/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef RESULTWINDOW_H
#define RESULTWINDOW_H

#include <QObject>
#include "mainwindow.h"

class QProgressBar;
class QTimer;

namespace MediaConch {

class WebView;
class ProgressBar;

class ResultWindow : public QObject
{
    Q_OBJECT

public:
    explicit ResultWindow(MainWindow* m);
    ~ResultWindow();

    void display_results();

    void set_default_update_timer(int time) { default_update_timer = time; }

private:
    // Visual elements
    MainWindow               *mainwindow;
    WebView                  *view;
    ProgressBar              *progressBar;
    unsigned int              result_index;
    std::vector<std::string> to_update_files;
    QTimer                   *update_timer;
    int                       default_update_timer;

    void          clear_visual_elements();
    void          set_web_view_content(QString& html);
    void          add_displays_file_detail_start(QString& html);
    void          update_html_with_results(QString& html);
    void          create_html_base(QString& html);
    void          add_displays_file_detail_element(MainWindow::FileRegistered* file, QString& html);
    void          add_displays_file_detail_modal(MainWindow::FileRegistered* file, QString& html);
    void          add_displays_file_detail_table_end(QString& html);
    void          add_displays_file_detail_end(QString& html);
    void          change_body_in_template(QString& body, QString& html);
    void          select_the_correct_value(const QString& value, const QString& selector, QString& html);

    void          change_html_file_detail(MainWindow::FileRegistered* file, QString& html);

private Q_SLOTS:
    void          create_web_view_finished(bool ok);
    void          start_timer_update();
};

}

#endif // RESULTWINDOW_H
