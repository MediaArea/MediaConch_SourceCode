/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef RULEEDIT_H
#define RULEEDIT_H

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
    class RuleEdit;
}

class QPushButton;
class QLineEdit;
class QComboBox;
class QTextEdit;
struct Assert;
class MainWindow;
class QDialogButtonBox;
class QTableWidgetItem;
class QRadioButton;

class RuleEdit : public QFrame
{
    Q_OBJECT

public:
    explicit RuleEdit(QWidget *parent = 0);
    ~RuleEdit();

    
//***************************************************************************
// Functions
//***************************************************************************

void clear();
void assert_clicked(Assert *a);
void value_to_quotted_value(string&);
string get_validator_value_from_pretty_name(string pretty_name);
string get_validator_pretty_name_from_value(string value);
void fill_editor_fields(const Assert *a);

//***************************************************************************
// Visual element
//***************************************************************************

const QPushButton *get_delAssert_button();
const QPushButton *get_duplicateAssert_button();
const QLineEdit   *get_assertName_line();
QComboBox *get_type_select();
QComboBox *get_field_select();
QComboBox *get_validator_select();
QLineEdit *get_value_line();
QTextEdit *get_freeText_text();
QFrame *get_editor_frame();
QRadioButton *get_freeTextSelector_radio();
QRadioButton *get_editorSelector_radio();

private:
    MainWindow *mainwindow;
    Ui::RuleEdit *ui;

//***************************************************************************
// HELPER
//***************************************************************************

void add_values_to_selector();

void copy_visual_to_assert(Assert *a);

//***************************************************************************
// Slots
//***************************************************************************

private Q_SLOTS:
};

#endif // RULEEDIT_H
