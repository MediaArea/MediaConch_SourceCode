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

#### .deb distributions (Debian, Ubuntu, …)

*Build dependancies*

* git
* automake
* autoconf
* libtool
* pkg-config
* make
* g++

*CLI and GUI dependancies*

* zlib1g-dev
* libxml2-dev
* libxslt1-dev

*GUI only dependancies*

* libqt4-dev
* libqtwebkit-dev

*Optional dependancy (for online checker)*

* libcurl4-gnutls-dev

#### .rpm based distributions (RHEL, CentOS, SLE, openSUSE, …)

*Build dependancies*

* git
* automake
* autoconf
* libtool
* pkgconfig
* make
* gcc-c++

*CLI and GUI dependancies*

* zlib-devel
* libxml2-devel
* libxslt-devel

*GUI only dependancies*

yum
apt

A tester * libqt4-devel
* qt-devel (fedora)
* qtwebkit-devel (fedora)
* desktop-file-utils (fedora)
* libQtWebKit-devel (suse)
* update-desktop-files (suse)

*Optional dependancy (for online checker)*

* libcurl-devel

### MediaArea librairies: ZenLib and MediaInfoLib

#### ZenLib

##### Under Mac

To compile ZenLib under Mac:

```
cd $BUILD_DIR
git clone https://github.com/MediaArea/ZenLib.git
cd ZenLib/Project/GNU/Library
./autogen.sh
./configure --with-macosx-version-min=10.5
make
```

##### Under Linux

You can use the libzen devel package from your distribution, or compile it yourself.

To compile ZenLib under Linux:

```
cd ../path/to/builddir
git clone https://github.com/MediaArea/ZenLib.git
cd ZenLib/Project/GNU/Library
chmod +x autogen.sh configure
./autogen.sh
./configure
make clean
make
```

#### MediaInfoLib

##### Under Mac

To compile MediaInfoLib under Mac:

```
cd ../path/to/builddir
git clone https://github.com/MediaArea/MediaInfoLib.git
cd MediaInfoLib/Project/GNU/Library
chmod +x autogen.sh configure
./autogen.sh
./configure --with-libcurl=runtime
make clean
make
```

Si vous
./configure --with-libcurl



##### Under Linux

You can use the libmediainfo devel package from your distribution, or compile it yourself. However, it is recommended to compile it yourself if you want the online checker.

To compile MediaInfoLib under Linux:

```
cd ../path/to/builddir
git clone https://github.com/MediaArea/MediaInfoLib.git
cd MediaInfoLib/Project/GNU/Library
chmod +x autogen.sh configure
./autogen.sh
make clean
make
```

### Build MediaConch CLI

When you have done all the prerequisite for you configuration, you can build MediaConch. We start with the CLI.

##### Under Mac

To compile MediaConch CLI under Mac:

```
cd ../path/to/builddir
git clone https://github.com/MediaArea/MediaConch_SourceCode.git
cd MediaConch_SourceCode/Project/GNU/CLI
chmod +x autogen.sh configure
./autogen.sh
./configure --with-macosx-version-min=10.5 --enable-staticlibs
make clean
make
```

##### Under Linux

To compile MediaConch CLI under Linux:

```
cd ../path/to/builddir
rm -fr MediaConch_SourceCode
git clone https://github.com/MediaArea/MediaConch_SourceCode.git
cd MediaConch_SourceCode/Project/GNU/CLI
chmod +x autogen.sh configure
./autogen.sh
./configure --enable-shared
make clean
make
```

#### Launch the CLI

The binary is mediaconch.

```
cd ../path/to/builddir/MediaConch_SourceCode/Project/GNU/CLI
./mediaconch
```

### Build MediaConch GUI

To compile MediaConch GUI under Mac and Linux:

```
cd ../path/to/builddir
rm -fr MediaConch_SourceCode
git clone https://github.com/MediaArea/MediaConch_SourceCode.git
cd MediaConch_SourceCode/Project/Qt
chmod +x prepare
./prepare "DEFINES+=MEDIAINFO_LIBCURL_YES"
make clean
make
```

#### Launch the GUI

Under Mac, the binary is MediaConch.app/Contents/MacOS/MediaConch.

```
cd ../path/to/builddir/MediaConch_SourceCode/Project/Qt
./MediaConch.app/Contents/MacOS/MediaConch
```

Under Linux, the binary is mediaconch-gui.

```
cd ../path/to/builddir/MediaConch_SourceCode/Project/Qt
./mediaconch-gui
```

## Build under Windows
