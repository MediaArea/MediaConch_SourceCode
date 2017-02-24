/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "WebChoose.h"

#if defined(WEB_MACHINE_KIT)

#include <QWebFrame>
#include <QWebElement>
#include <QWebElementCollection>
#include <QFileDialog>
#include <QFile>
#include <QTextDocument>
#include <QTextStream>

#include "Common/FileRegistered.h"
#include "Common/XsltPolicy.h"
#include "mainwindow.h"
#include "WebPage.h"
#include <QtGlobal>
#include <QWebSettings>

namespace MediaConch
{
    WebPage::WebPage(MainWindow *m, QWidget *parent) : WebCommonPage(m, parent)
    {
        connect(this, SIGNAL(loadFinished(bool)), this, SLOT(on_load_finished(bool)));
        QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
#if defined(QT_DEBUG)
        settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

        inspector = new QWebInspector;
        inspector->setPage(this);
        inspector->setVisible(true);
#endif
    }

    WebPage::~WebPage()
    {
#if defined(QT_DEBUG)
        if (inspector)
        {
            delete inspector;
            inspector = NULL;
        }
#endif
        disconnect(this, SLOT(on_load_finished(bool)));
        QWebFrame* frame = mainFrame();
        if (frame)
            disconnect(frame, SLOT(loadFinished(bool)));
        triggerAction(QWebPage::Stop);
    }

    void WebPage::on_load_finished(bool ok)
    {
        if (!ok)
            return;

        QWebFrame* frame = mainFrame();
        if (frame)
            frame->addToJavaScriptWindowObject("webpage", this);
    }

    bool WebPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest& request,
                                          QWebPage::NavigationType type)
    {
        if (type == QWebPage::NavigationTypeFormSubmitted || type == QWebPage::NavigationTypeFormResubmitted)
            return false;
        return QWebPage::acceptNavigationRequest(frame, request, type);
    }

    QString WebPage::chooseFile(QWebFrame *, const QString&)
    {
        if (select_file_name.startsWith("settings_"))
            return choose_file_settings();
        else if (select_file_name.startsWith("xslPolicyImport"))
            return choose_file_import_policy();
        else if (select_file_name.startsWith("xslPolicyCreateFromFile"))
            return choose_file_policy_from_file();

        QString value_input;
        std::string suggested_str;
        if (select_file_name == "displayImport_displayFile")
            suggested_str = mainwindow->select_correct_load_display_path();
        else
            suggested_str = mainwindow->select_correct_load_files_path();

        QString suggested = QString().fromUtf8(suggested_str.c_str(), suggested_str.length());
        if (select_file_name == "checkerRepository_directory")
            value_input = QFileDialog::getExistingDirectory(view(), NULL, suggested);
        else
            value_input = QFileDialog::getOpenFileName(view(), NULL, suggested);

        QMap<QString, QStringList>::iterator it = file_selector.find(select_file_name);
        if (!value_input.length())
        {
            if (it != file_selector.end())
                file_selector.erase(it);
            return QString();
        }

        QDir info(QFileInfo(value_input).absoluteDir());
        if (select_file_name == "displayImport_displayFile")
            mainwindow->set_last_load_display_path(info.absolutePath().toUtf8().data());
        else
            mainwindow->set_last_load_files_path(info.absolutePath().toUtf8().data());

        if (it != file_selector.end())
            file_selector[select_file_name] << value_input;
        else
            file_selector.insert(select_file_name, QStringList(value_input));

        return value_input;
    }

    bool WebPage::extension(Extension extension, const ExtensionOption *,
                            ExtensionReturn *output)
    {
        if (extension == QWebPage::ChooseMultipleFilesExtension)
        {
            std::string suggested_str;
            suggested_str = mainwindow->select_correct_load_files_path();

            QString suggested = QString().fromUtf8(suggested_str.c_str(), suggested_str.length());
            QStringList names = QFileDialog::getOpenFileNames(view(), QString::null, suggested);
            ((ChooseMultipleFilesExtensionReturn*)output)->fileNames = names;

            if (names.size() > 0)
            {
                QDir info(QFileInfo(names[0]).absoluteDir());
                mainwindow->set_last_load_files_path(info.absolutePath().toUtf8().data());
            }

            QMap<QString, QStringList>::iterator it = file_selector.find(select_file_name);
            if (it != file_selector.end())
                file_selector[select_file_name] << names;
            else
                file_selector.insert(select_file_name, names);
            return true;
        }
        return false;
    }

    bool WebPage::supportsExtension(Extension extension) const
    {
        if (extension == ChooseMultipleFilesExtension)
            return true;
        return false;
    }

    void WebPage::use_javascript(const QString& js)
    {
        mainFrame()->evaluateJavaScript(js);
    }

}

#endif
