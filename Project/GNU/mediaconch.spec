%global mediaconch_version          24.06
%global libmediainfo_version        24.06
%global libzen_version              0.4.41

%if 0%{?fedora_version} || 0%{?rhel}
%global libmediaconch_name libmediaconch
%global libmediainfo_name libmediainfo
%global libzen_name libzen
%else
%global libmediaconch_name libmediaconch0
%global libmediainfo_name libmediainfo0
%global libzen_name libzen0
%endif

%if 0%{?mageia}
%ifarch x86_64
%global mageia_libname lib64
%else
%global mageia_libname lib
%endif
%endif

# CentOS also set rhel macro
%if ! 0%{?suse_version} || 0%{?suse_version} >= 1200
%global build_server 1
%else
%global build_server 0
%endif

%if ! 0%{?rhel} && ! 0%{?suse_version} || 0%{?suse_version} >= 1200
%global build_gui 1
%else
%global build_gui 0
%endif

%global build_lib 1

Name:           mediaconch
Version:        %{mediaconch_version}
Release:        1
Summary:        Implementation checker and policy checker for video and audio files (CLI)

Group:          Applications/Multimedia
License:        BSD-2-Clause
URL:            http://MediaArea.net/MediaConch
Packager:       MediaArea.net SARL <info@mediaarea.net>
Source0:        %{name}_%{version}.tar.gz

Requires:       %{libzen_name}%{?_isa} >= %{libzen_version}
Requires:       %{libmediainfo_name}%{?_isa} >= %{libmediainfo_version}

Prefix:         %{_prefix}
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
BuildRequires:  libxml2-devel
BuildRequires:  libxslt-devel
BuildRequires:  libevent-devel
BuildRequires:  libcurl-devel
%if 0%{?mageia}
BuildRequires: %{mageia_libname}sqlite3-devel
%else
BuildRequires:  sqlite-devel
%endif


%if 0%{?rhel} >= 8
BuildRequires:  alternatives
%endif

%if 0%{?fedora_version}
BuildRequires:  pkgconfig(systemd)
%endif

%if 0%{?suse_version}
%if 0%{?is_opensuse}
BuildRequires:  libjansson-devel
%endif
%else
BuildRequires:  jansson-devel
%endif

%if 0%{?mageia}
BuildRequires: %{mageia_libname}openssl-devel
%endif

# Lib dependencies
%if 0%{?build_lib}
%if ! 0%{?rhel}
BuildRequires:  python3-devel
%endif
%endif

# GUI dependencies
%if 0%{?build_gui}
%if 0%{?fedora_version}
BuildRequires:  pkgconfig(Qt5)
BuildRequires:  pkgconfig(Qt5WebEngine)
BuildRequires:  desktop-file-utils
BuildRequires:  libappstream-glib
%else
%if 0%{?suse_version}
%if 0%{?suse_version} >= 1500
BuildRequires: libqt5-qtwebengine-devel
%else
BuildRequires:  libQtWebKit-devel
%endif
BuildRequires:  update-desktop-files
%else
%if 0%{?mageia}
BuildRequires:  %{mageia_libname}qt5webengine-devel
%endif
%endif
%endif
%endif # GUI

%description
MediaConch is an implementation checker, policy checker, reporter,
and fixer that targets preservation-level audiovisual files
(specifically Matroska, Linear Pulse Code Modulation (LPCM)
and FF Video Codec 1 (FFV1)).

This project is maintained by MediaArea and funded by PREFORMA.

This package includes the command line interface.

%if 0%{?build_server}
%package server
Summary:    Implementation checker and policy checker for video and audio files (Server)
Group:      Applications/Multimedia
Requires:   %{libzen_name}%{?_isa} >= %{libzen_version}
Requires:   %{libmediainfo_name}%{?_isa} >= %{libmediainfo_version}

%description server
MediaConch is an implementation checker, policy checker, reporter,
and fixer that targets preservation-level audiovisual files
(specifically Matroska, Linear Pulse Code Modulation (LPCM)
and FF Video Codec 1 (FFV1)).

This project is maintained by MediaArea and funded by PREFORMA.

This package includes the server.

%if 0%{?fedora_version}
%{?systemd_requires}
%endif
%endif # Server

%if 0%{?build_lib}
%package -n %{libmediaconch_name}
Summary:    Implementation checker and policy checker for video and audio files (Library)
Group:      System/Libraries
Requires:   %{libzen_name}%{?_isa} >= %{libzen_version}
Requires:   %{libmediainfo_name}%{?_isa} >= %{libmediainfo_version}

%description -n %{libmediaconch_name}
MediaConch is an implementation checker, policy checker, reporter,
and fixer that targets preservation-level audiovisual files
(specifically Matroska, Linear Pulse Code Modulation (LPCM)
and FF Video Codec 1 (FFV1)).

