/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "resultwindow.h"
#include "mainwindow.h"
#include "Common/FileRegistered.h"
#include <QString>
#include <QProgressBar>
#include <QTimer>
#include <QDebug>

#include "WebView.h"
#include "WebPage.h"

#if defined(WEB_MACHINE_ENGINE)
#include <QWebChannel>
#endif
#if defined(WEB_MACHINE_KIT)
#include <QWebFrame>
#include <QWebElement>
#endif

#include "progressbar.h"

namespace MediaConch {

//***************************************************************************
// UpdateResultWindow
//***************************************************************************

//---------------------------------------------------------------------------
UpdateResultWindow::UpdateResultWindow(MainWindow* m, const std::vector<std::string>& to_update_files, WebPage* p, int t) : QThread(), mainwindow(m), page(p), update_timer(NULL), timer(t)
{
    files = to_update_files;
}

//---------------------------------------------------------------------------
UpdateResultWindow::~UpdateResultWindow()
{
    if (update_timer)
    {
        update_timer->stop();
        delete update_timer;
        update_timer = NULL;
    }
}

//---------------------------------------------------------------------------
void UpdateResultWindow::run()
{
    std::vector<std::string> vec;
    for (size_t i = 0; i < files.size(); ++i)
    {
        FileRegistered* file = mainwindow->get_file_registered_from_file(files[i]);
        if (!file)
        {
            vec.push_back(files[i]);
            continue;
        }

        if (!file->analyzed)
            vec.push_back(files[i]);
        page->emit_update_registered_file(file);
    }

    if (!vec.size())
    {
        files.clear();
        return;
    }

    files = vec;
    update_timer = new QTimer(0);
    update_timer->setSingleShot(true);
    update_timer->moveToThread(this);
    connect(update_timer, SIGNAL(timeout()), this, SLOT(restart_timer()), Qt::DirectConnection);
    update_timer->start(timer);
    exec();
    if (update_timer)
    {
        update_timer->stop();
        delete update_timer;
        update_timer = NULL;
    }
}

//---------------------------------------------------------------------------
void UpdateResultWindow::restart_timer()
{
    if (update_timer)
    {
        delete update_timer;
        update_timer = NULL;
    }

    std::vector<std::string> vec;
    for (size_t i = 0; i < files.size(); ++i)
    {
        FileRegistered* file = mainwindow->get_file_registered_from_file(files[i]);
        if (!file)
        {
            vec.push_back(files[i]);
            continue;
        }

        if (!file->analyzed)
        {
            vec.push_back(files[i]);
        }
        page->emit_update_registered_file(file);
    }

    if (vec.size())
    {
        files = vec;
        update_timer = new QTimer(0);
        update_timer->setSingleShot(true);
        update_timer->moveToThread(this);
        connect(update_timer, SIGNAL(timeout()), this, SLOT(restart_timer()), Qt::DirectConnection);
        update_timer->start(timer);
    }
    else
    {
        files.clear();
        quit();
    }
}

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

//---------------------------------------------------------------------------
ResultWindow::ResultWindow(MainWindow* m) : mainwindow(m), view(NULL), default_update_timer(5000)
{
    progressBar = NULL;
    updater = NULL;
}

//---------------------------------------------------------------------------
ResultWindow::~ResultWindow()
{
    clear_visual_elements();
    stop_thread();
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
    updater = new UpdateResultWindow(mainwindow, to_update_files, (WebPage*)view->page(),
                                     default_update_timer);
    updater->start();
}

//---------------------------------------------------------------------------
void ResultWindow::stop_thread()
{
    if (updater)
    {
        updater->quit();
        updater->wait();
        delete updater;
        updater = NULL;
    }
}

//---------------------------------------------------------------------------
void ResultWindow::add_displays_file_detail_end(QString& html)
{
    QFile template_html(":/fileDetailCheckerTableEnd.html");

    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = template_html.readAll();
    template_html.close();

    html += QString(data);
}

//---------------------------------------------------------------------------
void ResultWindow::add_displays_file_detail_table_end(QString& html)
{
    html += "</tbody></table>";
}

//---------------------------------------------------------------------------
void ResultWindow::select_the_correct_value(const QString& value, const QString& selector, QString& html)
{
    int pos = html.indexOf(selector);
    if (pos == -1)
        return;

    QRegExp reg("<option selected=\"selected\" value=\"-1\">[\\n\\r\\t\\s]*Choose");
    reg.setMinimal(true);

    if ((pos = reg.indexIn(html, pos)) != -1)
    {
        html.replace(pos, reg.matchedLength(), "<option value=\"-1\">Choose");
        reg = QRegExp(QString("value=\"%1\"").arg(value));
        if ((pos = reg.indexIn(html, pos)) != -1)
        {
            pos += reg.matchedLength();
            html.insert(pos, " selected=\"selected\"");
        }
    }
}

//---------------------------------------------------------------------------
void ResultWindow::add_displays_file_detail_modal(const FileRegistered* file, QString& base)
{
#if defined(WEB_MACHINE_ENGINE)
    QFile template_html(":/fileDetailCheckerTableModalEngine.html");
#elif defined(WEB_MACHINE_KIT)
    QFile template_html(":/fileDetailCheckerTableModalKit.html");
#endif
    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = template_html.readAll();
    template_html.close();

    QString html(data);

    QString policies;
    mainwindow->create_policy_options(policies);
    mainwindow->add_policy_to_html_selection(policies, html, "fileDetail_policy_selector");
    QString policy_value = QString("%1").arg(file->policy);
    select_the_correct_value(policy_value, "fileDetail_policy_selector", html);

    QString displays;
    mainwindow->create_displays_options(displays);
    mainwindow->add_display_to_html_selection(displays, html, "fileDetail_policy_display");
    mainwindow->add_display_to_html_selection(displays, html, "fileDetail_implementation_display");
    QString display_value = QString("%1").arg(file->display);
    select_the_correct_value(display_value, "fileDetail_policy_display", html);
    select_the_correct_value(display_value, "fileDetail_implementation_display", html);

    change_html_file_detail(file, html);

    base += QString(html);
}

//---------------------------------------------------------------------------
void ResultWindow::add_displays_file_detail_element(const FileRegistered* file, QString& base)
{
    QFile template_html(":/fileDetailCheckerTableElement.html");

    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = template_html.readAll();
    template_html.close();

    QString html(data);
    change_html_file_detail(file, html);

    base += QString(html);
}

//---------------------------------------------------------------------------
void ResultWindow::update_html_with_results(QString& html)
{
    QString bottom;
    std::map<std::string, FileRegistered> files;
    mainwindow->get_registered_files(files);
    std::map<std::string, FileRegistered>::iterator it = files.begin();

    for (; it != files.end(); ++it)
    {
        to_update_files.push_back(it->first);

        add_displays_file_detail_element(&it->second, html);
        add_displays_file_detail_modal(&it->second, bottom);
    }

    add_displays_file_detail_table_end(html);
    html += bottom;
}

//---------------------------------------------------------------------------
void ResultWindow::add_displays_file_detail_start(QString& html)
{

    QFile template_html(":/fileDetailCheckerTableStart.html");

    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = template_html.readAll();
    template_html.close();

    html = QString(data);
}

//---------------------------------------------------------------------------
void ResultWindow::change_body_in_template(QString& body, QString& html)
{
    QRegExp reg("\\{% block body %\\}\\{% endblock %\\}");
    int pos = 0;

    reg.setMinimal(true);
    if ((pos = reg.indexIn(html, pos)) != -1)
    {
        QString start = html.left(pos);
        QString end = html.right(html.length() - pos - reg.matchedLength());
        html = start + body + end;
    }
}

//---------------------------------------------------------------------------
void ResultWindow::create_html_base(QString& html)
{
#if defined(WEB_MACHINE_KIT)
    QFile template_html(":/baseKit.html");
#elif defined(WEB_MACHINE_ENGINE)
    QFile template_html(":/baseEngine.html");
#endif

    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = template_html.readAll();
    template_html.close();

    html = QString(data);
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
    view->setHtml(html.toUtf8(), url);
#else
    view->setContent(html.toUtf8(), "text/html", url);
#endif
}

//---------------------------------------------------------------------------
void ResultWindow::display_results()
{
    progressBar = new ProgressBar(mainwindow);
    mainwindow->set_widget_to_layout(progressBar);
    progressBar->get_progress_bar()->setValue(0);
    progressBar->show();

    QString html;
    create_html_base(html);

    QString body;
    add_displays_file_detail_start(body);
    update_html_with_results(body);
    add_displays_file_detail_end(body);

    change_body_in_template(body, html);

    set_web_view_content(html);
}

//---------------------------------------------------------------------------
void ResultWindow::clear_visual_elements()
{
    if (view)
    {
        mainwindow->remove_widget_from_layout(view);
#if defined(WEB_MACHINE_ENGINE)
        WebPage* page = (WebPage*)view->page();
        QWebChannel *channel = page ? page->webChannel() : NULL;
        if (channel)
            channel->deregisterObject(page);
#endif
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

//---------------------------------------------------------------------------
void ResultWindow::change_html_file_detail(const FileRegistered* file, QString& html)
{
    std::string full_name = file->filepath;
    if (full_name.length())
        full_name += "/";
    full_name += file->filename;

    QFileInfo f(QString().fromUtf8(full_name.c_str(), full_name.length()));
    QString policy;
    Policy *p = mainwindow->get_policy(file->policy);
    if (p)
        policy = QString().fromUtf8(p->title.c_str());
    else
        policy = "N/A";

    QRegExp reg("\\{\\{ check.getSource \\}\\}");
    int pos = 0;
    reg.setMinimal(true);
    QString filepath = f.filePath();
    filepath = filepath.replace("\\", "\\\\");
    filepath = filepath.replace("'", "\\'");
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), filepath);

    reg = QRegExp("\\{\\{ check.filename \\}\\}");
    pos = 0;
    reg.setMinimal(true);
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), f.fileName());

    reg = QRegExp("\\{\\{ check.policyname \\}\\}");
    pos = 0;
    reg.setMinimal(true);
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), policy);

    reg = QRegExp("\\{\\{ check.policyvalue \\}\\}");
    pos = 0;
    reg.setMinimal(true);
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), QString("%1").arg(file->policy));

    reg = QRegExp("\\{\\{ check.displayvalue \\}\\}");
    pos = 0;
    reg.setMinimal(true);
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), QString("%1").arg(file->display));

    reg = QRegExp("\\{\\{ check.filename \\| truncate\\(20\\) \\}\\}");
    pos = 0;
    reg.setMinimal(true);
    QString fname;
    if (f.fileName().length() > 20)
        fname = QString(f.fileName().left(20)) + "...";
    else
        fname = QString(f.fileName());
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), fname);

    reg = QRegExp("\\{\\{ check.policyname \\| truncate\\(20\\) \\}\\}");
    pos = 0;
    reg.setMinimal(true);
    if (policy.length() > 20)
        fname = QString(policy.left(20)) + "...";
    else
        fname = QString(policy);
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), fname);

    reg = QRegExp("\\{\\{ loop\\.index \\}\\}");
    pos = 0;
    reg.setMinimal(true);
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), QString("%1").arg(file->index));
}

}
