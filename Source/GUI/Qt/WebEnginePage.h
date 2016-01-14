/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include <QMap>
#include <QWebEnginePage>
#include <QWebEngineView>

namespace MediaConch {

class MainWindow;

class WebPage : public QWebEnginePage
{
    Q_OBJECT

public:
    explicit WebPage(MainWindow *m, QWidget *parent = 0);

    void changeLocalFiles(QStringList& files);
    void use_javascript(const QString& js);

protected:
    virtual bool acceptNavigationRequest(const QUrl& url, QWebEnginePage::NavigationType type, bool isMainFrame);
    QStringList chooseFiles(FileSelectionMode mode, const QStringList &oldFiles, const QStringList &acceptedMimeTypes);
    void onFileUploadSelected();
    void onFileOnlineSelected();
    void onFileRepositorySelected();
    void clean_forms();

public Q_SLOTS:
    void onFileUploadSelected(const QString& display_xslt, const QString& policy);
    void onFileOnlineSelected(const QString& url, const QString& display_xslt, const QString& policy);
    void onFileRepositorySelected(const QString& display_xslt, const QString& policy);
    void onInputChanged(const QString& inputName);
    void onButtonClicked(const QString& id);
    void onDownloadReport(const QString& target, const QString& save_name);
    void menu_link_checker(const QString& name);
    void onSaveInfo(const QString& target, const QString& save_name);
    void onSaveTrace(const QString& target, const QString& save_name);

private Q_SLOTS:
    void onLoadFinished(bool ok);

protected:
    MainWindow                 *mainwindow;
    QMap<QString, QStringList>  file_selector;
    QString                     select_file_name;
};

}
