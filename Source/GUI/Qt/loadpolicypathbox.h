/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef LOADPOLICYPATHBOX_H
#define LOADPOLICYPATHBOX_H

//---------------------------------------------------------------------------
#include <QMessageBox>

namespace MediaConch {

class LoadPolicyPathBox : public QMessageBox
{
public:
    LoadPolicyPathBox(const QString& value);
    ~LoadPolicyPathBox();

    int          exec();
    QString      get_path();

private:
    QString      path;
    QPushButton *get_directory;
    QPushButton *use_last;
};

}

#endif // LOADPOLICYPATHBOX_H
