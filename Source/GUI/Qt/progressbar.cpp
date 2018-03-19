/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "progressbar.h"
#include "ui_progressbar.h"

#include <QProgressBar>
#include <QFrame>

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

ProgressBar::ProgressBar(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ProgressBar)
{
    ui->setupUi(this);
}

ProgressBar::~ProgressBar()
{
    delete ui;
}

//***************************************************************************
// Visual element
//***************************************************************************

QProgressBar *ProgressBar::get_progress_bar()
{
    return ui->bar;
}

}
