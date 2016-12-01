dnl #########################################################################
dnl ### Init
dnl #########################################################################

dnl -------------------------------------------------------------------------
dnl Name and version
dnl
AC_INIT([mediaconch], [16.11.0])

dnl -------------------------------------------------------------------------
dnl Test if we are at the good place
dnl
AC_CONFIG_SRCDIR([../../../Source/CLI/CLI_Main.cpp])

dnl -------------------------------------------------------------------------
dnl sets build, host, target variables and the same with _alias
dnl
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([foreign -Wall])

dnl -------------------------------------------------------------------------
dnl Autotools init
dnl
CFLAGS="$CFLAGS"
CXXFLAGS="$CXXFLAGS"
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_PROG_INSTALL

dnl #########################################################################
dnl ### Options
dnl #########################################################################

dnl -------------------------------------------------------------------------
dnl Arguments - Enable
dnl
AC_ARG_ENABLE(debug,            AC_HELP_STRING([--enable-debug],            [Enable full debug]),                         , enable_debug=no)
AC_ARG_ENABLE(gprof,            AC_HELP_STRING([--enable-gprof],            [Enable gprof log]),                          , enable_gprof=no)
AC_ARG_ENABLE(universal_binary, AC_HELP_STRING([--enable-universal_binary], [Create Mac PPC/i386 Universal binary]),      , enable_universal_binary=no)
AC_ARG_ENABLE(arch-ppc,         AC_HELP_STRING([--enable-arch-ppc],         [Create Mac PPC architecture]),               , enable_arch_ppc=no)
AC_ARG_ENABLE(arch-ppc64,       AC_HELP_STRING([--enable-arch-ppc64],       [Create Mac PPC64 architecture]),             , enable_arch_ppc64=no)
AC_ARG_ENABLE(arch-i386,        AC_HELP_STRING([--enable-arch-i386],        [Create Mac i386 architecture]),              , enable_arch_i386=no)
AC_ARG_ENABLE(arch-x86_64,      AC_HELP_STRING([--enable-arch-x86_64],      [Create Mac x86_64 architecture]),            , enable_arch_x86_64=no)
AC_ARG_ENABLE(staticlibs,       AC_HELP_STRING([--enable-staticlibs],       [Use static libmediainfo and libzen]),        , enable_staticlibs=no)
AC_ARG_ENABLE([stream_missing],    [AS_HELP_STRING(([--enable-stream_missing],    [Disable stream support (old compilers)]))],    , [enable_stream_missing=no])

dnl -------------------------------------------------------------------------
dnl Arguments - With
dnl
AC_ARG_WITH(macosx-sdk,         AC_HELP_STRING([--with-macosx-sdk],         [Force the Mac SDK]),                         , with_macosx_sdk=no)
AC_ARG_WITH(macosx-version-min, AC_HELP_STRING([--with-macosx-version-min], [Force the Mac Version]),                     , with_macosx_version_min=no)
AC_ARG_WITH(dll,            AC_HELP_STRING([--with-dll],            [Do not link to libmediainfo, dynamic loading]), , with_dll=no)
AC_ARG_WITH(libmediainfo-name,  AC_HELP_STRING([--with-libmediainfo-name],  [Force libmediainfo filename]),                  		  , with_libmediainfo_name=mediainfo)
AC_ARG_WITH(sqlite,             AC_HELP_STRING([--with-sqlite],             [Enable SQLite DB]),                          , with_sqlite=yes)
AC_ARG_WITH(jansson,            AC_HELP_STRING([--with-jansson],            [Enable Jansson Library]),                    , with_jansson=yes)
AC_ARG_WITH(libevent,           AC_HELP_STRING([--with-libevent],           [Enable Libevent]),                           , with_libevent=yes)

dnl -------------------------------------------------------------------------
dnl External options
dnl

dnl #########################################################################
dnl ### Options from elsewhere
dnl #########################################################################

dnl -------------------------------------------------------------------------
dnl Unicode
dnl
if test -e ../../../../ZenLib/Project/GNU/Library/libzen-config; then
	enable_unicode="$(../../../../ZenLib/Project/GNU/Library/libzen-config Unicode)"
elif test "$(libzen-config Exists)" = "yes" ; then
	enable_unicode="$(libzen-config Unicode)"
else
	if pkg-config --exists libzen; then
		enable_unicode="$(pkg-config --variable=Unicode libzen)"
	else
		AC_MSG_ERROR([libzen configuration is not found])
	fi
fi

dnl #########################################################################
dnl ### Parsing options
dnl #########################################################################


