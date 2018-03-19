/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QFrame>

namespace Ui {
    class ProgressBar;
}

class QProgressBar;

namespace MediaConch {

class ProgressBar : public QFrame
{
    Q_OBJECT

public:
    explicit ProgressBar(QWidget *parent = 0);
    ~ProgressBar();

    
//***************************************************************************
// Functions
//***************************************************************************
    QProgressBar  *get_progress_bar();

//***************************************************************************
// Visual element
//***************************************************************************

private:
    Ui::ProgressBar *ui;
};

}

#endif // PROGRESSBAR_H
