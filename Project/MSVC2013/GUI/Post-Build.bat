if "%3"=="" echo "Post-Build.bat Qt_Dir Output_Dir Configuration_Name" | find /v "0 "

if not exist %1 (
echo Missing Qt directory %1. Correct the Qt path in project properties: Include Directories, Library Directories, Pre-Build, Post-Build.
%1\bin\qmake
) else if %3==Release (
xcopy /D %1\bin\Qt5Core.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5Quick.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5Qml.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5Widgets.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5Gui.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5MultimediaWidgets.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5Multimedia.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5Sensors.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5PrintSupport.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5Positioning.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5OpenGL.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5WebChannel.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5WebKit.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5WebKitWidgets.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5Network.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5Sql.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\icudt54.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\icuin54.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\icuuc54.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\plugins\platforms\qwindows.dll %2platforms\ | find /v "0 " || cd %CD%
) else (
xcopy /D %1\bin\Qt5Cored.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5Quickd.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5Qmld.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5Widgetsd.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5Guid.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5MultimediaWidgetsd.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5Multimediad.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5Sensorsd.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5PrintSupportd.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5Positioningd.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5OpenGLd.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5WebChanneld.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5WebKitd.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5WebKitWidgetsd.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5Networkd.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\Qt5Sqld.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\icudt54.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\icuin54.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\bin\icuuc54.dll %2 | find /v "0 " || cd %CD%
xcopy /D %1\plugins\platforms\qwindowsd.dll %2platforms\ | find /v "0 " || cd %CD%
)

