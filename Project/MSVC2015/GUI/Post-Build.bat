if "%2"=="" echo "Post-Build.bat Qt_Dir Output_Dir" | find /v "0 "

if not exist %1 (
echo Missing Qt directory %1. Correct the Qt path in project properties: Include Directories, Library Directories, Pre-Build, Post-Build.
%1\bin\windeployqt
) else (
%1\bin\windeployqt %2
)
