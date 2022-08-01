/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "WebCommonPage.h"
#include "mainwindow.h"
#include <Common/FileRegistered.h>
#include <Common/XsltPolicy.h>

#include <QDir>
#include <QTimer>
#include <QFileDialog>
#include <QTextStream>
#include <QDesktopServices>

namespace MediaConch
{
    void WebCommonPage::menu_link_checker(const QString& name)
    {
        menu_name = name;
        QTimer::singleShot(0, this, SLOT(timer_menu_link()));
    }

    void WebCommonPage::timer_menu_link()
    {
        if (!menu_name.compare("Checker"))
            mainwindow->on_actionChecker_triggered();
        else if (!menu_name.compare("Policies"))
            Q_EMIT mainwindow->on_actionPolicies_triggered();
        else if (!menu_name.compare("Public Policies"))
            Q_EMIT mainwindow->on_actionPublicPolicies_triggered();
        else if (!menu_name.compare("Display"))
            Q_EMIT mainwindow->on_actionDisplay_triggered();
        else if (!menu_name.compare("Database"))
            Q_EMIT mainwindow->on_actionDatabase_triggered();
        else if (!menu_name.compare("Settings"))
            Q_EMIT mainwindow->on_actionSettings_triggered();
        else if (!menu_name.compare("Help"))
            Q_EMIT mainwindow->on_actionGettingStarted_triggered();
        else
            mainwindow->on_actionChecker_triggered();
    }

    void WebCommonPage::on_input_changed(const QString& inputName)
    {
        select_file_name = inputName;
    }

    void WebCommonPage::on_download_report(const QString& report, long file_id, const QString& report_name)
    {
        if (report.isEmpty())
            return;

        QString filename = QString().fromUtf8(mainwindow->get_filename_from_registered_file_id(file_id).c_str());
        std::string proposed_path_str = mainwindow->select_correct_save_report_path();
        QDir proposed_dir(QString().fromUtf8(proposed_path_str.c_str(), proposed_path_str.length()));
        QString proposed_filename = QFileInfo(filename).fileName() + "_" + report_name + ".txt";

        QString proposed = proposed_dir.absoluteFilePath(proposed_filename);

        if (report_is_html(report))
            proposed.replace(proposed.length() - 3, 3, "html");
        else if (report_is_xml(report))
            proposed.replace(proposed.length() - 3, 3, "xml");
        else if (report_is_json(report))
            proposed.replace(proposed.length() - 3, 3, "json");
        else
            proposed.replace(proposed.length() - 3, 3, "txt");

        QString dl_file = QFileDialog::getSaveFileName(mainwindow, "Save report", proposed);

        if (!dl_file.length())
            return;

        QDir info(QFileInfo(dl_file).absoluteDir());
        mainwindow->set_last_save_report_path(info.absolutePath().toUtf8().data());

        QFile file(dl_file);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
            return;

        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << report;
    }

    QString WebCommonPage::on_fill_implementation_report(long file_id, const QString& display, const QString& verbosity)
    {
        QString report;
        int display_i = display.toInt();
        int *verbosity_p = NULL;
        int verbosity_i;
        if (verbosity.length())
        {
            verbosity_i = verbosity.toInt();
            verbosity_p = &verbosity_i;
        }

        std::string err;
        mainwindow->get_implementation_report(file_id, report, err, &display_i, verbosity_p);

        string_to_json(report);
        QString ret = QString("{\"report\":\"%1\",\"isHtmlReport\":%2}").arg(report).arg(report_is_html(report) ? "true" : "false");
        return ret;
    }

    void WebCommonPage::on_save_implementation_report(long file_id, const QString& display, const QString& verbosity)
    {
        QString report;
        int display_i = display.toInt();
        int *verbosity_p = NULL;
        int verbosity_i;
        if (verbosity.length())
        {
            verbosity_i = verbosity.toInt();
            verbosity_p = &verbosity_i;
        }

        std::string err;
        mainwindow->get_implementation_report(file_id, report, err, &display_i, verbosity_p);
        on_download_report(report, file_id, "ImplementationReport");
    }

    QString WebCommonPage::on_fill_policy_report(long file_id, const QString& policy, const QString& display)
    {
        int policy_i = policy.toInt();
        QString report;
        if (policy_i != -1)
        {
            int display_i = display.toInt();
            std::string err;
            mainwindow->validate_policy(file_id, report, err, policy_i, &display_i);
        }

        string_to_json(report);
        QString ret = QString("{\"report\":\"%1\",\"isHtmlReport\":%2}").arg(report).arg(report_is_html(report) ? "true" : "false");
        return ret;
    }

    void WebCommonPage::on_save_policy_report(long file_id, const QString& policy, const QString& display)
    {
        QString report;
        int policy_i = policy.toInt();
        int display_i = display.toInt();
        std::string err;
        mainwindow->validate_policy(file_id, report, err, policy_i, &display_i);
        on_download_report(report, file_id, "MediaConchReport");
    }

    QString WebCommonPage::on_fill_output_list()
    {
        std::string output_list;
        std::string err;
        mainwindow->get_mediainfo_output_list(output_list, err);

        return QString().fromUtf8(output_list.c_str());
    }

    QString WebCommonPage::on_fill_mediainfo_report(long file_id)
    {
        std::string err;
        return mainwindow->get_mediainfo_jstree(file_id, err);
    }

    void WebCommonPage::on_save_mediainfo_report(long file_id, const QString& output)
    {
        std::string display_name, display_content;
        std::string mi_inform = output.toStdString();
        std::string err;

        QString report_name = "MediaInfo";
        if (!output.isEmpty() && output != "XML" && output != "MIXML" && output != "Text" && output != "HTML")
            report_name.append(".").append(output);

        QString report = mainwindow->get_mediainfo_report(file_id, display_name, display_content, mi_inform, err);
        report.replace("\r\n", "\n"); //There are some incoherencies in inputs, so we put all to \n and file writer will emit the right carriage return
        on_download_report(report, file_id, report_name);
    }

