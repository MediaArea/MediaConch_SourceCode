@rem Copyright (c) MediaArea.net SARL. All Rights Reserved.
@rem
@rem Use of this source code is governed by a BSD-style license that can
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
del mediaconch_.tar.bz2
del MediaConch_Source.tar
del mediaconch_-1.tar.gz
del mediaconch__AllInclusive.7z
rmdir MediaConch_Source /S /Q
mkdir MediaConch_Source


@rem --- Copying : Sources ---
xcopy ..\Source\*.cpp MediaConch_Source\Source\ /S
xcopy ..\Source\*.h MediaConch_Source\Source\ /S
xcopy ..\Source\*.dfm MediaConch_Source\Source\ /S
xcopy ..\Source\*.xml MediaConch_Source\Source\ /S
xcopy ..\Source\*.rc MediaConch_Source\Source\ /S
xcopy ..\Source\*.csv MediaConch_Source\Source\ /S
xcopy ..\Source\*.ico MediaConch_Source\Source\ /S
xcopy ..\Source\Install\*.nsi MediaConch_Source\Source\Install\
xcopy ..\Source\*.sh  MediaConch_Source\Source\ /S
xcopy ..\Source\*.png MediaConch_Source\Source\ /S
xcopy ..\Source\*.xpm MediaConch_Source\Source\ /S
xcopy ..\Source\*.svg MediaConch_Source\Source\ /S
xcopy ..\Source\*.qrc MediaConch_Source\Source\ /S
xcopy ..\Source\*.ui MediaConch_Source\Source\ /S

@rem --- Copying : Projects ---
xcopy ..\Project\*.bpg MediaConch_Source\Project\ /S
xcopy ..\Project\*.bpf MediaConch_Source\Project\ /S
xcopy ..\Project\*.bpr MediaConch_Source\Project\ /S
xcopy ..\Project\*.bdsgroup MediaConch_Source\Project\ /S
xcopy ..\Project\*.bdsproj MediaConch_Source\Project\ /S
xcopy ..\Project\*.groupproj MediaConch_Source\Project\ /S
xcopy ..\Project\*.cbproj MediaConch_Source\Project\ /S
xcopy ..\Project\*.dev MediaConch_Source\Project\ /S
xcopy ..\Project\*.sln MediaConch_Source\Project\ /S
xcopy ..\Project\*.vc MediaConch_Source\Project\ /S
xcopy ..\Project\*.vcproj MediaConch_Source\Project\ /S
xcopy ..\Project\*.vcxproj MediaConch_Source\Project\ /S
xcopy ..\Project\*.vcxproj.filters MediaConch_Source\Project\ /S
xcopy ..\Project\*.ico MediaConch_Source\Project\ /S
xcopy ..\Project\*.workspace MediaConch_Source\Project\ /S
xcopy ..\Project\*.cbp MediaConch_Source\Project\ /S /Y
xcopy ..\Project\*.def MediaConch_Source\Project\ /S
xcopy ..\Project\*.idl MediaConch_Source\Project\ /S
xcopy ..\Project\*.rgs MediaConch_Source\Project\ /S
xcopy ..\Project\*.bat MediaConch_Source\Project\ /S
xcopy ..\Project\*.pro MediaConch_Source\Project\ /S
xcopy ..\Project\Qt\prepare MediaConch_Source\Project\Qt\ /S
xcopy ..\Project\BCB\MediaConch*.res MediaConch_Source\Project\BCB /S
xcopy ..\Project\MSVC2005\*.cpp MediaConch_Source\Project\MSVC2005\ /S
xcopy ..\Project\MSVC2005\*.c MediaConch_Source\Project\MSVC2005\ /S
xcopy ..\Project\MSVC2005\*.h MediaConch_Source\Project\MSVC2005\ /S
xcopy ..\Project\MSVC2005\*.rc MediaConch_Source\Project\MSVC2005\ /S
xcopy ..\Project\MSVC2008\*.cpp MediaConch_Source\Project\MSVC2008\ /S
xcopy ..\Project\MSVC2008\*.c MediaConch_Source\Project\MSVC2008\ /S
xcopy ..\Project\MSVC2008\*.h MediaConch_Source\Project\MSVC2008\ /S
xcopy ..\Project\MSVC2008\*.rc MediaConch_Source\Project\MSVC2008\ /S
xcopy ..\Project\MSVC2010\*.cpp MediaConch_Source\Project\MSVC2010\ /S
xcopy ..\Project\MSVC2010\*.c MediaConch_Source\Project\MSVC2010\ /S
xcopy ..\Project\MSVC2010\*.h MediaConch_Source\Project\MSVC2010\ /S
xcopy ..\Project\MSVC2010\*.rc MediaConch_Source\Project\MSVC2010\ /S
xcopy ..\Project\MSVC2012\*.cpp MediaConch_Source\Project\MSVC2012\ /S
xcopy ..\Project\MSVC2012\*.c MediaConch_Source\Project\MSVC2012\ /S
xcopy ..\Project\MSVC2012\*.h MediaConch_Source\Project\MSVC2012\ /S
xcopy ..\Project\MSVC2012\*.rc MediaConch_Source\Project\MSVC2012\ /S
xcopy ..\Project\MSVC2013\*.cpp MediaConch_Source\Project\MSVC2013\ /S
xcopy ..\Project\MSVC2013\*.c MediaConch_Source\Project\MSVC2013\ /S
xcopy ..\Project\MSVC2013\*.h MediaConch_Source\Project\MSVC2013\ /S
xcopy ..\Project\MSVC2013\*.rc MediaConch_Source\Project\MSVC2013\ /S
xcopy ..\Project\GNU\* MediaConch_Source\Project\GNU\ /S
xcopy ..\Project\Solaris\* MediaConch_Source\Project\Solaris\ /S

