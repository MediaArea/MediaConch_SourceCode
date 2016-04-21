/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "policycombobox.h"
#include "ui_policycombobox.h"
#include <QComboBox>

namespace MediaConch {

//***************************************************************************
// PolicyCombobox
//***************************************************************************

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

//---------------------------------------------------------------------------
PolicyCombobox::PolicyCombobox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PolicyFrame)
{
    ui->setupUi(this);
}

//---------------------------------------------------------------------------
PolicyCombobox::~PolicyCombobox()
{
    delete ui;
}

//---------------------------------------------------------------------------
void PolicyCombobox::fill_policy_box(const std::vector<std::pair<QString, QString> >& list)
{
    ui->policy->addItem("Last used policy", QVariant("last"));
    ui->policy->addItem("No policy", QVariant(""));
    for (size_t i = 0; i < list.size(); ++i)
        ui->policy->addItem(list[i].second, QVariant(list[i].first));
}

//***************************************************************************
// Visual element
//***************************************************************************

//---------------------------------------------------------------------------
QComboBox *PolicyCombobox::get_policy_combo()
{
    return ui->policy;
}

//---------------------------------------------------------------------------
QDialogButtonBox *PolicyCombobox::get_buttons_box()
{
    return ui->buttons;
}

}
