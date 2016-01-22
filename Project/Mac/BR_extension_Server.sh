#! /bin/sh

# Because of the autotools bug
cd ZenLib/Project/GNU/Library
./autogen.sh
cd ../../../../MediaInfoLib/Project/GNU/Library
./autogen.sh
cd ../../../../MediaConch/Project/GNU/Server
./autogen.sh
cd ../../../..

# Dependencies
cp -r ~/Documents/almin/libxml2 .
cp -r ~/Documents/almin/libxslt .
cp -r ~/Documents/almin/sqlite .
cp -r ~/Documents/almin/jansson .
cp -r ~/Documents/almin/libevent .

./Server_Compile.sh
