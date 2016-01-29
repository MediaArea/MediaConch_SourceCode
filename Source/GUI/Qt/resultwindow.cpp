/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "resultwindow.h"
#include "mainwindow.h"
#include <QWebView>
#include <QString>
#include <QProgressBar>

#include "WebView.h"
#include "WebPage.h"
#include "progressbar.h"

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

//---------------------------------------------------------------------------
ResultWindow::ResultWindow(MainWindow* m) : mainwindow(m), view(NULL)
{
}

//---------------------------------------------------------------------------
ResultWindow::~ResultWindow()
{
    clear_visual_elements();
}

//---------------------------------------------------------------------------
void ResultWindow::create_web_view_finished(bool ok)
{
    if (!view || !ok)
        return;

    if (progressBar)
    {
        mainwindow->remove_widget_from_layout(progressBar);
        delete progressBar;
        progressBar = NULL;
    }
    mainwindow->set_widget_to_layout(view);
}

//---------------------------------------------------------------------------
void ResultWindow::update_html_with_results(QString& html)
{
    const std::vector<std::string>& vec = mainwindow->get_registered_files();

    for (size_t i = 0; i< vec.size(); ++i)
        html += QString("<h2>File number:%1: %2</h2>").arg(i).arg(vec[i].c_str());
}

//---------------------------------------------------------------------------
void ResultWindow::set_web_view_content(QString& html)
{
    view = new WebView(mainwindow);

    WebPage* page = new WebPage(mainwindow, view);
    view->setPage(page);

    QObject::connect(view, SIGNAL(loadProgress(int)), progressBar->get_progress_bar(), SLOT(setValue(int)));
    QObject::connect(view, SIGNAL(loadFinished(bool)), this, SLOT(create_web_view_finished(bool)));

    QUrl url = QUrl("qrc:/html");
    if (!url.isValid())
        return;

#if defined(WEB_MACHINE_ENGINE)
    QWebChannel *channel = new QWebChannel(page);
    page->setWebChannel(channel);
    channel->registerObject("webpage", page);
#endif
    view->setContent(html.toUtf8(), "text/html", url);
}

//---------------------------------------------------------------------------
void ResultWindow::display_results()
{
    progressBar = new ProgressBar(mainwindow);
    mainwindow->set_widget_to_layout(progressBar);
    progressBar->get_progress_bar()->setValue(0);
    progressBar->show();

    QString html("<h1>RESULT PAGE</h1>");
    update_html_with_results(html);
    set_web_view_content(html);
}

//---------------------------------------------------------------------------
void ResultWindow::clear_visual_elements()
{
    if (view)
    {
        mainwindow->remove_widget_from_layout(view);
        delete view;
        view = NULL;
    }

    if (progressBar)
    {
        mainwindow->remove_widget_from_layout(progressBar);
        delete progressBar;
        progressBar=NULL;
    }
}

}
