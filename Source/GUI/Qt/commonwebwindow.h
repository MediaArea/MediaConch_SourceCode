/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef COMMONWEBWINDOW_H
#define COMMONWEBWINDOW_H

#include <QWidget>
#include "WebView.h"

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

protected:
    void                        display_html();
    void                        clear_visual_elements();

    virtual void                create_html(QString &html) = 0;
    virtual void                create_web_view_finished(bool ok) {};

    MainWindow*                 main_window;
    WebView*                    web_view;

private Q_SLOTS:
    void                        on_loadFinished(bool ok);
    void                        on_loadProgress(int progress);
};

}

#endif // COMMONWEBWINDOW_H
