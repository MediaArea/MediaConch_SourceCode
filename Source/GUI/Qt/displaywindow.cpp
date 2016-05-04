/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "displaywindow.h"
#include "displaymenu.h"
#include "mainwindow.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QDir>
#include <QFileDialog>
#include <QPushButton>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

DisplayWindow::DisplayWindow(MainWindow* m) : displayMenu(NULL), mainwindow(m)
{
    // Visual elements
    displayMenu = NULL;
}

DisplayWindow::~DisplayWindow()
{
    clearDisplay();
}

void DisplayWindow::displayDisplay()
{
    displayMenu = new DisplayMenu(mainwindow);
    fillTable();
    mainwindow->set_widget_to_layout(displayMenu);

    QTableWidget *table = displayMenu->get_display_table();
    if (table)
    {
        table->verticalHeader()->hide();
        table->horizontalHeader()->setStretchLastSection(true);
        table->resizeColumnsToContents();
        table->resizeRowsToContents();
    }

    QObject::connect(displayMenu->get_addFile_button(), SIGNAL(clicked()),
                     this, SLOT(add_new_file()));
    QObject::connect(displayMenu->get_exportFile_button(), SIGNAL(clicked()),
                     this, SLOT(export_file()));
    QObject::connect(displayMenu->get_delFile_button(), SIGNAL(clicked()),
                     this, SLOT(delete_file()));
}

void DisplayWindow::clearDisplay()
{
    if (!displayMenu)
        return;

    mainwindow->remove_widget_from_layout(displayMenu);
    delete displayMenu;
    displayMenu = NULL;
}

void DisplayWindow::fillTable()
{
    if (!displayMenu)
        return;

    QTableWidget *table = displayMenu->get_display_table();
    if (!table)
        return;

    table->clear();
    table->setRowCount(0);

    QTableWidgetItem *itemHeaderFile = new QTableWidgetItem(tr("File"));
    table->setHorizontalHeaderItem(0, itemHeaderFile);
    QTableWidgetItem *itemHeaderFullPath = new QTableWidgetItem(tr("Path"));
    table->setHorizontalHeaderItem(1, itemHeaderFullPath);

    std::vector<QString>& displays = mainwindow->get_displays();
    for (size_t i = 0; i < displays.size(); ++i)
    {
        QFileInfo file(displays[i]);

        QString fullPath = file.absoluteFilePath();
        if (fullPath.startsWith(":/displays"))
            fullPath = QString("MediaConch example");

        QTableWidgetItem *itemFile = new QTableWidgetItem(file.baseName());
        QTableWidgetItem *itemFullPath = new QTableWidgetItem(fullPath);

        Qt::ItemFlags flag = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsTristate;
        itemFullPath->setFlags(flag);
        itemFile->setFlags(flag);
        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, itemFile);
        table->setItem(row, 1, itemFullPath);
    }

    table->resizeColumnsToContents();
    table->resizeRowsToContents();
    table->horizontalHeader()->setStretchLastSection(true);
}

void DisplayWindow::add_new_file()
{
    if (!displayMenu)
        return;

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
    fillTable();
}

void DisplayWindow::export_file()
{
    if (!displayMenu)
        return;

    QTableWidget *table = displayMenu->get_display_table();
    if (!table)
        return;

    QItemSelectionModel *select = table->selectionModel();

    if (!select->hasSelection())
        return;

    QModelIndexList list = select->selectedRows();
    QTableWidgetItem* itemDir = table->item(list.first().row(), 1);

    if (!itemDir)
        return;

    QString save_file = itemDir->text();
    if (save_file == "MediaConch example")
    {
        QTableWidgetItem* itemFile = table->item(list.first().row(), 0);
        std::vector<QString>& vec = mainwindow->get_displays();
        for (size_t i = 0; i < vec.size(); ++i)
        {
            QFileInfo info(vec[i]);
            if (info.baseName() == itemFile->text())
            {
                save_file = vec[i];
                break;
            }
        }
    }

    QString suggested = QString().fromUtf8(mainwindow->select_correct_save_display_path().c_str());
    QString file = QFileDialog::getSaveFileName(mainwindow, tr("Save display file"), suggested, tr("Display file (*.xsl)"));
    if (!file.length())
        return;

    QFile f(save_file);
    f.copy(file);

    QDir info(QFileInfo(save_file).absoluteDir());
    mainwindow->set_last_load_display_path(info.absolutePath().toUtf8().data());
}

void DisplayWindow::delete_file()
{
    if (!displayMenu)
        return;

    QTableWidget *table = displayMenu->get_display_table();
    if (!table)
        return;

    QItemSelectionModel *select = table->selectionModel();

    if (!select->hasSelection())
        return;

    QModelIndexList list = select->selectedRows();

    for (int i = 0; i < list.count(); ++i)
    {
        QTableWidgetItem* itemDir = table->item(list[i].row(), 1);
        if (!itemDir)
            continue;

        QFile file(itemDir->text());
        file.remove();
        mainwindow->get_displays().erase(mainwindow->get_displays().begin() + list[i].row());
        table->removeRow(list[i].row());
    }
}

}
