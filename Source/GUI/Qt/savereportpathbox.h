/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef SAVEREPORTPATHBOX_H
#define SAVEREPORTPATHBOX_H

//---------------------------------------------------------------------------
#include <QMessageBox>

namespace MediaConch {

class SaveReportPathBox : public QMessageBox
{
public:
    SaveReportPathBox(const QString& value);
    ~SaveReportPathBox();

    int          exec();
    QString      get_path();

private:
    QString      path;
    QPushButton *get_directory;
    QPushButton *use_last;
};

}

#endif // SAVEREPORTPATHBOX_H
