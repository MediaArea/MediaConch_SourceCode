@rem Copyright (c) MediaArea.net SARL. All Rights Reserved.
@rem
@rem Use of this source code is governed by a BSD-style license that can
@rem be found in the License.html file in the root of the source tree.
@rem

@echo off

rem --- Clean up ---
del MediaConch_GUI_GNU_Prepare.7z
rmdir MediaConch_GUI_GNU_Prepare /S /Q
mkdir MediaConch_GUI_GNU_Prepare


rem --- Copying : Sources ---
xcopy ..\Source\Common\*.cpp MediaConch_GUI_GNU_Prepare\Source\Common\ /S
xcopy ..\Source\Common\*.h MediaConch_GUI_GNU_Prepare\Source\Common\ /S
xcopy ..\Source\CLI\*.cpp MediaConch_GUI_GNU_Prepare\Source\CLI\ /S
xcopy ..\Source\CLI\*.h MediaConch_GUI_GNU_Prepare\Source\CLI\ /S
xcopy ..\Source\GUI\Common\*.cpp MediaConch_GUI_GNU_Prepare\Source\GUI\Common\ /S
xcopy ..\Source\GUI\Common\*.h MediaConch_GUI_GNU_Prepare\Source\GUI\Common\ /S
xcopy ..\Source\GUI\WxWidgets\*.cpp MediaConch_GUI_GNU_Prepare\Source\GUI\WxWidgets\ /S
xcopy ..\Source\GUI\WxWidgets\*.h MediaConch_GUI_GNU_Prepare\Source\GUI\WxWidgets\ /S
xcopy ..\Source\*.png MediaConch_GUI_GNU_Prepare\Source\ /S
xcopy ..\Source\*.xpm MediaConch_GUI_GNU_Prepare\Source\ /S
xcopy ..\Source\*.icns MediaConch_GUI_GNU_Prepare\Source\ /S

rem --- Copying : Projects ---
xcopy ..\debian\* MediaConch_GUI_GNU_Prepare\debian\
xcopy ..\Project\GNU\mediaconch.* MediaConch_GUI_GNU_Prepare\Project\GNU\
xcopy ..\Project\GNU\GUI\autogen MediaConch_GUI_GNU_Prepare\Project\GNU\GUI\ /S
xcopy ..\Project\GNU\GUI\configure.ac MediaConch_GUI_GNU_Prepare\Project\GNU\GUI\ /S
xcopy ..\Project\GNU\GUI\Makefile.am MediaConch_GUI_GNU_Prepare\Project\GNU\GUI\ /S
xcopy ..\Project\GNU\GUI\wxwin.m4 MediaConch_GUI_GNU_Prepare\Project\GNU\GUI\ /S
xcopy ..\Project\GNU\GUI\AddThisToRoot_GUI_compile.sh MediaConch_GUI_GNU_Prepare\Project\GNU\GUI\ /S
xcopy ..\Project\GNU\GUI\mediaconch-gui.* MediaConch_GUI_GNU_Prepare\Project\GNU\GUI\ /S

rem --- Copying : Release ---
xcopy Release_GUI_GNU.sub MediaConch_GUI_GNU_Prepare\Release\
xcopy Release_GUI_*.sh MediaConch_GUI_GNU_Prepare\Release\

rem --- Copying : Information files ---
copy ..\History_GUI.txt MediaConch_GUI_GNU_Prepare\
copy ..\License.html MediaConch_GUI_GNU_Prepare\
copy ReadMe_GUI_Linux.txt MediaConch_GUI_GNU_Prepare\Release\
copy ReadMe_GUI_Mac.txt MediaConch_GUI_GNU_Prepare\Release\


rem --- Preparing Archive : MediaConch ---
move MediaConch_GUI_GNU_Prepare MediaConch
mkdir MediaConch_GUI_GNU_Prepare
move MediaConch MediaConch_GUI_GNU_Prepare

rem --- Preparing Archive : MediaInfoLib ---
cd ..\..\MediaInfoLib\Release
call Release_Lib_GNU_Prepare.bat SkipCleanUp SkipCompression
cd ..\..\MediaConch\Release
move ..\..\MediaInfoLib\Release\MediaConch_Lib_GNU_Prepare MediaConch_GUI_GNU_Prepare\MediaInfoLib

rem --- Preparing Archive : ZenLib ---
cd ..\..\ZenLib\Release
call Release_GNU_Prepare.bat SkipCleanUp SkipCompression
cd ..\..\MediaConch\Release
move ..\..\ZenLib\Release\ZenLib_GNU_Prepare MediaConch_GUI_GNU_Prepare\ZenLib

rem --- Shared ---
mkdir MediaConch_GUI_GNU_Prepare\Shared
xcopy ..\..\Shared\Project\_Common\* MediaConch_GUI_GNU_Prepare\Shared\Project\_Common\ /S
xcopy ..\..\Shared\Project\ZLib\*.sh MediaConch_GUI_GNU_Prepare\Shared\Project\ZLib\ /S
xcopy ..\..\Shared\Project\WxWidgets\*.sh MediaConch_GUI_GNU_Prepare\Shared\Project\WxWidgets\ /S


rem --- Compressing Archive ---
if "%2"=="SkipCompression" goto SkipCompression
..\..\Shared\Binary\Windows_i386\7-Zip\7z a -r -t7z -mx9 MediaConch_GUI_GNU_Prepare.7z MediaConch_GUI_GNU_Prepare\*
:SkipCompression

rem --- Clean up ---
if "%1"=="SkipCleanUp" goto SkipCleanUp
rmdir MediaConch_GUI_GNU_Prepare /S /Q
:SkipCleanUp
