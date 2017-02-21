/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef PUBLICPOLICIESWINDOW_H
#define PUBLICPOLICIESWINDOW_H

#include <QFileInfo>
#include <QString>
#include "WebChoose.h"

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

class PublicPoliciesWindow : public QObject
{
    Q_OBJECT

public:
    explicit PublicPoliciesWindow(MainWindow *parent = 0);
    virtual ~PublicPoliciesWindow();

//***************************************************************************
// HELPER
//***************************************************************************
    void                        display_public_policies();

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
private Q_SLOTS:
    void create_web_view_finished(bool ok);
};

}

#endif // PUBLICPOLICIESWINDOW_H
