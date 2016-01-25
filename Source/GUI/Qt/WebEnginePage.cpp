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

#include "mainwindow.h"
#include "WebPage.h"
#include <QtGlobal>
#include <QWebChannel>

namespace MediaConch
{
    WebPage::WebPage(MainWindow *m, QWidget *parent) : QWebEnginePage(parent), mainwindow(m)
    {
        connect(this, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
    }

    void WebPage::onLoadFinished(bool ok)
    {
        if (!ok)
            return;
    }

    void WebPage::menu_link_checker(const QString& name)
    {
        if (!name.compare("Checker"))
            mainwindow->checker_selected();
        else if (!name.compare("Policies"))
            mainwindow->policies_selected();
        else if (!name.compare("Display"))
            mainwindow->display_selected();
        else
            mainwindow->checker_selected();
    }

    void WebPage::onInputChanged(const QString& inputName)
    {
        select_file_name = inputName;
    }

    void WebPage::onButtonClicked(const QString& id)
    {
        if (id == "checkerUpload_Check file")
            onFileUploadSelected();
        else if (id == "checkerRepository_Check files")
            onFileRepositorySelected();
        else if (id == "checkerOnline_Check file")
            onFileOnlineSelected();
    }

    void WebPage::onDownloadReport(const QString& report, const QString& save_name)
    {
        if (report.isEmpty())
            return;

        QString dl_file = QFileDialog::getSaveFileName(view(), "Save report", save_name);

        if (!dl_file.length())
            return;

        QFile file(dl_file);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        QTextStream out(&file);
        if (dl_file.endsWith(".html"))
        {
            out << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\" \"http://www.w3.org/TR/REC-html40/loose.dtd\">\n";
            out << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n";
            out << report;
        }
        else
        {
            QTextDocument text;
            text.setHtml(report.toUtf8());
            out << text.toPlainText() << "\n";
        }
    }

    void WebPage::onSaveInfo(const QString& report, const QString& save_name)
    {
        onDownloadReport(report, save_name);
    }

    void WebPage::onSaveTrace(const QString& report, const QString& save_name)
    {
        onDownloadReport(report, save_name);
    }

    void WebPage::clean_forms()
    {
        file_selector.clear();

        runJavaScript("document.getElementById('checkerUpload_step1_policy').value = -1;");
        runJavaScript("document.getElementById('checkerUpload_step1_xslt').value = '';");
        runJavaScript("document.getElementById('checkerUpload_step1_display_selector').value = -1;");
        runJavaScript("document.getElementById('checkerUpload_step1_display_xslt').value = '';");
        runJavaScript("document.getElementById('checkerUpload_file').value = \"\";");

        runJavaScript("document.getElementById('checkerOnline_step1_policy').value = -1;");
        runJavaScript("document.getElementById('checkerOnline_step1_xslt').value = '';");
        runJavaScript("document.getElementById('checkerOnline_step1_display_selector').value = -1;");
        runJavaScript("document.getElementById('checkerOnline_step1_display_xslt').value = '';");
        runJavaScript("document.getElementById('checkerOnline_file').value = \"\";");

        runJavaScript("document.getElementById('checkerRepository_step1_policy').value = -1;");
        runJavaScript("document.getElementById('checkerRepository_step1_xslt').value = '';");
        runJavaScript("document.getElementById('checkerRepository_step1_display_selector').value = -1;");
        runJavaScript("document.getElementById('checkerRepository_step1_display_xslt').value = '';");
        runJavaScript("document.getElementById('checkerRepository_directory').value = \"\";");
    }

    void WebPage::onFileUploadSelected(const QString& display_xslt, const QString& policy)
    {
        QStringList files = file_selector.value("checkerUpload[file]", QStringList());

        if (!files.size())
            return;

        QFileInfoList list;
        for (int i = 0; i < files.size(); ++i)
            list << QFileInfo(files[i]);

        if (display_xslt == "-1")
        {
            QStringList display_xslt_list = file_selector.value("checkerUpload[step1][display_xslt]", QStringList());
            if (display_xslt_list.length())
                mainwindow->add_xslt_display(display_xslt_list.last());
        }
        else
        {
            int index = display_xslt.toInt();
            if (index >= 0 && index < (int)mainwindow->get_displays().size())
                mainwindow->add_xslt_display(mainwindow->get_displays()[index]);
        }

        if (policy == "-1")
        {
            QStringList upload_list = file_selector.value("checkerUpload[step1][xslt]", QStringList());
            if (upload_list.length() && upload_list.last().length())
            {
                mainwindow->checker_add_policy_files(list, upload_list.last());
                return;
            }
        }

        mainwindow->checker_add_files(list, policy.toInt());
    }

    void WebPage::onFileUploadSelected()
    {
        runJavaScript("webpage.onFileUploadSelected($('#checkerUpload_step1_display_selector').val(), $('#checkerUpload_step1_policy').val());");
    }

    void WebPage::onFileOnlineSelected(const QString& url, const QString& display_xslt, const QString& policy)
    {
        if (!url.length())
            return;

        if (display_xslt == "-1")
        {
            QStringList display_xslt_list = file_selector.value("checkerOnline[step1][display_xslt]", QStringList());
            if (display_xslt_list.length())
                mainwindow->add_xslt_display(display_xslt_list.last());
        }
        else
        {
            int index = display_xslt.toInt();
            if (index >= 0 && index < (int)mainwindow->get_displays().size())
                mainwindow->add_xslt_display(mainwindow->get_displays()[index]);
        }
        if (policy == "-1")
        {
            QStringList upload_list = file_selector.value("checkerOnline[step1][xslt]", QStringList());
            if (upload_list.length() && upload_list.last().length())
            {
                mainwindow->checker_add_policy_file(url, upload_list.last());
                return;
            }
        }
        mainwindow->checker_add_file(url, policy.toInt());
    }

    void WebPage::onFileOnlineSelected()
    {
        runJavaScript("webpage.onFileOnlineSelected($('#checkerOnline_file').val(), "
                                                   "$('#checkerOnline_step1_display_selector').val(), "
                                                   "$('#checkerOnline_step1_policy').val());");
    }

    void WebPage::onFileRepositorySelected(const QString& display_xslt, const QString& policy)
    {
        QStringList dirname = file_selector.value("checkerRepository[directory]", QStringList());

        if (dirname.empty())
            return;

        QDir dir(dirname.last());

        QFileInfoList list = dir.entryInfoList(QDir::Files);
        if (!list.count())
            return;

        if (display_xslt == "-1")
        {
            QStringList display_xslt_list = file_selector.value("checkerRepository[step1][display_xslt]", QStringList());
            if (display_xslt_list.length())
                mainwindow->add_xslt_display(display_xslt_list.last());
        }
        else
        {
            int index = display_xslt.toInt();
            if (index >= 0 && index < (int)mainwindow->get_displays().size())
                mainwindow->add_xslt_display(mainwindow->get_displays()[index]);
        }

        if (policy == "-1")
        {
            QStringList upload_list = file_selector.value("checkerRepository[step1][xslt]", QStringList());
            if (upload_list.length() && upload_list.last().length())
            {
                mainwindow->checker_add_policy_files(list, upload_list.last());
                return;
            }
        }
        mainwindow->checker_add_files(list, policy.toInt());
    }

    void WebPage::onFileRepositorySelected()
    {
        runJavaScript("webpage.onFileRepositorySelected($('#checkerRepository_step1_display_selector').val(), "
                                                       "$('#checkerRepository_step1_policy').val());");
    }

    bool WebPage::acceptNavigationRequest(const QUrl& url, QWebEnginePage::NavigationType type,
                                          bool isMainFrame)
    {
        if (type == QWebEnginePage::NavigationTypeFormSubmitted)
            clean_forms();
        return false;
    }

    QStringList WebPage::chooseFiles(FileSelectionMode mode, const QStringList &oldFiles, const QStringList&)
    {
        QStringList list;
        if (mode == QWebEnginePage::FileSelectOpen)
        {
            QString value_input;
            QString suggested = oldFiles.first();
            if (select_file_name == "checkerRepository[directory]")
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

            if (it != file_selector.end())
                file_selector[select_file_name] << value_input;
            else
                file_selector.insert(select_file_name, QStringList(value_input));

            list = QStringList(value_input);
        }
        else
        {
            QStringList names = QFileDialog::getOpenFileNames(view(), QString::null);

            list = names;
            QMap<QString, QStringList>::iterator it = file_selector.find("checkerUpload[file]");
            if (it != file_selector.end())
                file_selector["checkerUpload[file]"] = names;
            else
                file_selector.insert("checkerUpload[file]", names);
        }

        return list;
    }

    void WebPage::changeLocalFiles(QStringList& files)
    {
        QString query;
        query += ("document.getElementById('collapseOnline').setAttribute('class', 'panel-collapse collapse'); ");
        query += ("document.getElementById('collapseOnline').setAttribute('aria-expanded', 'false'); ");
        query += ("document.getElementById('collapseOnline').setAttribute('style', '\"height: 20px\"'); ");

        query += ("document.getElementById('collapseRepository').setAttribute('class', 'panel-collapse collapse'); ");
        query += ("document.getElementById('collapseRepository').setAttribute('aria-expanded', 'false'); ");
        query += ("document.getElementById('collapseRepository').setAttribute('style', '\"height: 20px\"'); ");

        query += ("document.getElementById('collapseUpload').setAttribute('class', 'panel-collapse in'); ");
        query += ("document.getElementById('collapseUpload').setAttribute('aria-expanded', 'true'); ");
        query += ("document.getElementById('collapseUpload').setAttribute('style', ''); ");

        runJavaScript(query);
        QMap<QString, QStringList>::iterator it = file_selector.find("checkerUpload[file]");
        if (it != file_selector.end())
            file_selector["checkerUpload[file]"] << files;
        else
            file_selector.insert("checkerUpload[file]", files);

        onFileUploadSelected();
    }

    void WebPage::use_javascript(const QString& js)
    {
        runJavaScript(js);
    }
}

#endif
