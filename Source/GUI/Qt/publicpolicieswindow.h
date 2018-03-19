/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef PUBLICPOLICIESWINDOW_H
#define PUBLICPOLICIESWINDOW_H

#include <QFileInfo>
#include <QString>
#include "WebChoose.h"
#include "commonwebwindow.h"

class QVBoxLayout;
class QLabel;
class QTreeWidgetItem;
class QFile;
class QStringList;
class QFrame;

namespace MediaConch {

class PublicPoliciesWindow : public CommonWebWindow
{
public:
    explicit PublicPoliciesWindow(MainWindow *parent = 0);
    virtual ~PublicPoliciesWindow();

//***************************************************************************
// HELPER
//***************************************************************************
    void                        display_public_policies();

//***************************************************************************
// WEB
//***************************************************************************
    void change_qt_scripts_in_template(QString& html);
    void set_webmachine_script_in_template(QString& html);
    void remove_result_in_template(QString& html);
    void change_checker_in_template(const QString& policy, QString& html);
    void create_html_base(const QString& policy, QString& base);
    void create_html_policy(QString& policy);
    void create_html(QString &html);
};

}

#endif // PUBLICPOLICIESWINDOW_H
