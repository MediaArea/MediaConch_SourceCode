%define mediaconch_version          16.03
%define libmediainfo_version        0.7.84
%define libzen_version              0.4.33

Name:           mediaconch
Version:        %{mediaconch_version}
Release:        1
Summary:        Most relevant technical and tag data for video and audio files (CLI)

Group:          Applications/Multimedia
License:        MPL-2.0+/GPL-3.0+
URL:            http://MediaArea.net/MediaConch
Packager:       MediaArea.net SARL <info@mediaarea.net>
Source0:        %{name}_%{version}.tar.gz

BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root
BuildRequires:  gcc-c++
BuildRequires:  pkgconfig
BuildRequires:  libmediainfo-devel >= %{libmediainfo_version}
BuildRequires:  libzen-devel >= %{libzen_version}
BuildRequires:  pkgconfig
BuildRequires:  zlib-devel
BuildRequires:  libtool
BuildRequires:  automake
BuildRequires:  autoconf
BuildRequires:  zlib-devel
BuildRequires:  libqt4-devel
BuildRequires:  libxml2-devel
BuildRequires:  libxslt-devel
BuildRequires:  libcurl-devel
BuildRequires:  sqlite-devel
BuildRequires:  libevent-devel

%if 0%{?fedora_version}
BuildRequires:  qt-devel
BuildRequires:  qtwebkit-devel
BuildRequires:  desktop-file-utils
%endif

%if 0%{?suse_version}
BuildRequires:  libQtWebKit-devel
BuildRequires:  update-desktop-files
%endif

%if 0%{?suse_version}
%if 0%{?is_opensuse}
BuildRequires:  libjansson-devel
%endif
%else
BuildRequires:  jansson-devel
%endif

%description
MediaConch is an implementation checker, policy checker, reporter,
and fixer that targets preservation-level audiovisual files
(specifically Matroska, Linear Pulse Code Modulation (LPCM)
and FF Video Codec 1 (FFV1)).

This project is maintained by MediaArea and funded by PREFORMA.

This package includes the command line interface.

%package server
Summary:    Supplies technical and tag information about a video or audio file (Server)
Group:      Applications/Multimedia
Requires:   libzen0 >= %{libzen_version}
Requires:   libmediainfo0 >= %{libmediainfo_version}

%package gui
Summary:    Supplies technical and tag information about a video or audio file (GUI)
Group:      Applications/Multimedia
Requires:   libzen0 >= %{libzen_version}
Requires:   libmediainfo0 >= %{libmediainfo_version}

%description gui
MediaConch is an implementation checker, policy checker, reporter,
and fixer that targets preservation-level audiovisual files
(specifically Matroska, Linear Pulse Code Modulation (LPCM)
and FF Video Codec 1 (FFV1)).

This project is maintained by MediaArea and funded by PREFORMA.

This package includes the graphical user interface.

%description server
MediaConch is an implementation checker, policy checker, reporter,
and fixer that targets preservation-level audiovisual files
(specifically Matroska, Linear Pulse Code Modulation (LPCM)
and FF Video Codec 1 (FFV1)).

This project is maintained by MediaArea and funded by PREFORMA.

This package includes the server.