dnl #########################################################################
dnl ### MediaConch flags
dnl #########################################################################


dnl #########################################################################
dnl ### C/C++ compiler options used to compile
dnl #########################################################################

dnl -------------------------------------------------------------------------
dnl Unicode
dnl
if test "$enable_unicode" = "yes"; then
	AC_DEFINE(UNICODE)
	MediaConch_CXXFLAGS="$MediaConch_CXXFLAGS -DUNICODE"
	MediaConch_Unicode="yes"
else
	MediaConch_Unicode="no"
fi

dnl -------------------------------------------------------------------------
dnl Use DLL
dnl
if test "$with_dll" = "yes"; then
	AC_DEFINE(MEDIAINFO_DLL_RUNTIME)
	LIBS="$LIBS -ldl"
fi

dnl -------------------------------------------------------------------------
dnl Check if stream is available
dnl
AC_LANG([C++])
if test "$enable_unicode" = "yes"; then
    AC_COMPILE_IFELSE([AC_LANG_SOURCE([
        #include <sstream>
        int main () {std::wstringstream message; message<<1.0; return 0;}])], , [enable_stream_missing="yes"])
else
    AC_COMPILE_IFELSE([AC_LANG_SOURCE([
        #include <sstream>
        int main () {std::stringstream message; message<<1.0; return 0;}])], , [enable_stream_missing="yes"])
fi

if test "$enable_stream_missing" = "yes"; then
	CXXFLAGS="$CXXFLAGS -DSTREAM_MISSING"
fi

dnl -------------------------------------------------------------------------
dnl GNU compiler
dnl
if test "$GXX" = yes ; then
	if test "$enable_debug" = "yes"; then
		CFLAGS="$CFLAGS -O0 -g -Wall -Wundef"
		CXXFLAGS="$CXXFLAGS -O0 -g -Wall -W -Wundef -Wcast-qual -Wno-ctor-dtor-privacy"
	else
		CFLAGS="$CFLAGS -O2"
		CXXFLAGS="$CXXFLAGS -O2"
	fi
	if test "$enable_gprof" = "yes"; then
		CFLAGS="$CFLAGS -pg -g"
		CXXFLAGS="$CXXFLAGS -pg -g"
	fi
fi

dnl #########################################################################
dnl ### Needed libs
dnl #########################################################################

dnl -------------------------------------------------------------------------
dnl libmediainfo-name
dnl
MediaInfoLib_LibName="$with_libmediainfo_name"

dnl -------------------------------------------------------------------------
dnl MediaInfoLib
dnl
if test "$with_dll" != "yes"; then
	if test -e ../../../../MediaInfoLib/Project/GNU/Library/libmediainfo-config; then
		CXXFLAGS="$CXXFLAGS $(../../../../MediaInfoLib/Project/GNU/Library/libmediainfo-config CXXFLAGS)"
		if test "$enable_staticlibs" = "yes"; then
			with_mediainfolib="builtin (static)"
			LIBS="$LIBS $(../../../../MediaInfoLib/Project/GNU/Library/libmediainfo-config LIBS_Static)"
		else
			with_mediainfolib="builtin"
			LIBS="$LIBS $(../../../../MediaInfoLib/Project/GNU/Library/libmediainfo-config LIBS)"
		fi
	elif test "$(libmediainfo-config Exists)" = "yes" ; then
		CXXFLAGS="$CXXFLAGS $(libmediainfo-config CXXFLAGS)"
		if test "$enable_staticlibs" = "yes"; then
			with_mediainfolib="system (static)"
			LIBS="$LIBS $(libmediainfo-config LIBS_Static)"
		else
			with_mediainfolib="system"
			LIBS="$LIBS $(libmediainfo-config LIBS)"
		fi
	else
		if pkg-config --exists lib$with_libmediainfo_name; then
			CXXFLAGS="$CXXFLAGS $(pkg-config --cflags libmediainfo)"
			if test "$enable_staticlibs" = "yes"; then
				with_mediainfolib="system (static)"
				LIBS="$LIBS $(pkg-config --variable=Libs_Static lib$with_libmediainfo_name)"
			else
				with_mediainfolib="system"
				LIBS="$LIBS $(pkg-config --libs lib$with_libmediainfo_name)"
			fi
		else
			AC_MSG_ERROR([libmediainfo configuration is not found])
		fi
	fi
else
	with_mediainfolib="load at runtime"
fi

