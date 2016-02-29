@rem Copyright (c) MediaArea.net SARL. All Rights Reserved.
@rem
@rem Use of this source code is governed by a BSD-style license that can
@rem be found in the License.html file in the root of the source tree.
@rem

@rem echo off

@rem --- Clean up ---
del   MediaConch_Server_Windows_i386.zip
rmdir MediaConch_Server_Windows_i386 /S /Q
mkdir MediaConch_Server_Windows_i386

rem --- Copying : Exe ---
xcopy ..\Project\MSVC2013\Win32\Release\MediaConch.exe MediaConch_Server_Windows_i386\ /S
xcopy ..\Project\MSVC2013\Win32\Release\LIBCURL.DLL MediaConch_Server_Windows_i386\ /S

rem --- Copying : Information files ---
copy ..\License.* MediaConch_Server_Windows_i386\
copy ..\History_Server.txt MediaConch_Server_Windows_i386\History.txt
copy Readme_Server_Windows.txt MediaConch_Server_Windows_i386\ReadMe.txt

rem --- Compressing Archive ---
cd MediaConch_Server_Windows_i386\
..\..\..\Shared\Binary\Windows_i386\7-Zip\7z a -r -tzip ..\MediaConch_Server_Windows_i386.zip *
cd ..


rem --- Clean up ---
if "%1"=="SkipCleanUp" goto SkipCleanUp
rmdir MediaConch_Server_Windows_i386\ /S /Q
:SkipCleanUp