@rem --- Copying : Release ---
xcopy *.txt MediaConch_Source\Release\
xcopy *.bat MediaConch_Source\Release\
xcopy *.sh  MediaConch_Source\Release\
xcopy *.sub MediaConch_Source\Release\
xcopy BCB\*.txt MediaConch_Source\Release\BCB\ /S
xcopy MSVC\*.txt MediaConch_Source\Release\MSVC\ /S
xcopy GCC_MinGW32\*.txt MediaConch_Source\Release\GCC_MinGW32\ /S
xcopy GCC_Linux_i386\*.txt MediaConch_Source\Release\GCC_Linux_i386\ /S
xcopy Make_GCC.sh MediaConch_Source\Release\

@rem --- Copying : debian ---
xcopy ..\debian\* MediaConch_Source\debian\ /S

rem --- Copying : Information files ---
xcopy ..\*.txt MediaConch_Source\
xcopy ..\*.html MediaConch_Source\

rem --- Copying : Contrib files ---
xcopy ..\Contrib\*.doc MediaConch_Source\Contrib\ /S

rem --- Copying : CVS files ---
xcopy ..\*.cvsignore MediaConch_Source\


rem --- Compressing Archive ---
if "%2"=="SkipCompression" goto SkipCompression
move MediaConch_Source MediaConch
%BPATH%\Windows\7-zip\7z a -r -ttar -mx9 MediaConch_Source.tar MediaConch\*
%BPATH%\Windows\7-zip\7z a -r -tbzip2 -mx9 mediaconch_.tar.bz2 MediaConch_Source.tar
%BPATH%\Windows\7-zip\7z a -r -tgzip -mx9 mediaconch_-1.tar.gz MediaConch_Source.tar
:SkipCompression

rem --- Clean up ---
if "%1"=="SkipCleanUp" goto SkipCleanUp
rmdir MediaConch_Source /S /Q
rmdir MediaConch /S /Q
:SkipCleanUp