    QString WebCommonPage::on_fill_mediatrace_report(long file_id)
    {
        std::string err;
        return mainwindow->get_mediatrace_jstree(file_id, err);
    }

    void WebCommonPage::on_save_mediatrace_report(long file_id)
    {
        std::string display_name, display_content;
        std::string err;
        QString report = mainwindow->get_mediatrace_xml(file_id, display_name, display_content, err);
        on_download_report(report, file_id, "MediaTrace");
    }

    QString WebCommonPage::on_create_policy_from_file(long file_id)
    {
        std::string n;
        QString err;

        int id = mainwindow->xslt_policy_create_from_file(file_id, err);
        if (id < 0 || mainwindow->policy_get_name(id, n, err) < 0)
            n = std::string();
        else
            mainwindow->policy_save(id, err);

        QString name = QString().fromUtf8(n.c_str(), n.length());
        string_to_json(name);

        QString script;
        if (n.size())
            script = QString("{\"result\":true,\"policyId\":%1, \"policyName\":\"%2\"}")
                         .arg(id)
                         .arg(name);
        else
            script = QString("{}");
        return script;
    }

    QString WebCommonPage::checker_force_analyze(long id)
    {
        std::string err;

        if (mainwindow->analyze_force_file_to_list(id, err) < 0)
        {
            QString err_str = QString().fromUtf8(err.c_str(), err.size());
            string_to_json(err_str);
            return QString("{\"error\": \"%1\"}").arg(err_str);
        }

        return QString("{}");
    }

    void WebCommonPage::on_file_selected(const QString& filename, const QString& path,
                                         const QString& policy, const QString& display,
                                         const QString& verbosity, bool fixer, const QStringList& options,
                                         QString& ret, std::string& err_str)
    {
        std::string error;
        std::string full_file(path.toUtf8().data());
        if (mainwindow->add_file_to_list(filename, path, policy,
                                         display, verbosity, fixer, false, options, error) < 0)
        {
            if (err_str.size())
                err_str += "\n";
            err_str += error;
        }
        else
        {
#ifdef WINDOWS
            if (full_file.size() && full_file[full_file.size() - 1] != '/' && full_file[full_file.size() - 1] != '\\')
                full_file += "/";
#else
            if (full_file.length())
                full_file += "/";
#endif
            full_file += filename.toUtf8().data();

            FileRegistered* fr = mainwindow->get_file_registered_from_file(full_file);
            if (!fr)
            {
                err_str += "File not correctly registered.";
                return;
            }

            if (ret.size())
                ret += ",";

            QString filename = QString().fromUtf8(full_file.c_str(), full_file.size());
            string_to_json(filename);
            ret += QString("{\"success\":true, \"filename\":\"%1\", \"transactionId\":%2}")
                .arg(filename)
                .arg(fr->file_id);
        }
    }

    void WebCommonPage::on_file_selected_end(const std::string& err_str, QString& ret)
    {
        QString val("{\"success\":[");
        val += ret;
        val += "]";

        ret = val;

        QString err;
        if (err_str.size())
            err = QString("\"error\":\"%1\"").arg(QString().fromUtf8(err_str.c_str(), err_str.size()));

        if (err.size())
        {
            if (ret.size() > 1)
                ret += ",";
            ret += err;
        }

        file_selector.clear();
        clean_forms();
        ret += "}";
    }

    void WebCommonPage::on_file_selected_formValues(const FileRegistered* fr, QString& formValues)
    {
        formValues = "{";

        //Policy
        formValues += QString("\"policy\":%1,").arg(fr->policy);
        if (fr->policy == -1)
            formValues += "\"policyText\":\"N/A\"";
        else
        {
            QString err;
            std::string n;
            if (mainwindow->policy_get_name(fr->policy, n, err) < 0)
                n = "N/A";

            QString name = QString().fromUtf8(n.c_str(), n.length());
            string_to_json(name);

            formValues += QString("\"policyText\":\"%1\"").arg(name);
        }

        //display
        formValues += QString(",\"display\":%1").arg(fr->display);

        //verbosity
        formValues += QString(",\"verbosity\":%1").arg(fr->verbosity);

        formValues += "}";
    }

    QString WebCommonPage::on_file_upload_selected(const QString& policy, const QString& display,
                                                   const QString& verbosity, bool fixer, const QStringList& options)
    {
        QStringList files = file_selector.value("checkerUpload_file", QStringList());

        if (!files.size())
            return QString("{\"error\": \"No folder selected.\"}");

        std::string err_str;
        QString ret;
        for (int i = 0; i < files.size(); ++i)
        {
            QFileInfo f = QFileInfo(files[i]);
            on_file_selected(f.fileName(), f.absolutePath(), policy,
                             display, verbosity, fixer, options, ret, err_str);
        }
        on_file_selected_end(err_str, ret);

        return ret;
    }

    QString WebCommonPage::on_file_online_selected(const QString& url, const QString& policy,
                                                   const QString& display, const QString& verbosity,
                                                   bool, const QStringList& options)
    {
        if (!url.length())
            return QString("{\"error\": \"No URL given.\"}");

        std::string err_str;
        QString ret;
        on_file_selected(url, "", policy, display, verbosity, false, options, ret, err_str);
        on_file_selected_end(err_str, ret);

        return ret;
    }

    QString WebCommonPage::on_file_repository_selected(const QString& policy, const QString& display,
                                                       const QString& verbosity, bool fixer,
                                                       const QStringList& options)
    {
        QStringList dirname = file_selector.value("checkerRepository_directory", QStringList());
        if (dirname.empty())
            return QString("{\"error\": \"No folder selected.\"}");

        QDir dir(dirname.last());
        QFileInfoList list;
        add_sub_directory_files_to_list(dir, list);
        if (!list.count())
            return QString("{\"error\": \"Folder selected is empty.\"}");

        std::string err_str;
        QString ret;
        for (int i = 0; i < list.size(); ++i)
            on_file_selected(list[i].fileName(), list[i].absolutePath(), policy, display, verbosity, fixer, options, ret, err_str);
        on_file_selected_end(err_str, ret);

        return ret;
    }

