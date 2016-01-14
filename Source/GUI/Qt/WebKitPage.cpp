/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>
#include <QWebElementCollection>
#include <QFileDialog>
#include <QFile>
#include <QTextDocument>
#include <QTextStream>

#include "mainwindow.h"
#include "WebPage.h"
#include <QtGlobal>

namespace MediaConch
{
    WebPage::WebPage(MainWindow *m, QWidget *parent) : QWebPage(parent), mainwindow(m)
    {
        connect(this, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
    }

    void WebPage::onLoadFinished(bool ok)
    {
        if (!ok)
            return;

        QWebFrame* frame = mainFrame();
        frame->addToJavaScriptWindowObject("webpage", this);
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
        QWebElement button = currentFrame()->documentElement().findFirst(QString("button[id=\"%1\"]").arg(id));
        QString form_id = button.parent().parent().attribute("id");
        if (form_id == "checkerUpload")
            onFileUploadSelected(button.parent().parent());
        else if (form_id == "checkerOnline")
            onFileOnlineSelected(button.parent().parent());
        else if (form_id == "checkerRepository")
            onFileRepositorySelected(button.parent().parent());
    }

    void WebPage::onDownloadReport(const QString& target, const QString& save_name)
    {
        QWebFrame* frame = mainFrame();
        QWebElement reportDiv = frame->findFirstElement(target);

        QWebElement report = reportDiv.findFirst(".modal-body");
        if (report.isNull())
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
            out << report.toInnerXml();
        }
        else
        {
            QTextDocument text(report.toPlainText().trimmed());
            out << text.toPlainText() << "\n";
        }
    }

    void WebPage::onSaveInfo(const QString& target, const QString& save_name)
    {
        onDownloadReport(target, save_name);
    }

    void WebPage::onSaveTrace(const QString& target, const QString& save_name)
    {
        onDownloadReport(target, save_name);
    }

    void WebPage::onFileUploadSelected(QWebElement form)
    {
        QWebElement policyElement = form.findFirst("#checkerUpload_step1_policy");
        QString policy = policyElement.evaluateJavaScript("this.value").toString();
        QStringList files = file_selector.value("checkerUpload[file]", QStringList());

        if (!files.size())
            return;

        QFileInfoList list;
        for (int i = 0; i < files.size(); ++i)
            list << QFileInfo(files[i]);

        QWebElement displayElement = form.findFirst("#checkerUpload_step1_display_selector");
        QString display_xslt = displayElement.evaluateJavaScript("this.value").toString();

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

    void WebPage::onFileOnlineSelected(QWebElement form)
    {
        QWebElement policyElement = form.findFirst("#checkerOnline_step1_policy");
        QString policy = policyElement.evaluateJavaScript("this.value").toString();
        QWebElement urlElement = form.findFirst("#checkerOnline_file");
        QString url = urlElement.evaluateJavaScript("this.value").toString();

        if (!url.length())
            return;

        QWebElement displayElement = form.findFirst("#checkerOnline_step1_display_selector");
        QString display_xslt = displayElement.evaluateJavaScript("this.value").toString();

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

    void WebPage::onFileRepositorySelected(QWebElement form)
    {
        QWebElement policyElement = form.findFirst("#checkerRepository_step1_policy");
        QString policy = policyElement.evaluateJavaScript("this.value").toString();
        QStringList dirname = file_selector.value("checkerRepository[directory]", QStringList());

        if (dirname.empty())
            return;

        QDir dir(dirname.last());

        QFileInfoList list = dir.entryInfoList(QDir::Files);
        if (!list.count())
            return;

        QWebElement displayElement = form.findFirst("#checkerRepository_step1_display_selector");
        QString display_xslt = displayElement.evaluateJavaScript("this.value").toString();

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

    bool WebPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest& request,
                                          QWebPage::NavigationType type)
    {
        if (type == QWebPage::NavigationTypeFormSubmitted || type == QWebPage::NavigationTypeFormResubmitted)
            return false;
        return QWebPage::acceptNavigationRequest(frame, request, type);
    }

    QString WebPage::chooseFile(QWebFrame *, const QString& suggested)
    {
        QString value_input;
        if (select_file_name == "checkerRepository[directory]")
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
        if (it != file_selector.end())
            file_selector[select_file_name] << value_input;
        else
            file_selector.insert(select_file_name, QStringList(value_input));

        return value_input;
    }

    bool WebPage::extension(Extension extension, const ExtensionOption *option,
                            ExtensionReturn *output)
    {
        if (extension == QWebPage::ChooseMultipleFilesExtension)
        {
            QStringList suggested = ((const ChooseMultipleFilesExtensionOption*)option)->suggestedFileNames;
            QStringList names = QFileDialog::getOpenFileNames(view(), QString::null);
            ((ChooseMultipleFilesExtensionReturn*)output)->fileNames = names;
            QMap<QString, QStringList>::iterator it = file_selector.find("checkerUpload[file]");
            if (it != file_selector.end())
                file_selector["checkerUpload[file]"] << names;
            else
                file_selector.insert("checkerUpload[file]", names);
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
    
    void WebPage::changeLocalFiles(QStringList& files)
    {
        QWebFrame* frame = mainFrame();

        QWebElement form = frame->findFirstElement("#collapseOnline");
        if (!form.isNull())
        {
            form.setAttribute("class", "panel-collapse collapse");
            form.setAttribute("aria-expanded", "false");
            form.setAttribute("style", "\"height: 20px\"");
        }
        form = frame->findFirstElement("#collapseRepository");
        if (!form.isNull())
        {
            form.setAttribute("aria-expanded", "false");
            form.setAttribute("class", "panel-collapse collapse");
            form.setAttribute("style", "\"height: 20px\"");
        }

        form = frame->findFirstElement("#collapseUpload");
        if (form.isNull())
            return;
        form.setAttribute("aria-expanded", "true");
        form.setAttribute("class", "panel-collapse in");
        form.setAttribute("style", "");

        QWebElement input = form.findFirst("#checkerUpload_file");
        if (input.isNull())
            return;

        QString file = files[0];
        input.setAttribute("value", file);
        QMap<QString, QStringList>::iterator it = file_selector.find("checkerUpload[file]");
        if (it != file_selector.end())
            file_selector["checkerUpload[file]"] << files;
        else
            file_selector.insert("checkerUpload[file]", files);

        onFileUploadSelected(form);
    }

    void WebPage::use_javascript(const QString& js)
    {
        mainFrame()->evaluateJavaScript(js);
    }
}
