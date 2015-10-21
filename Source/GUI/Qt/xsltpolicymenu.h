/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef XSLTPOLICYMENU_H
#define XSLTPOLICYMENU_H

//---------------------------------------------------------------------------
#ifdef MEDIAINFO_DLL_RUNTIME
    #include "MediaInfoDLL/MediaInfoDLL.h"
    #define MediaInfoNameSpace MediaInfoDLL
#elif defined MEDIAINFO_DLL_STATIC
    #include "MediaInfoDLL/MediaInfoDLL_Static.h"
    #define MediaInfoNameSpace MediaInfoDLL
#else
    #include "MediaInfo/MediaInfoList.h"
    #define MediaInfoNameSpace MediaInfoLib
#endif
#include <QFrame>
using namespace MediaInfoNameSpace;
using namespace std;

namespace Ui {
    class XsltPolicyMenu;
}

class QPushButton;
class QLineEdit;

namespace MediaConch {

class XsltPolicyMenu : public QFrame
{
    Q_OBJECT

public:
    explicit XsltPolicyMenu(QWidget *parent = 0);
    ~XsltPolicyMenu();
    
//***************************************************************************
// Functions
//***************************************************************************

//***************************************************************************
// Visual element
//***************************************************************************
    QLineEdit   *get_title_line();
    QLineEdit   *get_description_line();
    QPushButton *get_addNewRule_button();
    QPushButton *get_deletePolicy_button();
    QPushButton *get_duplicatePolicy_button();
    QPushButton *get_savePolicy_button();
    QPushButton *get_savePolicyTo_button();

private:
    Ui::XsltPolicyMenu *ui;
};

}

#endif // POLICYMENU_H
