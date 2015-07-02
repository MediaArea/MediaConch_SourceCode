/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef POLICIESEDIT_H
#define POLICIESEDIT_H

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
    class PoliciesEdit;
}

class QPushButton;
struct Rule;
class MainWindow;
class QDialogButtonBox;
class QTableWidgetItem;

class PoliciesEdit : public QFrame
{
    Q_OBJECT

public:
    explicit PoliciesEdit(QWidget *parent = 0);
    ~PoliciesEdit();

    
//***************************************************************************
// Functions
//***************************************************************************

void clear();
void add_error(String error);
void show_errors();
void clear_errors();
void add_rule(Rule *r);
void set_name(string& policyName);
string get_new_name() const;
const vector<Rule *>& get_pattern() const;

//***************************************************************************
// Visual element
//***************************************************************************

const QPushButton *get_newRule_button() const;
const QDialogButtonBox *get_validation_button() const;

private:
    MainWindow *mainwindow;
    Ui::PoliciesEdit *ui;
    list<String> errors;
    string policyName;
    vector<Rule *> rules;

//***************************************************************************
// HELPER
//***************************************************************************

QString getSelectedRuleName();
QTableWidgetItem* getSelectedRuleItem();
void add_values_to_selector();
void clear_editor_fields();
string get_validator_value_from_pretty_name(string pretty_name);
string get_validator_pretty_name_from_value(string value);
void copy_visual_to_rule(Rule *r);
void fill_editor_fields(const Rule *r);
void value_to_quotted_value(string&);

//***************************************************************************
// Slots
//***************************************************************************

private Q_SLOTS:
    void on_addNewRule();
    void cell_clicked(int row, int column);
    void on_deleteRule();
    void rule_selected_changed();
    void free_text_selected();
    void editor_selected();
    void editRule_type();
    void editRule_field();
    void editRule_validator();
    void editRule_value();
    void editRule_freeText();
    void editRule_ruleName();
};

#endif // POLICIESEDIT_H
