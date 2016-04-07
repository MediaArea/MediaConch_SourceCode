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

#include "Common/FileRegistered.h"
#include "mainwindow.h"
#include "WebPage.h"
#include <QtGlobal>
#include <QDebug>

namespace MediaConch
{
    WebPage::WebPage(MainWindow *m, QWidget *parent) : QWebPage(parent), mainwindow(m)
    {
        connect(this, SIGNAL(loadFinished(bool)), this, SLOT(on_load_finished(bool)));
    }

    void WebPage::on_load_finished(bool ok)
    {
        if (!ok)
            return;

        mainFrame()->addToJavaScriptWindowObject("webpage", this);
    }

    void WebPage::menu_link_checker(const QString& name)
    {
        if (!name.compare("Checker"))
            mainwindow->checker_selected();
        else if (!name.compare("Policies"))
            mainwindow->policies_selected();
        else if (!name.compare("Display"))
            mainwindow->display_selected();
        else if (!name.compare("Help"))
            mainwindow->on_actionGettingStarted_triggered();
        else
            mainwindow->checker_selected();
    }

    void WebPage::on_input_changed(const QString& inputName)
    {
        select_file_name = inputName;
    }

    void WebPage::on_download_report(const QString& report, const QString& filename, const QString& report_name)
    {
        if (report.isEmpty())
            return;

        QString proposed = filename + "_" + report_name + ".txt";
        bool is_html = report_is_html(report);

        if (is_html)
            proposed.replace(proposed.length() - 3, 3, "html");
        else
        {
            bool is_xml = report_is_xml(report);
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

    QString WebPage::on_fill_implementation_report(const QString& file, const QString& display, const QString& verbosity)
    {
        std::string file_s = std::string(file.toUtf8().data(), file.toUtf8().length());
        QString report;
        int display_i = display.toInt();
        int *verbosity_p = NULL;
        int verbosity_i;
        if (verbosity.length())
        {
            verbosity_i = verbosity.toInt();
            verbosity_p = &verbosity_i;
        }

        mainwindow->get_implementation_report(file_s, report, &display_i, verbosity_p);
        return report;
    }

    void WebPage::on_save_implementation_report(const QString& file, const QString& display, const QString& verbosity)
    {
        std::string file_s = std::string(file.toUtf8().data(), file.toUtf8().length());
        QString report;
        int display_i = display.toInt();
        int *verbosity_p = NULL;
        int verbosity_i;
        if (verbosity.length())
        {
            verbosity_i = verbosity.toInt();
            verbosity_p = &verbosity_i;
        }
        mainwindow->get_implementation_report(file_s, report, &display_i, verbosity_p);
        on_download_report(report, file, "ImplementationReport");
    }

    QString WebPage::on_fill_policy_report(const QString& file, const QString& policy, const QString& display)
    {
        int policy_i = policy.toInt();
        QString report;
        if (policy_i != -1)
        {
            std::string file_s = std::string(file.toUtf8().data(), file.toUtf8().length());
            int display_i = display.toInt();
            mainwindow->validate_policy(file_s, report, policy_i, &display_i);
        }
        return report;
    }

    void WebPage::on_save_policy_report(const QString& file, const QString& policy, const QString& display)
    {
        std::string file_s = std::string(file.toUtf8().data(), file.toUtf8().length());
        QString report;
        int policy_i = policy.toInt();
        int display_i = display.toInt();
        mainwindow->validate_policy(file_s, report, policy_i, &display_i);
        on_download_report(report, file, "MediaConchReport");
    }

    QString WebPage::on_fill_mediainfo_report(const QString& file)
    {
        std::string file_s = std::string(file.toUtf8().data(), file.toUtf8().length());
        return mainwindow->get_mediainfo_jstree(file_s);
    }

    void WebPage::on_save_mediainfo_report(const QString& file)
    {
        std::string file_s = std::string(file.toUtf8().data(), file.toUtf8().length());
        std::string display_name, display_content;
        QString report = mainwindow->get_mediainfo_xml(file_s, display_name, display_content);
        on_download_report(report, file, "MediaInfo");
    }

    QString WebPage::on_fill_mediatrace_report(const QString& file)
    {
        std::string file_s = std::string(file.toUtf8().data(), file.toUtf8().length());
        return mainwindow->get_mediatrace_jstree(file_s);
    }

    void WebPage::on_save_mediatrace_report(const QString& file)
    {
        std::string file_s = std::string(file.toUtf8().data(), file.toUtf8().length());
        std::string display_name, display_content;
        QString report = mainwindow->get_mediatrace_xml(file_s, display_name, display_content);
        on_download_report(report, file, "MediaTrace");
    }

    void WebPage::on_file_upload_selected(const QString& policy, const QString& display_xslt,
                                          const QString& verbosity)
    {
        QStringList files = file_selector.value("checkerUpload_file", QStringList());
        if (!files.size())
            return;

        for (int i = 0; i < files.size(); ++i)
        {
            QFileInfo f = QFileInfo(files[i]);
            mainwindow->add_file_to_list(f.fileName(), f.absolutePath(), policy, display_xslt, verbosity);
        }
        clean_forms();
        file_selector.clear();
    }

    void WebPage::on_file_online_selected(const QString& url, const QString& policy, const QString& display_xslt,
                                          const QString& verbosity)
    {
        if (!url.length())
            return;

        mainwindow->add_file_to_list(url, "", policy, display_xslt, verbosity);
        clean_forms();
        file_selector.clear();
    }

    void WebPage::on_file_repository_selected(const QString& policy, const QString& display_xslt,
                                              const QString& verbosity)
    {
        QStringList dirname = file_selector.value("checkerRepository_directory", QStringList());
        if (dirname.empty())
            return;

        QDir dir(dirname.last());
        QFileInfoList list = dir.entryInfoList(QDir::Files);
        if (!list.count())
            return;

        for (int i = 0; i < list.size(); ++i)
            mainwindow->add_file_to_list(list[i].fileName(), list[i].absolutePath(), policy, display_xslt, verbosity);
        clean_forms();
        file_selector.clear();
    }

    void WebPage::clean_forms()
    {
        file_selector.clear();

        use_javascript("document.getElementById('checkerUpload_policy').value = -1;");
        use_javascript("document.getElementById('checkerUpload_display_selector').value = -1;");
        use_javascript("document.getElementById('checkerUpload_verbosity_selector').value = -1;");
        use_javascript("document.getElementById('checkerUpload_file').value = \"\";");

#if defined(MEDIAINFO_LIBCURL_YES)

        use_javascript("document.getElementById('checkerOnline_policy').value = -1;");
        use_javascript("document.getElementById('checkerOnline_display_selector').value = -1;");
        use_javascript("document.getElementById('checkerOnline_verbosity_selector').value = -1;");
        use_javascript("document.getElementById('checkerOnline_file').value = \"\";");

#endif

        use_javascript("document.getElementById('checkerRepository_policy').value = -1;");
        use_javascript("document.getElementById('checkerRepository_display_selector').value = -1;");
        use_javascript("document.getElementById('checkerRepository_verbosity_selector').value = -1;");
        use_javascript("document.getElementById('checkerRepository_directory').value = \"\";");
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
            QMap<QString, QStringList>::iterator it = file_selector.find("checkerUpload_file");
            if (it != file_selector.end())
                file_selector["checkerUpload_file"] << names;
            else
                file_selector.insert("checkerUpload_file", names);
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

    void WebPage::change_local_files(QStringList& files)
    {
        QWebFrame* frame = mainFrame();
        QString active(" active");

        QWebElement form = frame->findFirstElement("#url");
        if (!form.isNull())
        {
            QString attr = form.attribute("class");
            int pos = -1;
            if ((pos = attr.indexOf(active)) != -1)
            {
                attr.replace(pos, active.length(), "");
                form.setAttribute("class", attr);
            }
        }

        form = frame->findFirstElement("#repository");
        if (!form.isNull())
        {
            QString attr = form.attribute("class");
            int pos = -1;
            if ((pos = attr.indexOf(active)) != -1)
            {
                attr.replace(pos, active.length(), "");
                form.setAttribute("class", attr);
            }
        }

        form = frame->findFirstElement("#file");
        if (form.isNull())
            return;

        QString attr = form.attribute("class");
        if (attr.indexOf(active) == -1)
            form.setAttribute("class", attr + active);

        QWebElement input = form.findFirst("#checkerUpload_file");
        if (input.isNull())
            return;

        QString file = files[0];
        input.setAttribute("value", file);
        QMap<QString, QStringList>::iterator it = file_selector.find("checkerUpload_file");
        if (it != file_selector.end())
            file_selector["checkerUpload_file"] << files;
        else
            file_selector.insert("checkerUpload_file", files);

        on_file_upload_selected("-1", "-1", "-1");
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

    QString WebPage::get_file_tool(const QString& file)
    {
        FileRegistered* fr = mainwindow->get_file_registered_from_file(file.toUtf8().data());
        if (!fr)
            return "-1";

        int report_kind = fr->report_kind;
        delete fr;
        return QString().setNum(report_kind);
    }

    QString WebPage::get_file_policy_id(const QString& file)
    {
        FileRegistered* fr = mainwindow->get_file_registered_from_file(file.toUtf8().data());
        if (!fr)
            return "-1";

        int policy = fr->policy;
        delete fr;
        return QString().setNum(policy);
    }

    QString WebPage::get_file_display_id(const QString& file)
    {
        FileRegistered* fr = mainwindow->get_file_registered_from_file(file.toUtf8().data());
        if (!fr)
            return "-1";

        int display = fr->display;
        delete fr;
        return QString().setNum(display);
    }

    QString WebPage::get_file_verbosity_id(const QString& file)
    {
        FileRegistered* fr = mainwindow->get_file_registered_from_file(file.toUtf8().data());
        if (!fr)
            return "-1";

        int verbosity = fr->verbosity;
        delete fr;
        return QString().setNum(verbosity);
    }

    bool WebPage::policy_is_valid(const QString& file)
    {
        FileRegistered* fr = mainwindow->get_file_registered_from_file(file.toUtf8().data());
        if (!fr)
            return false;

        if (!fr->analyzed)
        {
            delete fr;
            return false;
        }

        bool policy_valid = fr->policy_valid;
        delete fr;

        return policy_valid;
    }

    bool WebPage::implementation_is_valid(const QString& file)
    {
        FileRegistered* fr = mainwindow->get_file_registered_from_file(file.toUtf8().data());
        if (!fr)
            return false;

        if (!fr->analyzed)
        {
            delete fr;
            return false;
        }

        bool implementation_valid = fr->implementation_valid;
        delete fr;

        return implementation_valid;
    }

    bool WebPage::file_is_analyzed(const QString& file)
    {
        FileRegistered* fr = mainwindow->get_file_registered_from_file(file.toUtf8().data());
        if (!fr)
            return false;

        bool analyzed = fr->analyzed;
        delete fr;

        return analyzed;
    }

}

#endif
