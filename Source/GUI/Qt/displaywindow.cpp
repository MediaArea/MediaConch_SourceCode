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

DisplayWindow::DisplayWindow(MainWindow* m) : mainwindow(m), web_view(NULL), progress_bar(NULL), is_finished(false)
{
}

DisplayWindow::~DisplayWindow()
{
    clear_display();
}

void DisplayWindow::display_display()
{
    clear_display();
    create_html();
}

void DisplayWindow::clear_display()
{
    if (progress_bar)
    {
        mainwindow->remove_widget_from_layout(progress_bar);
        delete progress_bar;
        progress_bar = NULL;
    }

    if (web_view)
    {
        mainwindow->remove_widget_from_layout(web_view);
#if defined(WEB_MACHINE_ENGINE)
        WebPage* page = (WebPage*)web_view->page();
        QWebChannel *channel = page ? page->webChannel() : NULL;
        if (channel)
            channel->deregisterObject(page);
#endif
        delete web_view;
        web_view = NULL;
    }
}

void DisplayWindow::fill_table()
{
    if (!web_view || !is_finished)
        return;

    std::vector<QString>& displays = mainwindow->get_displays();
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
        ((WebPage*)web_view->page())->use_javascript(script);
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

    std::vector<QString>& displays = mainwindow->get_displays();
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

    std::vector<QString>& displays = mainwindow->get_displays();
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
            ((WebPage*)web_view->page())->use_javascript(script);
            break;
        }

        if (i + 1 == files.size())
            mainwindow->set_last_load_display_path(file_info.absolutePath().toUtf8().data());
    }

    return pos;
}

void DisplayWindow::export_file(const QString& name)
{
    std::vector<QString>& displays = mainwindow->get_displays();

    size_t i = 0;
    for (; i < displays.size(); ++i)
    {
        QFileInfo f(displays[i]);
        if (f.completeBaseName() == name)
            break;
    }
    if (i == displays.size())
        return;

    QString suggested = QString().fromUtf8(mainwindow->select_correct_save_display_path().c_str());
    QString file = QFileDialog::getSaveFileName(mainwindow, tr("Save display file"), suggested, tr("Display file (*.xsl)"));
    if (!file.length())
        return;

    QFile f(displays[i]);
    if (QFile::exists(file))
        QFile::remove(file);
    f.copy(file);

    QDir info(QFileInfo(file).absoluteDir());
    mainwindow->set_last_save_display_path(info.absolutePath().toUtf8().data());
}

void DisplayWindow::delete_file(const QString& name)
{
    std::vector<QString>& displays = mainwindow->get_displays();

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
    mainwindow->get_displays().erase(mainwindow->get_displays().begin() + i);
}

//---------------------------------------------------------------------------
void DisplayWindow::change_body_script_in_template(QString& html)
{
    QRegExp reg("\\{% block bodyScript %\\}\\{% endblock %\\}");
    QString script;
    int     pos = 0;

    reg.setMinimal(true);
#if defined(WEB_MACHINE_KIT)
    script = "";
#elif defined(WEB_MACHINE_ENGINE)
    script = "<script>\n"
        "var webpage;\n"
        "$(document).ready(function()\n"
        "{\n"
        "// Register Qt WebPage object\n"
        "new QWebChannel(qt.webChannelTransport, function (channel) {\n"
        "webpage = channel.objects.webpage;\n"
        "});\n"
        "});\n"
        "</script>";
#endif
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
void DisplayWindow::create_html_display(QString& display)
{
    QFile template_html(":/display.html");
    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray html = template_html.readAll();
    template_html.close();

    display = QString(html);

    set_webmachine_script_in_template(display);
    change_body_script_in_template(display);
}

//---------------------------------------------------------------------------
void DisplayWindow::create_web_view_finished(bool ok)
{
    if (!web_view || !ok)
    {
        create_html();
        mainwindow->set_msg_to_status_bar("Problem to load the checker page");
        return;
    }
    is_finished = true;

    if (progress_bar)
    {
        mainwindow->remove_widget_from_layout(progress_bar);
        delete progress_bar;
        progress_bar = NULL;
    }

    web_view->show();
    fill_table();
    web_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainwindow->set_widget_to_layout(web_view);
}

//---------------------------------------------------------------------------
void DisplayWindow::create_html()
{
    QString html;
    create_html_display(html);

    progress_bar = new ProgressBar(mainwindow);
    mainwindow->set_widget_to_layout(progress_bar);
    progress_bar->get_progress_bar()->setValue(0);
    progress_bar->show();

    web_view = new WebView(mainwindow);
    web_view->hide();

    WebPage* page = new WebPage(mainwindow, web_view);
    web_view->setPage(page);

    QObject::connect(web_view, SIGNAL(loadProgress(int)), progress_bar->get_progress_bar(), SLOT(setValue(int)));
    QObject::connect(web_view, SIGNAL(loadFinished(bool)), this, SLOT(create_web_view_finished(bool)));

    QUrl url = QUrl("qrc:/html");
    if (!url.isValid())
        return;

#if defined(WEB_MACHINE_ENGINE)
    QWebChannel *channel = new QWebChannel(page);
    page->setWebChannel(channel);
    channel->registerObject("webpage", page);
#endif
    web_view->setContent(html.toUtf8(), "text/html", url);
}

}
