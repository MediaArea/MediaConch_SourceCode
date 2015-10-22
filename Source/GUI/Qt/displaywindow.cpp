/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "displaywindow.h"
#include "mainwindow.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QDir>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

DisplayWindow::DisplayWindow(MainWindow* m) : table(NULL), mainwindow(m)
{
    // Visual elements
    ;
}

DisplayWindow::~DisplayWindow()
{
    clearDisplay();
}

void DisplayWindow::displayDisplay()
{
    table = new QTableWidget(0, 2, mainwindow);
    fillTable();
    mainwindow->set_widget_to_layout(table);
}

void DisplayWindow::clearDisplay()
{
    if (!table)
        return;

    mainwindow->remove_widget_from_layout(table);
    delete table;
    table = NULL;
}

void DisplayWindow::fillTable()
{
    if (!table)
        return;

    table->horizontalHeader()->setStretchLastSection(true);
    if (!table->horizontalHeaderItem(0))
    {
        QTableWidgetItem* header = new QTableWidgetItem(tr("File"));
        table->setHorizontalHeaderItem(0, header);
    }
    else
        table->horizontalHeaderItem(0)->setText(tr("File"));
    if (!table->horizontalHeaderItem(1))
    {
        QTableWidgetItem* header = new QTableWidgetItem(tr("Directory"));
        table->setHorizontalHeaderItem(1, header);
    }
    else
        table->horizontalHeaderItem(1)->setText(tr("Directory"));
    table->verticalHeader()->hide();

    std::vector<QString> displays = mainwindow->get_displays();
    for (size_t i = 0; i < displays.size(); ++i)
    {
        QFileInfo file(displays[i]);

        QTableWidgetItem *itemFile = new QTableWidgetItem(file.baseName());
        QTableWidgetItem *itemDir = new QTableWidgetItem(file.absoluteFilePath());

        Qt::ItemFlags flag = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsTristate;
        itemDir->setFlags(flag);
        itemFile->setFlags(flag);
        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, itemFile);
        table->setItem(row, 1, itemDir);
    }

    table->resizeColumnsToContents();
    table->resizeRowsToContents();
}

}
