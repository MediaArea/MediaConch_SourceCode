mkdir _Automated
..\..\..\..\Qt\qtbase\bin\moc ..\..\..\Source\GUI\Qt\mainwindow.h -o_Automated\moc_mainwindow.cpp
..\..\..\..\Qt\qtbase\bin\uic ..\..\..\Source\GUI\Qt\mainwindow.ui -o _Automated\ui_mainwindow.h
..\..\..\..\Qt\qtbase\bin\rcc ..\..\..\Source\Resource\Resources.qrc -o _Automated\qrc_Resources.cpp
