/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "WebChoose.h"

#if defined(WEB_MACHINE_KIT)

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
        else if (!name.compare("Result"))
            mainwindow->result_selected();
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

    void WebPage::onFillImplementationReport(const QString& file, const QString& target)
    {
        std::string file_s = file.toStdString();
        QString report;
        mainwindow->get_implementation_report(file_s, report);
        QWebElement form = mainFrame()->findFirstElement(target + " .modal-body");

        if (report_is_html(report))
            form.setInnerXml(report);
        else
            form.setPlainText(report);
    }

    void WebPage::onFillPolicyReport(const QString& file, const QString& target)
    {
        std::string file_s = file.toStdString();
        QString report;
        mainwindow->validate_policy(file_s, report);
        QWebElement form = mainFrame()->findFirstElement(target + " .modal-body");

        if (report_is_html(report))
            form.setInnerXml(report);
        else
            form.setPlainText(report);
    }

    QString WebPage::onFillMediaInfoReport(const QString& file)
    {
        std::string file_s = file.toStdString();
        return mainwindow->get_mediainfo_jstree(file_s);
    }

    QString WebPage::onFillMediaTraceReport(const QString& file)
    {
        std::string file_s = file.toStdString();
        return mainwindow->get_mediatrace_jstree(file_s);
    }

    void WebPage::onDownloadReport(const QString& report, const QString& save_name)
    {
        if (report.isEmpty())
            return;

        QString proposed = save_name;
        bool is_html = report_is_html(report);
        bool is_xml = false;
        if (is_html)
            proposed.replace(proposed.length() - 3, 3, "html");
        else
        {
            is_xml = report_is_xml(report);
            if (is_xml)
                proposed.replace(proposed.length() - 3, 3, "xml");
            else
                proposed.replace(proposed.length() - 3, 3, "txt");
        }

        QString dl_file = QFileDialog::getSaveFileName(view(), "Save report", proposed);

        if (!dl_file.length())
            return;

        QFile file(dl_file);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        QTextStream out(&file);
        out << report;
    }

    void WebPage::onSaveImplementationReport(const QString& file, const QString& save_name)
    {
        std::string file_s = file.toStdString();
        QString report;
        mainwindow->get_implementation_report(file_s, report);
        onDownloadReport(report, save_name);
    }

    void WebPage::onSavePolicyReport(const QString& file, const QString& save_name)
    {
        std::string file_s = file.toStdString();
        QString report;
        mainwindow->validate_policy(file_s, report);
        onDownloadReport(report, save_name);
    }

    void WebPage::onSaveInfo(const QString& file, const QString& save_name)
    {
        std::string file_s = file.toStdString();
        std::string display_name, display_content;
        QString report = mainwindow->get_mediainfo_xml(file_s, display_name, display_content);
        onDownloadReport(report, save_name);
    }

    void WebPage::onSaveTrace(const QString& file, const QString& save_name)
    {
        std::string file_s = file.toStdString();
        std::string display_name, display_content;
        QString report = mainwindow->get_mediatrace_xml(file_s, display_name, display_content);
        onDownloadReport(report, save_name);
    }

    int WebPage::onFileUploadSelected(QWebElement form)
    {
        QStringList files = file_selector.value("checkerUpload[file]", QStringList());

        if (!files.size())
            return 1;

        QWebElement policyElement = form.findFirst("#checkerUpload_step1_policy");
        QString policy = policyElement.evaluateJavaScript("this.value").toString();
        QWebElement displayElement = form.findFirst("#checkerUpload_step1_display_selector");
        QString display_xslt = displayElement.evaluateJavaScript("this.value").toString();

        for (int i = 0; i < files.size(); ++i)
        {
            QFileInfo f = QFileInfo(files[i]);
            mainwindow->add_file_to_list(f.fileName(), f.absolutePath(), policy, display_xslt);
        }

        mainwindow->set_result_view();
        return 0;
    }

    int WebPage::onFileOnlineSelected(QWebElement form)
    {
        QWebElement urlElement = form.findFirst("#checkerOnline_file");
        QString url = urlElement.evaluateJavaScript("this.value").toString();

        if (!url.length())
            return 1;

        QWebElement policyElement = form.findFirst("#checkerOnline_step1_policy");
        QString policy = policyElement.evaluateJavaScript("this.value").toString();
        QWebElement displayElement = form.findFirst("#checkerOnline_step1_display_selector");
        QString display_xslt = displayElement.evaluateJavaScript("this.value").toString();

        mainwindow->add_file_to_list(url, "", policy, display_xslt);
        mainwindow->set_result_view();
        return 0;
    }

    int WebPage::onFileRepositorySelected(QWebElement form)
    {
        QStringList dirname = file_selector.value("checkerRepository[directory]", QStringList());

        if (dirname.empty())
            return 1;

        QDir dir(dirname.last());
        QFileInfoList list = dir.entryInfoList(QDir::Files);
        if (!list.count())
            return 1;

        QWebElement policyElement = form.findFirst("#checkerRepository_step1_policy");
        QString policy = policyElement.evaluateJavaScript("this.value").toString();
        QWebElement displayElement = form.findFirst("#checkerRepository_step1_display_selector");
        QString display_xslt = displayElement.evaluateJavaScript("this.value").toString();

        for (int i = 0; i < list.size(); ++i)
            mainwindow->add_file_to_list(list[i].fileName(), list[i].absolutePath(), policy, display_xslt);
        mainwindow->set_result_view();
        return 0;
    }

    void WebPage::close_all()
    {
        mainwindow->clear_file_list();
    }

    void WebPage::close_element(const QString& file)
    {
        mainwindow->remove_file_to_list(file);
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

    //---------------------------------------------------------------------------
    bool WebPage::report_is_html(const QString& report)
    {
        QRegExp reg("<\\!DOCTYPE.*html", Qt::CaseInsensitive);

        if (reg.indexIn(report, 0) != -1)
            return true;

        return false;
    }

    //---------------------------------------------------------------------------
    bool WebPage::report_is_xml(const QString& report)
    {
        QRegExp reg("<\\?xml ", Qt::CaseInsensitive);

        if (reg.indexIn(report, 0) != -1)
            return true;

        return false;
    }

    //---------------------------------------------------------------------------
    void WebPage::update_status_registered_file(MainWindow::FileRegistered* file)
    {
        set_analyzed_status(file);
        set_implementation_status(file);
        set_policy_status(file);
    }

    //---------------------------------------------------------------------------
    void WebPage::set_analyzed_status(MainWindow::FileRegistered* file)
    {
        QWebElement status = currentFrame()->findFirstElement(QString("#analyzeStatus%1").arg(file->index));
        QWebElement percent = currentFrame()->findFirstElement(QString("#analyzePercent%1").arg(file->index));
        QString percent_str = QString("%1%").arg(file->analyze_percent);
        if (file->analyzed)
        {
            status.setAttribute("class", "success");
            percent.setPlainText("Analyzed");
        }
        else
        {
            status.setAttribute("class", "info");
            percent.setPlainText(percent_str);
        }
    }

    //---------------------------------------------------------------------------
    void WebPage::set_implementation_status(MainWindow::FileRegistered* file)
    {
        QWebElement status = currentFrame()->findFirstElement(QString("#implementationStatus%1").arg(file->index));
        if (file->analyzed)
        {
            status.setAttribute("class", "success");
            QString html = status.toInnerXml();
            if (file->implementation_valid)
            {
                QString newHtml("<span class=\"glyphicon glyphicon-ok text-success\" aria-hidden=\"true\"></span> Valid");
                status.setInnerXml(newHtml + html);
            }
            else
            {
                QString newHtml("<span class=\"glyphicon glyphicon-remove\" aria-hidden=\"true\"></span> Not Valid");
                status.setInnerXml(newHtml + html);
            }
        }
        else
            status.setAttribute("class", "info");
    }

    //---------------------------------------------------------------------------
    void WebPage::set_policy_status(MainWindow::FileRegistered* file)
    {
        QString state("info");
        QWebElement status = currentFrame()->findFirstElement(QString("#policyStatus%1").arg(file->index));

        if (file->analyzed && file->policy != -1)
        {
            state = file->policy_valid ? "success" : "danger";
            status.setAttribute("class", state);
            QString html = status.toInnerXml();
            if (file->policy_valid)
            {
                QString newHtml("<span class=\"glyphicon glyphicon-ok text-success\" aria-hidden=\"true\"></span> ");
                status.setInnerXml(newHtml + html);
            }
            else
            {
                QString newHtml("<span class=\"glyphicon glyphicon-remove\" aria-hidden=\"true\"></span> ");
                status.setInnerXml(newHtml + html);
            }
        }
        else
            status.setAttribute("class", state);
    }

}

#endif