This project is maintained by MediaArea and funded by PREFORMA.

This package includes the library.

%package -n libmediaconch-devel
Summary:    Implementation checker and policy checker for video and audio files (development files)
Group:      Development/Libraries
Requires:   %{libmediainfo_name}%{?_isa} = %{version}

%description -n libmediaconch-devel
MediaConch is an implementation checker, policy checker, reporter,
and fixer that targets preservation-level audiovisual files
(specifically Matroska, Linear Pulse Code Modulation (LPCM)
and FF Video Codec 1 (FFV1)).

This project is maintained by MediaArea and funded by PREFORMA.

This package includes the library programming interface.

%endif # Lib

%if 0%{?build_gui}
%package gui
Summary:    Implementation checker and policy checker for video and audio files (GUI)
Group:      Applications/Multimedia
Requires:   %{libzen_name}%{?_isa} >= %{libzen_version}
Requires:   %{libmediainfo_name}%{?_isa} >= %{libmediainfo_version}

%description gui
MediaConch is an implementation checker, policy checker, reporter,
and fixer that targets preservation-level audiovisual files
(specifically Matroska, Linear Pulse Code Modulation (LPCM)
and FF Video Codec 1 (FFV1)).

This project is maintained by MediaArea and funded by PREFORMA.

This package includes the graphical user interface.
%endif # GUI

