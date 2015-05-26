##  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 #
 #  Use of this source code is governed by a BSD-style license that can
 #  be found in the License.html file in the root of the source tree.
 ##

#! /bin/sh

#-----------------------------------------------------------------------
# Clean up
test -e MediaConch_GUI_GNU_FromSource.tar     && rm    MediaConch_GUI_GNU_FromSource.tar
test -e MediaConch_GUI_GNU_FromSource.tar.bz2 && rm    MediaConch_GUI_GNU_FromSource.tar.bz2
test -d MediaConch_GUI_GNU_FromSource         && rm -r MediaConch_GUI_GNU_FromSource
mkdir MediaConch_GUI_GNU_FromSource


#-----------------------------------------------------------------------
# Preparing : Project
cd ../Project/GNU/GUI
chmod u+x autogen
./autogen
cd ../../../Release


#-----------------------------------------------------------------------
# Copying : Project
mkdir -p MediaConch_GUI_GNU_FromSource/Project/GNU/GUI
cp ../Project/GNU/GUI/aclocal.m4 MediaConch_GUI_GNU_FromSource/Project/GNU/GUI/
cp ../Project/GNU/GUI/autogen MediaConch_GUI_GNU_FromSource/Project/GNU/GUI/
cp ../Project/GNU/GUI/config.guess MediaConch_GUI_GNU_FromSource/Project/GNU/GUI/
cp ../Project/GNU/GUI/config.sub MediaConch_GUI_GNU_FromSource/Project/GNU/GUI/
cp ../Project/GNU/GUI/configure MediaConch_GUI_GNU_FromSource/Project/GNU/GUI/
cp ../Project/GNU/GUI/configure.ac MediaConch_GUI_GNU_FromSource/Project/GNU/GUI/
cp ../Project/GNU/GUI/depcomp MediaConch_GUI_GNU_FromSource/Project/GNU/GUI/
cp ../Project/GNU/GUI/install-sh MediaConch_GUI_GNU_FromSource/Project/GNU/GUI/
cp ../Project/GNU/GUI/ltmain.sh MediaConch_GUI_GNU_FromSource/Project/GNU/GUI/
cp ../Project/GNU/GUI/Makefile.am MediaConch_GUI_GNU_FromSource/Project/GNU/GUI/
cp ../Project/GNU/GUI/Makefile.in MediaConch_GUI_GNU_FromSource/Project/GNU/GUI/
cp ../Project/GNU/GUI/missing MediaConch_GUI_GNU_FromSource/Project/GNU/GUI/
cp ../Project/GNU/GUI/wxwin.m4 MediaConch_GUI_GNU_FromSource/Project/GNU/GUI/
cp ../Project/GNU/GUI/mediaconch-gui.appdata.xml MediaConch_GUI_GNU_FromSource/Project/GNU/GUI/
cp ../Project/GNU/GUI/mediaconch-gui.desktop MediaConch_GUI_GNU_FromSource/Project/GNU/GUI/
cp ../Project/GNU/GUI/mediaconch-gui.kde3.desktop MediaConch_GUI_GNU_FromSource/Project/GNU/GUI/
cp ../Project/GNU/GUI/mediaconch-gui.kde4.desktop MediaConch_GUI_GNU_FromSource/Project/GNU/GUI/