dnl -------------------------------------------------------------------------
dnl ZenLib
dnl
if test -e ../../../../ZenLib/Project/GNU/Library/libzen-config; then
	CXXFLAGS="$CXXFLAGS $(../../../../ZenLib/Project/GNU/Library/libzen-config CXXFLAGS)"
	MediaInfoLib_CXXFLAGS="$MediaInfoLib_CXXFLAGS $(../../../../ZenLib/Project/GNU/Library/libzen-config CXXFLAGS)"
	if test "$enable_staticlibs" = "yes"; then
		with_zenlib="builtin (static)"
		LIBS="$LIBS $(../../../../ZenLib/Project/GNU/Library/libzen-config LIBS_Static)"
	else
		with_zenlib="builtin"
		LIBS="$LIBS $(../../../../ZenLib/Project/GNU/Library/libzen-config LIBS)"
	fi
elif test "$(libzen-config Exists)" = "yes" ; then
	CXXFLAGS="$CXXFLAGS $(libzen-config CXXFLAGS)"
	MediaInfoLib_CXXFLAGS="$MediaInfoLib_CXXFLAGS $(libzen-config CXXFLAGS)"
	if test "$enable_staticlibs" = "yes"; then
		with_zenlib="system (static)"
		LIBS="$LIBS $(libzen-config LIBS_Static)"
	else
		with_zenlib="system"
		LIBS="$LIBS $(libzen-config LIBS)"
	fi
else
	if pkg-config --exists libzen; then
		CXXFLAGS="$CXXFLAGS $(pkg-config --cflags libzen)"
		MediaInfoLib_CXXFLAGS="$MediaInfoLib_CXXFLAGS $(pkg-config --cflags libzen)"
		if test "$enable_staticlibs" = "yes"; then
			with_zenlib="system (static)"
			LIBS="$LIBS $(pkg-config --variable=Libs_Static libzen)"
		else
			with_zenlib="system"
			LIBS="$LIBS $(pkg-config --libs libzen)"
		fi
	else
		AC_MSG_ERROR([libzen configuration is not found])
	fi
fi

dnl -------------------------------------------------------------------------
dnl libxml2
dnl
if test -e ../../../../libxml2/libxml2.la; then
	CXXFLAGS="$CXXFLAGS  -I../../../../libxml2/include"
	MediaInfoLib_CXXFLAGS="$MediaInfoLib_CXXFLAGS -I../../../../libxml2/include"
	if test "$enable_staticlibs" = "yes"; then
		with_libxml2="builtin (static)"
		LIBS="$LIBS  ../../../../libxml2/.libs/libxml2.a"
	else
		with_libxml2="builtin"
		LIBS="$LIBS  -L../../../../libxml2/.libs -lxml2"
	fi
else
	if pkg-config --exists libxml2; then
		CXXFLAGS="$CXXFLAGS $(pkg-config --cflags libxml2)"
		MediaInfoLib_CXXFLAGS="$MediaInfoLib_CXXFLAGS $(pkg-config --cflags libxml2)"
		if test "$enable_staticlibs" = "yes"; then
			with_libxml2="system (static)"
			LIBS="$LIBS $(pkg-config --static --libs libxml2)"
		else
			with_libxml2="system"
			LIBS="$LIBS $(pkg-config --libs libxml2)"
		fi
	elif pkg-config --exists libxml-2.0; then
		CXXFLAGS="$CXXFLAGS $(pkg-config --cflags libxml-2.0)"
		MediaInfoLib_CXXFLAGS="$MediaInfoLib_CXXFLAGS $(pkg-config --cflags libxml-2.0)"
		if test "$enable_staticlibs" = "yes"; then
			with_libxml2="system (static)"
			LIBS="$LIBS $(pkg-config --static --libs libxml-2.0)"
		else
			with_libxml2="system"
			LIBS="$LIBS $(pkg-config --libs libxml-2.0)"
		fi
	else
		AC_MSG_ERROR([libxml2 configuration is not found])
	fi
fi

dnl -------------------------------------------------------------------------
dnl libxslt
dnl
if test -e ../../../../libxslt/libxslt/.libs/libxslt.la; then
	CXXFLAGS="$CXXFLAGS  -I../../../../libxslt/libxslt"
	MediaInfoLib_CXXFLAGS="$MediaInfoLib_CXXFLAGS -I../../../../libxslt/libxslt"
	MediaInfoLib_CXXFLAGS="$MediaInfoLib_CXXFLAGS -I../../../../libxslt/libexslt"
	if test "$enable_staticlibs" = "yes"; then
		with_libxslt="builtin (static)"
		LIBS="$LIBS  ../../../../libxslt/libxslt/.libs/libxslt.a"
		LIBS="$LIBS  ../../../../libxslt/libexslt/.libs/libexslt.a"
	else
		with_libxslt="builtin"
		LIBS="$LIBS  -L../../../../libxslt/libxslt/.libs -lxslt"
		LIBS="$LIBS  -L../../../../libxslt/libexslt/.libs -lexslt"
	fi
