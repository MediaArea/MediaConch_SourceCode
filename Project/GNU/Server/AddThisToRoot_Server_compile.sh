#!/bin/sh

##################################################################

Parallel_Make () {
    local numprocs=1
    case $OS in
    'linux')
        numprocs=`grep -c ^processor /proc/cpuinfo 2>/dev/null`
        ;;
    'mac')
        if type sysctl &> /dev/null; then
            numprocs=`sysctl -n hw.ncpu`
        fi
        ;;
    #"solaris')
    #    on Solaris you need to use psrinfo -p instead
    #    ;;
    #'freebsd')
    #    ;;
    *) ;;
    esac
    if [ "$numprocs" = "" ] || [ "$numprocs" = "0" ]; then
        numprocs=1
    fi
    make -s -j$numprocs
}

##################################################################
# Init

Home=`pwd`
ZenLib_Options=""
MacOptions="--with-macosx-version-min=10.5"

OS=$(uname -s)
# expr isn't available on mac
if [ "$OS" = "Darwin" ]; then
    OS="mac"
# if the 5 first caracters of $OS equal "Linux"
elif [ "$(expr substr $OS 1 5)" = "Linux" ]; then
    OS="linux"
#elif [ "$(expr substr $OS 1 5)" = "SunOS" ]; then
#    OS="solaris"
#elif [ "$(expr substr $OS 1 7)" = "FreeBSD" ]; then
#    OS="freebsd"
fi

if [ "$OS" = "mac" ] ; then
    export CXXFLAGS="-mmacosx-version-min=10.5 $CXXFLAGS"
    export CFLAGS="-mmacosx-version-min=10.5 $CFLAGS"
    export LDFLAGS="-mmacosx-version-min=10.5 $LDFLAGS"
fi

##################################################################
# Configure zlib

if test -e Shared/Source/zlib/configure; then
    cd Shared/Source/zlib
    test -e Makefile && rm Makefile
    chmod +x configure
    ./configure --static $*
    if test ! -e Makefile; then
        echo Problem while configuring zlib
        exit
    fi
else
    echo zlib directory is not found
    exit
fi
cd $Home

##################################################################
# Configure libxml2

if test -e libxml2/configure; then
    cd libxml2
    test -e Makefile && rm Makefile
    chmod +x configure
    ./configure --without-python --without-modules --without-iconv --without-ftp --without-http --without-c14n --without-catalog --with-xpath --without-xptr --without-xinclude --without-iconv --without-icu --without-iso8859x --without-zlib --without-lzma --without-mem_debug --without-run_debug --without-regexps --with-tree --without-writer --with-pattern --with-push --without-valid --with-sax1 --without-legacy --with-output --without-schemas --with-schematron --enable-static --disable-shared $*
    if test ! -e Makefile; then
        echo Problem while configuring libxml2
        exit
    fi
else
    echo libxml2 directory is not found
    exit
fi
cd $Home

##################################################################
# Configure libxslt

if test -e libxslt/configure; then
    cd libxslt
    test -e Makefile && rm Makefile
    chmod +x configure
    ./configure --with-libxml-src="$Home"/libxml2 --without-python --without-modules --without-crypto --enable-static --disable-shared $*
    if test ! -e Makefile; then
        echo Problem while configuring libxslt
        exit
    fi
else
    echo libxslt directory is not found
    exit
fi
cd $Home

##################################################################
# Configure jansson

if test -e jansson/configure; then
    cd jansson
    test -e Makefile && rm Makefile
    chmod +x configure
    ./configure --enable-static --disable-shared $*
    if test ! -e Makefile; then
        echo Problem while configuring jansson
        exit
    fi
else
    echo jansson directory is not found
    exit
fi
cd $Home

##################################################################
# Configure libevent

if test -e libevent/configure; then
    cd libevent
    test -e Makefile && rm Makefile
    chmod +x configure
    ./configure --disable-openssl --enable-static --disable-shared $*
    if test ! -e Makefile; then
        echo Problem while configuring libevent
        exit
    fi
else
    echo libevent directory is not found
    exit
fi
cd $Home

##################################################################
# Configure sqlite

if test -e sqlite/configure; then
    cd sqlite
    test -e Makefile && rm Makefile
    chmod +x configure
    ./configure --enable-static --disable-shared $*
    if test ! -e Makefile; then
        echo Problem while configuring sqlite
        exit
    fi
