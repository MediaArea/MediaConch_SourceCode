/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef DATABASEWINDOW_H
#define DATABASEWINDOW_H

#include <QFileInfo>
#include <QString>
#include <QStringList>
#include "commonwebwindow.h"

namespace MediaConch {

class DatabaseWindow : public CommonWebWindow
{
public:
    explicit DatabaseWindow(MainWindow* m);
    ~DatabaseWindow();

    void display_database();
    void create_web_view_finished();

private:
    void create_html(QString& html);

    void create_html_database(QString& database);

    void create_html_base(QString& base, const QString& database);
    void set_webmachine_script_in_template(QString& base);
    void change_qt_scripts_in_template(QString& base);
    void change_checker_in_template(QString& base, const QString& database);
    void remove_result_in_template(QString& base);
};

}

#endif // DATABASEWINDOW_H
