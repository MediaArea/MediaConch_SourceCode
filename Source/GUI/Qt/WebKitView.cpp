/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "WebChoose.h"

#if defined(WEB_MACHINE_KIT)

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

    WebView::~WebView()
    {
        stop();
    }

    void WebView::dropEvent(QDropEvent *event)
    {
        if (event->mimeData()->hasUrls())
        {
            QList<QUrl> urls=event->mimeData()->urls();

            WebPage* p = (WebPage*)page();
            QStringList files;
            for (int i = 0; i < urls.size(); ++i)
                files << urls[i].toLocalFile();
            p->change_local_files(files);
        }
    }
}

#endif
