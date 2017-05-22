/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "databasewindow.h"
#include "mainwindow.h"
#include "progressbar.h"
#include "WebView.h"
#include "WebPage.h"
#include <QDir>
#include <QFileDialog>
#include <QProgressBar>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif
#if defined(WEB_MACHINE_ENGINE)
#include <QWebChannel>
#endif

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

DatabaseWindow::DatabaseWindow(MainWindow* m) : CommonWebWindow(m)
{
}

DatabaseWindow::~DatabaseWindow()
{
}

void DatabaseWindow::display_database()
{
    display_html();
}

//---------------------------------------------------------------------------
void DatabaseWindow::create_web_view_finished()
{
}

//---------------------------------------------------------------------------
void DatabaseWindow::create_html(QString& html)
{
    QString database;
    create_html_database(database);
    create_html_base(html, database);
}

//---------------------------------------------------------------------------
void DatabaseWindow::create_html_database(QString& database)
{
    QFile database_file(":/database.html");

    database_file.open(QIODevice::ReadOnly | QIODevice::Text);
    database = QString(database_file.readAll());
    database_file.close();
}

//---------------------------------------------------------------------------
void DatabaseWindow::create_html_base(QString& base, const QString& database)
{
    set_webmachine_script_in_template(base);
    change_qt_scripts_in_template(base);
    change_checker_in_template(base, database);
    remove_result_in_template(base);
}

//***************************************************************************
// HELPER
//***************************************************************************

//---------------------------------------------------------------------------
void DatabaseWindow::change_qt_scripts_in_template(QString& html)
{
    QRegExp reg("\\{\\{ QT_SCRIPTS \\}\\}");
    QString script;
    int     pos = 0;

    reg.setMinimal(true);
#if defined(WEB_MACHINE_KIT)
    script += "        <script type=\"text/javascript\" src=\"qrc:/database/database.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/database/databaseWebKit.js\"></script>\n";
#elif defined(WEB_MACHINE_ENGINE)
    script += "        <script type=\"text/javascript\" src=\"qrc:/qtwebchannel/qwebchannel.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/webengine.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/database/database.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/database/databaseWebEngine.js\"></script>\n";
#endif
    script += "        <script type=\"text/javascript\" src=\"qrc:/utils/url.js\"></script>\n";
    script += "        <script type=\"text/javascript\" src=\"qrc:/menu.js\"></script>\n";
    if ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), script);
}

//---------------------------------------------------------------------------
void DatabaseWindow::set_webmachine_script_in_template(QString& html)
{
    QRegExp reg("\\{\\{[\\s]+webmachine[\\s]\\}\\}");
    QString machine;
    int     pos = 0;

    reg.setMinimal(true);
#if defined(WEB_MACHINE_KIT)
    machine = "WEB_MACHINE_KIT";
#elif defined(WEB_MACHINE_ENGINE)
    machine = "WEB_MACHINE_ENGINE";
#endif
    if ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), machine);
}

//---------------------------------------------------------------------------
void DatabaseWindow::change_checker_in_template(QString& html, const QString& database)
{
    QRegExp reg("\\{% block checker %\\}\\{% endblock %\\}");
    int pos = 0;

    reg.setMinimal(true);
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), database);
}

//---------------------------------------------------------------------------
void DatabaseWindow::remove_result_in_template(QString& html)
{
    QRegExp reg("\\{% block result %\\}\\{% endblock %\\}");
    int pos = 0;

    reg.setMinimal(true);
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), "");
}

}
