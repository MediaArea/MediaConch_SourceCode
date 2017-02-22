/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "displaywindow.h"
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

DisplayWindow::DisplayWindow(MainWindow* m) : CommonWebWindow(m)
{
}

DisplayWindow::~DisplayWindow()
{
}

void DisplayWindow::display_display()
{
    display_html();
}

//---------------------------------------------------------------------------
void DisplayWindow::create_web_view_finished()
{
    fill_table();
}

void DisplayWindow::fill_table()
{
    std::vector<QString>& displays = main_window->get_displays();
    for (size_t i = 0; i < displays.size(); ++i)
    {
        QFileInfo file(displays[i]);

        QString script;
        QString fullPath = file.absoluteFilePath();
        if (fullPath.startsWith(":/displays"))
            script = "addSystemDisplay";
        else
            script = "addUserDisplay";

        script += QString("('%2');")
            .arg(file.completeBaseName());
        ((WebPage*)main_window->web_view->page())->use_javascript(script);
    }
}

int DisplayWindow::add_new_file(const QString& name, const QString& filename)
{
#if QT_VERSION >= 0x050400
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#elif QT_VERSION >= 0x050000
    QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
    QString path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif

    path += ("/displays");

    QDir dir(path);

    if (!dir.exists())
        if (!dir.mkpath(dir.absolutePath()))
            return -1;

    QString display_name = name;
    if (!display_name.length())
    {
        QFileInfo file_info(filename);
        display_name = file_info.completeBaseName();
    }

    std::vector<QString>& displays = main_window->get_displays();
    int pos = (int)displays.size();
    for (int j = 0; 1; ++j)
    {
        QString str;
        if (!j)
            str = QString("%1/%2.xsl").arg(dir.absolutePath()).arg(display_name);
        else
            str = QString("%1/%2_%3.xsl").arg(dir.absolutePath()).arg(display_name).arg(j);
        QFile info(str);
        if (info.exists())
            continue;

        QFile::copy(filename, str);
        displays.push_back(str);
        break;
    }

    return pos;
}

int DisplayWindow::add_new_files(const QStringList& files)
{
#if QT_VERSION >= 0x050400
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#elif QT_VERSION >= 0x050000
    QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
    QString path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif

    path += ("/displays");

    QDir dir(path);

    if (!dir.exists())
        if (!dir.mkpath(dir.absolutePath()))
            return -1;

    std::vector<QString>& displays = main_window->get_displays();
    int pos = (int)displays.size();
    for (int i = 0; i < files.size(); ++i)
    {
        QFileInfo file_info(files[i]);
        QString name = file_info.completeBaseName();
        for (int j = 0; 1; ++j)
        {
            QString str;
            if (!j)
                str = QString("%1/%2.xsl").arg(dir.absolutePath()).arg(name);
            else
                str = QString("%1/%2_%3.xsl").arg(dir.absolutePath()).arg(name).arg(j);
            QFile info(str);
            if (info.exists())
                continue;

            QFile::copy(files[i], str);
            displays.push_back(str);

            QString script = QString("addUserDisplay('%2');").arg(file_info.completeBaseName());
            ((WebPage*)main_window->web_view->page())->use_javascript(script);
            break;
        }

        if (i + 1 == files.size())
            main_window->set_last_load_display_path(file_info.absolutePath().toUtf8().data());
    }

    return pos;
}

void DisplayWindow::export_file(const QString& name)
{
    std::vector<QString>& displays = main_window->get_displays();

    size_t i = 0;
    for (; i < displays.size(); ++i)
    {
        QFileInfo f(displays[i]);
        if (f.completeBaseName() == name)
            break;
    }
    if (i == displays.size())
        return;

    QString suggested = QString().fromUtf8(main_window->select_correct_save_display_path().c_str());
    QString file = QFileDialog::getSaveFileName(main_window, tr("Save display file"), suggested, tr("Display file (*.xsl)"));
    if (!file.length())
        return;

    QFile f(displays[i]);
    if (QFile::exists(file))
        QFile::remove(file);
    f.copy(file);

    QDir info(QFileInfo(file).absoluteDir());
    main_window->set_last_save_display_path(info.absolutePath().toUtf8().data());
}

void DisplayWindow::delete_file(const QString& name)
{
    std::vector<QString>& displays = main_window->get_displays();

    size_t i = 0;
    for (; i < displays.size(); ++i)
    {
        if (displays[i].startsWith(":/displays"))
            continue;

        QFileInfo f(displays[i]);
        if (f.completeBaseName() == name)
            break;
    }
    if (i == displays.size())
        return;

    QFile file(displays[i]);
    file.remove();
    main_window->get_displays().erase(main_window->get_displays().begin() + i);
}

//---------------------------------------------------------------------------
void DisplayWindow::create_html(QString& html)
{
    QString display;
    create_html_display(display);
    create_html_base(html, display);
}

//---------------------------------------------------------------------------
void DisplayWindow::create_html_display(QString& display)
{
    QFile display_file(":/display.html");

    display_file.open(QIODevice::ReadOnly | QIODevice::Text);
    display = QString(display_file.readAll());
    display_file.close();
}

//---------------------------------------------------------------------------
void DisplayWindow::create_html_base(QString& base, const QString& display)
{
    set_webmachine_script_in_template(base);
    change_qt_scripts_in_template(base);
    change_checker_in_template(base, display);
    remove_result_in_template(base);
}

//***************************************************************************
// HELPER
//***************************************************************************

//---------------------------------------------------------------------------
void DisplayWindow::change_qt_scripts_in_template(QString& html)
{
    QRegExp reg("\\{\\{ QT_SCRIPTS \\}\\}");
    QString script;
    int     pos = 0;

    reg.setMinimal(true);
#if defined(WEB_MACHINE_KIT)
    script = "        <script type=\"text/javascript\" src=\"qrc:/display.js\"></script>\n";
#elif defined(WEB_MACHINE_ENGINE)
    script = "        <script type=\"text/javascript\" src=\"qrc:/qtwebchannel/qwebchannel.js\"></script>\n"
             "        <script type=\"text/javascript\" src=\"qrc:/webengine.js\"></script>\n"
             "        <script type=\"text/javascript\" src=\"qrc:/display.js\"></script>\n";
#endif
    script += "        <script type=\"text/javascript\" src=\"qrc:/utils/url.js\"></script>\n";
    script += "        <script type=\"text/javascript\" src=\"qrc:/menu.js\"></script>\n";
    if ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), script);
}

//---------------------------------------------------------------------------
void DisplayWindow::set_webmachine_script_in_template(QString& html)
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
void DisplayWindow::change_checker_in_template(QString& html, const QString& display)
{
    QRegExp reg("\\{% block checker %\\}\\{% endblock %\\}");
    int pos = 0;

    reg.setMinimal(true);
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), display);
}

//---------------------------------------------------------------------------
void DisplayWindow::remove_result_in_template(QString& html)
{
    QRegExp reg("\\{% block result %\\}\\{% endblock %\\}");
    int pos = 0;

    reg.setMinimal(true);
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), "");
}

}
