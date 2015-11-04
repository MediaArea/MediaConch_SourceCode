# MediaConch_SourceCode README

MediaConch is an open source software project consisting of a toolset that aims to further develop the standardization and validation of preservation-level audiovisual files used within various memory institutions and communities. The software consists of an implementation checker, policy checker, reporter and fixer that will work together to offer its users an advanced level of ability to validate, assess and find solutions to repair the digital files within their collections. Accessible via either the command line, a graphical user interface (GUI), or web-based shell, the MediaConch project will serve to provide detailed individual and batch-level conformance checking analysis using its adaptable, flexible and interoperable software application interface. With a project focus dedicated to furthering the longevity of Matroska, Linear Pulse Code Modulation (LPCM) and FF Video Codec 1 (FFV1) as recommended digital preservation audiovisual formats, MediaConch will anticipate and contribute to the further development of the standardization of these formats. The MediaConch open source project was created and is currently under development by MediaArea, notable for the creation of MediaInfo, an open source media checker software application.

Website: <a href="https://mediaarea.net/MediaConch/">:shell: MediaConch project</a>.

# Funding and Licensing

This project has received funding from PREFORMA, co-funded by the European Commission under its FP7-ICT Programme. All software and source code developed by MediaArea during the PREFORMA project will be provided under the following two open source licenses: GNU General Public License 3.0 (GPLv3 or later), Mozilla Public License (MPLv2 or later).

All open source digital assets for the software developed by MediaArea during the PREFORMA project will be made available under the open access license: Creative Commons license attribution – Sharealike 4.0 International (CC BY-SA v4.0). All assets will exist in open file formats within an open platform (an open standard as defined in the European Interoperability Framework for Pan-European eGovernment Service (version 1.0 2004)).

# How to build MediaConch

## Build under Unix (Mac and Linux)

First, you must create a directory which will receive the MediaConch directory, and some of its dependencies depending on your configuration: ZenLib and MediaInfoLib if you decide to compile them yourself, and under Mac libxml2 and libxslt.

In this document, this directory will be refered as $BUILD_DIR.

### Dependancies under Mac

#### macport

Some dependancies are availables with macport. To install macport:
https://guide.macports.org/#installing

```sh
port install autoconf automake libtool pkgconfig zlib
```

#### libxml2

libxml2 and libxslt must be manually compiled.

Download ftp://xmlsoft.org/libxml2/libxml2-2.9.2.tar.gz, and then:

```sh
mv libxml2-2.9.2 $BUILD_DIR/libxml2
cd $BUILD_DIR/libxml2
./configure --disable-shared --enable-static --disable-ipv6 --without-ftp --without-http --without-c14n --without-catalog --with-xpath --without-xptr --without-xinclude --without-iconv --without-icu --without-iso8859x --without-zlib --without-lzma --without-mem_debug --without-run_debug --without-regexps --without-modules --with-tree --without-writer --with-pattern --with-push --without-valid --with-sax1 --without-legacy --with-output --without-schemas --with-schematron --without-python
make
```

#### libxslt

Download ftp://xmlsoft.org/libxml2/libxslt-1.1.28.tar.gz, and then:

```sh
mv libxslt-1.1.28 $BUILD_DIR/libxslt
cd $BUILD_DIR/libxslt
touch libtoolT
./configure --with-libxml-src=$BUILD_DIR/libxml2 --without-crypto
make
```

#### Qt

To install Qt, download and execute:
http://download.qt.io/archive/qt/5.3/5.3.2/qt-opensource-mac-x64-clang-5.3.2.dmg.mirrorlist

### Dependancies under Linux

#### Listing

*Build tools*

* git
* automake
* autoconf
* libtool
* pkgconfig
* make
* g++

*MediaArea tools*

* libzen0
* libmediainfo0

*CLI and GUI dependancies*

* zlib
* libxml2
* libxslt

*GUI only dependancies*

* qt4
* qtwebkit

*Optional dependancy (for online checker)*

* libcurl

#### Ubuntu

*Build tools and CLI/GUI dependancies*

```sh
apt-get install git automake autoconf libtool pkg-config make g++ zlib1g-dev libxml2-dev libxslt1-dev
```
 
*MediaArea tools*

Go to https://mediaarea.net/MediaConch/downloads/ubuntu.html and download the libmediainfo0, libmediainfo-dev, libzen0 and libzen-dev packages corresponding to your Ubuntu version (Ubuntu 15.10 is not supported yet). Then install them with :

```sh
dpkg -i libmediainfo* libzen*
```

*GUI only dependancies*

