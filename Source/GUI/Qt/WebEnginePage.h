/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef WEBENGINEPAGE_H
#define WEBENGINEPAGE_H

#include "mainwindow.h"
#include "WebCommonPage.h"
#include <QWebEnginePage>
#include <QWebChannel>
#include <QMap>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>

namespace MediaConch {

class WebPage : public WebCommonPage
{
    Q_OBJECT

public:
    explicit WebPage(MainWindow *m, QWidget *parent = 0);
    virtual ~WebPage() {}

    virtual void use_javascript(const QString& js);

protected:
    virtual bool acceptNavigationRequest(const QUrl& url, QWebEnginePage::NavigationType type, bool isMainFrame);
    QStringList  chooseFiles(FileSelectionMode mode, const QStringList &oldFiles, const QStringList &acceptedMimeTypes);

private Q_SLOTS:
    void         on_load_finished(bool ok);
};

}

#endif
