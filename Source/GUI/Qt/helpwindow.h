/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef HELPWINDOW_H
#define HELPWINDOW_H

//---------------------------------------------------------------------------
#include <QDialog>

class QPushButton;
class QTabWidget;

namespace MediaConch {

class Help : public QDialog
{
    Q_OBJECT

public:
    explicit Help(QWidget *parent = 0);
    ~Help();

//***************************************************************************
// Functions
//***************************************************************************

    void About();
    void GettingStarted();

private:
    QTabWidget*  Central;
    QPushButton* Close;
};

}

#endif // HELPWINDOW_H
