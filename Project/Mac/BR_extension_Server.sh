#! /bin/sh

# Because of the autotools bug
cd ZenLib/Project/GNU/Library
./autogen.sh
cd ../../../../MediaInfoLib/Project/GNU/Library
./autogen.sh
cd ../../../../MediaConch/Project/GNU/Server
./autogen.sh
cd ../../../..

./Server_Compile.sh
