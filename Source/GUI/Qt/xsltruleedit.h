/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef XSLTRULEEDIT_H
#define XSLTRULEEDIT_H

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
#include <QSpinBox>
#include <list>
using namespace MediaInfoNameSpace;
using namespace std;

namespace Ui {
    class XsltRuleEdit;
}

class QPushButton;
class QLineEdit;
class QComboBox;
class QDialogButtonBox;
class QTableWidgetItem;
class QRadioButton;
class QTextEdit;

namespace MediaConch {

class XsltRule;
class MainWindow;

class XsltRuleEdit : public QFrame
{
    class CustomSpinBox : public QSpinBox
    {
    public:
        explicit CustomSpinBox(QWidget *parent = 0);
        ~CustomSpinBox();
        QString textFromValue(int value) const;
        int valueFromText(QString& text) const;
    };

    Q_OBJECT

public:
    explicit XsltRuleEdit(QWidget *parent = 0);
    ~XsltRuleEdit();

//***************************************************************************
// Visual element
//***************************************************************************
    void rule_clicked(XsltRule *r);
    void fill_editor_fields(XsltRule *r);
    const QPushButton *get_delRule_button();
    const QPushButton *get_duplicateRule_button();
    const QLineEdit   *get_name_line();
    QComboBox         *get_type_select();
    QComboBox         *get_field_select();
    QComboBox         *get_operator_select();
    QLineEdit         *get_value_line();
    QSpinBox          *get_occurrence_box();
    QFrame            *get_editor_frame();
    QTextEdit         *get_freeText_text();
    QRadioButton      *get_freeTextSelector_radio();
    QRadioButton      *get_editorSelector_radio();
    void               change_values_of_field_selector();

private:
    MainWindow *mainwindow;
    Ui::XsltRuleEdit *ui;

    void               add_values_to_selector();
    void               change_occurence_spin_box();
};

}

#endif // RULEEDIT_H
