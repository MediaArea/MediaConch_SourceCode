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

#### libxml2

#### libxslt

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

apt-get install git automake autoconf libtool pkg-config make g++ zlib1g-dev libxml2-dev libxslt1-dev

*GUI only dependancies*

apt-get install libqt4-dev libqtwebkit-dev

*Optional dependancy (for online checker)*

apt-get install libcurl4-gnutls-dev

#### Fedora

*Build tools and CLI/GUI dependancies*

yum install git automake autoconf libtool pkgconfig make gcc-c++ zlib-devel libxml2-devel libxslt-devel

*GUI only dependancies*

yum install qt-devel qtwebkit-devel desktop-file-utils

*Optional dependancy (for online checker)*

yum install libcurl-devel

#### Debian

*Build tools and CLI/GUI dependancies*

apt-get install git automake autoconf libtool pkg-config make g++ zlib1g-dev libxml2-dev libxslt1-dev

*GUI only dependancies*

apt-get install libqt4-dev libqtwebkit-dev

*Optional dependancy (for online checker)*

apt-get install libcurl4-gnutls-dev

#### OpenSUSE

*Build tools and CLI/GUI dependancies*

??? yast2 install ??? git automake autoconf libtool pkgconfig make gcc-c++ zlib-devel libxml2-devel libxslt-devel

*GUI only dependancies*

??? yast2 install ??? libQtWebKit-devel update-desktop-files
À tester : libqt4-devel

*Optional dependancy (for online checker)*

??? yast2 install ??? libcurl-devel

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

Then, under Linux:

```sh
./configure --enable-shared
make
```

#### Launch the CLI

```sh
./mediaconch
```

### Build MediaConch GUI

If you have already build the GUI, no need to run git twice. If the directory MediaConch_SourceCode already exist, git will complain.

To compile MediaConch GUI under Mac and Linux:

```sh
cd $BUILD_DIR
git clone https://github.com/MediaArea/MediaConch_SourceCode.git
cd MediaConch_SourceCode/Project/Qt
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

Then, under Mac:

```sh
./configure --with-libcurl=runtime
make
```

Then, under Linux:

```sh
./configure --with-libcurl
make
```

## Build under Windows

You need to install Microsoft Visual Studio 2013.
