/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
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
        return report;
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
        return report;
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

    QString WebCommonPage::on_fill_mediainfo_report(long file_id)
    {
        std::string err;
        return mainwindow->get_mediainfo_jstree(file_id, err);
    }

    void WebCommonPage::on_save_mediainfo_report(long file_id)
    {
        std::string display_name, display_content;
        std::string err;
        QString report = mainwindow->get_mediainfo_xml(file_id, display_name, display_content, err);
        on_download_report(report, file_id, "MediaInfo");
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
        if (mainwindow->policy_get_name(id, n, err) < 0)
            n = std::string();
        QString name = QString().fromUtf8(n.c_str(), n.length());

        QString script = QString("{\"policyId\":%1, \"policyName\":\"%2\"}")
                             .arg(id)
                             .arg(name);
        return script;
    }

    QString WebCommonPage::on_file_upload_selected(const QString& policy, const QString& display,
                                                const QString& verbosity, bool fixer)
    {
        int ret = 0;
        QStringList files = file_selector.value("checkerUpload_file", QStringList());

        if (!files.size())
            return QString();

        for (int i = 0; i < files.size(); ++i)
        {
            QFileInfo f = QFileInfo(files[i]);
            std::string error;
            if (mainwindow->add_file_to_list(f.fileName(), f.absolutePath(), policy,
                                             display, verbosity, fixer, error) < 0)
                return QString("%1\n").arg(QString().fromUtf8(error.c_str(), error.size()));
        }
        file_selector.clear();
        clean_forms();
        use_javascript(QString("startWaitingLoop()"));

        return QString();
    }

    QString WebCommonPage::on_file_online_selected(const QString& url, const QString& policy,
                                                   const QString& display, const QString& verbosity,
                                                   bool fixer)
    {
        QString ret;
        if (!url.length())
            return ret;

        std::string error;
        if (mainwindow->add_file_to_list(url, "", policy, display, verbosity, fixer, error) < 0)
            ret += QString("%1\n").arg(QString().fromUtf8(error.c_str(), error.size()));
        file_selector.clear();
        clean_forms();
        use_javascript(QString("startWaitingLoop()"));
        return ret;    }

    QString WebCommonPage::on_file_repository_selected(const QString& policy, const QString& display,
                                                    const QString& verbosity, bool fixer)
    {
        QString ret;
        QStringList dirname = file_selector.value("checkerRepository_directory", QStringList());
        if (dirname.empty())
            return ret;

        QDir dir(dirname.last());
        QFileInfoList list;
        add_sub_directory_files_to_list(dir, list);
        if (!list.count())
            return ret;

        for (int i = 0; i < list.size(); ++i)
        {
            std::string error;
            if (mainwindow->add_file_to_list(list[i].fileName(), list[i].absolutePath(), policy,
                                             display, verbosity, fixer, error))
                ret += QString("%1\n").arg(QString().fromUtf8(error.c_str(), error.size()));
        }
        file_selector.clear();
        clean_forms();
        use_javascript(QString("startWaitingLoop()"));

        return ret;
    }

    void WebCommonPage::on_save_settings_selected(const QString& policy,
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

    void WebCommonPage::close_all()
    {
        mainwindow->clear_file_list();
    }

    void WebCommonPage::close_element(const QString& file)
    {
        mainwindow->remove_file_to_list(file);
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
        QString value_input = QFileDialog::getOpenFileName(mainwindow, "Open file", suggested, "XML file (*.xml);;XSL file (*.xsl);;All (*.*)", 0, QFileDialog::DontUseNativeDialog);

        QMap<QString, QStringList>::iterator it = file_selector.find(select_file_name);
        if (it != file_selector.end())
            file_selector.erase(it);

        if (!value_input.length())
            return QString();

        mainwindow->set_last_load_policy_path(value_input.toUtf8().data());
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

        QString err = on_file_upload_selected(QString().setNum(mainwindow->select_correct_policy()),
                                              QString().setNum(mainwindow->select_correct_display()),
                                              QString().setNum(mainwindow->select_correct_verbosity()),
                                              false);
        if (err.size())
            mainwindow->set_str_msg_to_status_bar(err.toUtf8().data());
    }

    //---------------------------------------------------------------------------
    bool WebCommonPage::report_is_html(const QString& report)
    {
        QRegExp reg("<\\!DOCTYPE.*html", Qt::CaseInsensitive);

        if (reg.indexIn(report, 0) != -1)
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

    QString WebCommonPage::policy_is_valid(long file_id)
    {
        //{"valid":false,"fileId":"fileId","error":null}
        std::string err;
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

        bool policy_valid = false;
        std::vector<MediaConchLib::Checker_ValidateRes*> res;
        if (mainwindow->validate_policy(file_id, fr->policy, res, err) < 0)
        {
            json += QString("\"valid\":false,\"error\":\"%2\"}")
                .arg(QString().fromUtf8(err.c_str(), err.size()));
            delete fr;
            return json;
        }

        if (res.size() == 1)
            policy_valid = res[0]->valid;

        for (size_t j = 0; j < res.size() ; ++j)
            delete res[j];
        res.clear();

        json += QString("\"valid\":%1,\"error\":%2}")
            .arg(policy_valid ? "true" : "false").arg("null");

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

    QString WebCommonPage::implementation_and_policy_is_valid(long file_id)
    {
        //{"implemReport":{"valid":true,"fileId":"fileId","error":null},"statusReport":{"valid":false,"fileId":"fileId","error":null}}
        QString json = QString("{\"implemReport\":");
        json += implementation_is_valid(file_id);
        json += ",\"statusReport\":";
        json += policy_is_valid(file_id);
        json += "}";
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

            QString analyzed = QString("\"%1\": {\"finish\":%2,\"tool\":%3,\"percent\":%4}")
                .arg(fr->file_id)                     // id
                .arg(fr->analyzed ? "true" : "false") // finish
                .arg(report_kind)                     // tool
                .arg(fr->analyze_percent);            // percent
            delete fr;

            if (one_added)
                json += ",";
            json += analyzed;

            one_added = true;
        }

        json += "}}";

        return json;
    }

    void WebCommonPage::change_policy_for_file(long file_id, const QString& policy)
    {
        mainwindow->update_policy_of_file_in_list(file_id, policy);
    }

    void WebCommonPage::add_sub_directory_files_to_list(const QDir dir, QFileInfoList& list)
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

        if (r->occurrence >= 0)
            rule_data += QString(",\"occurrence\":%1").arg(r->occurrence);
        else
            rule_data += ",\"occurrence\":\"-1\"";

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

    QString WebCommonPage::policy_edit(int id, const QString& name, const QString& description, const QString& license,
                                       const QString& type, const QString&)
    {
        //return: error?
        QString json;
        QString err;
        int code;
        if ((code = mainwindow->policy_change_info((size_t)id, name.toUtf8().data(), description.toUtf8().data(),
                                                   license.toUtf8().data(), err)) < 0)
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

    QString WebCommonPage::xslt_policy_rule_edit(int rule_id, int policy_id, const QString& title, const QString& type, const QString& field, int occurrence, const QString& ope, const QString& value, const QString& scope)
    {
        QString err;
        QString json;

        XsltPolicyRule rule;
        rule.node_name     = title.toUtf8().data();
        rule.ope           = ope.toUtf8().data();
        rule.track_type    = type.toUtf8().data();
        rule.field         = field.toUtf8().data();
        rule.occurrence    = occurrence;
        rule.value         = value.toUtf8().data();
        rule.scope         = scope.toUtf8().data();

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

    void WebCommonPage::string_to_json(QString& str)
    {
        str.replace("\"", "\\\"");
        str.replace("\n", "\\\n");
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
}
