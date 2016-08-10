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
#include "Common/XsltPolicy.h"
#include "mainwindow.h"
#include "WebPage.h"
#include <QtGlobal>

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
        else if (!name.compare("Settings"))
            mainwindow->settings_selected();
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

        std::string proposed_path_str = mainwindow->select_correct_save_report_path();
        QDir proposed_dir(QString().fromUtf8(proposed_path_str.c_str(), proposed_path_str.length()));
        QString proposed_filename = QFileInfo(filename).fileName() + "_" + report_name + ".txt";

        QString proposed = proposed_dir.absoluteFilePath(proposed_filename);
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

        QDir info(QFileInfo(dl_file).absoluteDir());
        mainwindow->set_last_save_report_path(info.absolutePath().toUtf8().data());

        QFile file(dl_file);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        QTextStream out(&file);
        out.setCodec("UTF-8");
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

    QString WebPage::on_create_policy_from_file(const QString& file)
    {
        int id = mainwindow->xslt_policy_create_from_file(file);
        std::string n, err;
        mainwindow->policy_get_name(id, n, err);;
        QString name = QString().fromUtf8(n.c_str(), n.length());

        QString script = QString("{\"policyId\":%1, \"policyName\":\"%2\"}")
                             .arg(id)
                             .arg(name);
        return script;
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
        use_javascript(QString("startWaitingLoop()"));
    }

    void WebPage::on_file_online_selected(const QString& url, const QString& policy, const QString& display_xslt,
                                          const QString& verbosity)
    {
        if (!url.length())
            return;

        mainwindow->add_file_to_list(url, "", policy, display_xslt, verbosity);
        clean_forms();
        file_selector.clear();
        use_javascript(QString("startWaitingLoop()"));
    }

    void WebPage::on_file_repository_selected(const QString& policy, const QString& display_xslt,
                                              const QString& verbosity)
    {
        QStringList dirname = file_selector.value("checkerRepository_directory", QStringList());
        if (dirname.empty())
            return;

        QDir dir(dirname.last());
        QFileInfoList list;
        add_sub_directory_files_to_list(dir, list);
        if (!list.count())
            return;

        for (int i = 0; i < list.size(); ++i)
            mainwindow->add_file_to_list(list[i].fileName(), list[i].absolutePath(), policy, display_xslt, verbosity);
        clean_forms();
        file_selector.clear();
        use_javascript(QString("startWaitingLoop()"));
    }

    void WebPage::on_save_settings_selected(const QString& policy,
                                            const QString& display,
                                            const QString& verbosity,
                                            const QString& save_report_path, const QString& load_files_path,
                                            const QString& save_policy_path, const QString& load_policy_path,
                                            const QString& save_display_path, const QString& load_display_path)
    {
        UiSettings& settings = mainwindow->get_settings();

        //policy
        if (policy == "-2")
            settings.change_default_policy("last");
        else
            settings.change_default_policy(policy.toUtf8().data());

        //display
        if (display == "-1")
            settings.change_default_display("last");
        else
        {
            std::vector<QString>& displays = mainwindow->get_displays();
            int display_i = display.toInt();
            if (display_i >= 0 && (size_t)display_i < displays.size())
                settings.change_default_display(displays[display_i].toUtf8().data());
            else
                settings.change_default_display("");
        }

        //verbosity
        settings.change_default_verbosity(verbosity.toInt());

        //Save report path
        std::string r_s_path;
        if (save_report_path == "last")
            r_s_path = "last";
        else
        {
            QStringList list = file_selector["settings_save_report_path_input"];
            if (list.size())
                r_s_path = list[list.size() - 1].toUtf8().data();
        }
        if (r_s_path.length())
        {
            settings.change_default_save_report_path(r_s_path);
            use_javascript(QString("set_save_report_selected('%1');")
                           .arg(QString().fromUtf8(settings.get_default_save_report_path().c_str())));
        }
        use_javascript("document.getElementById('settings_save_report_path_input').value = \"\";");

        //Load files path
        std::string f_l_path;
        if (load_files_path == "last")
            f_l_path = "last";
        else
        {
            QStringList list = file_selector["settings_load_files_path_input"];
            if (list.size())
                f_l_path = list[list.size() - 1].toUtf8().data();
        }
        if (f_l_path.length())
        {
            settings.change_default_load_files_path(f_l_path);
            use_javascript(QString("set_load_files_selected('%1');")
                           .arg(QString().fromUtf8(settings.get_default_load_files_path().c_str())));
        }
        use_javascript("document.getElementById('settings_load_files_path_input').value = \"\";");

        //Save policy path
        std::string p_s_path;
        if (save_policy_path == "last")
            p_s_path = "last";
        else
        {
            QStringList list = file_selector["settings_save_policy_path_input"];
            if (list.size())
                p_s_path = list[list.size() - 1].toUtf8().data();
        }
        if (p_s_path.length())
        {
            settings.change_default_save_policy_path(p_s_path);
            use_javascript(QString("set_save_policy_selected('%1');")
                           .arg(QString().fromUtf8(p_s_path.c_str(), p_s_path.length())));
        }
        use_javascript("document.getElementById('settings_save_policy_path_input').value = \"\";");

        //Load policy path
        std::string p_l_path;
        if (load_policy_path == "last")
            p_l_path = "last";
        else
        {
            QStringList list = file_selector["settings_load_policy_path_input"];
            if (list.size())
                p_l_path = list[list.size() - 1].toUtf8().data();
        }
        if (p_l_path.length())
        {
            settings.change_default_load_policy_path(p_l_path);
            use_javascript(QString("set_load_policy_selected('%1');")
                           .arg(QString().fromUtf8(p_l_path.c_str(), p_l_path.length())));
        }
        use_javascript("document.getElementById('settings_load_policy_path_input').value = \"\";");

        //Save display path
        std::string d_s_path;
        if (save_display_path == "last")
            d_s_path = "last";
        else
        {
            QStringList list = file_selector["settings_save_display_path_input"];
            if (list.size())
                d_s_path = list[list.size() - 1].toUtf8().data();
        }
        if (d_s_path.length())
        {
            settings.change_default_save_display_path(d_s_path);
            use_javascript(QString("set_save_display_selected('%1');")
                           .arg(QString().fromUtf8(d_s_path.c_str(), d_s_path.length())));
        }
        use_javascript("document.getElementById('settings_save_display_path_input').value = \"\";");

        //Load display path
        std::string d_l_path;
        if (load_display_path == "last")
            d_l_path = "last";
        else
        {
            QStringList list = file_selector["settings_load_display_path_input"];
            if (list.size())
                d_l_path = list[list.size() - 1].toUtf8().data();
        }
        if (d_l_path.length())
        {
            settings.change_default_load_display_path(d_l_path);
            use_javascript(QString("set_load_display_selected('%1');")
                           .arg(QString().fromUtf8(d_l_path.c_str(), d_l_path.length())));
        }
        use_javascript("document.getElementById('settings_load_display_path_input').value = \"\";");
        file_selector.clear();
    }

    void WebPage::clean_forms()
    {
        file_selector.clear();

        int policy_i = mainwindow->select_correct_policy();
        int display_i = mainwindow->select_correct_display();
        int verbosity_i = mainwindow->select_correct_verbosity();

        use_javascript(QString("document.getElementById('checkerUpload_policy').value = %1;").arg(policy_i));
        use_javascript(QString("document.getElementById('checkerUpload_display_selector').value = %1;").arg(display_i));
        use_javascript(QString("document.getElementById('checkerUpload_verbosity_selector').value = %1;").arg(verbosity_i));
        use_javascript("document.getElementById('checkerUpload_file').value = \"\";");

#if defined(MEDIAINFO_LIBCURL_YES)

        use_javascript(QString("document.getElementById('checkerOnline_policy').value = %1;").arg(policy_i));
        use_javascript(QString("document.getElementById('checkerOnline_display_selector').value = %1;").arg(display_i));
        use_javascript(QString("document.getElementById('checkerOnline_verbosity_selector').value = %1;").arg(verbosity_i));
        use_javascript("document.getElementById('checkerOnline_file').value = \"\";");

#endif

        use_javascript(QString("document.getElementById('checkerRepository_policy').value = %1;").arg(policy_i));
        use_javascript(QString("document.getElementById('checkerRepository_display_selector').value = %1;").arg(display_i));
        use_javascript(QString("document.getElementById('checkerRepository_verbosity_selector').value = %1;").arg(verbosity_i));
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

    QString WebPage::choose_file_settings()
    {
        std::string suggested_str;

        if (select_file_name == "settings_save_report_path_input")
            suggested_str = mainwindow->get_settings().get_default_save_report_path();
        else if (select_file_name == "settings_load_files_path_input")
            suggested_str = mainwindow->get_settings().get_default_load_files_path();
        else if (select_file_name == "settings_save_policy_path_input")
            suggested_str = mainwindow->get_settings().get_default_save_policy_path();
        else if (select_file_name == "settings_load_policy_path_input")
            suggested_str = mainwindow->get_settings().get_default_load_policy_path();
        else if (select_file_name == "settings_save_display_path_input")
            suggested_str = mainwindow->get_settings().get_default_save_display_path();
        else if (select_file_name == "settings_load_display_path_input")
            suggested_str = mainwindow->get_settings().get_default_load_display_path();
        else
            return QString();

        QString suggested = QString().fromUtf8(suggested_str.c_str(), suggested_str.length());
        QString value_input = QFileDialog::getExistingDirectory(view(), NULL, suggested);

        QMap<QString, QStringList>::iterator it = file_selector.find(select_file_name);
        if (it != file_selector.end())
            file_selector.erase(it);

        if (!value_input.length())
            return QString();

        file_selector.insert(select_file_name, QStringList(value_input));

        return value_input;
    }

    QString WebPage::chooseFile(QWebFrame *, const QString&)
    {
        if (select_file_name.startsWith("settings_"))
            return choose_file_settings();

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

    void WebPage::charge_local_dir(const QString& path, QStringList& tmp)
    {
        QFileInfo info(path);
        if (info.isFile())
        {
            tmp << path;
            return;
        }

        QDir dir(path);
        QFileInfoList list;
        add_sub_directory_files_to_list(dir, list);
        for (int i = 0; i < list.size(); ++i)
            tmp << list[i].absoluteFilePath();
    }

    void WebPage::change_local_files(const QStringList& files)
    {
        QStringList tmp;
        for (int i = 0; i < files.size(); ++i)
            charge_local_dir(files[i], tmp);

        QMap<QString, QStringList>::iterator it = file_selector.find("checkerUpload_file");
        if (it != file_selector.end())
            file_selector["checkerUpload_file"] << tmp;
        else
            file_selector.insert("checkerUpload_file", tmp);

        if (files.size() > 0)
        {
            QDir info(QFileInfo(files[0]).absoluteDir());
            mainwindow->set_last_load_files_path(info.absolutePath().toUtf8().data());
        }

        on_file_upload_selected(QString().setNum(mainwindow->select_correct_policy()),
                                QString().setNum(mainwindow->select_correct_display()),
                                QString().setNum(mainwindow->select_correct_verbosity()));
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

    QString WebPage::file_is_analyzed(const QString& file)
    {
        FileRegistered* fr = mainwindow->get_file_registered_from_file(file.toUtf8().data());
        if (!fr)
            return QString();

        int report_kind = fr->report_kind;

        if (report_kind >= 0 && report_kind <= 2)
            report_kind = 2;

        QString analyzed = QString("{\"finish\":%1,\"tool\":%2,\"percent\":%3}")
            .arg(fr->analyzed ? "true" : "false") // finish
            .arg(report_kind)                     // tool
            .arg(fr->analyze_percent);            // percent
        delete fr;

        return analyzed;
    }

    QString WebPage::change_policy_for_file(const QString& filename, const QString& policy, const QString& fileId)
    {
        mainwindow->update_policy_of_file_in_list(filename, policy);
        return fileId;
    }

    void WebPage::add_sub_directory_files_to_list(const QDir dir, QFileInfoList& list)
    {
        QFileInfoList tmp = dir.entryInfoList(QDir::Files);
        list << tmp;

        tmp = dir.entryInfoList(QDir::Dirs | QDir::NoDot | QDir::NoDotDot);
        for (int i = 0; i < tmp.size(); ++i)
        {
            QDir tmp_dir(tmp[i].absoluteFilePath());
            add_sub_directory_files_to_list(tmp_dir, list);
        }
    }

    QString WebPage::display_add_file(const QString& name)
    {
        QStringList files = file_selector.value("displayImport_displayFile", QStringList());
        if (files.empty())
            return "";

        QString filename = files.last();
        int id = mainwindow->display_add_file(name, filename);
        if (id < 0)
            return "";

        file_selector.clear();
        std::vector<QString>& displays = mainwindow->get_displays();
        QFileInfo file(displays[id]);
        return file.completeBaseName();
    }

    void WebPage::display_export_id(const QString& name)
    {
        mainwindow->display_export_id(name);
    }

    void WebPage::display_delete_id(const QString& name)
    {
        mainwindow->display_delete_id(name);
    }

    void WebPage::create_rule_tree(XsltPolicyRule *r, int index, QString& rule_data)
    {
        if (!r)
            return;

        rule_data = "{";
        rule_data += QString("\"ruleId\":%1").arg(index);

        int len = r->track_type.length();
        if (len > 0)
        {
            QString type = QString().fromUtf8(r->track_type.c_str(), r->track_type.length());
            string_to_json(type);
            rule_data += QString(",\"trackType\":\"%1\"").arg(type);
        }
        else
            rule_data += ",\"trackType\":null";

        len = r->field.length();
        if (len > 0)
        {
            QString field = QString().fromUtf8(r->field.c_str(), r->field.length());
            string_to_json(field);
            rule_data += QString(",\"field\":\"%1\"").arg(field);
        }
        else
            rule_data += ",\"field\":null";

        if (r->occurrence >= 0)
            rule_data += QString(",\"occurrence\":%1").arg(r->occurrence);
        else
            rule_data += ",\"occurrence\":\"*\"";

        len = r->ope.length();
        if (len > 0)
        {
            QString validator = QString().fromUtf8(r->ope.c_str(), r->ope.length());
            string_to_json(validator);
            rule_data += QString(",\"validator\":\"%1\"").arg(validator);
        }
        else
            rule_data += ",\"validator\":null";

        len = r->value.length();
        if (len > 0)
        {
            QString value = QString().fromUtf8(r->value.c_str(), r->value.length());
            string_to_json(value);
            rule_data += QString(",\"value\":\"%1\"").arg(value);
        }
        else
            rule_data += ",\"value\":null";
        rule_data += "}";
    }

    void WebPage::create_xslt_policy_rules_tree(XsltPolicy *policy, QString& rules)
    {
        if (!policy || policy->type != Policies::POLICY_XSLT)
            return;

        rules = "[";
        // for (size_t j = 0; j < policy->rules.size(); ++j)
        // {
        //     XsltPolicyRule *r = policy->rules[j];
        //     if (!r)
        //         continue;

        //     if (j)
        //         rules += ",";

        //     QString rule_data;
        //     create_rule_tree(r, j, rule_data);
        //     QString type = QString().fromUtf8(r->name.c_str(), r->name.length());
        //     string_to_json(type);
        //     rules += QString("{\"text\":\"%1\",\"type\":\"r\",\"data\":%2}")
        //                  .arg(type)
        //                  .arg(rule_data);
        // }
        rules += "]";
    }

    QString WebPage::get_policies_tree()
    {
        // size_t nb_policies;

        // nb_policies = mainwindow->get_policies_count();
        // bool has_user = false;
        // bool has_system = false;
        // QString user("{\"id\":\"u_p\",\"text\":\"User Policies\",\"type\":\"up\",\"state\":{\"opened\":true, \"selected\": true},\"children\":[");
        // QString system("{\"id\":\"s_p\",\"text\":\"System Policies\",\"type\":\"sp\",\"state\":{\"opened\":true},\"children\":[");
        // for (size_t i = 0; i < nb_policies; ++i)
        // {
        //     MediaConchLib::Policy_Policy *p = mainwindow->policy_get(i);
        //     if (!p)
        //         continue;

        //     QString name = QString().fromUtf8(p->name.c_str(), p->name.length());
        //     QString description = QString().fromUtf8(p->description.c_str(), p->description.length());
        //     string_to_json(name);
        //     string_to_json(description);
        //     if (p->is_system)
        //     {
        //         if (has_system)
        //             system += ",";
        //         system += QString("{\"text\":\"%1\",\"type\":\"s\",\"data\":{\"policyId\":%2,\"description\":\"%3\", \"isEditable\": false}")
        //                       .arg(name)
        //                       .arg(i)
        //                       .arg(description);
        //         if (p->type==Policies::POLICY_XSLT)
        //         {
        //             QString rules;
        //             create_xslt_policy_rules_tree((XsltPolicy*)p, rules);
        //             system += QString(",\"children\":%3}").arg(rules);
        //         }
        //         else
        //             system += "}";
        //         has_system = true;
        //     }
        //     else
        //     {
        //         if (has_user)
        //             user += ",";
        //         user += QString("{\"text\":\"%1\",\"type\":\"u\",\"data\":{\"policyId\":%2,\"description\":\"%3\"")
        //                     .arg(name)
        //                     .arg(i)
        //                     .arg(description);
        //         if (p->type==Policies::POLICY_XSLT)
        //         {
        //             QString rules;
        //             create_xslt_policy_rules_tree((XsltPolicy*)p, rules);
        //             user += QString(",\"isEditable\":true},\"children\":%4}").arg(rules);
        //         }
        //         else
        //             user += ",\"isEditable\":false}}";
        //         has_user = true;
        //     }
        // }
        // user += "]}";
        // system += "]}";
        // QString res = QString("{\"policiesTree\":[%1,%2]}").arg(user).arg(system);
        // return res;
        return QString();
    }

    QString WebPage::import_policy()
    {
        //return: policyName, policyId, policyRules
        QString file;
        QStringList list = file_selector["xslPolicyImport[policyFile]"];
        if (list.size())
            file = list.last().toUtf8().data();

        file_selector.clear();

        if (!file.length())
            return QString("{\"error\":\"No file selected\"}");

        return import_policy(file);
    }

    QString WebPage::import_policy(const QString& file)
    {
        QString json;
        std::string err;
        int policy_id = -1;
        if ((policy_id = mainwindow->policy_import(file, err)) < 0)
        {
            QString error = QString().fromUtf8(err.c_str(), err.length());
            string_to_json(error);
            json = QString("{\"error\":\"%1\"}").arg(error);
            return json;
        }
        mainwindow->policy_save(policy_id, err);

        // MediaConchLib::Policy_Policy *p = mainwindow->policy_get(policy_id, err);
        // if (!p)
        // {
        //     QString error = QString().fromUtf8(err.c_str(), err.length());
        //     string_to_json(error);
        //     json = QString("{\"error\":\"%1\"}").arg(error);
        //     return json;
        // }

        // QString type = QString().fromUtf8(p->name.c_str(), p->name.length());
        // QString description = QString().fromUtf8(p->description.c_str(), p->description.length());
        // string_to_json(type);
        // string_to_json(description);
        // json = QString("{\"policyName\":\"%1\",\"policyDescription\":\"%2\",\"policyId\":%3,\"isEditable\":%4")
        //            .arg(type)
        //            .arg(description)
        //            .arg(nb_policies)
        //            .arg(p->type==Policies::POLICY_XSLT?"true":"false");
        // if (p->type==Policies::POLICY_XSLT)
        // {
        //     QString rules;
        //     create_xslt_policy_rules_tree((XsltPolicy *)p, rules);
        //     json += QString(",\"policyRules\":%4}").arg(rules);
        // }
        // else
            json += "}";
        return json;
    }

    int WebPage::import_policy(const QStringList& files)
    {
        int ret = 0;

        QString script;
        for (int i = 0; i < files.size(); ++i)
        {
            QString tmp = import_policy(files[i]);
            if (tmp.startsWith("{\"error\":"))
                ret = -1;
            else
                script += QString("policyImportDrag(%1);").arg(tmp);
        }
        if (script.length())
            use_javascript(script);
        return ret;
    }

    QString WebPage::create_policy()
    {
        //return: policyName, policyId
        QString json;
        std::string err;

        int ret = -1;
        if ((ret = mainwindow->xslt_policy_create(err)) < 0)
        {
            QString error = QString().fromUtf8(err.c_str(), err.length());
            string_to_json(error);
            json = QString("{\"error\":\"%1\"}").arg(error);
            return json;
        }

        std::string n;
        if (mainwindow->policy_get_name(ret, n, err) < 0)
        {
            json = "{\"error\":\"Cannot create a policy\"}";
            return json;
        }
        mainwindow->policy_save(ret, err);

        QString name = QString().fromUtf8(n.c_str(), n.length());
        string_to_json(name);
        json = QString("{\"policyName\":\"%1\",\"policyId\":%3}")
                   .arg(name)
                   .arg(ret);
        return json;
    }

    QString WebPage::duplicate_policy(int id)
    {
        //return: policyName, policyId, policyRules
        QString json;
        std::string err;
        size_t nb_policies = mainwindow->get_policies_count();
        int ret = -1;

        if ((ret = mainwindow->policy_duplicate(id, err)) < 0)
        {
            QString error = QString().fromUtf8(err.c_str(), err.length());
            string_to_json(error);
            json = QString("{\"error\":\"%1\"}").arg(error);
            return json;
        }

        if (ret != (int)nb_policies)
        {
            json = "{\"error\":\"Cannot duplicate the policy\"}";
            return json;
        }

        MediaConchLib::Policy_Policy *p = mainwindow->policy_get(ret);
        if (!p)
        {
            json = "{\"error\":\"Cannot duplicate the policy\"}";
            return json;
        }
        mainwindow->policy_save(ret, err);

        // QString rules;
        // if (p->type == Policies::POLICY_XSLT)
        //     create_xslt_policy_rules_tree((XsltPolicy *)p, rules);
        // else
        //     rules = "[]";

        // QString name = QString().fromUtf8(p->name.c_str(), p->name.length());
        // QString description = QString().fromUtf8(p->description.c_str(), p->description.length());
        // string_to_json(name);
        // string_to_json(description);
        // json = QString("{\"policyName\":\"%1\",\"policyDescription\":\"%2\",\"policyId\":%3,\"isEditable\":%4,\"policyRules\":%5}")
        //            .arg(name)
        //            .arg(description)
        //            .arg(ret)
        //            .arg(p->type == Policies::POLICY_XSLT?"true":"false")
        //            .arg(rules);
        return json;
    }

    QString WebPage::delete_policy(int id)
    {
        //return:
        QString json;

        if (id < 0)
        {
            json = "{\"error\":\"Policy does not exist\"}";
            return json;
        }

        std::string err;
        if (mainwindow->policy_remove((size_t)id, err) < 0)
        {
            QString error = QString().fromUtf8(err.c_str(), err.length());
            string_to_json(error);
            json = QString("{\"error\":\"%1\"}").arg(error);
            return json;
        }

        json = QString("{}");
        return json;
    }

    QString WebPage::export_policy(int id)
    {
        //return: error?
        QString json;
        std::string err;
        if (mainwindow->policy_export((size_t)id, err) < 0)
        {
            QString error = QString().fromUtf8(err.c_str(), err.length());
            string_to_json(error);
            json = QString("{\"error\":\"%1\"}").arg(error);
            return json;
        }

        json = QString("{}");
        return json;
    }

    QString WebPage::policy_change_info(int id, const QString& name, const QString& description)
    {
        //return: error?
        QString json;
        std::string err;
        if (mainwindow->policy_change_info((size_t)id, name.toUtf8().data(), description.toUtf8().data(), err) < 0)
        {
            QString error = QString().fromUtf8(err.c_str(), err.length());
            string_to_json(error);
            json = QString("{\"error\":\"%1\"}").arg(error);
            return json;
        }
        mainwindow->policy_save(id, err);

        QString n = name;
        QString desc = description;
        string_to_json(n);
        string_to_json(desc);
        json = QString("{\"policyName\":\"%1\",\"policyDescription\":\"%2\"}").arg(name).arg(desc);
        return json;
    }

    QString WebPage::policy_change_type(int id, const QString& type)
    {
        //return: error?
        QString json;
        std::string err;
        if (mainwindow->policy_change_type((size_t)id, type.toUtf8().data(), err) < 0)
        {
            QString error = QString().fromUtf8(err.c_str(), err.length());
            string_to_json(error);
            json = QString("{\"error\":\"%1\"}").arg(error);
            return json;
        }
        mainwindow->policy_save(id, err);

        QString n = type;
        string_to_json(n);
        json = QString("{\"policyType\":\"%1\"}").arg(type);
        return json;
    }

    QString WebPage::policy_rule_create(int policy_id)
    {
        //return: rule
        std::string err;
        QString json;
        int new_rule_id = -1;
        if ((new_rule_id = mainwindow->xslt_policy_rule_create(policy_id, err)) < 0)
        {
            QString error = QString().fromUtf8(err.c_str(), err.length());
            string_to_json(error);
            json = QString("{\"error\":\"%1\"}").arg(error);
            return json;
        }

        // XsltPolicy* p = (XsltPolicy*)mainwindow->policy_get(policy_id);
        // XsltPolicyRule* r = p->rules[new_rule_id];
        // mainwindow->policy_save(policy_id, err);

        // QString rule_data;
        // create_rule_tree(r, new_rule_id, rule_data);
        // QString name = QString().fromUtf8(r->name.c_str());
        // string_to_json(name);
        // json = QString("{\"rule\":{\"text\":\"%1\",\"type\":\"r\",\"data\":%2}}").arg(name).arg(rule_data);
        return json;
    }

    QString WebPage::policy_rule_edit(int policy_id, int rule_id, const QString& title, bool is_editor, const QString& type, const QString& field, int occurrence, const QString& ope, const QString& value, const QString& free_text)
    {
        //return: rule
        std::string err;
        QString json;

        // XsltPolicyRule rule;
        // rule.name          = title.toUtf8().data();
        // rule.ope           = ope.toUtf8().data();
        // rule.use_free_text = !is_editor;
        // rule.track_type    = track_type.toUtf8().data();
        // rule.field         = field.toUtf8().data();
        // rule.occurrence    = occurrence;
        // rule.value         = value.toUtf8().data();
        // rule.test          = free_text.toUtf8().data();

        // if (mainwindow->xslt_policy_rule_edit(policy_id, rule_id, &rule, err) < 0)
        // {
        //     QString error = QString().fromUtf8(err.c_str(), err.length());
        //     string_to_json(error);
        //     json = QString("{\"error\":\"%1\"}").arg(error);
        //     return json;
        // }

        // XsltPolicy* p = (XsltPolicy*)mainwindow->policy_get(policy_id);
        // XsltPolicyRule* r = p->rules[rule_id];
        // mainwindow->policy_save(policy_id, err);

        // QString rule_data;
        // create_rule_tree(r, rule_id, rule_data);
        // QString t = QString().fromUtf8(r->name.c_str());
        // string_to_json(t);
        // json = QString("{\"rule\":{\"text\":\"%1\",\"type\":\"r\",\"data\":%2}}").arg(t).arg(rule_data);
        return json;
    }

    QString WebPage::policy_rule_duplicate(int policy_id, int rule_id)
    {
        //return: rule
        std::string err;
        QString json;
        int new_rule_id = -1;
        if ((new_rule_id = mainwindow->xslt_policy_rule_duplicate(policy_id, rule_id, err)) < 0)
        {
            QString error = QString().fromUtf8(err.c_str(), err.length());
            string_to_json(error);
            json = QString("{\"error\":\"%1\"}").arg(error);
            return json;
        }

        // XsltPolicy* p = (XsltPolicy*)mainwindow->policy_get(policy_id);
        // XsltPolicyRule* r = p->rules[new_rule_id];
        // mainwindow->policy_save(policy_id, err);

        // QString rule_data;
        // create_rule_tree(r, new_rule_id, rule_data);
        // QString name = QString().fromUtf8(r->name.c_str());
        // string_to_json(name);
        // json = QString("{\"rule\":{\"text\":\"%1\",\"type\":\"r\",\"data\":%2}}").arg(name).arg(rule_data);
        return json;
    }

    QString WebPage::policy_rule_delete(int policy_id, int rule_id)
    {
        //return: ruleId
        std::string err;
        QString json;
        if (mainwindow->xslt_policy_rule_delete(policy_id, rule_id, err) < 0)
        {
            QString error = QString().fromUtf8(err.c_str(), err.length());
            string_to_json(error);
            json = QString("{\"error\":\"%1\"}").arg(error);
            return json;
        }
        mainwindow->policy_save(policy_id, err);

        json = QString("{\"ruleId\":%1}").arg(rule_id);
        return json;
    }

    QString WebPage::get_fields_list(const QString& type, const QString& field)
    {
        //return: {values:[value,value]}
        QString json;

        std::vector<std::string> fields;
        if (mainwindow->get_fields_for_type(type.toUtf8().data(), fields) < 0)
        {
            json = "{\"error\":\"\"}";
            return json;
        }

        json = "{\"fields\":[";
        bool has_field = false;
        for (size_t i = 0; i < fields.size(); ++i)
        {
            if (i)
                json += ",";

            QString f = QString().fromUtf8(fields[i].c_str());
            string_to_json(f);

            json += QString("\"%1\"").arg(f);
            if (fields[i] == field.toUtf8().data())
                has_field = true;
        }
        if (!has_field && field.length())
            json += QString("%1\"%2\"").arg(fields.size() > 0 ? "," : "").arg(field);
        json += "]}";
        return json;
    }

    QString WebPage::get_values_list(const QString& type, const QString& field, const QString& value)
    {
        //return: {values:[value,value]}
        QString json;

        std::vector<std::string> values;
        if (mainwindow->get_values_for_type_field(type.toUtf8().data(), field.toUtf8().data(), values) < 0)
        {
            json = "{\"error\":\"\"}";
            return json;
        }

        json = "{\"values\":[";
        bool has_value = false;
        for (size_t i = 0; i < values.size(); ++i)
        {
            if (i)
                json += ",";

            QString v = QString().fromUtf8(values[i].c_str());
            string_to_json(v);

            json += QString("\"%1\"").arg(v);
            if (values[i] == value.toUtf8().data())
                has_value = true;
        }
        if (!has_value && value.length())
            json += QString("%1\"%2\"").arg(values.size() > 0 ? "," : "").arg(value);
        json += "]}";
        return json;
    }

    void WebPage::string_to_json(QString& str)
    {
        str.replace("\"", "\\\"");
        str.replace("\n", "\\\n");
    }
}

#endif
