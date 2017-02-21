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

CommonWebWindow::CommonWebWindow(MainWindow *p) : main_window(p), web_view(NULL), progress_bar(NULL)
{
}

CommonWebWindow::~CommonWebWindow()
{
}

//***************************************************************************
// Slots
//***************************************************************************

//---------------------------------------------------------------------------
void CommonWebWindow::on_loadFinished(bool ok)
{
    create_web_view_finished(ok);
}

}