else
    echo sqlite directory is not found
    exit
fi
cd $Home

##################################################################
# Configure ZenLib

if test -e ZenLib/Project/GNU/Library/configure; then
    cd ZenLib/Project/GNU/Library/
    test -e Makefile && rm Makefile
    chmod +x configure
    if [ "$OS" = "mac" ]; then
        ./configure --enable-static --disable-shared $MacOptions $ZenLib_Options $*
    else
        ./configure --enable-static --disable-shared $ZenLib_Options $*
    fi
    if test ! -e Makefile; then
        echo Problem while configuring ZenLib
        exit
    fi
else
    echo ZenLib directory is not found
    exit
fi
cd $Home

##################################################################
# Compile zlib

cd Shared/Source/zlib
make clean
Parallel_Make
if test -e libz.a; then
    echo zlib compiled
else
    echo Problem while compiling zlib
    exit
fi
cd $Home

##################################################################
# Compile ZenLib

cd ZenLib/Project/GNU/Library/
make clean
Parallel_Make
if test -e libzen.la; then
    echo ZenLib compiled
else
    echo Problem while compiling ZenLib
    exit
fi
cd $Home

##################################################################
# Configure MediaInfoLib

if test -e MediaInfoLib/Project/GNU/Library/configure; then
    cd MediaInfoLib/Project/GNU/Library/
    test -e Makefile && rm Makefile
    chmod +x configure
    if [ "$OS" = "mac" ]; then
        ./configure --enable-static --disable-shared $MacOptions --with-libcurl=runtime $*
    else
        ./configure --enable-static --disable-shared --with-libcurl=runtime $*
    fi
    if test ! -e Makefile; then
        echo Problem while configuring MediaInfoLib
        exit
    fi
else
    echo MediaInfoLib directory is not found
    exit
fi
cd $Home

##################################################################
# Compile libxml2

cd libxml2
make clean
Parallel_Make
if test -e libxml2.la; then
    echo libxml2 compiled
else
    echo Problem while compiling libxml2
    exit
fi
cd $Home

##################################################################
# Compile libxslt

cd libxslt
make clean
Parallel_Make
if test -e libxslt/.libs/libxslt.la ; then
    echo libxslt compiled
else
    echo Problem while compiling libxslt
    exit
fi
cd $Home

##################################################################
# Compile jansson

cd jansson
make clean
Parallel_Make
if test -e src/.libs/libjansson.la; then
    echo jansson compiled
else
    echo Problem while compiling jansson
    exit
fi
cd $Home

##################################################################
# Compile libevent

cd libevent
make clean
Parallel_Make
if test -e .libs/libevent.la; then
    echo libevent compiled
else
    echo Problem while compiling libevent
    exit
fi
cd $Home

##################################################################
# Compile sqlite

cd sqlite
make clean
Parallel_Make
if test -e .libs/libsqlite3.la; then
    echo sqlite compiled
else
    echo Problem while compiling sqlite
    exit
fi
cd $Home

##################################################################
# Configure MediaConch

if test -e MediaConch/Project/GNU/Server/configure; then
    cd MediaConch/Project/GNU/Server/
    test -e Makefile && rm Makefile
    chmod +x configure
    if [ "$OS" = "mac" ]; then
        ./configure $MacOptions --enable-staticlibs $*
    else
        ./configure --enable-staticlibs $*
    fi
    if test ! -e Makefile; then
        echo "Problem while configuring MediaConch (Server)"
        exit
    fi
else
    echo MediaConch directory is not found
    exit
fi
cd $Home

##################################################################
# Compile MediaInfoLib

cd MediaInfoLib/Project/GNU/Library/
make clean
Parallel_Make
if test -e libmediainfo.la; then
    echo MediaInfoLib compiled
else
    echo Problem while compiling MediaInfoLib
    exit
fi
cd $Home

##################################################################
# Compile MediaConch

cd MediaConch/Project/GNU/Server/
make clean
Parallel_Make
if test -e mediaconchd; then
    echo "MediaConch (Server) compiled"
else
    echo "Problem while compiling MediaConch (Server)"
    exit
fi
cd $Home

##################################################################

echo "MediaConch Server executable is MediaConch/Project/GNU/Server/mediaconchd"
echo "For installing, cd MediaConch/Project/GNU/Server && make install"

unset -v Home ZenLib_Options MacOptions OS
