/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef POLICYCOMBOBOX_H
#define POLICYCOMBOBOX_H

//---------------------------------------------------------------------------
#include <QWidget>
#include <QComboBox>
#include <vector>

namespace Ui {
    class PolicyFrame;
}

class QDialogButtonBox;

namespace MediaConch {

class PolicyCombobox : public QWidget
{
    Q_OBJECT

public:
    explicit PolicyCombobox(QWidget *parent = 0);
    ~PolicyCombobox();

    void fill_policy_box(const std::vector<std::pair<QString, QString> >&);

//***************************************************************************
// Visual element
//***************************************************************************
    QComboBox        *get_policy_combo();
    QDialogButtonBox *get_buttons_box();

private:
    Ui::PolicyFrame *ui;
};

}

#endif // POLICYCOMBOBOX_H