    QString WebCommonPage::on_file_from_db_selected()
    {
        QString ret;
        std::string err_str;
        std::map<std::string, FileRegistered*> files;
        mainwindow->get_registered_files(files);

        if (!files.size())
            return ret;

        std::map<std::string, FileRegistered*>::iterator it = files.begin();

        for (; it != files.end(); ++it)
        {
            if (!it->second)
                continue;

            if (ret.size())
                ret += ",";

            QString filename = QString().fromUtf8(it->first.c_str(), it->first.size());
            string_to_json(filename);

            QString formValues;
            on_file_selected_formValues(it->second, formValues);
            ret += QString("{\"success\":true, \"filename\":\"%1\", \"transactionId\":%2, \"formValues\":%3}")
                .arg(filename)
                .arg(it->second->file_id)
                .arg(formValues);
        }

        on_file_selected_end(err_str, ret);
        return ret;
    }

    QString WebCommonPage::status_reports_multi(const QStringList& ids, const QStringList& policy_ids)
    {
        QString ret("[");
        for (int i = 0; i < ids.size(); ++i)
        {
            if (ret.size() > 1)
                ret += ",{";
            else
                ret += "{";
            long file_id = ids[i].toInt();
            ret += "\"implemReport\":" + implementation_is_valid(file_id);

            if (policy_ids[i] == "-1")
            {
                ret += "}";
                continue;
            }

            ret += ",\"policyReport\":" + policy_is_valid(file_id, policy_ids[i].toInt());

            ret += "}";
        }
        ret += "]";
        return ret;
    }

    void WebCommonPage::on_save_settings_selected(const QString& policy,
                                            const QString& display,
                                            const QString& verbosity,
                                            const QString& parsespeed,
                                            const QString& save_report_path, const QString& load_files_path,
                                            const QString& save_policy_path, const QString& load_policy_path,
                                            const QString& save_display_path, const QString& load_display_path)
    {
        UiSettings& settings = mainwindow->get_settings();

        //policy
        if (policy == "-2")
            settings.change_default_policy("last");
        else if (policy == "-1")
            settings.change_default_policy("");
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

        //verbosity
        if (parsespeed == "-1")
            settings.change_default_parsespeed("last");
        else
            settings.change_default_parsespeed(parsespeed.toStdString());

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

    void WebCommonPage::clean_forms()
    {
        file_selector.clear();

        int policy_i = mainwindow->select_correct_policy();
        int display_i = mainwindow->select_correct_display();
        int verbosity_i = mainwindow->select_correct_verbosity();
        bool has_libcurl = mainwindow->mil_has_curl_enabled();
        QString parsespeed = QString().fromStdString(mainwindow->select_correct_parsespeed());

        use_javascript(QString("document.getElementById('checkerUpload_policy').value = %1;").arg(policy_i));
        use_javascript(QString("document.getElementById('checkerUpload_display_selector').value = %1;").arg(display_i));
        use_javascript(QString("document.getElementById('checkerUpload_verbosity_selector').value = %1;").arg(verbosity_i));
        use_javascript(QString("document.getElementById('checkerUpload_parsespeed_selector').value = \"%1\";").arg(parsespeed));
        use_javascript("document.getElementById('checkerUpload_file').value = \"\";");

        if (has_libcurl)
        {
            use_javascript(QString("document.getElementById('checkerOnline_policy').value = %1;").arg(policy_i));
            use_javascript(QString("document.getElementById('checkerOnline_display_selector').value = %1;").arg(display_i));
            use_javascript(QString("document.getElementById('checkerOnline_verbosity_selector').value = %1;").arg(verbosity_i));
            use_javascript(QString("document.getElementById('checkerOnline_parsespeed_selector').value = \"%1\";").arg(parsespeed));
            use_javascript("document.getElementById('checkerOnline_file').value = \"\";");
        }

        use_javascript(QString("document.getElementById('checkerRepository_policy').value = %1;").arg(policy_i));
        use_javascript(QString("document.getElementById('checkerRepository_display_selector').value = %1;").arg(display_i));
        use_javascript(QString("document.getElementById('checkerRepository_verbosity_selector').value = %1;").arg(verbosity_i));
        use_javascript(QString("document.getElementById('checkerRepository_parsespeed_selector').value = \"%1\";").arg(parsespeed));
        use_javascript("document.getElementById('checkerRepository_directory').value = \"\";");
    }

    void WebCommonPage::close_all()
    {
        mainwindow->clear_file_list();
    }

    void WebCommonPage::close_element(long file_id)
    {
        mainwindow->remove_file_to_list(file_id);
    }

    QString WebCommonPage::choose_file_settings()
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
        QString value_input = QFileDialog::getExistingDirectory(mainwindow, NULL, suggested);

        QMap<QString, QStringList>::iterator it = file_selector.find(select_file_name);
        if (it != file_selector.end())
            file_selector.erase(it);

        if (!value_input.length())
            return QString();

        file_selector.insert(select_file_name, QStringList(value_input));

        return value_input;
    }

    QString WebCommonPage::choose_file_import_policy()
    {
        std::string suggested_str;

        if (select_file_name == "xslPolicyImport[policyFile]")
            suggested_str = mainwindow->select_correct_load_policy_path();
        else
            return QString();

        QString suggested = QString().fromUtf8(suggested_str.c_str(), suggested_str.length());
        QString value_input = QFileDialog::getOpenFileName(mainwindow, "Open file", suggested, "XML file (*.xml);;XSL file (*.xsl);;All (*.*)", 0, QFileDialog::Option(0));

        QMap<QString, QStringList>::iterator it = file_selector.find(select_file_name);
        if (it != file_selector.end())
            file_selector.erase(it);

        if (!value_input.length())
            return QString();

        mainwindow->set_last_load_policy_path(value_input.toUtf8().data());
        file_selector.insert(select_file_name, QStringList(value_input));

        return value_input;
    }

