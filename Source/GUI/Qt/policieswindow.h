/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef POLICIESWINDOW_H
#define POLICIESWINDOW_H

#include <QFileInfo>
#include <QString>

class QVBoxLayout;
class QLabel;
class QTreeWidgetItem;
class QFile;
class QStringList;
class QFrame;

namespace MediaConch {

class MainWindow;
class WebView;
class ProgressBar;

class PoliciesWindow : public QObject
{
    Q_OBJECT

public:
    explicit PoliciesWindow(MainWindow *parent = 0);
    ~PoliciesWindow();

//***************************************************************************
// HELPER
//***************************************************************************
    void                        display_policies();
    int                         add_new_policies(const QStringList& files);

private:
    MainWindow   *main_window;
    WebView      *web_view;
    ProgressBar  *progress_bar;

//***************************************************************************
// VISUAL
//***************************************************************************
    void clear_visual_elements();

//***************************************************************************
// WEB
//***************************************************************************
    void change_qt_scripts_in_template(QString& html);
    void set_webmachine_script_in_template(QString& html);
    void remove_result_in_template(QString& html);
    void change_checker_in_template(const QString& policy, QString& html);
    void create_html_base(const QString& policy, QString& base);
    void create_html_policy(QString& policy);
    void create_html();

//***************************************************************************
// SLOTS
//***************************************************************************
public Q_SLOTS:
    void use_javascript(const QString& script);

private Q_SLOTS:
    void create_web_view_finished(bool ok);
};

}

#endif // POLICIESWINDOW_H
