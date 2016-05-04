/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef SAVEDISPLAYPATHBOX_H
#define SAVEDISPLAYPATHBOX_H

//---------------------------------------------------------------------------
#include <QMessageBox>

namespace MediaConch {

class SaveDisplayPathBox : public QMessageBox
{
public:
    SaveDisplayPathBox(const QString& value);
    ~SaveDisplayPathBox();

    int          exec();
    QString      get_path();

private:
    QString      path;
    QPushButton *get_directory;
    QPushButton *use_last;
};

}

#endif // SAVEDISPLAYPATHBOX_H
