/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef WEBKITPAGE_H
#define WEBKITPAGE_H

#include "mainwindow.h"
#include "WebCommonPage.h"
#include <QWebInspector>

namespace MediaConch {

class WebPage : public WebCommonPage
{
    Q_OBJECT

public:
    explicit WebPage(MainWindow *m, QWidget *parent = 0);
    virtual ~WebPage();

    virtual void use_javascript(const QString& js);

protected:
    virtual bool   acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest& request, QWebPage::NavigationType type);
    QString        chooseFile(QWebFrame *frame, const QString& suggested);
    bool           extension(Extension extension, const ExtensionOption * option = 0, ExtensionReturn *output = 0);
    bool           supportsExtension(Extension extension) const;

public Q_SLOTS:
    void           on_load_finished(bool ok);

private:
    QWebInspector *inspector;
};

}

#endif
