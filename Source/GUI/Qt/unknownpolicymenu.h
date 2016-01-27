/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef UNKNOWNPOLICYMENU_H
#define UNKNOWNPOLICYMENU_H

//---------------------------------------------------------------------------
#include <QFrame>

namespace Ui {
    class UnknownPolicyMenu;
}

class QPushButton;
class QLineEdit;

namespace MediaConch {

class UnknownPolicyMenu : public QFrame
{
    Q_OBJECT

public:
    explicit UnknownPolicyMenu(QWidget *parent = 0);
    ~UnknownPolicyMenu();
    
//***************************************************************************
// Functions
//***************************************************************************

//***************************************************************************
// Visual element
//***************************************************************************
    QLineEdit   *get_title_line();
    QPushButton *get_deletePolicy_button();
    QPushButton *get_duplicatePolicy_button();
    QPushButton *get_savePolicy_button();
    QPushButton *get_savePolicyTo_button();

private:
    Ui::UnknownPolicyMenu *ui;
};

}

#endif // POLICYMENU_H
