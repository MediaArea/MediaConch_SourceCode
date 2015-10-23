/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef DISPLAYMENU_H
#define DISPLAYMENU_H

#include <QFrame>

namespace Ui {
    class DisplayMenu;
}

class QTableWidget;
class QPushButton;

namespace MediaConch {

class DisplayMenu : public QFrame
{
    Q_OBJECT

public:
    explicit DisplayMenu(QWidget *parent = 0);
    ~DisplayMenu();
    
//***************************************************************************
// Functions
//***************************************************************************
    QTableWidget *get_display_table();
    QFrame       *get_menu_frame();
    QPushButton  *get_addFile_button();
    QPushButton  *get_exportFile_button();
    QPushButton  *get_delFile_button();

//***************************************************************************
// Visual element
//***************************************************************************

private:
    Ui::DisplayMenu *ui;
};

}

#endif // DISPLAYMENU_H
