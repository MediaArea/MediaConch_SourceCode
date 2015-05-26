@rem Copyright (c) MediaArea.net SARL. All Rights Reserved.
@rem
@rem Use of this source code is governed by a BSD-style license that can
@rem be found in the License.html file in the root of the source tree.
@rem

@rem echo off

@rem --- Clean up ---
del   MediaConch_GUI_Windows_x64.exe
del   MediaConch_GUI_Windows_x64_WithoutInstaller.7z
rmdir MediaConch_GUI_Windows_x64 /S /Q
mkdir MediaConch_GUI_Windows_x64


@rem --- Copying : Exe ---
copy  ..\Project\MSVC2013\GUI\Win32\Release\MediaConch_GUI.exe MediaConch_GUI_Windows_x64\MediaConch.exe

@rem --- Copying : Information files ---
copy ..\License.html MediaConch_GUI_Windows_x64\
copy ..\History_GUI.txt MediaConch_GUI_Windows_x64\History.txt
copy ReadMe_GUI_Windows.txt MediaConch_GUI_Windows_x64\ReadMe.txt


rem --- Compressing Archive ---
cd MediaConch_GUI_Windows_x64\
..\..\..\Shared\Binary\Windows_i386\7-Zip\7z a -r -t7z -mx9 ..\MediaConch_GUI_Windows_x64_WithoutInstaller.7z *
cd ..

rem --- Installer ---
..\..\Shared\Binary\Windows_i386\NSIS\makensis ..\Source\Install\MediaConch_GUI_Windows_x64.nsi

rem --- Clean up ---
if "%1"=="SkipCleanUp" goto SkipCleanUp
rmdir MediaConch_GUI_Windows_x64\ /S /Q
:SkipCleanUp