#-----------------------------------------------------------------------
# Copying : Source
mkdir -p MediaConch_GUI_GNU_FromSource/Source/Common
cp -r ../Source/Common/*.h MediaConch_GUI_GNU_FromSource/Source/Common/
cp -r ../Source/Common/*.cpp MediaConch_GUI_GNU_FromSource/Source/Common/
mkdir -p MediaConch_GUI_GNU_FromSource/Source/CLI
cp -r ../Source/CLI/*.h MediaConch_GUI_GNU_FromSource/Source/CLI/
cp -r ../Source/CLI/*.cpp MediaConch_GUI_GNU_FromSource/Source/CLI/
mkdir -p MediaConch_GUI_GNU_FromSource/Source/GUI/Common
cp -r ../Source/GUI/Common/*.h MediaConch_GUI_GNU_FromSource/Source/GUI/Common/
cp -r ../Source/GUI/Common/*.cpp MediaConch_GUI_GNU_FromSource/Source/GUI/Common/
mkdir -p MediaConch_GUI_GNU_FromSource/Source/GUI/WxWidgets
cp -r ../Source/GUI/WxWidgets/*.h MediaConch_GUI_GNU_FromSource/Source/GUI/WxWidgets/
cp -r ../Source/GUI/WxWidgets/*.cpp MediaConch_GUI_GNU_FromSource/Source/GUI/WxWidgets/
mkdir -p MediaConch_GUI_GNU_FromSource/Source/Resource/Image/Menu/K20
cp -r ../Source/Resource/Image/*.png MediaConch_GUI_GNU_FromSource/Source/Resource/Image/
cp -r ../Source/Resource/Image/*.xpm MediaConch_GUI_GNU_FromSource/Source/Resource/Image/
cp -r ../Source/Resource/Image/*.icns MediaConch_GUI_GNU_FromSource/Source/Resource/Image/
cp -r ../Source/Resource/Image/Menu/K20/*.xpm MediaConch_GUI_GNU_FromSource/Source/Resource/Image/Menu/K20/

#-----------------------------------------------------------------------
# Copying : Release
mkdir -p MediaConch_GUI_GNU_FromSource/Release
cp Release_GUI_GNU.sub MediaConch_GUI_GNU_FromSource/Release/
cp *.sh MediaConch_GUI_GNU_FromSource/Release/
chmod u+x MediaConch_GUI_GNU_FromSource/Release/*.sh

#-----------------------------------------------------------------------
# Copying : Information
cp ../License.html MediaConch_GUI_GNU_FromSource/
cp ../History_GUI.txt MediaConch_GUI_GNU_FromSource/
dos2unix MediaConch_GUI_GNU_FromSource/History_GUI.txt
cp ReadMe_GUI_Linux.txt MediaConch_GUI_GNU_FromSource/Release/
cp ReadMe_GUI_Mac.txt MediaConch_GUI_GNU_FromSource/Release/


#-----------------------------------------------------------------------
# Preparing Archive : MediaConch
mv MediaConch_GUI_GNU_FromSource MediaConch
mkdir MediaConch_GUI_GNU_FromSource
mv MediaConch MediaConch_GUI_GNU_FromSource/

#-----------------------------------------------------------------------
# Preparing Archive : From MediaInfoLib
cd ../../MediaInfoLib/Release
. ./Release_Lib_GNU_FromSource.sh
tar xf MediaConch_Lib_GNU_FromSource.tar.bz2
rm MediaConch_Lib_GNU_FromSource.tar.bz2
cd ../../MediaConch/Release
mv ../../MediaInfoLib/Release/MediaConch_Lib_GNU_FromSource MediaConch_GUI_GNU_FromSource/MediaInfoLib

#-----------------------------------------------------------------------
# Preparing Archive : From ZenLib
cd ../../ZenLib/Release
. ./Release_GNU_FromSource.sh
tar xf ZenLib_GNU_FromSource.tar.bz2
rm ZenLib_GNU_FromSource.tar.bz2
cd ../../MediaConch/Release
mv ../../ZenLib/Release/ZenLib_GNU_FromSource MediaConch_GUI_GNU_FromSource/ZenLib

#-----------------------------------------------------------------------
# Preparing Archive : From Shared
mkdir -p MediaConch_GUI_GNU_FromSource/Shared/Project/_Common
cp -r ../../Shared/Project/_Common/*.sh MediaConch_GUI_GNU_FromSource/Shared/Project/_Common
mkdir -p MediaConch_GUI_GNU_FromSource/Shared/Project/ZLib
cp -r ../../Shared/Project/ZLib/*.sh MediaConch_GUI_GNU_FromSource/Shared/Project/ZLib
mkdir -p MediaConch_GUI_GNU_FromSource/Shared/Project/WxWidgets
cp -r ../../Shared/Project/WxWidgets/*.sh MediaConch_GUI_GNU_FromSource/Shared/Project/WxWidgets

#-----------------------------------------------------------------------
# Preparing Archive : Automation
cp ../Project/GNU/GUI/AddThisToRoot_GUI_compile.sh MediaConch_GUI_GNU_FromSource/GUI_Compile.sh
chmod u+x MediaConch_GUI_GNU_FromSource/GUI_Compile.sh


#-----------------------------------------------------------------------
# Compressing Archive
tar jchf MediaConch_GUI_GNU_FromSource.tar.bz2 MediaConch_GUI_GNU_FromSource/*

#-----------------------------------------------------------------------
# Clean up
rm -r MediaConch_GUI_GNU_FromSource
