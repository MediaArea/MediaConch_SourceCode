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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // Functions
    void dragEnterEvent         (QDragEnterEvent *event);
    void dropEvent              (QDropEvent *event);

    // UI
    void                        Ui_Init                     ();

    // Helpers
    void                        Run();
    QString                     ask_for_schematron_file();

private:
    Ui::MainWindow *ui;

    // Internal
    Core C;

    // Visual elements
    QVBoxLayout*                Layout;
    QPlainTextEdit*             MainText;
    QLabel*                     DragDrop_Image;
    QLabel*                     DragDrop_Text;
    void                        clearVisualElements();
    void                        createDragDrop();
    void                        createMainText();

private Q_SLOTS:

    void on_actionOpen_triggered();
    void on_actionCloseAll_triggered();
    void on_actionConch_triggered();
    void on_actionInfo_triggered();
    void on_actionTrace_triggered();
    void on_actionSchematron_triggered();
    void on_actionText_triggered();
    void on_actionXml_triggered();
    void on_actionChooseSchematron_triggered();
};

#endif // MAINWINDOW_H
