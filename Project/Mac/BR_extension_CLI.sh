#! /bin/sh

# Because of the autotools bug
cd ZenLib/Project/GNU/Library
./autogen.sh
cd ../../../../MediaInfoLib/Project/GNU/Library
./autogen.sh
cd ../../../../MediaConch/Project/GNU/CLI
./autogen.sh
cd ../../../..

# libxml2 and libcurl doesn’t like multiarch
#./CLI_Compile.sh --enable-arch-x86_64 --enable-arch-i386
./CLI_Compile.sh
