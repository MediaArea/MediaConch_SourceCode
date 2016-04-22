/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "resulttable.h"
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
// Constructor / Desructor
//***************************************************************************

//---------------------------------------------------------------------------
ResultTable::ResultTable(MainWindow* m, WebPage *p) : mainwindow(m), page(p)
{
}

//---------------------------------------------------------------------------
ResultTable::~ResultTable()
{
}

//---------------------------------------------------------------------------
void ResultTable::add_file_to_result_table(const std::string& full_path)
{
    FileRegistered* file = mainwindow->get_file_registered_from_file(full_path);
    if (!file)
        return;

    std::string filename = file->filename;
    QUrl url(QString().fromUtf8(filename.c_str(), filename.length()));
    QFileInfo info(url.path());

    QString script = QString("addFile('%1', '%2', '%3');")
        .arg(QString().fromUtf8(full_path.c_str(), full_path.length()))
        .arg(info.fileName())
        .arg(file->index);
    page->use_javascript(script);
}

}