```sh
apt-get install libqt4-dev libqtwebkit-dev
```

*Optional dependancy (for online checker)*

```sh
apt-get install libcurl4-gnutls-dev
```

#### Fedora

*Build tools and CLI/GUI dependancies*

```sh
yum install git automake autoconf libtool pkgconfig make gcc-c++ zlib-devel libxml2-devel libxslt-devel
```

*MediaArea tools*

Go to https://mediaarea.net/MediaConch/downloads/fedora.html and download the libmediainfo0, libmediainfo-devel, libzen0 and libzen-devel packages corresponding to your Fedora version. Then install them with :

```sh
rpm -i libmediainfo* libzen*
```

*GUI only dependancies*

```sh
yum install qt-devel qtwebkit-devel desktop-file-utils
```

*Optional dependancy (for online checker)*

```sh
yum install libcurl-devel libopenssh2-devel libidn-devel openldap-devel
```

#### Debian

*Build tools and CLI/GUI dependancies*

```sh
apt-get install git automake autoconf libtool pkg-config make g++ zlib1g-dev libxml2-dev libxslt1-dev
```

*MediaArea tools*

Go to https://mediaarea.net/MediaConch/downloads/debian.html and download the libmediainfo0, libmediainfo-dev, libzen0 and libzen-dev packages corresponding to your Debian version. Then install them with :

```sh
dpkg -i libmediainfo* libzen*
```

*GUI only dependancies*

```sh
apt-get install libqt4-dev libqtwebkit-dev
```

*Optional dependancy (for online checker)*

```sh
apt-get install libcurl4-gnutls-dev
```

#### OpenSUSE

*Build tools and CLI/GUI dependancies*

```sh
zypper install git automake autoconf libtool pkgconfig make gcc-c++ zlib-devel libxml2-devel libxslt-devel
```

*MediaArea tools*

Go to https://mediaarea.net/MediaConch/downloads/opensuse.html and download the libmediainfo0, libmediainfo-devel, libzen0 and libzen-devel packages corresponding to your OpenSuse version. Then install them with :

```sh
rpm -i libmediainfo* libzen*
```

*GUI only dependancies*

```sh
zypper install libqt4-devel libQtWebKit-devel update-desktop-files
```

*Optional dependancy (for online checker)*

```sh
zypper install libcurl-devel
```

### Build MediaConch CLI

When you have done all the prerequisite for you configuration, you can build MediaConch. We start with the CLI.

```sh
cd $BUILD_DIR
git clone https://github.com/MediaArea/MediaConch_SourceCode.git
cd MediaConch_SourceCode/Project/GNU/CLI
./autogen.sh
```

Then, under Mac:

```sh
./configure --enable-staticlibs
make
```

Under Linux:

```sh
./configure --enable-shared
make
```

#### Launch the CLI

```sh
./mediaconch
```

### Build MediaConch GUI

If you have already build the CLI, no need to run git twice. In fact, if you re-run git with an existing MediaConch_SourceCode directory, git will complain and exit.

To compile MediaConch GUI under Mac and Linux:

```sh
cd $BUILD_DIR
git clone https://github.com/MediaArea/MediaConch_SourceCode.git
cd MediaConch_SourceCode/Project/Qt
```

##### Without online checker

Run:

```sh
./prepare
make
```

##### With online checker

You must install the online checker dependancies. Then run:

```sh
./prepare "DEFINES+=MEDIAINFO_LIBCURL_YES"
make
```

#### Launch the GUI

Under Mac:

```sh
./MediaConch.app/Contents/MacOS/MediaConch
```

Under Linux:

```sh
./mediaconch-gui
```

### Optional : build ZenLib and MediaInfoLib

#### ZenLib

To compile ZenLib under Mac and Linux:

```sh
cd $BUILD_DIR
git clone https://github.com/MediaArea/ZenLib.git
cd ZenLib/Project/GNU/Library
./autogen.sh
./configure
make
```

#### MediaInfoLib

To compile MediaInfoLib under Mac and Linux:

```sh
cd $BUILD_DIR
git clone https://github.com/MediaArea/MediaInfoLib.git
cd MediaInfoLib/Project/GNU/Library
./autogen.sh
```

##### Without online checker

Run:

```sh
./configure --with-libcurl=runtime
make
```

##### With online checker

Under Mac:

```sh
./configure --with-libcurl=runtime
make
```

Under Linux:

You must install the online checker dependancies. Then run:

```sh
./configure --with-libcurl
make
```

## Build under Windows

You need to install Microsoft Visual Studio 2013.
