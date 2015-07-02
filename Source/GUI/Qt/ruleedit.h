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
struct Assert;
class MainWindow;
class QDialogButtonBox;
class QTableWidgetItem;

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
void add_error(String error);
void show_errors();
void clear_errors();
void add_assert(Assert *a);
void set_name(string& policyName);
string get_new_name() const;
const vector<Assert *>& get_asserts() const;

//***************************************************************************
// Visual element
//***************************************************************************

const QPushButton *get_newAssert_button() const;
const QDialogButtonBox *get_validation_button() const;

private:
    MainWindow *mainwindow;
    Ui::RuleEdit *ui;
    list<String> errors;
    string policyName;
    vector<Assert *> asserts;

//***************************************************************************
// HELPER
//***************************************************************************

QString getSelectedAssertName();
QTableWidgetItem* getSelectedAssertItem();
void add_values_to_selector();
void clear_editor_fields();
string get_validator_value_from_pretty_name(string pretty_name);
string get_validator_pretty_name_from_value(string value);
void copy_visual_to_assert(Assert *a);
void fill_editor_fields(const Assert *a);
void value_to_quotted_value(string&);

//***************************************************************************
// Slots
//***************************************************************************

private Q_SLOTS:
    void on_addNewAssert();
    void cell_clicked(int row, int column);
    void on_deleteAssert();
    void assert_selected_changed();
    void free_text_selected();
    void editor_selected();
    void editAssert_type();
    void editAssert_field();
    void editAssert_validator();
    void editAssert_value();
    void editAssert_freeText();
    void editAssert_assertName();
};

#endif // RULEEDIT_H
