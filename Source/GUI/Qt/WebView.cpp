/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include <QWebView>
#include <QMimeData>
#include <QDropEvent>
#include "WebView.h"
#include "WebPage.h"

namespace MediaConch
{
    WebView::WebView(QWidget *parent) : QWebView(parent)
    {
        setAcceptDrops(true);
    }

    void WebView::dropEvent(QDropEvent *event)
    {
        if (event->mimeData()->hasUrls())
        {
            QList<QUrl> urls=event->mimeData()->urls();

            if (urls.size() > 0)
            {
                WebPage* p = (WebPage*)page();
                QString file = urls[0].toLocalFile();
                p->changeLocalFile(file);
            }
        }
    }
}