    QString WebCommonPage::choose_file_policy_from_file()
    {
        std::string suggested_str;

        suggested_str = mainwindow->select_correct_load_files_path();

        QString suggested = QString().fromUtf8(suggested_str.c_str(), suggested_str.length());
        QString value_input = QFileDialog::getOpenFileName(mainwindow, "Open file", suggested);

        QMap<QString, QStringList>::iterator it = file_selector.find(select_file_name);
        if (it != file_selector.end())
            file_selector.erase(it);

        if (!value_input.length())
            return QString();

        file_selector.insert(select_file_name, QStringList(value_input));

        return value_input;
    }

    void WebCommonPage::charge_local_dir(const QString& path, QStringList& tmp)
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

    void WebCommonPage::change_local_files(const QStringList& files)
    {
        QStringList tmp;
        for (int i = 0; i < files.size(); ++i)
            charge_local_dir(files[i], tmp);

        if (!tmp.size())
            return;

        QMap<QString, QStringList>::iterator it = file_selector.find("checkerUpload_file");
        if (it != file_selector.end())
            file_selector["checkerUpload_file"] << tmp;
        else
            file_selector.insert("checkerUpload_file", tmp);

        if (files.size())
        {
            QDir info(QFileInfo(files[0]).absoluteDir());
            mainwindow->set_last_load_files_path(info.absolutePath().toUtf8().data());
        }

        QStringList options;
        if (!checker_parse_speed.isEmpty())
        {
            options << "file_parsespeed";
            options << checker_parse_speed;
        }
        QString res = on_file_upload_selected(QString().setNum(mainwindow->select_correct_policy()),
                                              QString().setNum(mainwindow->select_correct_display()),
                                              QString().setNum(mainwindow->select_correct_verbosity()),
                                              false, options);

        FileRegistered* fr = mainwindow->get_file_registered_from_file(tmp[0].toUtf8().data());
        if (!fr)
            return;

        QString formValues;
        on_file_selected_formValues(fr, formValues);

        string_to_json(res);
        QString script = QString("checkerAjax.formRequestResponse(\"%1\", %2);").arg(res).arg(formValues);
        if (script.length())
            use_javascript(script);

    }

    //---------------------------------------------------------------------------
    bool WebCommonPage::report_is_html(const QString& report)
    {
        QRegExp reg("^(<\\!DOCTYPE.*html|<html>.*</html>$)", Qt::CaseInsensitive);

        if (reg.indexIn(report.trimmed(), 0) != -1)
            return true;

        return false;
    }

    //---------------------------------------------------------------------------
    bool WebCommonPage::report_is_xml(const QString& report)
    {
        QRegExp reg("<\\?xml ", Qt::CaseInsensitive);

        if (reg.indexIn(report, 0) != -1)
            return true;

        return false;
    }

    //---------------------------------------------------------------------------
    bool WebCommonPage::report_is_json(const QString& report)
    {
        QRegExp reg("^\\{.*\\}$", Qt::CaseInsensitive);

        if (reg.indexIn(report.trimmed(), 0) != -1)
            return true;

        return false;
    }

    QString WebCommonPage::get_file_tool(const QString& file)
    {
        FileRegistered* fr = mainwindow->get_file_registered_from_file(file.toUtf8().data());
        if (!fr)
            return "-1";

        int report_kind = fr->report_kind;
        delete fr;
        return QString().setNum(report_kind);
    }

    QString WebCommonPage::get_file_policy_id(const QString& file)
    {
        FileRegistered* fr = mainwindow->get_file_registered_from_file(file.toUtf8().data());
        if (!fr)
            return "-1";

        int policy = fr->policy;
        delete fr;
        return QString().setNum(policy);
    }

    QString WebCommonPage::get_file_display_id(const QString& file)
    {
        FileRegistered* fr = mainwindow->get_file_registered_from_file(file.toUtf8().data());
        if (!fr)
            return "-1";

        int display = fr->display;
        delete fr;
        return QString().setNum(display);
    }

    QString WebCommonPage::get_file_verbosity_id(const QString& file)
    {
        FileRegistered* fr = mainwindow->get_file_registered_from_file(file.toUtf8().data());
        if (!fr)
            return "-1";

        int verbosity = fr->verbosity;
        delete fr;
        return QString().setNum(verbosity);
    }

    QString WebCommonPage::policy_is_valid(long file_id, long policy_id)
    {
        //{"valid":false,"info":false,"warn":false,"fileId":"fileId","error":null}
        std::string err;
        QString json = QString("{\"fileId\":\"%1\",").arg(file_id);
        bool policy_valid = false;
        if (mainwindow->update_policy_of_file_in_list(file_id, policy_id, err) < 0)
        {
            json += QString("\"valid\":false,\"info\":false,\"warn\":false,\"error\":\"%2\"}")
                .arg(QString().fromUtf8(err.c_str(), err.size()));
        }

        FileRegistered* fr = mainwindow->get_file_registered_from_id(file_id);
        if (!fr)
        {
            json += QString("\"valid\":false,\"info\":false,\"warn\":false,\"error\":\"%2\"}")
                .arg(QString().fromUtf8("File has been removed."));
        }

        json += QString("\"valid\":%1,\"info\":%2,\"warn\":%3,\"error\":%4}")
            .arg(fr->policy_valid ? "true" : "false").arg(fr->policy_has_info ? "true" : "false").arg(fr->policy_has_warning ? "true" : "false").arg("null");

        delete fr;
        return json;
    }

    QString WebCommonPage::implementation_is_valid(long file_id)
    {
        //{"valid":true,"fileId":"fileId","error":null}
        QString json = QString("{\"fileId\":\"%1\",").arg(file_id);
        FileRegistered* fr = mainwindow->get_file_registered_from_id(file_id);
        if (!fr)
        {
            json += QString("\"valid\":%1,\"error\":\"%2\"}")
                .arg("false").arg("File not reachable");
            return json;
        }

        if (!fr->analyzed)
        {
            json += QString("\"valid\":%1,\"error\":\"%2\"}")
                .arg("false").arg("File not analyzed");
            delete fr;
            return json;
        }

        json += QString("\"valid\":%1,\"error\":%2}")
            .arg(fr->implementation_valid ? "true" : "false").arg("null");

        delete fr;
        return json;
    }

