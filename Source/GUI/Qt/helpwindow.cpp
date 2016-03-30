/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "helpwindow.h"
#include <QPushButton>
#include <QTabWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QTextBrowser>

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

//---------------------------------------------------------------------------
Help::Help(QWidget *parent) : QDialog(parent)
{
    move(QApplication::desktop()->screenGeometry().width()/5, y());
    resize(QApplication::desktop()->screenGeometry().width()-QApplication::desktop()->screenGeometry().width()/5*2,
           QApplication::desktop()->screenGeometry().height()*3/4);
    setWindowFlags(windowFlags()&(0xFFFFFFFF-Qt::WindowContextHelpButtonHint));
    setWindowTitle("MediaConch help");

    Close=new QPushButton("&Close");
    Close->setDefault(true);

    QDialogButtonBox* Dialog=new QDialogButtonBox();
    Dialog->addButton(Close, QDialogButtonBox::AcceptRole);
    connect(Dialog, SIGNAL(accepted()), this, SLOT(close()));

    QVBoxLayout* L=new QVBoxLayout();
    Central=new QTabWidget(this);

    QTextBrowser* Text1=new QTextBrowser(this);
    Text1->setReadOnly(true);
    Text1->setOpenExternalLinks(true);
    Text1->setSource(QUrl("qrc:/Help/FAQ/FAQ.html"));
    Central->addTab(Text1, tr("FAQ"));

    QTextBrowser* Text2=new QTextBrowser(this);
    Text2->setReadOnly(true);
    Text2->setOpenExternalLinks(true);
    Text2->setSource(QUrl("qrc:/Help/How To Use/How To Use.html"));
    Central->addTab(Text2, tr("How To Use"));

    QTextBrowser* Text3=new QTextBrowser(this);
    Text3->setReadOnly(true);
    Text3->setOpenExternalLinks(true);
    Text3->setSource(QUrl("qrc:/Help/Data Format/Data Format.html"));
    Central->addTab(Text3, tr("Data Format"));

    QTextBrowser* Text4=new QTextBrowser(this);
    Text4->setReadOnly(true);
    Text4->setOpenExternalLinks(true);
    Text4->setSource(QUrl("qrc:/Help/About/About.html"));
    Central->addTab(Text4, tr("About"));

    L->addWidget(Central);
    L->addWidget(Close);
    setLayout(L);
}

//---------------------------------------------------------------------------
Help::~Help()
{
}

//***************************************************************************
// Functions
//***************************************************************************

//---------------------------------------------------------------------------
void Help::GettingStarted()
{
    Central->setCurrentIndex(0);
    show();
}

//---------------------------------------------------------------------------
void Help::HowToUse()
{
    Central->setCurrentIndex(1);
    show();
}

//---------------------------------------------------------------------------
void Help::DataFormat()
{
    Central->setCurrentIndex(2);
    show();
}

//---------------------------------------------------------------------------
void Help::About()
{
    Central->setCurrentIndex(3);
    show();
}

}
