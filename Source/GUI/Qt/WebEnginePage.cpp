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
#include "mainwindow.h"
#include "WebPage.h"

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
        connect(this, SIGNAL(update_registered_file(FileRegistered*)),
                this, SLOT(update_status_registered_file(FileRegistered*)));
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
        else if (!name.compare("Help"))
            mainwindow->on_actionGettingStarted_triggered();
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

    void WebPage::onFillImplementationReport(const QString& file, const QString& target, const QString& display, const QString& verbosity)
    {
        std::string file_s = file.toStdString();
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
        QString script = QString("$('%1 .modal-body')").arg(target);

        report = report.replace("\r", "");
        if (report_is_html(report))
        {
            report = report.replace("\\", "\\\\");
            report = report.replace("'", "\\'");
            report = report.replace("\n", "");
            script += QString(".html('%1');").arg(report);
        }
        else
        {
#if QT_VERSION >= 0x050200
            report = report.toHtmlEscaped();
#else
            report = Qt::escape(report);
#endif
            report = report.replace("\n", "<br/>");
            script += QString(".html('%1');").arg(report);
        }
        runJavaScript(script);
    }

    void WebPage::onFillPolicyReport(const QString& file, const QString& target, const QString& policy, const QString& display)
    {
        int policy_i = policy.toInt();
        QString report;
        if (policy_i != -1)
        {
            std::string file_s = file.toStdString();
            int display_i = display.toInt();
            mainwindow->validate_policy(file_s, report, policy_i, &display_i);
        }
        QString script = QString("$('%1 .modal-body')").arg(target);

        report = report.replace("\r", "");
        if (report_is_html(report))
        {
            report = report.replace("\\", "\\\\");
            report = report.replace("'", "\\'");
            report = report.replace("\n", "");
            script += QString(".html('%1');").arg(report);
        }
        else
        {
#if QT_VERSION >= 0x050200
            report = report.toHtmlEscaped();
#else
            report = Qt::escape(report);
#endif
            report = report.replace("\n", "<br/>");
            report = report.replace("'", "\\'");
            script += QString(".html('%1');").arg(report);
        }
        runJavaScript(script);
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

    void WebPage::onSaveImplementationReport(const QString& file, const QString& save_name, const QString& display, const QString& verbosity)
    {
        std::string file_s = file.toStdString();
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
        onDownloadReport(report, save_name);
    }

    void WebPage::onSavePolicyReport(const QString& file, const QString& save_name, const QString& policy, const QString& display)
    {
        std::string file_s = file.toStdString();
        QString report;
        int policy_i = policy.toInt();
        int display_i = display.toInt();
        mainwindow->validate_policy(file_s, report, policy_i, &display_i);
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

    void WebPage::clean_forms()
    {
        file_selector.clear();

        runJavaScript("document.getElementById('checkerUpload_step1_policy').value = -1;");
        runJavaScript("document.getElementById('checkerUpload_step1_xslt').value = '';");
        runJavaScript("document.getElementById('checkerUpload_step1_display_selector').value = -1;");
        runJavaScript("document.getElementById('checkerUpload_step1_display_xslt').value = '';");
        runJavaScript("document.getElementById('checkerUpload_file').value = \"\";");

#if defined(MEDIAINFO_LIBCURL_YES)

        runJavaScript("document.getElementById('checkerOnline_step1_policy').value = -1;");
        runJavaScript("document.getElementById('checkerOnline_step1_xslt').value = '';");
        runJavaScript("document.getElementById('checkerOnline_step1_display_selector').value = -1;");
        runJavaScript("document.getElementById('checkerOnline_step1_display_xslt').value = '';");
        runJavaScript("document.getElementById('checkerOnline_file').value = \"\";");

#endif

        runJavaScript("document.getElementById('checkerRepository_step1_policy').value = -1;");
        runJavaScript("document.getElementById('checkerRepository_step1_xslt').value = '';");
        runJavaScript("document.getElementById('checkerRepository_step1_display_selector').value = -1;");
        runJavaScript("document.getElementById('checkerRepository_step1_display_xslt').value = '';");
        runJavaScript("document.getElementById('checkerRepository_directory').value = \"\";");
    }

    void WebPage::onFileUploadSelected(const QString& display_xslt, const QString& policy, const QString& verbosity)
    {
        QStringList files = file_selector.value("checkerUpload[file]", QStringList());

        if (!files.size())
            return;

        for (int i = 0; i < files.size(); ++i)
        {
            QFileInfo f = QFileInfo(files[i]);
            mainwindow->add_file_to_list(f.fileName(), f.absolutePath(), policy, display_xslt, verbosity);
        }
        mainwindow->set_result_view();
    }

    void WebPage::onFileUploadSelected()
    {
        runJavaScript("webpage.onFileUploadSelected($('#checkerUpload_step1_display_selector').val(), $('#checkerUpload_step1_policy').val(),"
                      " $('#checkerUpload_step1_verbosity_selector').val());");
    }

    void WebPage::onFileOnlineSelected(const QString& url, const QString& display_xslt, const QString& policy, const QString& verbosity)
    {
        if (!url.length())
            return;

        mainwindow->add_file_to_list(url, "", policy, display_xslt, verbosity);
        mainwindow->set_result_view();
    }

    void WebPage::onFileOnlineSelected()
    {
        runJavaScript("webpage.onFileOnlineSelected($('#checkerOnline_file').val(), "
                                                   "$('#checkerOnline_step1_display_selector').val(), "
                                                   "$('#checkerOnline_step1_policy').val(), "
                                                   "$('#checkerOnline_step1_verbosity_selector').val());");
    }

    void WebPage::onFileRepositorySelected(const QString& display_xslt, const QString& policy, const QString& verbosity)
    {
        QStringList dirname = file_selector.value("checkerRepository[directory]", QStringList());
        if (dirname.empty())
            return;

        QDir dir(dirname.last());
        QFileInfoList list = dir.entryInfoList(QDir::Files);
        if (!list.count())
            return;

        for (int i = 0; i < list.size(); ++i)
            mainwindow->add_file_to_list(list[i].fileName(), list[i].absolutePath(), policy, display_xslt, verbosity);
        mainwindow->set_result_view();
    }

    void WebPage::onFileRepositorySelected()
    {
        runJavaScript("webpage.onFileRepositorySelected($('#checkerRepository_step1_display_selector').val(), "
                                                       "$('#checkerRepository_step1_policy').val(), "
                                                       "$('#checkerRepository_step1_verbosity_selector').val());");
    }

    void WebPage::close_all()
    {
        mainwindow->clear_file_list();
    }

    void WebPage::close_element(const QString& file)
    {
        mainwindow->remove_file_to_list(file);
    }

    bool WebPage::acceptNavigationRequest(const QUrl&, QWebEnginePage::NavigationType,
                                          bool)
    {
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
    void WebPage::update_status_registered_file(FileRegistered* file)
    {
        set_analyzed_status(file);
        set_implementation_status(file);
        set_policy_status(file);
    }

    //---------------------------------------------------------------------------
    void WebPage::emit_update_registered_file(FileRegistered* file)
    {
        Q_EMIT update_registered_file(file);
    }

    //---------------------------------------------------------------------------
    void WebPage::set_analyzed_status(FileRegistered* file)
    {
        QString percent_str;
        percent_str = QString("%1").arg(file->analyze_percent == 0 ? "In queue" : "Analyzing");
        QString script = QString("$('#analyzeStatus%1').prop('class', '%2');").arg(file->index).arg(file->analyzed ? "success" : "info");
        script += QString("$('#analyzePercent%1').text('%2');").arg(file->index).arg(file->analyzed ? "Analyzed" : percent_str);
        use_javascript(script);
    }

    //---------------------------------------------------------------------------
    void WebPage::set_implementation_status(FileRegistered* file)
    {
        QString status("info");
        if (file->analyzed)
            status = file->implementation_valid ? "success" : "danger";
        QString script = QString("$('#implementationStatus%1').prop('class', '%2');").arg(file->index).arg(status);
        if (file->analyzed && file->implementation_valid)
            script += QString("var old = $('#implementationStatus%1').html(); $('#implementationStatus%1').html('<span class=\"glyphicon glyphicon-ok text-success\" aria-hidden=\"true\"></span> Valid' + old);").arg(file->index);
        else if (file->analyzed && !file->implementation_valid)
            script += QString("var old = $('#implementationStatus%1').html(); $('#implementationStatus%1').html('<span class=\"glyphicon glyphicon-remove\" aria-hidden=\"true\"></span> Not Valid' + old);").arg(file->index);

        use_javascript(script);
    }

    //---------------------------------------------------------------------------
    void WebPage::set_policy_status(FileRegistered* file)
    {
        QString status("info");
        if (file->analyzed && file->policy != -1)
            status = file->policy_valid ? "success" : "danger";

        QString script = QString("$('#policyStatus%1').prop('class', '%2');").arg(file->index).arg(status);
        if (file->analyzed && file->policy != -1 && file->policy_valid)
            script += QString("var old = $('#policyStatus%1').html(); $('#policyStatus%1').html('<span class=\"glyphicon glyphicon-ok text-success\" aria-hidden=\"true\"></span> ' + old);").arg(file->index);
        else if (file->analyzed && file->policy != -1 && !file->policy_valid)
            script += QString("var old = $('#policyStatus%1').html(); $('#policyStatus%1').html('<span class=\"glyphicon glyphicon-remove\" aria-hidden=\"true\"></span> ' + old);").arg(file->index);

        use_javascript(script);
    }

}

#endif
