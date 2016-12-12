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
#include "mainwindow.h"

namespace MediaConch
{
    WebView::WebView(QWidget *parent) : QWebView(parent), mainwindow((MainWindow*)parent)
    {
        setAcceptDrops(true);
        setContextMenuPolicy(Qt::CustomContextMenu);
    }

    WebView::~WebView()
    {
        stop();
        QWebPage* wpage = page();
        if (wpage)
        {
            delete wpage;
            wpage = NULL;
        }
    }

    void WebView::dragEnterEvent(QDragEnterEvent *event)
    {
        if (event->mimeData()->hasUrls())
            event->acceptProposedAction();
    }

    void WebView::dropEvent(QDropEvent *event)
    {
        if (event->mimeData()->hasUrls())
        {
            QList<QUrl> urls=event->mimeData()->urls();

            QStringList files;
            for (int i = 0; i < urls.size(); ++i)
                files << urls[i].toLocalFile();
            mainwindow->drag_and_drop_files_action(files);
        }
    }
}

#endif
