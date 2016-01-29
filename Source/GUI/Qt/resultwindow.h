/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef RESULTWINDOW_H
#define RESULTWINDOW_H

#include <QObject>

class QProgressBar;

namespace MediaConch {

class MainWindow;
class WebView;
class ProgressBar;

class ResultWindow : public QObject
{
    Q_OBJECT

public:
    explicit ResultWindow(MainWindow* m);
    ~ResultWindow();

    void display_results();

private:
    // Visual elements
    MainWindow   *mainwindow;
    WebView      *view;
    ProgressBar  *progressBar;

    void          clear_visual_elements();
    void          set_web_view_content(QString& html);
    void          update_html_with_results(QString& html);

private Q_SLOTS:
    void          create_web_view_finished(bool ok);
};

}

#endif // RESULTWINDOW_H