%prep
%setup -q -n MediaConch
sed -i 's/.$//' *.txt *.html Release/*.txt

find Source -type f -exec chmod 644 {} ';'
chmod 644 *.html *.txt Release/*.txt

pushd Project/GNU/CLI
    autoreconf -i
popd

pushd Project/GNU/Server
    autoreconf -i
popd

pushd Project/Qt
    chmod u+x prepare
    %if 0%{?suse_version} && ! 0%{?is_opensuse}
        %if 0%{?suse_version} < 1200
            ./prepare "DEFINES+=MEDIAINFO_LIBCURL_YES" NO_JANSSON=yes NO_LIBEVENT=yes
        %else
            ./prepare "DEFINES+=MEDIAINFO_LIBCURL_YES" NO_JANSSON=yes
        %endif
    %else
        %if 0%{?suse_version} && 0%{?suse_version} < 1200
            ./prepare "DEFINES+=MEDIAINFO_LIBCURL_YES" NO_LIBEVENT=yes
        %else
            ./prepare "DEFINES+=MEDIAINFO_LIBCURL_YES"
        %endif
    %endif
popd

%build
export CFLAGS="%{optflags}"
export CXXFLAGS="%{optflags}"

# build CLI
pushd Project/GNU/CLI
    %if 0%{?suse_version} && ! 0%{?is_opensuse}
        %if 0%{?suse_version} < 1200
            %configure --without-jansson --without-libevent
        %else
            %configure --without-jansson
        %endif
    %else
        %if 0%{?suse_version} && 0%{?suse_version} < 1200
            %configure --without-libevent
        %else
            %configure
        %endif
    %endif
    make %{?_smp_mflags}
popd

# build server
pushd Project/GNU/Server
    %if 0%{?suse_version} && ! 0%{?is_opensuse}
        %if 0%{?suse_version} < 1200
            %configure --without-jansson --without-libevent
        %else
            %configure --without-jansson
        %endif
    %else
        %if 0%{?suse_version} && 0%{?suse_version} < 1200
            %configure --without-libevent
        %else
            %configure
        %endif
    %endif
    make %{?_smp_mflags}
popd

# now build GUI
pushd Project/Qt
    make %{?_smp_mflags}
popd


%install
pushd Project/GNU/CLI
    make install-strip DESTDIR=%{buildroot}
popd

pushd Project/GNU/Server
    make install-strip DESTDIR=%{buildroot}
popd

pushd Project/Qt
	install -dm 755 %{buildroot}%{_bindir}
	install -m 755 mediaconch-gui %{buildroot}%{_bindir}
popd

# icon
install -dm 755 %{buildroot}%{_datadir}/icons/hicolor/256x256/apps
install -m 644 Source/Resource/Image/MediaConch.png %{buildroot}%{_datadir}/icons/hicolor/256x256/apps/%{name}.png
install -dm 755 %{buildroot}%{_datadir}/pixmaps
install -m 644 Source/Resource/Image/MediaConch.png %{buildroot}%{_datadir}/pixmaps/%{name}.png

# menu-entry
install -dm 755 %{buildroot}/%{_datadir}/applications
install -m 644 Project/GNU/GUI/mediaconch-gui.desktop %{buildroot}/%{_datadir}/applications
%if 0%{?suse_version}
  %suse_update_desktop_file -n mediaconch-gui AudioVideo AudioVideoEditing
%endif
%if 0%{?fedora_version}
  desktop-file-install --dir="%{buildroot}%{_datadir}/applications" -m 644 Project/GNU/GUI/mediaconch-gui.desktop
%endif
install -dm 755 %{buildroot}/%{_datadir}/apps/konqueror/servicemenus
install -m 644 Project/GNU/GUI/mediaconch-gui.kde3.desktop %{buildroot}/%{_datadir}/apps/konqueror/servicemenus/mediaconch-gui.desktop
%if 0%{?suse_version}
  %suse_update_desktop_file -n %{buildroot}/%{_datadir}/apps/konqueror/servicemenus/mediaconch-gui.desktop AudioVideo AudioVideoEditing
%endif
install -dm 755 %{buildroot}/%{_datadir}/kde4/services/ServiceMenus/
install -m 644 Project/GNU/GUI/mediaconch-gui.kde4.desktop %{buildroot}/%{_datadir}/kde4/services/ServiceMenus/mediaconch-gui.desktop
install -dm 755 %{buildroot}/%{_datadir}/appdata/
install -m 644 Project/GNU/GUI/mediaconch-gui.appdata.xml %{buildroot}/%{_datadir}/appdata/mediaconch-gui.appdata.xml
%if 0%{?suse_version}
  %suse_update_desktop_file -n %{buildroot}/%{_datadir}/kde4/services/ServiceMenus/mediaconch-gui.desktop AudioVideo AudioVideoEditing
%endif


%files
%defattr(-,root,root,-)
%doc Release/ReadMe_CLI_Linux.txt License.html History_CLI.txt
%{_bindir}/mediaconch

%files server
%defattr(-,root,root,-)
%{_bindir}/mediaconchd

%files gui
%defattr(-,root,root,-)
%doc Release/ReadMe_GUI_Linux.txt License.html History_GUI.txt
%{_bindir}/mediaconch-gui
%{_datadir}/applications/*.desktop
%{_datadir}/pixmaps/*.png
%dir %{_datadir}/icons/hicolor
%dir %{_datadir}/icons/hicolor/256x256
%dir %{_datadir}/icons/hicolor/256x256/apps
%{_datadir}/icons/hicolor/256x256/apps/*.png
%dir %{_datadir}/apps
%dir %{_datadir}/apps/konqueror
%dir %{_datadir}/apps/konqueror/servicemenus
%{_datadir}/apps/konqueror/servicemenus/*.desktop
%dir %{_datadir}/kde4
%dir %{_datadir}/kde4/services
%dir %{_datadir}/kde4/services/ServiceMenus
%{_datadir}/kde4/services/ServiceMenus/*.desktop
%dir %{_datadir}/appdata
%{_datadir}/appdata/*.xml

%changelog
* Mon May 25 2015 MediaArea.net SARL <info@mediaarea.net> - %{mediaconch_version}
- See History.txt for more info and real dates