    QString WebCommonPage::file_is_analyzed(const QStringList& ids)
    {
        QString json = QString("{\"status\":{");
        bool one_added = false;
        for (int i = 0; i < ids.size(); ++i)
        {
            long file = ids[i].toInt();
            FileRegistered* fr = mainwindow->get_file_registered_from_id(file);
            if (!fr)
                continue;

            int report_kind = fr->report_kind;
            if ((report_kind >= 0 && report_kind <= 2) || report_kind == 5) //MC/MI/MT/MMT
                report_kind = 2;

            QString analyzed = QString("\"%1\":{\"finish\":%2,")
                .arg(fr->file_id)                      // id
                .arg(fr->analyzed ? "true" : "false"); // finish

            if (fr->analyzed)
            {
                analyzed += QString("\"tool\":%1")
                    .arg(report_kind);                 // tool
            }
            else
            {
                analyzed += QString("\"percent\":%1")
                    .arg(fr->analyze_percent);         // percent
            }

            if (fr->generated_id.size())
            {
                analyzed += QString(",\"associatedFiles\":{"); // associated files

                for (size_t x = 0; x < fr->generated_id.size(); ++x)
                {
                    if (x)
                        analyzed += ",";

                    std::string filename, err;
                    if (mainwindow->checker_file_from_id(fr->generated_id[x], filename, err) < 0)
                    {
                        mainwindow->set_str_msg_to_status_bar(err);
                        filename = "";
                    }

                    QStringList opt;
                    for (size_t k = 0; k < fr->options.size(); ++k)
                        opt << QString().fromUtf8(fr->options[k].c_str(), fr->options[k].size());
                    if (mainwindow->add_attachment_to_list(QString().fromUtf8(filename.c_str(), filename.size()),
                                                           fr->policy, fr->display, fr->verbosity, opt,
                                                           err) < 0)
                        mainwindow->set_str_msg_to_status_bar(err);

                    analyzed += QString("\"%1\":\"%2\"").arg(QString().number(fr->generated_id[x]))
                        .arg(QString().fromUtf8(filename.c_str(), filename.size()));
                }
                analyzed += QString("}");
            }

            delete fr;
            analyzed += "}";

            if (one_added)
                json += ",";
            json += analyzed;

            one_added = true;
        }

        json += "}}";

        return json;
    }

    void WebCommonPage::add_sub_directory_files_to_list(const QDir dir, QFileInfoList& list)
    {
        QFileInfoList tmp = dir.entryInfoList(QDir::Files);
        list << tmp;

        tmp = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (int i = 0; i < tmp.size(); ++i)
        {
            QDir tmp_dir(tmp[i].absoluteFilePath());
            add_sub_directory_files_to_list(tmp_dir, list);
        }
    }

    QString WebCommonPage::display_add_file(const QString& name)
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

    void WebCommonPage::display_export_id(const QString& name)
    {
        mainwindow->display_export_id(name);
    }

    void WebCommonPage::display_delete_id(const QString& name)
    {
        mainwindow->display_delete_id(name);
    }

    void WebCommonPage::create_rule_tree(XsltPolicyRule *r, QString& rule_data)
    {
        if (!r)
            return;

        rule_data = "{";
        rule_data += QString("\"id\":%1").arg(r->id);

        int len = r->node_name.length();
        if (len > 0)
        {
            QString name = QString().fromUtf8(r->node_name.c_str(), r->node_name.length());
            string_to_json(name);
            rule_data += QString(",\"name\":\"%1\"").arg(name);
        }
        else
            rule_data += ",\"name\":\"\"";

        len = r->track_type.length();
        if (len > 0)
        {
            QString type = QString().fromUtf8(r->track_type.c_str(), r->track_type.length());
            string_to_json(type);
            rule_data += QString(",\"tracktype\":\"%1\"").arg(type);
        }
        else
            rule_data += ",\"tracktype\":\"\"";

        len = r->field.length();
        if (len > 0)
        {
            QString field = QString().fromUtf8(r->field.c_str(), r->field.length());
            string_to_json(field);
            rule_data += QString(",\"field\":\"%1\"").arg(field);
        }
        else
            rule_data += ",\"field\":\"\"";

        len = r->occurrence.length();
        if (len > 0)
        {
            QString occurrence = QString().fromUtf8(r->occurrence.c_str(), r->occurrence.length());
            string_to_json(occurrence);
            rule_data += QString(",\"occurrence\":\"%1\"").arg(occurrence);
        }
        else
            rule_data += ",\"occurrence\":\"*\"";

        len = r->ope.length();
        if (len > 0)
        {
            QString validator = QString().fromUtf8(r->ope.c_str(), r->ope.length());
            string_to_json(validator);
            rule_data += QString(",\"ope\":\"%1\"").arg(validator);
        }
        else
            rule_data += ",\"ope\":\"\"";

        len = r->value.length();
        if (len > 0)
        {
            QString value = QString().fromUtf8(r->value.c_str(), r->value.length());
            string_to_json(value);
            rule_data += QString(",\"value\":\"%1\"").arg(value);
        }
        else
            rule_data += ",\"value\":\"\"";

        len = r->scope.length();
        if (len > 0)
        {
            QString scope = QString().fromUtf8(r->scope.c_str(), r->scope.length());
            string_to_json(scope);
            rule_data += QString(",\"scope\":\"%1\"").arg(scope);
        }
        else
            rule_data += ",\"scope\":\"\"";

        len = r->level.length();
        if (len > 0)
        {
            QString level = QString().fromUtf8(r->level.c_str(), r->level.length());
            string_to_json(level);
            rule_data += QString(",\"level\":\"%1\"").arg(level);
        }
        else
            rule_data += ",\"level\":\"\"";

        rule_data += "}";
    }

