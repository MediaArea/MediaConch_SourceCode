/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "mainwindow.h"
#include <QApplication>

//#include <QtCore/QtPlugin>
//#if defined(_WIN32) && QT_VERSION >= 0x00050000 //Qt5
    //Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
//#endif

#ifdef __MACOSX__
    #include <ApplicationServices/ApplicationServices.h>
#endif //__MACOSX__

int main(int argc, char *argv[])
{
#if QT_VERSION >= 0x050700
    char disable_security[] = "--disable-web-security"; //Fonts are not loaded if this is not set

    char **old_argv = argv;
    argv = new char*[argc + 2]; 
    memcpy(argv, old_argv, argc * sizeof(char*));
    argv[argc] = disable_security;
    argc++;
    argv[argc] = NULL;
#endif

    QApplication a(argc, argv);
    a.setApplicationName("MediaConch");
    MediaConch::MainWindow w;
    w.show();
    int ret = a.exec();

#if QT_VERSION >= 0x050700
    delete [] argv;
#endif

    return ret;
}
