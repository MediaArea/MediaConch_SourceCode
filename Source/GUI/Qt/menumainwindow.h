/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef MENUMAINWINDOW_H
#define MENUMAINWINDOW_H

#include <QProgressBar>
#include <QWidget>
#include "WebView.h"

namespace MediaConch
{

class MenuMainWindow : QObject
{
    Q_OBJECT

public:
    explicit MenuMainWindow(QWidget *parent = 0);
    ~MenuMainWindow();

private:
    QWidget*      parent;
    WebView*      MenuView;
    QProgressBar* progressBar;

    void          createMenu();

private Q_SLOTS:
    void createMenuFinished(bool ok);
};

}

#endif // MAINWINDOW_H
