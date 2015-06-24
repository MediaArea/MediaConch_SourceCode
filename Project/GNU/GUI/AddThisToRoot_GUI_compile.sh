#! /bin/sh

#############################################################################
# Configure
Home=`pwd`
ZenLib_Options=" --without-subdirs --enable-gui"

#############################################################################
# Setup for parallel builds
Zen_Make()
{
 numprocs=1
 if test -e /proc/stat; then
  numprocs=`egrep -c ^cpu[0-9]+ /proc/stat || :`
  if [ "$numprocs" = "" ] || [ "$numprocs" = "0" ]; then
   numprocs=1
  fi
 fi
 if type sysctl &> /dev/null; then
  numprocs=`sysctl -n hw.ncpu`
  if [ "$numprocs" = "" ] || [ "$numprocs" = "0" ]; then
   numprocs=1
  fi
 fi
 make -s -j$numprocs
}

#############################################################################
# ZenLib
if test -e ZenLib/Project/GNU/Library/configure; then
 cd ZenLib/Project/GNU/Library/
 test -e Makefile && rm Makefile
 chmod u+x configure
 ./configure $ZenLib_Options $*
 if test -e Makefile; then
  make clean
  Zen_Make
  if test -e libzen.la; then
   echo ZenLib compiled
  else
   echo Problem while compiling ZenLib
   exit
  fi
 else
  echo Problem while configuring ZenLib
  exit
 fi
else
 echo ZenLib directory is not found
 exit
fi
cd $Home

#############################################################################
# MediaInfoLib
if test -e MediaInfoLib/Project/GNU/Library/configure; then
 cd MediaInfoLib/Project/GNU/Library/
 test -e Makefile && rm Makefile
 chmod u+x configure
 ./configure $*
 if test -e Makefile; then
  make clean
  Zen_Make
  if test -e libmediainfo.la; then
   echo MediaInfoLib compiled
  else
   echo Problem while compiling MediaInfoLib
   exit
  fi
 else
  echo Problem while configuring MediaInfoLib
  exit
 fi
else
 echo MediaInfoLib directory is not found
 exit
fi
cd $Home

#############################################################################
# MediaConch
if test -e MediaConch/Project/Qt/prepare; then
 cd MediaConch/Project/Qt
 test -e Makefile && rm Makefile
 chmod u+x prepare
 ./prepare $*
 if test -e Makefile; then
  make clean
  Zen_Make
  if test -e mediaconch-gui; then
   echo MediaConch compiled
  else
   echo Problem while compiling MediaConch
   exit
  fi
 else
  echo Problem while configuring MediaConch
  exit
 fi
else
 echo MediaConch directory is not found
 exit
fi
cd $Home

#############################################################################
# Going home
cd $Home
echo "MediaConch executable is in MediaConch/Project/Qt"
