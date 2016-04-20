/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef DISPLAYCOMBOBOX_H
#define DISPLAYCOMBOBOX_H

//---------------------------------------------------------------------------
#include <QWidget>
#include <QComboBox>
#include <vector>

namespace Ui {
    class DisplayFrame;
}

class QDialogButtonBox;

namespace MediaConch {

class DisplayCombobox : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayCombobox(QWidget *parent = 0);
    ~DisplayCombobox();

    void fill_display_box(const std::vector<QString>&);

//***************************************************************************
// Visual element
//***************************************************************************
    QComboBox        *get_display_combo();
    QDialogButtonBox *get_buttons_box();

private:
    Ui::DisplayFrame *ui;
};

}

#endif // DISPLAYCOMBOBOX_H
