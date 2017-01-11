/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "WebChoose.h"

#if defined(WEB_MACHINE_ENGINE)

#include <QWebEnginePage>
#include <QFileDialog>
#include <QFile>
#include <QTextDocument>
#include <QTextStream>
#include <QtGlobal>
#include <QWebChannel>

#include "Common/FileRegistered.h"
#include "Common/XsltPolicy.h"
#include "mainwindow.h"
#include "WebPage.h"

namespace MediaConch
{
    WebPage::WebPage(MainWindow *m, QWidget *parent) : WebCommonPage(m, parent)
    {
        connect(this, SIGNAL(loadFinished(bool)), this, SLOT(on_load_finished(bool)));
    }

    void WebPage::on_load_finished(bool)
    {
    }

    bool WebPage::acceptNavigationRequest(const QUrl&, QWebEnginePage::NavigationType,
                                          bool)
    {
        return false;
    }

    QStringList WebPage::chooseFiles(FileSelectionMode mode, const QStringList &, const QStringList&)
    {
        if (select_file_name.startsWith("settings_"))
        {
            QString file = choose_file_settings();
            if (!file.size())
                return QStringList();
            return QStringList(file);
        }
        else if (select_file_name.startsWith("xslPolicyImport"))
        {
            QString file = choose_file_import_policy();
            if (!file.size())
                return QStringList();
            return QStringList(file);
        }
        else if (select_file_name.startsWith("xslPolicyCreateFromFile"))
        {
            QString file = choose_file_policy_from_file();
            if (!file.size())
                return QStringList();
            return QStringList(file);
        }

        QStringList list;
        std::string suggested_str;
        if (select_file_name == "displayImport_displayFile")
            suggested_str = mainwindow->select_correct_load_display_path();
        else
            suggested_str = mainwindow->select_correct_load_files_path();
        QString suggested = QString().fromUtf8(suggested_str.c_str(), suggested_str.length());
        if (mode == QWebEnginePage::FileSelectOpen)
        {
            QString value_input;
            if (select_file_name == "checkerRepository_directory")
                value_input = QFileDialog::getExistingDirectory(view(), NULL, suggested);
            else
                value_input = QFileDialog::getOpenFileName(view(), NULL, suggested);

            QMap<QString, QStringList>::iterator it = file_selector.find(select_file_name);
            if (!value_input.length())
            {
                if (it != file_selector.end())
                    file_selector.erase(it);
                return QStringList();
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

            list = QStringList(value_input);
        }
        else
        {
            QStringList names = QFileDialog::getOpenFileNames(view(), QString::null, suggested);

            if (names.size())
            {
                QDir info(QFileInfo(names[0]).absoluteDir());
                mainwindow->set_last_load_files_path(info.absolutePath().toUtf8().data());
            }

            list = names;
            QMap<QString, QStringList>::iterator it = file_selector.find(select_file_name);
            if (it != file_selector.end())
                file_selector[select_file_name] = names;
            else
                file_selector.insert(select_file_name, names);
        }

        return list;
    }

    void WebPage::use_javascript(const QString& js)
    {
        runJavaScript(js);
    }

}

#endif
