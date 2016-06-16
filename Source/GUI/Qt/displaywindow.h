/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef DISPLAYWINDOW_H
#define DISPLAYWINDOW_H

#include <QFileInfo>
#include <QString>

namespace MediaConch {

class MainWindow;
class WebView;
class ProgressBar;

class DisplayWindow : public QObject
{
    Q_OBJECT

public:
    explicit DisplayWindow(MainWindow* m);
    ~DisplayWindow();

    void display_display();

    int  add_new_file(const QString& name, const QString& filename);
    void export_file(const QString& name);
    void delete_file(const QString& name);

private:
    // Visual elements
    MainWindow   *mainwindow;
    WebView      *web_view;
    ProgressBar  *progress_bar;
    bool          is_finished;

    void clear_display();
    void create_html();
    void create_html_display(QString& html);
    void change_body_script_in_template(QString& html);
    void set_webmachine_script_in_template(QString& html);
    void fill_table();

private Q_SLOTS:
    void create_web_view_finished(bool ok);
};

}

#endif // DISPLAYWINDOW_H
