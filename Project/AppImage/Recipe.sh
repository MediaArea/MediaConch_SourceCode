#!/bin/bash

#  Copyright (c) MediaArea.net SARL. All Rights Reserved.
#
#  Use of this source code is governed by a GPL v3+ and MPL v2+ license that can
#  be found in the License.html file in the root of the source tree.
#

# This script build mediaconch mediaconch-server and mediaconch-gui AppImages
# This script is supposed to be run on a CentOS 6 container or chroot"

function Make_image() {
    local APP=$1 BIN=$2 DESKTOP=$3 ICON=$4 LOWERAPP=${1,,}

    mkdir -p $LOWERAPP/$LOWERAPP.AppDir
    pushd $LOWERAPP
    pushd $LOWERAPP.AppDir

    mkdir -p usr/bin

    cp $BIN usr/bin/
    cp $DESKTOP $ICON .

    if [ "$LOWERAPP" == "mediaconch-gui" ] ; then
        if [ "$ARCH" == "x86_64" ] ; then
            mkdir -p usr/lib64/qt5
            cp -r /usr/lib64/qt5/plugins usr/lib64/qt5
        else
            mkdir -p usr/lib/qt5
            cp -r /usr/lib/qt5/plugins usr/lib/qt5
        fi
        get_desktopintegration $LOWERAPP
    fi

    get_apprun
    # Multiple runs to ensure we catch indirect ones
    copy_deps; copy_deps; copy_deps; copy_deps
    move_lib
    delete_blacklisted
    if [ "$ARCH" == "x86_64" ] ; then
        cp -f /usr/lib64/libnss3.so usr/lib64
    else
        cp -f /usr/lib/libnss3.so usr/lib
    fi
    popd
    generate_appimage
    popd
}

# Detect host
if ! grep "CentOS release 6\..*" /etc/centos-release ; then
    echo "This script is supposed to be run on a CentOS 6 container or chroot"
    exit 1
fi

# Setup evironment
VERSION=17.08

if [ "$(arch)" == "i386" ] ; then
    ARCH="i686"
else
    ARCH="$(arch)"
fi

PREFIX=$PWD/prefix

# Get AppImage utils
curl -L -O https://github.com/probonopd/AppImages/raw/master/functions.sh

# Fix functions.sh
sed -i "s/-x86_64/-$ARCH/g" functions.sh

source ./functions.sh

# Add EPEL repository
curl -L -O https://dl.fedoraproject.org/pub/epel/epel-release-latest-6.noarch.rpm
rpm -i --nodeps epel-release-*.rpm

# Install build and AppImage dependencies
yum install -y file wget tar fuse-libs fuse gcc-c++ pkgconfig libtool automake autoconf git zlib-devel libxml2-devel \
               libxslt-devel sqlite-devel libcurl-devel jansson-devel qt5-qtbase-devel qt5-qtwebkit-devel

# Compile libevent (too old on CentOS 6)
curl -L https://github.com/libevent/libevent/releases/download/release-2.0.22-stable/libevent-2.0.22-stable.tar.gz | tar xz

pushd libevent-2.0.22-stable
if [ "$ARCH" == "x86_64" ] ; then
    ./configure --enable-shared --disable-static --prefix=/usr --libdir=/usr/lib64
else
    ./configure --enable-shared --disable-static --prefix=/usr
fi
    if test ! -e Makefile; then
        echo Problem while configuring libevent
        exit 1
    fi

    make -j$(nproc)

    if test ! -e libevent.la; then
        echo Problem while compiling libevent
        exit 1
    fi

    make install
popd

# Compile ZenLib
if test -e ZenLib/Project/GNU/Library; then
    pushd ZenLib/Project/GNU/Library
    autoreconf -i

    if [ "$ARCH" == "x86_64" ] ; then
        ./configure --enable-shared --disable-static --prefix=/usr --libdir=/usr/lib64
    else
        ./configure --enable-shared --disable-static --prefix=/usr
    fi

    if test ! -e Makefile; then
        echo Problem while configuring ZenLib
        exit 1
    fi

    make -j$(nproc)

    if test ! -e libzen.la; then
        echo Problem while compiling ZenLib
        exit 1
    fi

    make install
    popd
else
    echo ZenLib directory is not found
    exit 1
fi

# Compile MediaInfoLib
if test -e MediaInfoLib/Project/GNU/Library; then
    pushd MediaInfoLib/Project/GNU/Library
    autoreconf -i

    if [ "$ARCH" == "x86_64" ] ; then
        ./configure --enable-shared --disable-static --prefix=/usr --libdir=/usr/lib64 --with-libcurl
    else
        ./configure --enable-shared --disable-static --prefix=/usr --with-libcurl
    fi

    if test ! -e Makefile; then
        echo Problem while configuring MediaInfoLib
        exit 1
    fi

    make -j$(nproc)

    if test ! -e libmediainfo.la; then
        echo Problem while compiling MediaInfoLib
        exit 1
    fi

    make install
    popd
else
    echo MediaInfoLib directory is not found
    exit 1
fi


# Compile MediaConch
if test -e MediaConch; then
    # CLI
    pushd MediaConch/Project/GNU/CLI
    autoreconf -i
    ./configure --prefix=$PREFIX

    if test ! -e Makefile; then
        echo Problem while configuring MediaConch
        exit 1
    fi

    make -j$(nproc)

    if test ! -e mediaconch; then
        echo Problem while compiling MediaConch
        exit 1
    fi

    make install
    popd

   # Server
    pushd MediaConch/Project/GNU/Server
    autoreconf -i
    ./configure --prefix=$PREFIX

    if test ! -e Makefile; then
        echo Problem while configuring MediaConch Server
        exit 1
    fi

    make -j$(nproc)

    if test ! -e mediaconchd; then
        echo Problem while compiling MediaConch Server
        exit 1
    fi

    make install
    popd

    # GUI
    pushd MediaConch/Project/Qt
    ./prepare USE_WEBKIT=1

    if test ! -e Makefile; then
        echo Problem while configuring MediaConch GUI
        exit 1
    fi

    make -j$(nproc)

    if test ! -e mediaconch-gui; then
        echo Problem while compiling MediaConch GUI
        exit 1
    fi

    install -m 755 -d $PREFIX/bin
    install -m 755 mediaconch-gui $PREFIX/bin

    popd
else
    echo MediaConch directory is not found
    exit 1
fi

# Make appImages
cp ${PWD}/MediaConch/Source/Resource/Image/MediaConch.png mediaconch.png

cat > mediaconch.desktop <<EOF
[Desktop Entry]
Comment=Implementation checker, policy checker, reporter, and fixer (CLI)
Name=mediaconch
Exec=mediaconch
Icon=mediaconch
Terminal=true
Categories=Multimedia;
EOF

cat > mediaconch-server.desktop <<EOF
[Desktop Entry]
Comment=Implementation checker, policy checker, reporter, and fixer (Server)
Name=mediaconchd
Exec=mediaconchd
Icon=mediaconch
Terminal=true
Categories=Multimedia;
EOF

Make_image mediaconch $PREFIX/bin/mediaconch \
                      ${PWD}/mediaconch.desktop \
                      ${PWD}/mediaconch.png

Make_image mediaconch-server $PREFIX/bin/mediaconchd \
                             ${PWD}/mediaconch-server.desktop \
                             ${PWD}/mediaconch.png

Make_image mediaconch-gui $PREFIX/bin/mediaconch-gui \
                          ${PWD}/MediaConch/Project/GNU/GUI/mediaconch-gui.desktop \
                          ${PWD}/mediaconch.png
