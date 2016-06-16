/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "displaywindow.h"
#include "mainwindow.h"
#include <QDir>
#include <QFileDialog>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

DisplayWindow::DisplayWindow(MainWindow* m) : mainwindow(m)
{
}

DisplayWindow::~DisplayWindow()
{
    clear_display();
}

void DisplayWindow::display_display()
{
}

void DisplayWindow::clear_display()
{
}

void DisplayWindow::fill_table()
{
    std::vector<QString>& displays = mainwindow->get_displays();
    for (size_t i = 0; i < displays.size(); ++i)
    {
        QFileInfo file(displays[i]);

        QString fullPath = file.absoluteFilePath();
        if (fullPath.startsWith(":/displays"))
            fullPath = QString("MediaConch example");
    }
}

void DisplayWindow::add_new_file()
{
    QString suggested = QString().fromUtf8(mainwindow->select_correct_load_display_path().c_str());
    QStringList List = QFileDialog::getOpenFileNames(mainwindow, "Open file", suggested, "Display files (*.xsl);;All (*.*)", 0, QFileDialog::DontUseNativeDialog);
    if (List.empty())
        return;

    QDir info(QFileInfo(List[0]).absoluteDir());
    mainwindow->set_last_load_display_path(info.absolutePath().toUtf8().data());

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
            return;

    std::vector<QString>& displays = mainwindow->get_displays();
    for (int i = 0; i < List.count(); ++i)
    {
        QFileInfo file(List[i]);
        for (int j = 0; 1; ++j)
        {
            QString str;
            if (!j)
                str = QString("%1/%2.xsl").arg(dir.absolutePath()).arg(file.baseName());
            else
                str = QString("%1/%2_%3.xsl").arg(dir.absolutePath()).arg(file.baseName()).arg(j);
            QFile info(str);
            if (info.exists())
                continue;

            QFile::copy(file.absoluteFilePath(), str);
            displays.push_back(str);
            break;
        }
    }
}

void DisplayWindow::export_file()
{
    QString suggested = QString().fromUtf8(mainwindow->select_correct_save_display_path().c_str());
    QString file = QFileDialog::getSaveFileName(mainwindow, tr("Save display file"), suggested, tr("Display file (*.xsl)"));
    if (!file.length())
        return;

    // QFile f(save_file);
    // f.copy(file);

    // QDir info(QFileInfo(save_file).absoluteDir());
    // mainwindow->set_last_load_display_path(info.absolutePath().toUtf8().data());
}

void DisplayWindow::delete_file()
{
    // QFile file(selected);
    // file.remove();
    // mainwindow->get_displays().erase(mainwindow->get_displays().begin() + list[i].row());
}

}