    QString WebCommonPage::get_policies_tree()
    {
        MediaConchLib::Get_Policies policies;
        QString res;
        QString err;

        if (mainwindow->get_policies("JSTREE", policies, err) < 0)
        {
            string_to_json(err);
            res = "{\"error\":\"" + err + "\"}";
            return res;
        }

        if (policies.format != "JSTREE")
        {
            res = "{\"error\":\"Cannot generate the policies tree\"}";
            return res;
        }

        res = QString().fromUtf8(policies.jstree->c_str(), policies.jstree->length());
        return res;
    }

    QString WebCommonPage::policy_get_xml(int id)
    {
        std::string policy;
        QString err;
        if (mainwindow->policy_dump(id, policy, err) < 0)
            return err;

        return QString().fromUtf8(policy.c_str());
    }

    QString WebCommonPage::policy_get_jstree(int id)
    {
        QString json;
        QString err;

        MediaConchLib::Get_Policy p;
        if (mainwindow->policy_get(id, "JSTREE", p, err) < 0)
        {
            json = QString("{\"error\":\"") + err + "\"}";
            return json;
        }

        if (p.format != "JSTREE")
        {
            json = "{\"error\":\"Cannot generate the policy\"}";
            return json;
        }

        json = QString().fromUtf8(p.jstree->c_str(), p.jstree->length());
        return json;
    }

    QString WebCommonPage::import_policy()
    {
        QString file;
        QStringList list = file_selector["xslPolicyImport[policyFile]"];
        if (list.size())
            file = list.last().toUtf8().data();

        if (!file.length())
            return QString("{\"error\":\"No file selected\"}");

        file_selector.clear();

        return import_policy(file);
    }

    QString WebCommonPage::import_policy(const QString& file)
    {

        QString json;
        QString err;
        int id;
        if ((id = mainwindow->policy_import(file, err)) < 0)
        {
            string_to_json(err);
            json = QString("{\"error\":\"%1\"}").arg(err);
            return json;
        }

        mainwindow->policy_save(id, err);

        MediaConchLib::Get_Policy p;
        if (mainwindow->policy_get(id, "JSTREE", p, err) < 0)
        {
            string_to_json(err);
            json = QString("{\"error\":\"") + err + "\"}";
            return json;
        }

        if (p.format != "JSTREE")
        {
            json = "{\"error\":\"Cannot generate the policy\"}";
            return json;
        }

        json = QString().fromUtf8(p.jstree->c_str(), p.jstree->length());
        return json;
    }

    QString WebCommonPage::policy_from_file()
    {
        QString file;
        QStringList list = file_selector["xslPolicyCreateFromFile[file]"];
        if (list.size())
            file = list.last().toUtf8().data();

        if (!file.size())
            return QString("{\"error\":\"No file selected\"}");

        file_selector.clear();

        std::string error;
        QFileInfo f = QFileInfo(file);
        QStringList options;
        if (mainwindow->add_file_to_list(f.fileName(), f.absolutePath(),
                                         QString().setNum(mainwindow->select_correct_policy()),
                                         QString().setNum(mainwindow->select_correct_display()),
                                         QString().setNum(mainwindow->select_correct_verbosity()), false, true,
                                         options, error) < 0)
            return QString("{\"error\":\"%1\"").arg(QString().fromUtf8(error.c_str(), error.size()));

        return QString("{}");
    }

    QString WebCommonPage::policy_import_data(const QString& data)
    {

        QString json;
        QString err;
        int id;
        if ((id = mainwindow->policy_import_data(data, err)) < 0)
        {
            string_to_json(err);
            json = QString("{\"error\":\"%1\"}").arg(err);
            return json;
        }

        mainwindow->policy_save(id, err);

        json = QString("{\"policyId\":%1}").arg(id);

        return json;
    }

    int WebCommonPage::import_policy(const QStringList& files)
    {
        int ret = 0;

        QString script;
        for (int i = 0; i < files.size(); ++i)
        {
            QString tmp = import_policy(files[i]);
            if (tmp.startsWith("{\"error\":"))
                ret = -1;
            else
                script += QString("policyTreeAjax.policyDropped(%1);").arg(tmp);
        }
        if (script.length())
            use_javascript(script);
        return ret;
    }

    QString WebCommonPage::xslt_policy_create(int parent_id)
    {
        QString json;
        QString err;

        int id;
        if ((id = mainwindow->xslt_policy_create(parent_id, err)) < 0)
        {
            string_to_json(err);
            json = QString("{\"error\":\"%1\"}").arg(err);
            return json;
        }

        mainwindow->policy_save(id, err);

        MediaConchLib::Get_Policy p;
        if (mainwindow->policy_get(id, "JSTREE", p, err) < 0)
        {
            json = QString("{\"error\":\"") + err + "\"}";
            return json;
        }

        if (p.format != "JSTREE")
        {
            json = "{\"error\":\"Cannot generate the policy\"}";
            return json;
        }

        json = QString().fromUtf8(p.jstree->c_str(), p.jstree->length());
        return json;
    }

    QString WebCommonPage::policy_duplicate(int id, int dst_policy_id)
    {
        QString json;
        QString err;
        int ret = -1;

        if ((ret = mainwindow->policy_duplicate(id, dst_policy_id, err)) < 0)
        {
            string_to_json(err);
            json = QString("{\"error\":\"%1\"}").arg(err);
            return json;
        }
        mainwindow->policy_save(ret, err);

        MediaConchLib::Get_Policy p;
        if (mainwindow->policy_get(ret, "JSTREE", p, err) < 0)
        {
            json = QString("{\"error\":\"") + err + "\"}";
            return json;
        }

        if (p.format != "JSTREE")
        {
            json = "{\"error\":\"Cannot generate the policy\"}";
            return json;
        }

        json = QString().fromUtf8(p.jstree->c_str(), p.jstree->length());
        return json;
    }

