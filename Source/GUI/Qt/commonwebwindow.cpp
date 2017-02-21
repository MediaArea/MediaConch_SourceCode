/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "commonwebwindow.h"
#include "mainwindow.h"
#include "WebPage.h"
#include "progressbar.h"
#if defined(WEB_MACHINE_ENGINE)
#include <QWebEnginePage>
#include <QWebChannel>
#endif
#if defined(WEB_MACHINE_KIT)
#include <QWebFrame>
#endif
#include <QFile>
#include <QProgressBar>

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

CommonWebWindow::CommonWebWindow(MainWindow *p) : main_window(p), web_view(NULL)
{
}

CommonWebWindow::~CommonWebWindow()
{
    clear_visual_elements();
}

//***************************************************************************
// Visual elements
//***************************************************************************

//---------------------------------------------------------------------------
void CommonWebWindow::display_html()
{
    clear_visual_elements();

    QFile template_html(":/base.html");
    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QString html(template_html.readAll());
    template_html.close();
    create_html(html);

    web_view = new WebView(main_window);
    web_view->hide();

    WebPage* page = new WebPage(main_window, web_view);
    web_view->setPage(page);

    QObject::connect(web_view, SIGNAL(loadProgress(int)), this, SLOT(on_loadProgress(int)));
    QObject::connect(web_view, SIGNAL(loadFinished(bool)), this, SLOT(on_loadFinished(bool)));

#if defined(WEB_MACHINE_ENGINE)
    QWebChannel *channel = new QWebChannel(page);
    page->setWebChannel(channel);
    channel->registerObject("webpage", page);
#endif
    web_view->setHtml(html.toUtf8());
}

//---------------------------------------------------------------------------
void CommonWebWindow::clear_visual_elements()
{
    if (web_view)
    {
        main_window->remove_widget_from_layout(web_view);
#if defined(WEB_MACHINE_ENGINE)
        WebPage* page = (WebPage*)web_view->page();
        QWebChannel *channel = page ? page->webChannel() : NULL;
        if (channel)
            channel->deregisterObject(page);
#endif
        delete web_view;
        web_view = NULL;
    }
}

//***************************************************************************
// Slots
//***************************************************************************

//---------------------------------------------------------------------------
void CommonWebWindow::on_loadFinished(bool ok)
{
    if (!web_view || !ok)
    {
        main_window->set_msg_to_status_bar("Problem to load the checker page");
        return;
    }

    create_web_view_finished(ok);

    web_view->show();
    web_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    main_window->set_widget_to_layout(web_view);
}

//---------------------------------------------------------------------------
void CommonWebWindow::on_loadProgress(int progress)
{
    if (progress==100)
        main_window->status_bar_clear_message();
    else
        main_window->set_msg_to_status_bar(QString::number(progress)+"%");
}

}