else
	if pkg-config --exists libxslt; then
		CXXFLAGS="$CXXFLAGS $(pkg-config --cflags libxslt)"
		CXXFLAGS="$CXXFLAGS $(pkg-config --cflags libexslt)"
		MediaInfoLib_CXXFLAGS="$MediaInfoLib_CXXFLAGS $(pkg-config --cflags libxslt)"
		MediaInfoLib_CXXFLAGS="$MediaInfoLib_CXXFLAGS $(pkg-config --cflags libexslt)"
		if test "$enable_staticlibs" = "yes"; then
			with_libxslt="system (static)"
			LIBS="$LIBS $(pkg-config --static --libs libxslt)"
			LIBS="$LIBS $(pkg-config --static --libs libexslt)"
		else
			with_libxslt="system"
			LIBS="$LIBS $(pkg-config --libs libxslt)"
			LIBS="$LIBS $(pkg-config --libs libexslt)"
		fi
	else
		AC_MSG_ERROR([libxslt configuration is not found])
	fi
fi

dnl -------------------------------------------------------------------------
dnl libsqlite3
dnl
if test "$with_sqlite" = "yes"; then
    if test -e ../../../../sqlite/.libs/libsqlite3.la; then
        CXXFLAGS="$CXXFLAGS -DHAVE_SQLITE  -I../../../../sqlite"
        if test "$enable_staticlibs" = "yes"; then
            with_libsqlite3="builtin (static)"
            LIBS="$LIBS  ../../../../sqlite/.libs/libsqlite3.a"
        else
            with_libsqlite3="builtin"
            LIBS="$LIBS  -L../../../../sqlite/.libs -lsqlite3"
        fi
    elif pkg-config --exists sqlite3; then
        CXXFLAGS="$CXXFLAGS -DHAVE_SQLITE $(pkg-config --cflags sqlite3)"
        if test "$enable_staticlibs" = "yes"; then
            with_libsqlite3="system (static)"
            LIBS="$LIBS $(pkg-config --static --libs sqlite3)"
        else
            with_libsqlite3="system"
            LIBS="$LIBS $(pkg-config --libs sqlite3)"
        fi
    else
        AC_MSG_ERROR([libsqlite3 configuration is not found])
    fi
else
    with_libsqlite3="No"
fi

dnl -------------------------------------------------------------------------
dnl libjansson
dnl
if test "$with_jansson" = "yes"; then
    if test -e ../../../../jansson/src/.libs/libjansson.a; then
        CXXFLAGS="$CXXFLAGS -DHAVE_JANSSON -I../../../../jansson/src"
        if test "$enable_staticlibs" = "yes"; then
            with_libjansson="builtin (static)"
            LIBS="$LIBS ../../../../jansson/src/.libs/libjansson.a"
        else
            with_libjansson="builtin"
            LIBS="$LIBS  -L../../../../jansson/src/.libs -ljansson"
        fi
    elif pkg-config --exists jansson; then
        CXXFLAGS="$CXXFLAGS -DHAVE_JANSSON $(pkg-config --cflags jansson)"
        if test "$enable_staticlibs" = "yes"; then
            with_libjansson="system (static)"
            LIBS="$LIBS $(pkg-config --static --libs jansson)"
        else
            with_libjansson="system"
            LIBS="$LIBS $(pkg-config --libs jansson)"
        fi
    else
        AC_MSG_ERROR([libjansson configuration is not found])
    fi
else
    with_libjansson="No"
fi

dnl -------------------------------------------------------------------------
dnl libevent2
dnl
if test "$with_libevent" = "yes"; then
    if test -e ../../../../libevent/.libs/libevent.a; then
        CXXFLAGS="$CXXFLAGS -DHAVE_LIBEVENT -I../../../../libevent/include"
        if test "$enable_staticlibs" = "yes"; then
            with_libevent="builtin (static)"
            LIBS="$LIBS ../../../../libevent/.libs/libevent.a"
        else
            with_libevent="builtin"
            LIBS="$LIBS  -L../../../../libevent/.libs -levent"
        fi
    elif pkg-config --exists libevent; then
        CXXFLAGS="$CXXFLAGS -DHAVE_LIBEVENT $(pkg-config --cflags libevent)"
        if test "$enable_staticlibs" = "yes"; then
            with_libevent="system (static)"
            LIBS="$LIBS $(pkg-config --static --libs libevent)"
        else
            with_libevent="system"
            LIBS="$LIBS $(pkg-config --libs libevent)"
        fi
    else
        AC_MSG_ERROR([libevent configuration is not found])
    fi
