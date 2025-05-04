if "%1"=="" echo "Pre-Build.bat Kit|Engine" | find /v "0 "

qmake ..\..\Qt\MediaConch.pro USE_WEB%1=1 2> nul
nmake /NOLOGO /f Makefile.Release compiler_uic_make_all compiler_rcc_make_all mocables
