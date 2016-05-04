/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "savedisplaypathbox.h"
#include <QPushButton>
#include <QFileDialog>

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

//---------------------------------------------------------------------------
SaveDisplayPathBox::SaveDisplayPathBox(const QString& value) : QMessageBox(NULL), path(value)
{
    setText("Default Path to save display:");
    if (!path.length() || path == "last")
        path = "Last used path for saving display";
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setInformativeText(path);

    setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    get_directory = new QPushButton("&Get directory path");
    addButton(get_directory, QMessageBox::ActionRole);

    use_last = new QPushButton("&Use last directory");
    addButton(use_last, QMessageBox::ActionRole);

    setDefaultButton(QMessageBox::Ok);
}

//---------------------------------------------------------------------------
SaveDisplayPathBox::~SaveDisplayPathBox()
{
    delete get_directory;
    delete use_last;
}

//---------------------------------------------------------------------------
int SaveDisplayPathBox::exec()
{
    bool loop = true;
    QString new_path = path;
    while (loop)
    {
        int ret = QMessageBox::exec();
        if (ret == QMessageBox::Ok)
            path = new_path;
        else if (ret == QMessageBox::Cancel)
            new_path = path;
        else if (clickedButton() == get_directory)
        {
            new_path = QFileDialog::getExistingDirectory(this, NULL, new_path);
            setInformativeText(new_path);
            continue;
        }
        else if (clickedButton() == use_last)
        {
            new_path = "Last used path for saving display";
            setInformativeText(new_path);
            continue;
        }
        loop = false;
    }
    return 0;
}

//---------------------------------------------------------------------------
QString SaveDisplayPathBox::get_path()
{
    if (!path.length() || path == "Last used path for saving display")
        return QString("last");
    return path;
}

}
