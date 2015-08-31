/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include <QMap>
#include <QWebPage>
#include <QWebView>
namespace MediaConch {

class MainWindow;

class WebPage : public QWebPage
{
    Q_OBJECT

public:
    explicit WebPage(MainWindow *m, QWidget *parent = 0);

protected:
    virtual bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest& request, QWebPage::NavigationType type);
    QString chooseFile(QWebFrame *frame, const QString& suggested);
    bool extension(Extension extension, const ExtensionOption * option = 0, ExtensionReturn * output = 0);
    bool supportsExtension(Extension extension) const;
    void onFileUploadSelected(QWebElement form);
    void onFileOnlineSelected(QWebElement form);
    void onFileRepositorySelected(QWebElement form);

public Q_SLOTS:
    void onInputChanged(const QString& inputName);
    void onButtonClicked(const QString& id);
    void menu_link_checker(const QString& name);

private Q_SLOTS:
    void onLoadFinished(bool ok);

protected:
    MainWindow             *mainwindow;
    QMap<QString, QString>  file_selector;
    QString                 select_file_name;
    QString                 button_clicked_id;
};

}
