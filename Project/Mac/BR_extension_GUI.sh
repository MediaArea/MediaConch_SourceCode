#! /bin/sh

# Because of the autotools bug
cd ZenLib/Project/GNU/Library
./autogen.sh
cd ../../../../MediaInfoLib/Project/GNU/Library
./autogen.sh
cd ../../../..

# libxml2 and libcurl doesn’t like multiarch
#./GUI_Compile.sh --enable-arch-x86_64 --enable-arch-i386
./GUI_Compile.sh
