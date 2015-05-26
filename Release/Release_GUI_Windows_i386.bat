@rem Copyright (c) MediaArea.net SARL. All Rights Reserved.
@rem
@rem Use of this source code is governed by a BSD-style license that can
@rem be found in the License.html file in the root of the source tree.
@rem

@rem echo off

@rem --- Clean up ---
del   MediaConch_GUI_Windows_i386.exe
del   MediaConch_GUI_Windows_i386_WithoutInstaller.7z
rmdir MediaConch_GUI_Windows_i386 /S /Q
mkdir MediaConch_GUI_Windows_i386


@rem --- Copying : Exe ---
copy  ..\Project\MSVC2013\GUI\Win32\Release\MediaConch_GUI.exe MediaConch_GUI_Windows_i386\MediaConch.exe

@rem --- Copying : Information files ---
copy ..\License.html MediaConch_GUI_Windows_i386\
copy ..\History_GUI.txt MediaConch_GUI_Windows_i386\History.txt
copy ReadMe_GUI_Windows.txt MediaConch_GUI_Windows_i386\ReadMe.txt


rem --- Compressing Archive ---
cd MediaConch_GUI_Windows_i386\
..\..\..\Shared\Binary\Windows_i386\7-Zip\7z a -r -t7z -mx9 ..\MediaConch_GUI_Windows_i386_WithoutInstaller.7z *
cd ..

rem --- Installer ---
copy ..\Project\BCB\GUI\Release_Build\MediaConch_GUI.exe BCB\GUI\MediaConch.exe
..\..\Shared\Binary\Windows_i386\NSIS\makensis ..\Source\Install\MediaConch_GUI_Windows_i386.nsi

rem --- Clean up ---
if "%1"=="SkipCleanUp" goto SkipCleanUp
rmdir MediaConch_GUI_Windows_i386\ /S /Q
:SkipCleanUp