%define mediaconch_version          15.11
%define libmediainfo_version        0.7.80
%define libzen_version              0.4.32

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
BuildRequires:  libjansson-devel
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

%prep
%setup -q -n MediaConch
sed -i 's/.$//' *.txt *.html Release/*.txt

find Source -type f -exec chmod 644 {} ';'
chmod 644 *.html *.txt Release/*.txt

pushd Project/GNU/CLI
    autoreconf -i
popd

pushd Project/Qt
    chmod u+x prepare
    ./prepare "DEFINES+=MEDIAINFO_LIBCURL_YES"
popd

%build
export CFLAGS="%{optflags}"
export CXXFLAGS="%{optflags}"

# build CLI
pushd Project/GNU/CLI
    %configure
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
%if 1
# TODO install -dm 755 %{buildroot}/%{_datadir}/appdata/
# TODO install -m 644 Project/GNU/GUI/mediaconch-gui.appdata.xml %{buildroot}/%{_datadir}/kde4/services/ServiceMenus/mediaconch-gui.appdata.xml
%endif
%if 0%{?suse_version}
  %suse_update_desktop_file -n %{buildroot}/%{_datadir}/kde4/services/ServiceMenus/mediaconch-gui.desktop AudioVideo AudioVideoEditing
%endif


%files
%defattr(-,root,root,-)
%doc Release/ReadMe_CLI_Linux.txt License.html History_CLI.txt
%{_bindir}/mediaconch

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
%if 1
# TODO %dir %{_datadir}/appdata
# TODO %{_datadir}/appdata/*.xml
%endif


%changelog
* Mon May 25 2015 MediaArea.net SARL <info@mediaarea.net> - %{mediaconch_version}
- See History.txt for more info and real dates
