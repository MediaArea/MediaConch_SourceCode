/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef WEBENGINEVIEW_H
#define WEBENGINEVIEW_H

#include <QWebEngineView>

namespace MediaConch {

class WebView : public QWebEngineView
{
    Q_OBJECT

public:
    explicit WebView(QWidget *parent = 0);
    virtual ~WebView();

protected:
    virtual void dropEvent(QDropEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
};

}

#endif
