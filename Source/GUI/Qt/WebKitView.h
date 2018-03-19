/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef QWEBKITVIEW_H
#define QWEBKITVIEW_H

#include <QWebView>
namespace MediaConch {

class MainWindow;

class WebView : public QWebView
{
    Q_OBJECT

public:
    explicit WebView(QWidget *parent = 0);
    virtual ~WebView();

protected:
    virtual void dropEvent(QDropEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);

    MainWindow *mainwindow;
};

}

#endif
