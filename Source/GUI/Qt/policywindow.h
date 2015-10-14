/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef POLICYWINDOW_H
#define POLICYWINDOW_H

#include <QObject>
#include <QString>

class QTreeWidgetItem;

namespace MediaConch {

class MainWindow;
class PoliciesWindow;

class PolicyWindow : public QObject
{
    Q_OBJECT

public:
    explicit PolicyWindow(PoliciesWindow *window = 0, MainWindow *parent = 0);
    virtual ~PolicyWindow();

    // Visual elements
    virtual void displayPolicyMenu(QString title) = 0;
    virtual void display_selection(int level, QTreeWidgetItem *item) = 0;

protected:
    PoliciesWindow *policieswindow;
    MainWindow     *mainwindow;

    void clearPolicyElements();
};

}

#endif // POLICYWINDOW_H
