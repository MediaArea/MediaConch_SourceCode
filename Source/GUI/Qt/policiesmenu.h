/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef POLICIESMENU_H
#define POLICIESMENU_H

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
#include <QFrame>
#include <QStandardItem>
#include <list>
using namespace MediaInfoNameSpace;
using namespace std;

namespace Ui {
class PoliciesMenu;
}

class QPushButton;
class QTableWidget;
class MainWindow;

class PoliciesMenu : public QFrame
{
    Q_OBJECT

public:
    explicit PoliciesMenu(QWidget *parent = 0);
    ~PoliciesMenu();

    
//***************************************************************************
// Functions
//***************************************************************************

void clear();
void add_error(String error);
void show_errors();
void add_policy(string name);

//***************************************************************************
// Visual element
//***************************************************************************

QPushButton *get_schematron_button() const;
QPushButton *get_addNewPolicy_button() const;
QPushButton *get_editPolicy_button() const;
QTableWidget *get_policies_table() const;

private:
    Ui::PoliciesMenu *ui;
    MainWindow *mainwindow;
    list<String> errors;

//***************************************************************************
// Slots
//***************************************************************************

private Q_SLOTS:
    void delete_clicked();
    void policy_selected_change();
    void export_clicked();
};

#endif // POLICIESMENU_H
