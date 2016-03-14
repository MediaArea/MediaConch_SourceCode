##  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 #
 #  Use of this source code is governed by a GPL v3+ and MPL v2+ license that can
 #  be found in the License.html file in the root of the source tree.
 ##

#! /bin/sh

#-----------------------------------------------------------------------
# Clean up
rm    MediaConch_GUI_GNU.tar
rm    MediaConch_GUI_GNU.tar.bz2
rm -r MediaConch_GUI_GNU
mkdir MediaConch_GUI_GNU

#-----------------------------------------------------------------------
# Copying : Exe
cp ../Project/GNU/GUI/MediaConch MediaConch_GUI_GNU/MediaConch

#-----------------------------------------------------------------------
# Copying : Information file
cp ../License.html MediaConch_GUI_GNU/
cp ../History_GUI.txt MediaConch_GUI_GNU/History.txt
cp ReadMe_GUI_Linux.txt MediaConch_GUI_GNU/ReadMe.txt

#-----------------------------------------------------------------------
# Compressing Archive
tar jcf MediaConch_GUI_GNU.tar.bz2 MediaConch_GUI_GNU
rm MediaConch_GUI_GNU.tar

#-----------------------------------------------------------------------
# Clean up
#if "%1"=="SkipCleanUp" goto SkipCleanUp
rm -r MediaConch_GUI_GNU
#:SkipCleanUp
