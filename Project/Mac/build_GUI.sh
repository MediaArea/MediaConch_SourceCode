#! /bin/sh

# Because of the autotools bug
cd ZenLib/Project/GNU/Library
./autogen.sh
cd ../../../../MediaInfoLib/Project/GNU/Library
./autogen.sh
cd ../../../..

# Dependencies
cp -r ~/Documents/almin/libxml2 .
cp -r ~/Documents/almin/libxslt .

# libxml2 and libcurl doesn’t like multiarch
#./GUI_Compile.sh --enable-arch-x86_64 --enable-arch-i386
./GUI_Compile.sh

strip -u -r MediaConch/Project/Qt/MediaConch.app/Contents/MacOS/MediaConch
