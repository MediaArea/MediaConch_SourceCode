/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef VERBOSITYSPINBOX_H
#define VERBOSITYSPINBOX_H

//---------------------------------------------------------------------------
#include <QWidget>
#include <QSpinBox>

namespace Ui {
    class VerbosityFrame;
}

class QDialogButtonBox;

namespace MediaConch {

class VerbositySpinbox : public QWidget
{
    Q_OBJECT

    class VerbosityBox : public QSpinBox
    {
      public:
        explicit VerbosityBox(QWidget *parent = 0) : QSpinBox(parent) {}
        ~VerbosityBox() {}

      protected:
        virtual QString textFromValue(int value) const;
        virtual int valueFromText(const QString & text) const;

        static const QString default_text;
        static const int     default_value;
    };

public:
    explicit VerbositySpinbox(QWidget *parent = 0);
    ~VerbositySpinbox();

//***************************************************************************
// Visual element
//***************************************************************************
    QSpinBox         *get_verbosity_spin();
    QDialogButtonBox *get_buttons_box();

private:
    Ui::VerbosityFrame *ui;
};

}

#endif // POLICYMENU_H