    QString WebCommonPage::policy_move(int id, int dst_policy_id)
    {
        QString json;
        QString err;
        int ret = -1;

        if ((ret = mainwindow->policy_move(id, dst_policy_id, err)) < 0)
        {
            string_to_json(err);
            json = QString("{\"error\":\"%1\"}").arg(err);
            return json;
        }

        mainwindow->policy_save(ret, err);

        MediaConchLib::Get_Policy p;
        if (mainwindow->policy_get(ret, "JSTREE", p, err) < 0)
        {
            string_to_json(err);
            json = QString("{\"error\":\"") + err + "\"}";
            return json;
        }

        if (p.format != "JSTREE")
        {
            json = "{\"error\":\"Cannot generate the policy\"}";
            return json;
        }

        json = QString().fromUtf8(p.jstree->c_str(), p.jstree->length());
        return json;
    }

    QString WebCommonPage::policy_remove(int id)
    {
        //return:
        QString json;

        QString err;
        int code;
        if ((code = mainwindow->policy_remove(id, err)) < 0)
        {
            string_to_json(err);
            json = QString("{\"error\":\"%1\"}").arg(err);
            return json;
        }

        json = QString("{}");
        return json;
    }

    QString WebCommonPage::policy_export(int id)
    {
        //return: error?
        QString json;
        QString err;
        int code;
        if ((code = mainwindow->policy_export((size_t)id, err)) < 0)
        {
            string_to_json(err);
            json = QString("{\"error\":\"%1\"}").arg(err);
            return json;
        }

        json = QString("{}");
        return json;
    }

    QString WebCommonPage::policy_export_data(const QString& report)
    {
        QString json;
        QString err;
        int code;
        if ((code = mainwindow->policy_export_data(report, "Policy", err)) < 0)
        {
            string_to_json(err);
            json = QString("{\"error\":\"%1\"}").arg(err);
            return json;
        }

        json = QString("{}");
        return json;
    }

    QString WebCommonPage::policy_edit(int id, const QString& name, const QString& description, const QStringList& tags, const QString& license,
                                       const QString& type, const QString& level, const QString&)
    {
        //return: error?
        QString json;
        QString err;
        int code;

        std::vector<std::string> tgs;
        for(int i = 0; i < tags.size(); ++i)
            tgs.push_back(tags[i].toUtf8().data());

        if ((code = mainwindow->policy_change_info((size_t)id, name.toUtf8().data(), description.toUtf8().data(),
                                                   tgs, level.toUtf8().data(), license.toUtf8().data(), err)) < 0)
        {
            string_to_json(err);
            json = QString("{\"error\":\"%1\"}").arg(err);
            return json;
        }

        if (mainwindow->policy_change_type((size_t)id, type.toUtf8().data(), err) < 0)
        {
            string_to_json(err);
            json = QString("{\"error\":\"%1\"}").arg(err);
            return json;
        }

        //TODO: when publipolicies API is ready
        // bool is_public = false;
        // if (visibility == "1")
        //     is_public = true;
        // if (mainwindow->policy_change_is_public(id, is_public, err) < 0)
        // {
        //     QString error = QString().fromUtf8(err.c_str(), err.length());
        //     string_to_json(error);
        //     json = QString("{\"error\":\"%1\"}").arg(error);
        //     return json;
        // }

        mainwindow->policy_save(id, err);

        MediaConchLib::Get_Policy p;
        if (mainwindow->policy_get(id, "JSTREE", p, err) < 0)
        {
            json = QString("{\"error\":\"") + err + "\"}";
            return json;
        }

        if (p.format != "JSTREE")
        {
            json = "{\"error\":\"Cannot generate the policy\"}";
            return json;
        }

        json = QString().fromUtf8(p.jstree->c_str(), p.jstree->length());
        return json;
    }

    QString WebCommonPage::xslt_policy_rule_create(int policy_id)
    {
        QString err;
        QString json;
        int new_rule_id = -1;
        if ((new_rule_id = mainwindow->xslt_policy_rule_create(policy_id, err)) < 0)
        {
            string_to_json(err);
            json = QString("{\"error\":\"%1\"}").arg(err);
            return json;
        }

        mainwindow->policy_save(policy_id, err);

        XsltPolicyRule* r = mainwindow->xslt_policy_rule_get(policy_id, new_rule_id, err);
        if (!r)
        {
            json = QString("{\"error\":\"") + err + "\"}";
            return json;
        }

        QString rule_data;
        create_rule_tree(r, rule_data);
        json = QString("{\"rule\":");
        json += rule_data;
        json += "}";
        return json;
    }

    QString WebCommonPage::xslt_policy_rule_edit(int rule_id, int policy_id, const QString& title, const QString& type, const QString& field, const QString& occurrence, const QString& ope, const QString& value, const QString& scope, const QString& level)
    {
        QString err;
        QString json;

        XsltPolicyRule rule;
        rule.node_name     = title.toUtf8().data();
        rule.ope           = ope.toUtf8().data();
        rule.track_type    = type.toUtf8().data();
        rule.field         = field.toUtf8().data();
        rule.occurrence    = occurrence.toUtf8().data();
        rule.value         = value.toUtf8().data();
        rule.scope         = scope.toUtf8().data();
        rule.level         = level.toUtf8().data();

        int code;
        if ((code = mainwindow->xslt_policy_rule_edit(policy_id, rule_id, &rule, err)) < 0)
        {
            string_to_json(err);
            json = QString("{\"error\":\"%1\"}").arg(err);
            return json;
        }

        mainwindow->policy_save(policy_id, err);

        XsltPolicyRule* r = mainwindow->xslt_policy_rule_get(policy_id, rule_id, err);
        if (!r)
        {
            json = "{\"error\":\"Cannot get the policy rule edited\"}";
            return json;
        }

        QString rule_data;
        create_rule_tree(r, rule_data);
        json = QString("{\"rule\":");
        json += rule_data;
        json += "}";
        return json;
    }

