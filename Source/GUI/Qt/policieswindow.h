/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef POLICIESWINDOW_H
#define POLICIESWINDOW_H

#include "Common/Core.h"
#include "Common/SchematronPolicy.h"
#include "Common/XsltPolicy.h"

#include <QFileInfo>
#include <QString>

class QVBoxLayout;
class QStatusBar;
class QLabel;
class QTreeWidgetItem;
class QFile;
class QStringList;
class QFrame;

namespace MediaConch {

class MainWindow;
class PoliciesTree;
class PoliciesMenu;
class PolicyWindow;

class PoliciesWindow : public QObject
{
    Q_OBJECT

public:
    explicit PoliciesWindow(MainWindow *parent = 0);
    ~PoliciesWindow();

    // Helpers
    void                        displayPoliciesTree();
    int                         get_index_in_tree();
    QTreeWidgetItem*            get_item_in_tree();
    int                         get_index_of_item_backXX(QTreeWidgetItem* item, size_t back);
    bool                        is_all_policies_saved();

    // Visual elements
    void                        updatePoliciesTreeSchematronPattern(SchematronPattern *p, QTreeWidgetItem *parent);
    void                        updatePoliciesTreeSchematronRule(SchematronRule *rule, QTreeWidgetItem *parent);
    void                        updatePoliciesTreeXsltRule(XsltRule* p, QTreeWidgetItem *parent);
    void                        removeTreeChildren(QTreeWidgetItem* item);
    QStatusBar*                 get_error_bar();
    void                        set_widget_to_tree_layout(QWidget* w);
    void                        remove_widget_from_tree_layout(QWidget* w);
    QFrame*                     policyFrame();
    void                        policy_deleted(QTreeWidgetItem* item, int row);
    void                        save_policy(Policies::PolicyType type);
    int                         save_policy_to(Policies::PolicyType type);

private:
    MainWindow     *mainwindow;
    PoliciesTree   *policiesTree;
    PoliciesMenu   *policiesMenu;
    PolicyWindow   *policywindow;

    // Visual elements
    void                        clearPoliciesElements();
    void                        createPoliciesTree();
    void                        displayPoliciesMenu();
    void                        createPoliciesMenu();
    void                        updatePoliciesTree();
    void                        updatePoliciesTreeSchematronPolicy(SchematronPolicy* p, QTreeWidgetItem *parent);
    void                        updatePoliciesTreeXsltPolicy(XsltPolicy* p, QTreeWidgetItem *parent);

//***************************************************************************
// HELPER
//***************************************************************************
    void             add_new_schematron_policy(QTreeWidgetItem* parent);
    void             add_new_xslt_policy(QTreeWidgetItem* parent);

private Q_SLOTS:
    void import_schema();
    void add_new_policy();
    void delete_all_policies();
    void policiesTree_selectionChanged();
};

}

#endif // MAINWINDOW_H
