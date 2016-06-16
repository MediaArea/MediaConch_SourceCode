/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef DISPLAYWINDOW_H
#define DISPLAYWINDOW_H

#include <QFileInfo>
#include <QString>

namespace MediaConch {

class MainWindow;

class DisplayWindow : public QObject
{
    Q_OBJECT

public:
    explicit DisplayWindow(MainWindow* m);
    ~DisplayWindow();

    void display_display();
    void clear_display();
    void fill_table();

private:
    // Visual elements
    MainWindow   *mainwindow;

private Q_SLOTS:
    void add_new_file();
    void export_file();
    void delete_file();
};

}

#endif // DISPLAYWINDOW_H
