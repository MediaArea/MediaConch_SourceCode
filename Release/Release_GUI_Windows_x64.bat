@rem Copyright (c) MediaArea.net SARL. All Rights Reserved.
@rem
@rem Use of this source code is governed by a GPL v3+ and MPL v2+ license that can
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
del   MediaConch_GUI_Windows_x64.exe
del   MediaConch_GUI_Windows_x64_WithoutInstaller.7z
rmdir MediaConch_GUI_Windows_x64 /S /Q
mkdir MediaConch_GUI_Windows_x64


@rem --- Copying : Exe ---
copy  ..\Project\MSVC2017\x64\Release\MediaConch-GUI.exe MediaConch_GUI_Windows_x64\MediaConch.exe
copy  ..\Project\MSVC2017\x64\Release\LIBCURL.DLL MediaConch_GUI_Windows_x64\
copy  ..\Project\MSVC2017\x64\Release\Qt5Core.dll MediaConch_GUI_Windows_x64\
copy  ..\Project\MSVC2017\x64\Release\Qt5Gui.dll MediaConch_GUI_Windows_x64\
copy  ..\Project\MSVC2017\x64\Release\Qt5Network.dll MediaConch_GUI_Windows_x64\
copy  ..\Project\MSVC2017\x64\Release\libEGL.dll MediaConch_GUI_Windows_x64\
copy  ..\Project\MSVC2017\x64\Release\libGLESV2.dll MediaConch_GUI_Windows_x64\
copy  ..\Project\MSVC2017\x64\Release\d3dcompiler_47.dll MediaConch_GUI_Windows_x64\
copy  ..\Project\MSVC2017\x64\Release\Qt5Qml.dll MediaConch_GUI_Windows_x64\
copy  ..\Project\MSVC2017\x64\Release\Qt5Quick.dll MediaConch_GUI_Windows_x64\
copy  ..\Project\MSVC2017\x64\Release\Qt5WebChannel.dll MediaConch_GUI_Windows_x64\
copy  ..\Project\MSVC2017\x64\Release\Qt5WebEngineCore.dll MediaConch_GUI_Windows_x64\
copy  ..\Project\MSVC2017\x64\Release\Qt5WebEngineWidgets.dll MediaConch_GUI_Windows_x64\
copy  ..\Project\MSVC2017\x64\Release\Qt5WebKit.dll MediaConch_GUI_Windows_x64\
copy  ..\Project\MSVC2017\x64\Release\Qt5WebKitWidgets.dll MediaConch_GUI_Windows_x64\
copy  ..\Project\MSVC2017\x64\Release\Qt5WebSockets.dll MediaConch_GUI_Windows_x64\
copy  ..\Project\MSVC2017\x64\Release\Qt5PrintSupport.dll MediaConch_GUI_Windows_x64\
copy  ..\Project\MSVC2017\x64\Release\Qt5QuickWidgets.dll MediaConch_GUI_Windows_x64\
copy  ..\Project\MSVC2017\x64\Release\Qt5Positioning.dll MediaConch_GUI_Windows_x64\
copy  ..\Project\MSVC2017\x64\Release\Qt5Widgets.dll MediaConch_GUI_Windows_x64\
copy  ..\Project\MSVC2017\x64\Release\QtWebEngineProcess.exe MediaConch_GUI_Windows_x64\
copy  "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Redist\MSVC\14.11.25325\x64\Microsoft.VC141.CRT\concrt140.dll" MediaConch_GUI_Windows_x64\
copy  "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Redist\MSVC\14.11.25325\x64\Microsoft.VC141.CRT\msvcp140.dll" MediaConch_GUI_Windows_x64\
copy  "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Redist\MSVC\14.11.25325\x64\Microsoft.VC141.CRT\vccorlib140.dll" MediaConch_GUI_Windows_x64\
copy  "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Redist\MSVC\14.11.25325\x64\Microsoft.VC141.CRT\vcruntime140.dll" MediaConch_GUI_Windows_x64\
mkdir MediaConch_GUI_Windows_x64\resources
copy  ..\Project\MSVC2017\x64\Release\resources\icudtl.dat MediaConch_GUI_Windows_x64\resources\icudtl.dat
copy  ..\Project\MSVC2017\x64\Release\resources\qtwebengine_resources.pak MediaConch_GUI_Windows_x64\resources\qtwebengine_resources.pak
copy  ..\Project\MSVC2017\x64\Release\resources\qtwebengine_resources_100p.pak MediaConch_GUI_Windows_x64\resources\qtwebengine_resources_100p.pak
copy  ..\Project\MSVC2017\x64\Release\resources\qtwebengine_resources_200p.pak MediaConch_GUI_Windows_x64\resources\qtwebengine_resources_200p.pak
mkdir MediaConch_GUI_Windows_x64\platforms
copy  ..\Project\MSVC2017\x64\Release\platforms\qwindows.dll MediaConch_GUI_Windows_x64\platforms\
mkdir MediaConch_GUI_Windows_x64\translations\
copy  ..\Project\MSVC2017\x64\Release\translations\qt_en.qm MediaConch_GUI_Windows_x64\translations\qt_en.qm
mkdir MediaConch_GUI_Windows_x64\translations\qtwebengine_locales
copy  ..\Project\MSVC2017\x64\Release\translations\qtwebengine_locales\en-US.pak MediaConch_GUI_Windows_x64\translations\qtwebengine_locales\en-US.pak

@rem --- Copying : Information files ---
copy ..\License.html MediaConch_GUI_Windows_x64\
copy ..\License.GPLv3.html MediaConch_GUI_Windows_x64\
copy ..\License.MPLv2.html MediaConch_GUI_Windows_x64\
copy ..\History_GUI.txt MediaConch_GUI_Windows_x64\History.txt
copy ReadMe_GUI_Windows.txt MediaConch_GUI_Windows_x64\ReadMe.txt


rem --- Compressing Archive ---
cd MediaConch_GUI_Windows_x64\
%BPATH%\Windows\7-Zip\7z a -r -t7z -mx9 ..\MediaConch_GUI_Windows_x64_WithoutInstaller.7z *
cd ..

rem --- Clean up ---
if "%1"=="SkipCleanUp" goto SkipCleanUp
rmdir MediaConch_GUI_Windows_x64\ /S /Q
:SkipCleanUp
