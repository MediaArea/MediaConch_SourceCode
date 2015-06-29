/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Common/Core.h"

#include <QMainWindow>
#include <QString>

namespace Ui {
class MainWindow;
}

class QVBoxLayout;
class QPlainTextEdit;
class QLabel;
class PoliciesMenu;
class PoliciesEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // Functions
    void dragEnterEvent         (QDragEnterEvent *event);
    void dropEvent              (QDropEvent *event);
    void rule_to_add(Rule *r);
    void rule_to_delete(string);
    void policy_to_delete(int row);
    const Rule *get_rule_from_description(string description) const;
    void exporting_to_schematron_file();

    // UI
    void                        Ui_Init                     ();

    // Helpers
    void                        Run();
    QString                     ask_for_schematron_file();

    const list<string>* providePolicyExistingType() const {return &C.policies.existing_type; }
    const list<string>* providePolicyExistingField() const {return &C.policies.existing_field; }
    const list<Policies::validatorType>* providePolicyExistingValidator() const {return &C.policies.existing_validator; }

private:
    Ui::MainWindow *ui;
    PoliciesMenu   *policiesMenu;
    PoliciesEdit   *policiesEdit;

    // Internal
    Core C;

    //TEMP
    vector<Rule *> ruleToAdd;
    vector<string> ruleToDelete;

    // Visual elements
    QVBoxLayout*                Layout;
    QPlainTextEdit*             MainText;
    QLabel*                     DragDrop_Image;
    QLabel*                     DragDrop_Text;
    void                        clearVisualElements();
    void                        createDragDrop();
    void                        createMainText();
    void                        createPoliciesMenu();
    void                        displayPoliciesMenu();
    void                        createPoliciesEdit();
    void                        displayPoliciesEdit(int row);
    void                        choose_schematron_file();

//***************************************************************************
// HELPER
//***************************************************************************

QString getSelectedPolicyName();

private Q_SLOTS:

    void on_actionOpen_triggered();
    void on_actionCloseAll_triggered();
    void on_actionConch_triggered();
    void on_actionInfo_triggered();
    void on_actionTrace_triggered();
    void on_actionSchematron_triggered();
    void on_actionPolicies_triggered();
    void on_actionText_triggered();
    void on_actionXml_triggered();
    void on_actionChooseSchematron_triggered();
    void on_actionSavePolicies_triggered();
    void on_importSchematron();
    void on_addNewPolicy();
    void on_editPolicy(int row, int column);
    void on_editPolicy();
    void on_addNewRuleRejected();
    void on_addNewRuleAccepted();
};

#endif // MAINWINDOW_H
