##  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 #
 #  Use of this source code is governed by a BSD-style license that can
 #  be found in the License.html file in the root of the source tree.
 ##

#! /bin/sh

#-----------------------------------------------------------------------
# Clean up
test -e MediaConch_CLI_GNU_FromSource.tar     && rm    MediaConch_CLI_GNU_FromSource.tar
test -e MediaConch_CLI_GNU_FromSource.tar.bz2 && rm    MediaConch_CLI_GNU_FromSource.tar.bz2
test -d MediaConch_CLI_GNU_FromSource         && rm -r MediaConch_CLI_GNU_FromSource
mkdir MediaConch_CLI_GNU_FromSource


#-----------------------------------------------------------------------
# Preparing : Project
cd ../Project/GNU/CLI
chmod u+x autogen
./autogen
cd ../../../Release


#-----------------------------------------------------------------------
# Copying : Project
mkdir -p MediaConch_CLI_GNU_FromSource/Project/GNU/CLI
cp ../Project/GNU/CLI/aclocal.m4 MediaConch_CLI_GNU_FromSource/Project/GNU/CLI/
cp ../Project/GNU/CLI/autogen MediaConch_CLI_GNU_FromSource/Project/GNU/CLI/
cp ../Project/GNU/CLI/config.guess MediaConch_CLI_GNU_FromSource/Project/GNU/CLI/
cp ../Project/GNU/CLI/config.sub MediaConch_CLI_GNU_FromSource/Project/GNU/CLI/
cp ../Project/GNU/CLI/configure MediaConch_CLI_GNU_FromSource/Project/GNU/CLI/
cp ../Project/GNU/CLI/configure.ac MediaConch_CLI_GNU_FromSource/Project/GNU/CLI/
cp ../Project/GNU/CLI/depcomp MediaConch_CLI_GNU_FromSource/Project/GNU/CLI/
cp ../Project/GNU/CLI/install-sh MediaConch_CLI_GNU_FromSource/Project/GNU/CLI/
cp ../Project/GNU/CLI/ltmain.sh MediaConch_CLI_GNU_FromSource/Project/GNU/CLI/
cp ../Project/GNU/CLI/Makefile.am MediaConch_CLI_GNU_FromSource/Project/GNU/CLI/
cp ../Project/GNU/CLI/Makefile.in MediaConch_CLI_GNU_FromSource/Project/GNU/CLI/
cp ../Project/GNU/CLI/missing MediaConch_CLI_GNU_FromSource/Project/GNU/CLI/

#-----------------------------------------------------------------------
# Copying : Source
mkdir -p MediaConch_CLI_GNU_FromSource/Source/Common
cp -r ../Source/Common/*.h MediaConch_CLI_GNU_FromSource/Source/Common/
cp -r ../Source/Common/*.cpp MediaConch_CLI_GNU_FromSource/Source/Common/
mkdir -p MediaConch_CLI_GNU_FromSource/Source/CLI
cp -r ../Source/CLI/*.h MediaConch_CLI_GNU_FromSource/Source/CLI/
cp -r ../Source/CLI/*.cpp MediaConch_CLI_GNU_FromSource/Source/CLI/

#-----------------------------------------------------------------------
# Copying : Release
mkdir -p MediaConch_CLI_GNU_FromSource/Release
cp Release_CLI_GNU.sub MediaConch_CLI_GNU_FromSource/Release/
cp *.sh MediaConch_CLI_GNU_FromSource/Release/
chmod u+x MediaConch_CLI_GNU_FromSource/Release/*.sh

#-----------------------------------------------------------------------
# Copying : Information
cp ../License.html MediaConch_CLI_GNU_FromSource/
cp ../History_CLI.txt MediaConch_CLI_GNU_FromSource/
dos2unix MediaConch_CLI_GNU_FromSource/History_CLI.txt
cp ReadMe_CLI_Linux.txt MediaConch_CLI_GNU_FromSource/Release/
cp ReadMe_CLI_Mac.txt MediaConch_CLI_GNU_FromSource/Release/

#-----------------------------------------------------------------------
# Copying : Contrib
mkdir -p MediaConch_CLI_GNU_FromSource/Contrib
cp ../Contrib/CLI_Help.doc MediaConch_CLI_GNU_FromSource/Contrib/


#-----------------------------------------------------------------------
# Preparing Archive : MediaConch
mv MediaConch_CLI_GNU_FromSource MediaConch
mkdir MediaConch_CLI_GNU_FromSource
mv MediaConch MediaConch_CLI_GNU_FromSource/

#-----------------------------------------------------------------------
# Preparing Archive : From MediaInfoLib
cd ../../MediaInfoLib/Release
. ./Release_Lib_GNU_FromSource.sh
tar xf MediaConch_Lib_GNU_FromSource.tar.bz2
rm MediaConch_Lib_GNU_FromSource.tar.bz2
cd ../../MediaConch/Release
mv ../../MediaInfoLib/Release/MediaConch_Lib_GNU_FromSource MediaConch_CLI_GNU_FromSource/MediaInfoLib

#-----------------------------------------------------------------------
# Preparing Archive : From ZenLib
cd ../../ZenLib/Release
. ./Release_GNU_FromSource.sh
tar xf ZenLib_GNU_FromSource.tar.bz2
rm ZenLib_GNU_FromSource.tar.bz2
cd ../../MediaConch/Release
mv ../../ZenLib/Release/ZenLib_GNU_FromSource MediaConch_CLI_GNU_FromSource/ZenLib

#-----------------------------------------------------------------------
# Preparing Archive : From Shared
mkdir -p MediaConch_CLI_GNU_FromSource/Shared/Project/_Common
cp -r ../../Shared/Project/_Common/*.sh MediaConch_CLI_GNU_FromSource/Shared/Project/_Common
mkdir -p MediaConch_CLI_GNU_FromSource/Shared/Project/curl
cp -r ../../Shared/Project/curl/*.sh MediaConch_CLI_GNU_FromSource/Shared/Project/curl
mkdir -p MediaConch_CLI_GNU_FromSource/Shared/Project/ZLib
cp -r ../../Shared/Project/ZLib/*.sh MediaConch_CLI_GNU_FromSource/Shared/Project/ZLib
mkdir -p MediaConch_CLI_GNU_FromSource/Shared/Project/ZLib/Template/projects/GNU
cp -r ../../Shared/Project/ZLib/Template/projects/GNU/* MediaConch_CLI_GNU_FromSource/Shared/Project/ZLib/Template/projects/GNU
mkdir -p MediaConch_CLI_GNU_FromSource/Shared/Project/WxWidgets
cp -r ../../Shared/Project/WxWidgets/*.sh MediaConch_CLI_GNU_FromSource/Shared/Project/WxWidgets

#-----------------------------------------------------------------------
# Preparing Archive : Automation
cp ../Project/GNU/CLI/AddThisToRoot_CLI_compile.sh MediaConch_CLI_GNU_FromSource/CLI_Compile.sh
chmod u+x MediaConch_CLI_GNU_FromSource/CLI_Compile.sh


#-----------------------------------------------------------------------
# Compressing Archive
tar jchf MediaConch_CLI_GNU_FromSource.tar.bz2 MediaConch_CLI_GNU_FromSource/*

#-----------------------------------------------------------------------
# Clean up
rm -r MediaConch_CLI_GNU_FromSource
