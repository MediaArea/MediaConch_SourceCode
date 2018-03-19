@rem Copyright (c) MediaArea.net SARL. All Rights Reserved.
@rem
@rem Use of this source code is governed by a BSD-style license that can
@rem be found in the License.html file in the root of the source tree.
@rem

@rem echo off

rem --- Search binaries ---
set BPATH=
if exist "%~dp0\..\..\..\MediaArea-Utils-Binaries" set BPATH="%~dp0\..\..\..\MediaArea-Utils-Binaries"
if exist "%~dp0\..\..\MediaArea-Utils-Binaries" set BPATH="%~dp0\..\..\MediaArea-Utils-Binaries"
if "%BPATH%"=="" (
    echo "ERROR: binaries path not found"
    exit /b 1
)

@rem --- Clean up ---
del   MediaConch_GUI_Windows_i386.exe
del   MediaConch_GUI_Windows_i386_WithoutInstaller.7z
rmdir MediaConch_GUI_Windows_i386 /S /Q
mkdir MediaConch_GUI_Windows_i386


@rem --- Copying : Exe ---
copy  ..\Project\MSVC2017\Win32\Release\MediaConch-GUI.exe MediaConch_GUI_Windows_i386\MediaConch.exe
copy  ..\Project\MSVC2017\Win32\Release\LIBCURL.DLL MediaConch_GUI_Windows_i386\
copy  ..\Project\MSVC2017\Win32\Release\Qt5Core.dll MediaConch_GUI_Windows_i386\
copy  ..\Project\MSVC2017\Win32\Release\Qt5Gui.dll MediaConch_GUI_Windows_i386\
copy  ..\Project\MSVC2017\Win32\Release\Qt5Network.dll MediaConch_GUI_Windows_i386\
copy  ..\Project\MSVC2017\Win32\Release\libEGL.dll MediaConch_GUI_Windows_i386\
copy  ..\Project\MSVC2017\Win32\Release\libGLESV2.dll MediaConch_GUI_Windows_i386\
copy  ..\Project\MSVC2017\Win32\Release\d3dcompiler_47.dll MediaConch_GUI_Windows_i386\
copy  ..\Project\MSVC2017\Win32\Release\Qt5Qml.dll MediaConch_GUI_Windows_i386\
copy  ..\Project\MSVC2017\Win32\Release\Qt5Quick.dll MediaConch_GUI_Windows_i386\
copy  ..\Project\MSVC2017\Win32\Release\Qt5WebChannel.dll MediaConch_GUI_Windows_i386\
copy  ..\Project\MSVC2017\Win32\Release\Qt5WebEngineCore.dll MediaConch_GUI_Windows_i386\
copy  ..\Project\MSVC2017\Win32\Release\Qt5WebEngineWidgets.dll MediaConch_GUI_Windows_i386\
copy  ..\Project\MSVC2017\Win32\Release\Qt5WebKit.dll MediaConch_GUI_Windows_i386\
copy  ..\Project\MSVC2017\Win32\Release\Qt5WebKitWidgets.dll MediaConch_GUI_Windows_i386\
copy  ..\Project\MSVC2017\Win32\Release\Qt5WebSockets.dll MediaConch_GUI_Windows_i386\
copy  ..\Project\MSVC2017\Win32\Release\Qt5PrintSupport.dll MediaConch_GUI_Windows_i386\
copy  ..\Project\MSVC2017\Win32\Release\Qt5QuickWidgets.dll MediaConch_GUI_Windows_i386\
copy  ..\Project\MSVC2017\Win32\Release\Qt5Positioning.dll MediaConch_GUI_Windows_i386\
copy  ..\Project\MSVC2017\Win32\Release\Qt5Widgets.dll MediaConch_GUI_Windows_i386\
copy  ..\Project\MSVC2017\Win32\Release\QtWebEngineProcess.exe MediaConch_GUI_Windows_i386\
copy  "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Redist\MSVC\14.11.25325\x86\Microsoft.VC141.CRT\concrt140.dll" MediaConch_GUI_Windows_i386\
copy  "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Redist\MSVC\14.11.25325\x86\Microsoft.VC141.CRT\msvcp140.dll" MediaConch_GUI_Windows_i386\
copy  "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Redist\MSVC\14.11.25325\x86\Microsoft.VC141.CRT\vccorlib140.dll" MediaConch_GUI_Windows_i386\
copy  "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Redist\MSVC\14.11.25325\x86\Microsoft.VC141.CRT\vcruntime140.dll" MediaConch_GUI_Windows_i386\
mkdir MediaConch_GUI_Windows_i386\resources
copy  ..\Project\MSVC2017\Win32\Release\resources\icudtl.dat MediaConch_GUI_Windows_i386\resources\icudtl.dat
copy  ..\Project\MSVC2017\Win32\Release\resources\qtwebengine_resources.pak MediaConch_GUI_Windows_i386\resources\qtwebengine_resources.pak
copy  ..\Project\MSVC2017\Win32\Release\resources\qtwebengine_resources_100p.pak MediaConch_GUI_Windows_i386\resources\qtwebengine_resources_100p.pak
copy  ..\Project\MSVC2017\Win32\Release\resources\qtwebengine_resources_200p.pak MediaConch_GUI_Windows_i386\resources\qtwebengine_resources_200p.pak
mkdir MediaConch_GUI_Windows_i386\platforms
copy  ..\Project\MSVC2017\Win32\Release\platforms\qwindows.dll MediaConch_GUI_Windows_i386\platforms\
mkdir MediaConch_GUI_Windows_i386\translations\
copy  ..\Project\MSVC2017\Win32\Release\translations\qt_en.qm MediaConch_GUI_Windows_i386\translations\qt_en.qm
mkdir MediaConch_GUI_Windows_i386\translations\qtwebengine_locales
copy  ..\Project\MSVC2017\Win32\Release\translations\qtwebengine_locales\en-US.pak MediaConch_GUI_Windows_i386\translations\qtwebengine_locales\en-US.pak

@rem --- Copying : Information files ---
copy ..\License.html MediaConch_GUI_Windows_i386\
copy ..\History_GUI.txt MediaConch_GUI_Windows_i386\History.txt
copy ReadMe_GUI_Windows.txt MediaConch_GUI_Windows_i386\ReadMe.txt


rem --- Compressing Archive ---
cd MediaConch_GUI_Windows_i386\
%BPATH%\Windows\7-Zip\7z a -r -t7z -mx9 ..\MediaConch_GUI_Windows_i386_WithoutInstaller.7z *
cd ..

rem --- Clean up ---
if "%1"=="SkipCleanUp" goto SkipCleanUp
rmdir MediaConch_GUI_Windows_i386\ /S /Q
:SkipCleanUp