else
    with_libevent="No"
fi

dnl #########################################################################
dnl ### Compiler specific
dnl #########################################################################

dnl -------------------------------------------------------------------------
case "${build}" in
  powerpc-montavista* )
    CFLAGS="$CFLAGS -D__MONTAVISTA__"
    CXXFLAGS="$CXXFLAGS -D__MONTAVISTA__"
  ;;
esac

dnl -------------------------------------------------------------------------
dnl Mac Architecture and SDK
dnl
case "$host_os" in
    darwin* )
        LDFLAGS="$LDFLAGS -framework CoreFoundation"
    ;;
esac

if test "$enable_universal_binary" = "yes"; then
	enable_arch_ppc=yes
	enable_arch_i386=yes
	if test "$with_macosx_sdk" = no ; then
		with_macosx_sdk="/Developer/SDKs/MacOSX10.4u.sdk"
	fi
	if test "$with_macosx_version_min" = no ; then
		with_macosx_version_min="10.4"
	fi
fi
if test "$enable_arch_ppc" = "yes"; then
	CXXFLAGS="$CXXFLAGS -arch ppc"
	CFLAGS="$CFLAGS -arch ppc"
	LDFLAGS="$LDFLAGS -arch ppc"
fi
if test "$enable_arch_ppc64" = "yes"; then
	CXXFLAGS="$CXXFLAGS -arch ppc64"
	CFLAGS="$CFLAGS -arch ppc64"
	LDFLAGS="$LDFLAGS -arch ppc64"
fi
if test "$enable_arch_i386" = "yes"; then
	CXXFLAGS="$CXXFLAGS -arch i386"
	CFLAGS="$CFLAGS -arch i386"
	LDFLAGS="$LDFLAGS -arch i386"
fi
if test "$enable_arch_x86_64" = "yes"; then
	CXXFLAGS="$CXXFLAGS -arch x86_64"
	CFLAGS="$CFLAGS -arch x86_64"
	LDFLAGS="$LDFLAGS -arch x86_64"
fi
if test "$with_macosx_sdk" = "yes" ; then
	CXXFLAGS="-isysroot /Developer/SDKs/MacOSX10.4u.sdk $CXXFLAGS"
	CFLAGS="-isysroot /Developer/SDKs/MacOSX10.4u.sdk $CFLAGS"
	LDFLAGS="-isysroot /Developer/SDKs/MacOSX10.4u.sdk $LDFLAGS"
elif test "x$with_macosx_sdk" != xno ; then
	CXXFLAGS="-isysroot $with_macosx_sdk $CXXFLAGS"
	CFLAGS="-isysroot $with_macosx_sdk $CFLAGS"
	LDFLAGS="-isysroot $with_macosx_sdk $LDFLAGS"
fi
if test "x$with_macosx_version_min" != xno ; then
	CXXFLAGS="-mmacosx-version-min=$with_macosx_version_min $CXXFLAGS"
	CFLAGS="-mmacosx-version-min=$with_macosx_version_min $CFLAGS"
	LDFLAGS="-mmacosx-version-min=$with_macosx_version_min $LDFLAGS"
fi

dnl #########################################################################
dnl ### Output
dnl #########################################################################

AC_OUTPUT(Makefile)

dnl #########################################################################
dnl ### Report how we have been configured
dnl #########################################################################

echo ""
echo "Configured MediaConch for '$target'"
echo ""
echo "  Unicode?                                                $enable_unicode"
echo "  stream missing support?                                 $enable_stream_missing"
echo ""
echo "  Using libevent?                                         $with_libevent"
echo "  Using libjansson?                                       $with_libjansson"
echo "  Using libsqlite3?                                       $with_libsqlite3"
echo "  Using libxml2?                                          $with_libxml2"
echo "  Using libxslt?                                          $with_libxslt"
echo "  Using libzen?                                           $with_zenlib"
echo "  Using libmediainfo?                                     $with_mediainfolib"
echo ""
echo "  CXXFLAGS: $CXXFLAGS"
echo "  LIBS: $LIBS"
