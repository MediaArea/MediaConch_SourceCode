/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "policiestree.h"
#include "ui_policiestree.h"

#if QT_VERSION >= 0x050200
    #include <QFontDatabase>
#endif

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

PoliciesTree::PoliciesTree(QWidget *parent) :
    QTreeWidget(parent),
    ui(new Ui::PoliciesTree)
{
    ui->setupUi(this);
}

PoliciesTree::~PoliciesTree()
{
    delete ui;
}

//***************************************************************************
// Functions
//***************************************************************************

//***************************************************************************
// Visual element
//***************************************************************************

//***************************************************************************
// Slots
//***************************************************************************
