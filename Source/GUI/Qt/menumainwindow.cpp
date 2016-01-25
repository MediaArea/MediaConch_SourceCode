/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "menumainwindow.h"
#include "mainwindow.h"
#include "WebPage.h"
#if defined(WEB_MACHINE_ENGINE)
#include <QWebEnginePage>
#include <QWebChannel>
#endif
#if defined(WEB_MACHINE_KIT)
#include <QWebFrame>
#endif
#include <QFile>

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

MenuMainWindow::MenuMainWindow(QWidget *p)
{
    parent = p;
    createMenu();
}

MenuMainWindow::~MenuMainWindow()
{
}

//***************************************************************************
// Visual elements
//***************************************************************************

//---------------------------------------------------------------------------
void MenuMainWindow::createMenuFinished(bool)
{
    if (!MenuView)
        return;

#if defined(WEB_MACHINE_ENGINE)
    //TODO : remove scroll bar
#endif
#if defined(WEB_MACHINE_KIT)
    QWebFrame *frame = MenuView->page()->currentFrame();
    frame->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    frame->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
#endif
}

//---------------------------------------------------------------------------
void MenuMainWindow::createMenu()
{
    QFile menu_file(":/menu.html");

    MenuView = new WebView(parent);
    MenuView->setAcceptDrops(true);
    MenuView->setMaximumHeight(75);
    MenuView->setMinimumHeight(75);
    ((MainWindow*)parent)->set_widget_to_layout(MenuView);

    WebPage* page = new WebPage((MainWindow*)parent, MenuView);
    MenuView->setPage(page);

    menu_file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray html = menu_file.readAll();
    menu_file.close();

    QObject::connect(MenuView, SIGNAL(loadFinished(bool)), this, SLOT(createMenuFinished(bool)));

    QUrl url = QUrl("qrc:/html");
    if (!url.isValid())
        return;

#if defined(WEB_MACHINE_ENGINE)
    QWebChannel *channel = new QWebChannel(page);
    page->setWebChannel(channel);
    channel->registerObject("webpage", page);
    MenuView->setHtml(html, url);
#endif
#if defined(WEB_MACHINE_KIT)
    MenuView->setContent(html, "text/html", url);
#endif

}

}
