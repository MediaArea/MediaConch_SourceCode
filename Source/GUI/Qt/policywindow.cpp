/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "policywindow.h"
#include "policieswindow.h"
#include "mainwindow.h"

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

PolicyWindow::PolicyWindow(PoliciesWindow *window, MainWindow *parent) : policieswindow(window), mainwindow(parent)
{
}

PolicyWindow::~PolicyWindow()
{
    clearPolicyElements();
}

void PolicyWindow::clearPolicyElements()
{
}

}
