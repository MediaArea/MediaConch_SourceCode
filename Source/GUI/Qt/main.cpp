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
#if QT_VERSION < 0x050700
    QApplication a(argc, argv);
#else
    int new_argc = argc + 1;
    char **new_argv = new char* [new_argc + 1];

    const char disable_security[] = "--disable-web-security";

    for (int i = 0; i < argc; ++i)
    {
        int len = strlen(argv[i]);
        new_argv[i] = new char [len + 1];
        for (int j = 0; j < len; ++j)
            new_argv[i][j] = argv[i][j];
        new_argv[i][len] = '\0';
    }

    new_argv[argc] = new char [sizeof(disable_security) + 1];
    for (size_t i = 0; i < sizeof(disable_security); ++i)
        new_argv[argc][i] = disable_security[i];
    new_argv[argc][sizeof(disable_security)] = '\0';

    new_argv[new_argc] = NULL;

    QApplication a(new_argc, new_argv);
#endif

    a.setApplicationName("MediaConch");
    MediaConch::MainWindow w;
    w.show();

#if QT_VERSION < 0x050700
    return a.exec();
#else
    int ret = a.exec();

    for (int i = 0; i < new_argc; ++i)
        delete [] new_argv[i];
    delete [] new_argv;
    return ret;
#endif
}
