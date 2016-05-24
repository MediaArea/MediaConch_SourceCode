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
        connect(this, SIGNAL(loadFinished(bool)), this, SLOT(on_load_finished(bool)));
    }

    void WebPage::on_load_finished(bool)
    {
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

        QDir info(QFileInfo(dl_file).absoluteDir());
        mainwindow->set_last_save_report_path(info.absolutePath().toUtf8().data());

        QFile file(dl_file);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        QTextStream out(&file);
        out << report;
    }

    QString WebPage::on_fill_implementation_report(const QString& file, const QString& display, const QString& verbosity)
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
        return report;
    }

    void WebPage::on_save_implementation_report(const QString& file, const QString& display, const QString& verbosity)
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
        on_download_report(report, file, "ImplementationReport");
    }

    QString WebPage::on_fill_policy_report(const QString& file, const QString& policy, const QString& display)
    {
        int policy_i = policy.toInt();
        QString report;
        if (policy_i != -1)
        {
            std::string file_s = file.toStdString();
            int display_i = display.toInt();
            mainwindow->validate_policy(file_s, report, policy_i, &display_i);
        }
        return report;
    }

    void WebPage::on_save_policy_report(const QString& file, const QString& policy, const QString& display)
    {
        std::string file_s = file.toStdString();
        QString report;
        int policy_i = policy.toInt();
        int display_i = display.toInt();
        mainwindow->validate_policy(file_s, report, policy_i, &display_i);
        on_download_report(report, file, "MediaConchReport");
    }

    QString WebPage::on_fill_mediainfo_report(const QString& file)
    {
        std::string file_s = file.toStdString();
        return mainwindow->get_mediainfo_jstree(file_s);
    }

    void WebPage::on_save_mediainfo_report(const QString& file)
    {
        std::string file_s = file.toStdString();
        std::string display_name, display_content;
        QString report = mainwindow->get_mediainfo_xml(file_s, display_name, display_content);
        on_download_report(report, file, "MediaInfo");
    }

    QString WebPage::on_fill_mediatrace_report(const QString& file)
    {
        std::string file_s = file.toStdString();
        return mainwindow->get_mediatrace_jstree(file_s);
    }

    void WebPage::on_save_mediatrace_report(const QString& file)
    {
        std::string file_s = file.toStdString();
        std::string display_name, display_content;
        QString report = mainwindow->get_mediatrace_xml(file_s, display_name, display_content);
        on_download_report(report, file, "MediaTrace");
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
        else if (policy == "-1")
            settings.change_default_policy("");
        else
        {
            Policy* p = mainwindow->get_policy((size_t)policy.toInt());
            if (p)
                settings.change_default_policy(p->filename);
            else
                settings.change_default_policy("");
        }

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

        runJavaScript(QString("document.getElementById('checkerUpload_policy').value = %1;").arg(policy_i));
        runJavaScript(QString("document.getElementById('checkerUpload_display_selector').value = %1;").arg(display_i));
        runJavaScript(QString("document.getElementById('checkerUpload_verbosity_selector').value = %1;").arg(verbosity_i));
        runJavaScript("document.getElementById('checkerUpload_file').value = \"\";");

#if defined(MEDIAINFO_LIBCURL_YES)

        runJavaScript(QString("document.getElementById('checkerOnline_policy').value = %1;").arg(policy_i));
        runJavaScript(QString("document.getElementById('checkerOnline_display_selector').value = %1;").arg(display_i));
        runJavaScript(QString("document.getElementById('checkerOnline_verbosity_selector').value = %1;").arg(verbosity_i));
        runJavaScript("document.getElementById('checkerOnline_file').value = \"\";");

#endif

        runJavaScript(QString("document.getElementById('checkerRepository_policy').value = %1;").arg(policy_i));
        runJavaScript(QString("document.getElementById('checkerRepository_display_selector').value = %1;").arg(display_i));
        runJavaScript(QString("document.getElementById('checkerRepository_verbosity_selector').value = %1;").arg(verbosity_i));
        runJavaScript("document.getElementById('checkerRepository_directory').value = \"\";");
    }

    void WebPage::on_file_upload_selected(const QString& policy, const QString& display_xslt, const QString& verbosity)
    {
        QStringList files = file_selector.value("checkerUpload_file", QStringList());

        if (!files.size())
            return;

        for (int i = 0; i < files.size(); ++i)
        {
            QFileInfo f = QFileInfo(files[i]);
            mainwindow->add_file_to_list(f.fileName(), f.absolutePath(), policy, display_xslt, verbosity);
        }
        file_selector.clear();
        clean_forms();
    }

    void WebPage::on_file_online_selected(const QString& url, const QString& policy, const QString& display_xslt, const QString& verbosity)
    {
        if (!url.length())
            return;

        mainwindow->add_file_to_list(url, "", policy, display_xslt, verbosity);
        file_selector.clear();
        clean_forms();
    }

    void WebPage::on_file_repository_selected(const QString& policy, const QString& display_xslt, const QString& verbosity)
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
        file_selector.clear();
        clean_forms();
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

    QStringList WebPage::choose_file_settings()
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
            return QStringList();

        QString suggested = QString().fromUtf8(suggested_str.c_str(), suggested_str.length());
        QString value_input = QFileDialog::getExistingDirectory(view(), NULL, suggested);

        QMap<QString, QStringList>::iterator it = file_selector.find(select_file_name);
        if (it != file_selector.end())
            file_selector.erase(it);

        if (!value_input.length())
            return QStringList();

        file_selector.insert(select_file_name, QStringList(value_input));

        QStringList list;
        list = QStringList(value_input);
        return list;
    }

    QStringList WebPage::chooseFiles(FileSelectionMode mode, const QStringList &, const QStringList&)
    {
        if (select_file_name.startsWith("settings_"))
            return choose_file_settings();

        QStringList list;
        std::string suggested_str = mainwindow->select_correct_load_files_path();
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
            QMap<QString, QStringList>::iterator it = file_selector.find("checkerUpload_file");
            if (it != file_selector.end())
                file_selector["checkerUpload_file"] = names;
            else
                file_selector.insert("checkerUpload_file", names);
        }

        return list;
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

    void WebPage::change_local_files(QStringList& files)
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

        on_file_upload_selected(QString().setNum(mainwindow->select_correct_policy()),
                                QString().setNum(mainwindow->select_correct_display()),
                                QString().setNum(mainwindow->select_correct_verbosity()));
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

}

#endif
