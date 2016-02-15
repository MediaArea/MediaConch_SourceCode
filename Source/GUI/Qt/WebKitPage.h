/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef WEBKITPAGE_H
#define WEBKITPAGE_H

#include <QMap>
#include <QWebPage>
#include <QWebView>
#include "mainwindow.h"

namespace MediaConch {

class WebPage : public QWebPage
{
    Q_OBJECT

public:
    explicit WebPage(MainWindow *m, QWidget *parent = 0);

    void changeLocalFiles(QStringList& files);
    void use_javascript(const QString& js);
    void emit_update_registered_file(MainWindow::FileRegistered* file);

protected:
    virtual bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest& request, QWebPage::NavigationType type);
    QString chooseFile(QWebFrame *frame, const QString& suggested);
    bool extension(Extension extension, const ExtensionOption * option = 0, ExtensionReturn * output = 0);
    bool supportsExtension(Extension extension) const;

    int  onFileUploadSelected(QWebElement form);
    int  onFileOnlineSelected(QWebElement form);
    int  onFileRepositorySelected(QWebElement form);

    void set_analyzed_status(MainWindow::FileRegistered* file);
    void set_implementation_status(MainWindow::FileRegistered* file);
    void set_policy_status(MainWindow::FileRegistered* file);

    bool report_is_html(const QString& report);
    bool report_is_xml(const QString& report);

public Q_SLOTS:
    void onInputChanged(const QString& inputName);
    void onButtonClicked(const QString& id);

    void onDownloadReport(const QString& target, const QString& save_name);
    void onSaveImplementationReport(const QString& file, const QString& save_name, const QString& display = "-1");
    void onSavePolicyReport(const QString& file, const QString& save_name, const QString& policy = "-1", const QString& display = "-1");
    void onSaveInfo(const QString& target, const QString& save_name);
    void onSaveTrace(const QString& target, const QString& save_name);

    void menu_link_checker(const QString& name);
    void onFillImplementationReport(const QString& name, const QString& target, const QString& display);
    void onFillPolicyReport(const QString& file, const QString& target, const QString& policy, const QString& display);
    QString onFillMediaInfoReport(const QString& file);
    QString onFillMediaTraceReport(const QString& file);

    void close_all();
    void close_element(const QString& file);

private Q_SLOTS:
    void onLoadFinished(bool ok);
    void update_status_registered_file(MainWindow::FileRegistered* file);

Q_SIGNALS:
    void update_registered_file(MainWindow::FileRegistered* file);

protected:
    MainWindow                 *mainwindow;
    QMap<QString, QStringList>  file_selector;
    QString                     select_file_name;
    QString                     button_clicked_id;
};

}

#endif
