if "%1"=="" echo "Pre-Build.bat Qt_Dir" | find /v "0 "

if not exist %1 (
echo Missing Qt directory %1. Correct the Qt path in project properties: Include Directories, Library Directories, Pre-Build, Post-Build.
%1\bin\qmake
) else (
%1\bin\qmake ..\..\Qt\MediaConch.pro 2> nul
nmake /NOLOGO /f Makefile.Debug compiler_uic_make_all compiler_rcc_make_all mocables 
)