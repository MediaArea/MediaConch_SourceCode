/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef WEBENGINEPAGE_H
#define WEBENGINEPAGE_H

#include <QMap>
#include <QWebEnginePage>
#include <QWebEngineView>
#include "mainwindow.h"

namespace MediaConch {

class WebPage : public QWebEnginePage
{
    Q_OBJECT

public:
    explicit WebPage(MainWindow *m, QWidget *parent = 0);

    void changeLocalFiles(QStringList& files);
    void use_javascript(const QString& js);
    void update_status_registered_file(MainWindow::FileRegistered* file);

protected:
    virtual bool acceptNavigationRequest(const QUrl& url, QWebEnginePage::NavigationType type, bool isMainFrame);
    QStringList chooseFiles(FileSelectionMode mode, const QStringList &oldFiles, const QStringList &acceptedMimeTypes);

    void onFileUploadSelected();
    void onFileOnlineSelected();
    void onFileRepositorySelected();

    void clean_forms();

    void set_analyzed_status(MainWindow::FileRegistered* file);
    void set_implementation_status(MainWindow::FileRegistered* file);
    void set_policy_status(MainWindow::FileRegistered* file);

    bool report_is_html(const QString& report);
    bool report_is_xml(const QString& report);

public Q_SLOTS:
    void onFileUploadSelected(const QString& display_xslt, const QString& policy);
    void onFileOnlineSelected(const QString& url, const QString& display_xslt, const QString& policy);
    void onFileRepositorySelected(const QString& display_xslt, const QString& policy);
    void onInputChanged(const QString& inputName);
    void onButtonClicked(const QString& id);

    void menu_link_checker(const QString& name);

    void onDownloadReport(const QString& target, const QString& save_name);
    void onSaveImplementationReport(const QString& file, const QString& save_name);
    void onSavePolicyReport(const QString& file, const QString& save_name);
    void onSaveInfo(const QString& target, const QString& save_name);
    void onSaveTrace(const QString& target, const QString& save_name);

    void onFillImplementationReport(const QString& name, const QString& target);
    void onFillPolicyReport(const QString& file, const QString& target);
    QString onFillMediaInfoReport(const QString& file);
    QString onFillMediaTraceReport(const QString& file);

    void close_all();
    void close_element(const QString& file);

private Q_SLOTS:
    void onLoadFinished(bool ok);

protected:
    MainWindow                 *mainwindow;
    QMap<QString, QStringList>  file_selector;
    QString                     select_file_name;
};

}

#endif
