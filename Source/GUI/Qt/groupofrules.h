/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef GROUPOFRULES_H
#define GROUPOFRULES_H

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
class GroupOfRules;
}

class QPushButton;
class QLineEdit;

class GroupOfRules : public QFrame
{
    Q_OBJECT

public:
    explicit GroupOfRules(QWidget *parent = 0);
    ~GroupOfRules();
    
//***************************************************************************
// Functions
//***************************************************************************

//***************************************************************************
// Visual element
//***************************************************************************

    QLineEdit   *get_title_line() const;
    QPushButton *get_addNewRule_button() const;

private:
    Ui::GroupOfRules *ui;

//***************************************************************************
// Slots
//***************************************************************************

private Q_SLOTS:
};

#endif // GROUPOFRULES_H
