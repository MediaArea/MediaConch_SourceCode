/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef POLICIESTREE_H
#define POLICIESTREE_H

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
using namespace MediaInfoNameSpace;
using namespace std;
#include <QTreeWidget>

namespace Ui {
    class PoliciesTree;
}

class QTreeWidget;
class QFrame;
class QLayout;

namespace MediaConch {

class PoliciesTree : public QFrame
{
    Q_OBJECT

public:
    explicit PoliciesTree(QWidget *parent = 0);
    ~PoliciesTree();

    
//***************************************************************************
// Functions
//***************************************************************************
    QTreeWidget *get_policies_tree();
    QFrame      *get_menu_frame();
    QLayout     *get_menu_layout();

//***************************************************************************
// Visual element
//***************************************************************************

private:
    Ui::PoliciesTree *ui;

//***************************************************************************
// Slots
//***************************************************************************

private Q_SLOTS:
};

}

#endif // POLICIESTREE_H
