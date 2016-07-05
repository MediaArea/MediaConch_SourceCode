/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef POLICIESWINDOW_H
#define POLICIESWINDOW_H

#include "Common/Core.h"
#include "Common/SchematronPolicy.h"
#include "Common/UnknownPolicy.h"
#include "Common/XsltPolicy.h"

#include <QFileInfo>
#include <QString>

class QVBoxLayout;
class QLabel;
class QTreeWidgetItem;
class QFile;
class QStringList;
class QFrame;

namespace MediaConch {

class MainWindow;

class PoliciesWindow : public QObject
{
    Q_OBJECT

public:
    explicit PoliciesWindow(MainWindow *parent = 0);
    ~PoliciesWindow();

    // Helpers
    void                        display_policies();

private:
    MainWindow     *mainwindow;

//***************************************************************************
// HELPER
//***************************************************************************
};

}

#endif // POLICIESWINDOW_H
