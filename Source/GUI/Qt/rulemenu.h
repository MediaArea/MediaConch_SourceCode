/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef RULEMENU_H
#define RULEMENU_H

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
    class RuleMenu;
}

class QPushButton;

class RuleMenu : public QFrame
{
    Q_OBJECT

public:
    explicit RuleMenu(QWidget *parent = 0);
    ~RuleMenu();
    
//***************************************************************************
// Functions
//***************************************************************************

//***************************************************************************
// Visual element
//***************************************************************************

    QPushButton *get_addNewAssert_button() const;

private:
    Ui::RuleMenu *ui;

//***************************************************************************
// Slots
//***************************************************************************

private Q_SLOTS:
};

#endif // RULEMENU_H
