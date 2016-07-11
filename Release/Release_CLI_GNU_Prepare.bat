@rem Copyright (c) MediaArea.net SARL. All Rights Reserved.
@rem
@rem Use of this source code is governed by a GPL v3+ and MPL v2+ license that can
@rem be found in the License.html file in the root of the source tree.
@rem

@echo off

rem --- Search binaries ---
set BPATH=
if exist "%~dp0\..\..\..\MediaArea-Utils-Binaries" set BPATH="%~dp0\..\..\..\MediaArea-Utils-Binaries"
if exist "%~dp0\..\..\MediaArea-Utils-Binaries" set BPATH="%~dp0\..\..\MediaArea-Utils-Binaries"
if "%BPATH%"=="" (
    echo "ERROR: binaries path not found"
    exit /b 1
)

rem --- Clean up ---
del MediaConch_CLI_GNU_Prepare.7z
rmdir MediaConch_CLI_GNU_Prepare /S /Q
mkdir MediaConch_CLI_GNU_Prepare


rem --- Copying : Sources ---
xcopy ..\Source\Common\*.cpp MediaConch_CLI_GNU_Prepare\Source\Common\ /S
xcopy ..\Source\Common\*.h MediaConch_CLI_GNU_Prepare\Source\Common\ /S
xcopy ..\Source\CLI\*.cpp MediaConch_CLI_GNU_Prepare\Source\CLI\ /S
xcopy ..\Source\CLI\*.h MediaConch_CLI_GNU_Prepare\Source\CLI\ /S

rem --- Copying : Projects ---
xcopy ..\Project\GNU\CLI\autogen MediaConch_CLI_GNU_Prepare\Project\GNU\CLI\ /S
xcopy ..\Project\GNU\CLI\configure.ac MediaConch_CLI_GNU_Prepare\Project\GNU\CLI\ /S
xcopy ..\Project\GNU\CLI\Makefile.am MediaConch_CLI_GNU_Prepare\Project\GNU\CLI\ /S
xcopy ..\Project\GNU\CLI\AddThisToRoot_CLI_compile.sh MediaConch_CLI_GNU_Prepare\Project\GNU\CLI\ /S

rem --- Copying : Release ---
xcopy Release_CLI_GNU.sub MediaConch_CLI_GNU_Prepare\Release\
xcopy Release_CLI_*.sh MediaConch_CLI_GNU_Prepare\Release\

rem --- Copying : Information files ---
copy ..\History_CLI.txt MediaConch_CLI_GNU_Prepare\
copy ..\License.html MediaConch_CLI_GNU_Prepare\
copy ReadMe_CLI_Linux.txt MediaConch_CLI_GNU_Prepare\Release\
copy ReadMe_CLI_Mac.txt MediaConch_CLI_GNU_Prepare\Release\

rem --- Copying : Contrib files ---
mkdir MediaConch_CLI_GNU_Prepare\Contrib
copy ..\Contrib\CLI_Help.doc MediaConch_CLI_GNU_Prepare\Contrib\


rem --- Preparing Archive : MediaConch ---
move MediaConch_CLI_GNU_Prepare MediaConch
mkdir MediaConch_CLI_GNU_Prepare
move MediaConch MediaConch_CLI_GNU_Prepare

rem --- Preparing Archive : MediaInfoLib ---
cd ..\..\MediaInfoLib\Release
call Release_Lib_GNU_Prepare.bat SkipCleanUp SkipCompression
cd ..\..\MediaConch\Release
move ..\..\MediaInfoLib\Release\MediaConch_Lib_GNU_Prepare MediaConch_CLI_GNU_Prepare\MediaInfoLib

rem --- Preparing Archive : ZenLib ---
cd ..\..\ZenLib\Release
call Release_GNU_Prepare.bat SkipCleanUp SkipCompression
cd ..\..\MediaConch\Release
move ..\..\ZenLib\Release\ZenLib_GNU_Prepare MediaConch_CLI_GNU_Prepare\ZenLib

rem --- Shared ---
mkdir MediaConch_CLI_GNU_Prepare\Shared
xcopy ..\..\Shared\Project\_Common\* MediaConch_CLI_GNU_Prepare\Shared\Project\_Common\ /S
xcopy ..\..\Shared\Project\curl\*.sh MediaConch_CLI_GNU_Prepare\Shared\Project\curl\ /S
xcopy ..\..\Shared\Project\ZLib\*.sh MediaConch_CLI_GNU_Prepare\Shared\Project\ZLib\ /S
xcopy ..\..\Shared\Project\ZLib\Template\projects\GNU\* MediaConch_CLI_GNU_Prepare\Shared\Project\ZLib\Template\projects\GNU\ /S
xcopy ..\..\Shared\Project\WxWidgets\*.sh MediaConch_CLI_GNU_Prepare\Shared\Project\WxWidgets\ /S


rem --- Compressing Archive ---
if "%2"=="SkipCompression" goto SkipCompression
%BPATH%\Windows\7-Zip\7z a -r -t7z -mx9 MediaConch_CLI_GNU_Prepare.7z MediaConch_CLI_GNU_Prepare\*
:SkipCompression

rem --- Clean up ---
if "%1"=="SkipCleanUp" goto SkipCleanUp
rmdir MediaConch_CLI_GNU_Prepare /S /Q
:SkipCleanUp