    QString WebCommonPage::xslt_policy_rule_duplicate(int policy_id, int rule_id, int dst_policy_id)
    {
        //return: rule
        QString err;
        QString json;
        int new_rule_id = -1;
        if ((new_rule_id = mainwindow->xslt_policy_rule_duplicate(policy_id, rule_id, dst_policy_id, err)) < 0)
        {
            string_to_json(err);
            json = QString("{\"error\":\"%1\"}").arg(err);
            return json;
        }

        mainwindow->policy_save(dst_policy_id, err);

        XsltPolicyRule* r = mainwindow->xslt_policy_rule_get(dst_policy_id, new_rule_id, err);
        if (!r)
        {
            json = "{\"error\":\"Cannot get the policy rule duplicated\"}";
            return json;
        }

        QString rule_data;
        create_rule_tree(r, rule_data);
        json = QString("{\"rule\":");
        json += rule_data;
        json += "}";
        return json;
    }

    QString WebCommonPage::xslt_policy_rule_move(int policy_id, int rule_id, int dst_policy_id)
    {
        //return: rule
        QString err;
        QString json;
        int new_rule_id = -1;
        if ((new_rule_id = mainwindow->xslt_policy_rule_move(policy_id, rule_id, dst_policy_id, err)) < 0)
        {
            string_to_json(err);
            json = QString("{\"error\":\"%1\"}").arg(err);
            return json;
        }

        mainwindow->policy_save(policy_id, err);
        mainwindow->policy_save(dst_policy_id, err);

        XsltPolicyRule* r = mainwindow->xslt_policy_rule_get(dst_policy_id, new_rule_id, err);
        if (!r)
        {
            json = "{\"error\":\"Cannot get the policy rule moved\"}";
            return json;
        }

        QString rule_data;
        create_rule_tree(r, rule_data);
        json = QString("{\"rule\":");
        json += rule_data;
        json += "}";
        return json;
    }

    QString WebCommonPage::xslt_policy_rule_delete(int policy_id, int rule_id)
    {
        QString err;
        QString json;
        int code;

        if ((code = mainwindow->xslt_policy_rule_delete(policy_id, rule_id, err)) < 0)
        {
            string_to_json(err);
            json = QString("{\"error\":\"%1\"}").arg(err);
            return json;
        }

        mainwindow->policy_save(policy_id, err);

        json = "{}";
        return json;
    }

    QString WebCommonPage::get_fields_list(const QString& type, const QString& field)
    {
        //return: {values:[value,value]}
        QString json;

        std::vector<std::string> fields;
        if (mainwindow->get_fields_for_type(type.toUtf8().data(), fields) < 0)
        {
            json = "{\"error\":\"Cannot get the fields\"}";
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

    QString WebCommonPage::get_values_list(const QString& type, const QString& field, const QString& value)
    {
        //return: {values:[value,value]}
        QString json;

        std::vector<std::string> values;
        if (mainwindow->get_values_for_type_field(type.toUtf8().data(), field.toUtf8().data(), values) < 0)
        {
            json = "{\"error\":\"Cannot get the values\"}";
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

    QString WebCommonPage::checker_list()
    {
        std::vector<long> files;
        QString error;
        mainwindow->checker_list(files, error);
        QString ret("{\"files\":[");
        for (size_t i = 0; i < files.size(); ++i)
        {
            MediaConchLib::Checker_FileInfo fi;
            std::string err;
            if (mainwindow->checker_file_information(files[i], fi, err) < 0)
                error = QString().fromUtf8(err.c_str(), err.size());

            if (i)
                ret += ",";

            QString file = QString().fromUtf8(fi.filename.c_str(), fi.filename.size());
            string_to_json(file);
            ret += QString("{\"name\":\"%1\",\"id\":%2}").arg(file).arg(files[i]);
        }
        ret += "]";

        if (error.size())
            ret += QString(",\"error\":\"%1\"").arg(error);
        ret += "}";

        return ret;
    }

    QString WebCommonPage::remove_file_from_db(long id)
    {
        QString ret("{");
        QString error;
        if (mainwindow->checker_clear(id, error) < 0)
        {
            string_to_json(error);
            ret += QString("\"error\":\"%1\"").arg(error);
        }

        ret += "}";
        return ret;
    }

    QString WebCommonPage::add_file_to_checker_again(long id)
    {
        QString json("{");
        std::string err;
        int ret = mainwindow->add_file_to_list(id, err);
        if (ret < 0)
        {
            QString error = QString().fromUtf8(err.c_str(), err.size());
            string_to_json(error);
            json += QString("\"error\":\"%1\"").arg(error);
        }
        else if (ret > 0)
            json += QString("\"success\":\"File already in the checker\"");
        else
            json += QString("\"success\":\"File added to the checker\"");

        json += "}";
        return json;
    }

    QString WebCommonPage::remove_all_files_from_db()
    {
        QString ret("{");
        QString error;
        if (mainwindow->checker_clear(error) < 0)
        {
            string_to_json(error);
            ret += QString("\"error\":\"%1\"").arg(error);
        }

        ret += "}";
        return ret;
    }

    void WebCommonPage::string_to_json(QString& str)
    {
        str.replace("\"", "\\\"");
        str.replace("\n", "\\n");
        str.replace("\r", "");
        str.replace("\t", " ");
    }

    void WebCommonPage::call_tooltip(const QString& link)
    {
        QDesktopServices::openUrl(link);
    }

    QString WebCommonPage::get_version()
    {
        std::string version;
        mainwindow->get_version(version);
        return QString().fromUtf8(version.c_str(), version.size());
    }

    QString WebCommonPage::get_mco_token()
    {
        UiSettings& settings = mainwindow->get_settings();
        std::string token = settings.get_mco_token();
        return QString().fromUtf8(token.c_str(), token.size());
    }

    QString WebCommonPage::save_mco_token(const QString& token)
    {
        UiSettings& settings = mainwindow->get_settings();
        settings.change_mco_token(token.toUtf8().data());
        return QString("{}");
    }

    void WebCommonPage::set_parse_speed(const QString& parse_speed)
    {
        checker_parse_speed = parse_speed;
    }
}
