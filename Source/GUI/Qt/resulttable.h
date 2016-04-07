/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef RESULTWINDOW_H
#define RESULTWINDOW_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include "mainwindow.h"

class QProgressBar;
class QTimer;

namespace MediaConch {

class WebPage;
class ResultTable;
class FileRegistered;

class ResultTable : public QObject
{
    Q_OBJECT

public:
    explicit ResultTable(MainWindow* m, WebPage *p);
    ~ResultTable();

    void add_file_to_result_table(const std::string&);
    /* void remove_file_from_result_table(); */
    /* void remove_all_files_from_result_table(); */

private:
    // Visual elements
    MainWindow               *mainwindow;
    WebPage                  *page;
};

}

#endif // RESULTWINDOW_H
