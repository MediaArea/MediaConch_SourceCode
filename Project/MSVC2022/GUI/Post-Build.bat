if "%1"=="" echo "Post-Build.bat Output_Dir" | find /v "0 "

windeployqt --release %1
