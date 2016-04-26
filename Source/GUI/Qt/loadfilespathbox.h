/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef LOADFILESPATHBOX_H
#define LOADFILESPATHBOX_H

//---------------------------------------------------------------------------
#include <QMessageBox>

namespace MediaConch {

class LoadFilesPathBox : public QMessageBox
{
    Q_OBJECT

public:
    LoadFilesPathBox(const QString& value);
    ~LoadFilesPathBox();

    int          exec();
    QString      get_path();

private:
    QString      path;
    QPushButton *get_directory;
    QPushButton *use_last;
};

}

#endif // LOADFILESPATHBOX_H