%prep
%setup -q -n MediaConch
sed -i 's/.$//' *.txt *.html Release/*.txt

find Source -type f -exec chmod 644 {} ';'
chmod 644 *.html *.txt Release/*.txt

pushd Project/GNU/CLI
    autoreconf -fiv
popd

%if 0%{?build_server}
pushd Project/GNU/Server
    autoreconf -fiv
popd
%endif # Server

%if 0%{?build_lib}
pushd Project/GNU/Library
    autoreconf -fiv
popd
%endif # Lib

%if 0%{?build_gui}
pushd Project/Qt
    %if 0%{?fedora}
        %{qmake_qt5}
    %else
        chmod u+x prepare
        %if 0%{?suse_version} && ! 0%{?is_opensuse}
            ./prepare NO_JANSSON=yes
        %else
            ./prepare
        %endif
    %endif
popd
%endif # GUI

%build
export CFLAGS="-g %{optflags}"
export CXXFLAGS="-g %{optflags}"

# build CLI
pushd Project/GNU/CLI
    %if 0%{?suse_version} && ! 0%{?is_opensuse}
        %configure --without-jansson
    %else
        %configure
    %endif
    make %{?_smp_mflags}
popd

# build server
%if 0%{?build_server}
pushd Project/GNU/Server
    %if 0%{?suse_version} && ! 0%{?is_opensuse}
        %configure --without-jansson
    %else
        %configure
    %endif
    make %{?_smp_mflags}
popd
%endif #Server

# build lib
%if 0%{?build_lib}
pushd Project/GNU/Library
    %if 0%{?suse_version} && ! 0%{?is_opensuse}
        %configure --without-jansson
    %else
        %configure
    %endif
    make %{?_smp_mflags}
popd
%endif #Lib

# now build GUI
%if 0%{?build_gui}
pushd Project/Qt
    make %{?_smp_mflags}
popd
%endif # GUI


%install
pushd Project/GNU/CLI
    make install DESTDIR=%{buildroot}
popd

%if 0%{?build_server}
pushd Project/GNU/Server
    make install DESTDIR=%{buildroot}
popd
%endif # Server

%if 0%{?build_lib}
pushd Project/GNU/Library
    make install DESTDIR=%{buildroot}
popd
rm -f %{buildroot}%{_libdir}/libmediaconch.la
%endif # Lib

%if 0%{?build_gui}
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
install -dm 755 %{buildroot}%{_datadir}/applications
install -m 644 Project/GNU/GUI/mediaconch-gui.desktop %{buildroot}%{_datadir}/applications
install -dm 755 %{buildroot}%{_datadir}/apps/konqueror/servicemenus
install -m 644 Project/GNU/GUI/mediaconch-gui.kde3.desktop %{buildroot}%{_datadir}/apps/konqueror/servicemenus/mediaconch-gui.desktop
install -dm 755 %{buildroot}%{_datadir}/kde4/services/ServiceMenus/
install -m 644 Project/GNU/GUI/mediaconch-gui.kde4.desktop %{buildroot}%{_datadir}/kde4/services/ServiceMenus/mediaconch-gui.desktop
install -dm 755 %{buildroot}%{_datadir}/kservices5/ServiceMenus/
install -m 644 Project/GNU/GUI/mediaconch-gui.kde4.desktop %{buildroot}%{_datadir}/kservices5/ServiceMenus/mediaconch-gui.desktop
%if %{undefined fedora_version} || 0%{?fedora_version} < 26
install -dm 755 %{buildroot}%{_datadir}/appdata/
install -m 644 Project/GNU/GUI/mediaconch-gui.metainfo.xml %{buildroot}%{_datadir}/appdata/mediaconch-gui.appdata.xml
%else
install -dm 755 %{buildroot}%{_datadir}/metainfo/
install -m 644 Project/GNU/GUI/mediaconch-gui.metainfo.xml %{buildroot}%{_datadir}/metainfo/mediaconch-gui.metainfo.xml
%endif
%if 0%{?suse_version}
%suse_update_desktop_file -n %{buildroot}%{_datadir}/kservices5/ServiceMenus/mediaconch-gui.desktop AudioVideo AudioVideoEditing
%suse_update_desktop_file -n %{buildroot}%{_datadir}/kde4/services/ServiceMenus/mediaconch-gui.desktop AudioVideo AudioVideoEditing
%suse_update_desktop_file -n %{buildroot}%{_datadir}/apps/konqueror/servicemenus/mediaconch-gui.desktop AudioVideo AudioVideoEditing
%suse_update_desktop_file -n mediaconch-gui AudioVideo AudioVideoEditing
%endif
%if 0%{?fedora_version}
desktop-file-install --dir="%{buildroot}%{_datadir}/applications" -m 644 Project/GNU/GUI/mediaconch-gui.desktop
install -dm 755 %{buildroot}%{_unitdir}
install -m 644 -p Project/GNU/Server/mediaconchd.service  %{buildroot}%{_unitdir}/mediaconchd.service
install -dm 755 %{buildroot}%{_sysconfdir}/%{name}
install -m 644 -p Project/GNU/Server/MediaConch.rc  %{buildroot}%{_sysconfdir}/%{name}/MediaConch.rc
%check
%if 0%{?fedora_version} >= 26
appstream-util validate-relax --nonet %{buildroot}/%{_datadir}/metainfo/*.metainfo.xml
%else
appstream-util validate-relax --nonet %{buildroot}/%{_datadir}/appdata/*.appdata.xml
%endif
%endif
%endif #GUI

%if 0%{?fedora_version}
%post server
%systemd_post mediaconchd.service

%preun server
%systemd_preun mediaconchd.service

%postun server
%systemd_postun_with_restart mediaconchd.service

%post gui
/bin/touch --no-create %{_datadir}/icons/hicolor &>/dev/null || :

%postun gui
if [ $1 -eq 0 ] ; then
    /bin/touch --no-create %{_datadir}/icons/hicolor &>/dev/null
    /usr/bin/gtk-update-icon-cache %{_datadir}/icons/hicolor &>/dev/null || :
fi

%posttrans gui
/usr/bin/gtk-update-icon-cache %{_datadir}/icons/hicolor &>/dev/null || :
%endif

%files
%defattr(-,root,root,-)
%doc Release/ReadMe_CLI_Linux.txt History_CLI.txt
%if 0%{?fedora_version} || 0%{?rhel}
%license License.html
%else
%doc License.html
%endif
%{_bindir}/mediaconch

%if 0%{?build_server}
%files server
%defattr(-,root,root,-)
%doc Documentation/Daemon.md Documentation/Config.md Documentation/REST.md
%if 0%{?fedora_version} || 0%{?rhel}
%license License.html
%else
%doc License.html
%endif
%{_bindir}/mediaconchd
%if 0%{?fedora_version}
%config(noreplace) %{_sysconfdir}/%{name}/MediaConch.rc
%{_unitdir}/mediaconchd.service
%endif
%endif # Server

%if 0%{?build_lib}
%files -n %{libmediaconch_name}
%defattr(-,root,root,-)
%{_libdir}/libmediaconch.so.*

%files -n libmediaconch-devel
%defattr(-,root,root,-)
%{_includedir}/MediaConch
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libmediaconch.so
%endif # Library

%if 0%{?build_gui}
%files gui
%defattr(-,root,root,-)
%doc Release/ReadMe_GUI_Linux.txt History_GUI.txt
%if 0%{?fedora_version} || 0%{?rhel}
%license License.html
%else
%doc License.html
%endif
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
%dir %{_datadir}/kservices5
%dir %{_datadir}/kservices5/ServiceMenus
%{_datadir}/kservices5/ServiceMenus/*.desktop
%if 0%{?fedora_version} && 0%{?fedora_version} >= 26
%dir %{_datadir}/metainfo
%{_datadir}/metainfo/*.xml
%else
%dir %{_datadir}/appdata
%{_datadir}/appdata/*.xml
%endif
%endif # GUI

%changelog
* Mon May 25 2015 MediaArea.net SARL <info@mediaarea.net> - %{mediaconch_version}
- See History.txt for more info and real dates

# Contributor: Vascom, vascom2@gmail.com"
