/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef UNKNOWNWINDOW_H
#define UNKNOWNWINDOW_H

#include "policywindow.h"

#include <QObject>
#include <QString>

class QTreeWidgetItem;

namespace MediaConch {

class MainWindow;
class PoliciesWindow;
class PoliciesMenu;
class UnknownPolicyMenu;

class UnknownWindow : public PolicyWindow
{
    Q_OBJECT

public:
    explicit UnknownWindow(PoliciesWindow *window = 0, MainWindow *parent = 0);
    virtual ~UnknownWindow();

    // Visual elements
    void display_selection(int level, QTreeWidgetItem *item);
    void displayPolicyMenu(QString title);

private:
    UnknownPolicyMenu             *policyMenu;

    // Visual elements
    void                        clearPolicyElements();
    void                        createPolicyMenu();

private Q_SLOTS:
    void save_policy_to();
    void duplicate_policy();
    void delete_policy();
};

}

#endif // UNKNOWNWINDOW_H
