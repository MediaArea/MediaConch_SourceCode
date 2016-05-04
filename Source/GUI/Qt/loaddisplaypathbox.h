/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef LOADDISPLAYPATHBOX_H
#define LOADDISPLAYPATHBOX_H

//---------------------------------------------------------------------------
#include <QMessageBox>

namespace MediaConch {

class LoadDisplayPathBox : public QMessageBox
{
public:
    LoadDisplayPathBox(const QString& value);
    ~LoadDisplayPathBox();

    int          exec();
    QString      get_path();

private:
    QString      path;
    QPushButton *get_directory;
    QPushButton *use_last;
};

}

#endif // LOADDISPLAYPATHBOX_H
