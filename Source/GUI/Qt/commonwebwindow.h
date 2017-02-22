/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef COMMONWEBWINDOW_H
#define COMMONWEBWINDOW_H

#include <QWidget>

namespace MediaConch
{

class MainWindow;
class WebPage;
class ProgressBar;

class CommonWebWindow : public QObject
{
    Q_OBJECT

public:
    explicit CommonWebWindow(MainWindow* parent);
    ~CommonWebWindow();

    virtual void                create_web_view_finished() {};

protected:
    void                        display_html();

    virtual void                create_html(QString &html) = 0;

    MainWindow*                 main_window;

//***************************************************************************
// SLOTS
//***************************************************************************

public Q_SLOTS:
    void use_javascript(const QString& script);
};

}

#endif // COMMONWEBWINDOW_H
