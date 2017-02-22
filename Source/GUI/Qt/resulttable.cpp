/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "resulttable.h"
#include "mainwindow.h"
#include "Common/FileRegistered.h"
#include <QString>
#include <QProgressBar>
#include <QTimer>
#include <QDebug>

#include "WebView.h"
#include "WebPage.h"


#include "progressbar.h"

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

//---------------------------------------------------------------------------
ResultTable::ResultTable(MainWindow* m, WebPage *p) : mainwindow(m), page(p)
{
}

//---------------------------------------------------------------------------
ResultTable::~ResultTable()
{
}

//---------------------------------------------------------------------------
void ResultTable::add_file_to_result_table(const std::string& full_path)
{
    FileRegistered* file = mainwindow->get_file_registered_from_file(full_path);
    if (!file)
        return;

    std::string filename = file->filename;
    QUrl url(QString().fromUtf8(filename.c_str(), filename.length()));
    QFileInfo info(url.path());

    QString policyName;
    std::string name;
    QString err;
    if (file->policy != -1 && mainwindow->policy_get_name(file->policy, name, err) >= 0 && name.size())
        policyName = QString().fromUtf8(name.c_str(), name.length());
    else
        policyName = "N/A";
    // Make the javascript structure Arghh!
    QString formValues = QString("{\"policy\":\"");
    formValues += QString().number(file->policy);    // Policy
    formValues += QString("\",\"policyText\":\"");
    formValues += policyName;               // PolicyName/PolicyText
    formValues += "\",\"display\":\"";
    formValues += QString().number(file->display);            // Display
    formValues += "\",\"verbosity\":\"";
    formValues += QString().number(file->verbosity); // Verbosity
    formValues += "\"}";

    QString script = QString("updateFileOrAddFile('");
    script += QString().fromUtf8(full_path.c_str(), full_path.length());
    script += "', '";
    script += info.fileName();
    script += "', '";
    script += QString().number(file->file_id);
    script += "', '";
    script += formValues;
    script += "');";
    page->use_javascript(script);
}

}
