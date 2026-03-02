##  Copyright (c) MediaArea.net SARL. All Rights Reserved.
##
##  Use of this source code is governed by a BSD-style license that can
##  be found in the License.html file in the root of the source tree.
##

Param([parameter(Mandatory=$true)][String]$arch)

$ErrorActionPreference = "Stop"

#-----------------------------------------------------------------------
# Setup
$release_directory = $PSScriptRoot
$version = (Get-Content "${release_directory}\..\Project\version.txt" -Raw).Trim()
$arch_alt="${arch}"
if ("${arch}" -eq "Win32" ) {
    $arch_alt="i386"
}
$arch_vs="${arch}"
if ("${arch}" -eq "Win32" ) {
    $arch_vs="x86"
}

#-----------------------------------------------------------------------
# Cleanup
$artifact = "${release_directory}\MediaConch_GUI_${version}_Windows_${arch_alt}"
if (Test-Path "${artifact}") {
    Remove-Item -Force -Recurse "${artifact}"
}

$artifact = "${release_directory}\MediaConch_GUI_${version}_Windows_${arch_alt}_WithoutInstaller.7z"
if (Test-Path "${artifact}") {
    Remove-Item -Force "${artifact}"
}

$artifact = "${release_directory}\MediaConch_GUI_${version}_Windows_${arch_alt}.exe"
if (Test-Path "${artifact}") {
    Remove-Item -Force "${artifact}"
}

#-----------------------------------------------------------------------
# Package GUI
Push-Location "${release_directory}"
    New-Item -Force -ItemType Directory -Path "MediaConch_GUI_${version}_Windows_${arch_alt}"
    Push-Location "MediaConch_GUI_${version}_Windows_${arch_alt}"
        ### Copying: Exe ###
        Copy-Item -Force "..\..\Project\Qt\${arch}\MediaConch.exe" MediaConch.exe
        ### Copying: libCURL ###
        Copy-Item -Force "..\..\..\libcurl\${arch}\Release\LIBCURL.DLL" .
        ### Copying: Qt ###
        Copy-Item -Force "..\..\Project\Qt\${arch}\Qt5Core.dll" .
        Copy-Item -Force "..\..\Project\Qt\${arch}\Qt5Gui.dll" .
        Copy-Item -Force "..\..\Project\Qt\${arch}\Qt5Network.dll" .
        Copy-Item -Force "..\..\Project\Qt\${arch}\libEGL.dll" .
        Copy-Item -Force "..\..\Project\Qt\${arch}\libGLESV2.dll" .
        Copy-Item -Force "..\..\Project\Qt\${arch}\d3dcompiler_47.dll" .
        Copy-Item -Force "..\..\Project\Qt\${arch}\Qt5Qml.dll" .
        Copy-Item -Force "..\..\Project\Qt\${arch}\Qt5Quick.dll" .
        Copy-Item -Force "..\..\Project\Qt\${arch}\Qt5WebChannel.dll" .
        Copy-Item -Force "..\..\Project\Qt\${arch}\Qt5WebEngineCore.dll" .
        Copy-Item -Force "..\..\Project\Qt\${arch}\Qt5WebEngineWidgets.dll" .
        Copy-Item -Force "..\..\Project\Qt\${arch}\Qt5PrintSupport.dll" .
        Copy-Item -Force "..\..\Project\Qt\${arch}\Qt5QuickWidgets.dll" .
        Copy-Item -Force "..\..\Project\Qt\${arch}\Qt5Positioning.dll" .
        Copy-Item -Force "..\..\Project\Qt\${arch}\Qt5Widgets.dll" .
        Copy-Item -Force "..\..\Project\Qt\${arch}\QtWebEngineProcess.exe" .
        New-Item -ItemType directory -Path "resources"
        Push-Location -Path "resources"
            Copy-Item -Path "..\..\..\Project\Qt\${arch}\resources\icudtl.dat" .
            Copy-Item -Path "..\..\..\Project\Qt\${arch}\resources\qtwebengine_resources.pak" .
            Copy-Item -Path "..\..\..\Project\Qt\${arch}\resources\qtwebengine_resources_100p.pak" .
            Copy-Item -Path "..\..\..\Project\Qt\${arch}\resources\qtwebengine_resources_200p.pak" .
        Pop-Location
        New-Item -ItemType directory -Path "platforms"
        Push-Location -Path "platforms"
            Copy-Item -Path "..\..\..\Project\Qt\${arch}\platforms\qwindows.dll" .
        Pop-Location
        New-Item -ItemType directory -Path "translations"
        Push-Location -Path "translations"
            Copy-Item -Path "..\..\..\Project\Qt\${arch}\translations\qt_en.qm" .
            New-Item -ItemType directory -Path "qtwebengine_locales"
            Push-Location -Path "qtwebengine_locales"
                Copy-Item -Path "..\..\..\..\Project\Qt\${arch}\translations\qtwebengine_locales\en-US.pak" .
            Pop-Location
        Pop-Location
        ### Copying: VC runtimes ###
        Copy-Item -Path "${Env:VCToolsRedistDir}\${arch_vs}\Microsoft.VC143.CRT\concrt140.dll" .
        Copy-Item -Path "${Env:VCToolsRedistDir}\${arch_vs}\Microsoft.VC143.CRT\msvcp140.dll" .
        Copy-Item -Path "${Env:VCToolsRedistDir}\${arch_vs}\Microsoft.VC143.CRT\msvcp140_1.dll" .
        Copy-Item -Path "${Env:VCToolsRedistDir}\${arch_vs}\Microsoft.VC143.CRT\msvcp140_2.dll" .
        Copy-Item -Path "${Env:VCToolsRedistDir}\${arch_vs}\Microsoft.VC143.CRT\msvcp140_atomic_wait.dll" .
        Copy-Item -Path "${Env:VCToolsRedistDir}\${arch_vs}\Microsoft.VC143.CRT\msvcp140_codecvt_ids.dll" .
        Copy-Item -Path "${Env:VCToolsRedistDir}\${arch_vs}\Microsoft.VC143.CRT\vccorlib140.dll" .
        Copy-Item -Path "${Env:VCToolsRedistDir}\${arch_vs}\Microsoft.VC143.CRT\vcruntime140.dll" .
        if ("${arch}" -ne "Win32" ) {
            Copy-Item -Path "${Env:VCToolsRedistDir}\${arch_vs}\Microsoft.VC143.CRT\vcruntime140_1.dll" .
        }
        Copy-Item -Path "${Env:VCToolsRedistDir}\${arch_vs}\Microsoft.VC143.CRT\vcruntime140_threads.dll" .
        ### Copying: Information files ###
        Copy-Item -Force "..\..\License.html" .
        Copy-Item -Force "..\..\History_GUI.txt" "History.txt"
        Copy-Item -Force "..\Readme_GUI_Windows.txt" "ReadMe.txt"
        ### Archive
        7za.exe a -r -t7z -mx9 "..\MediaConch_GUI_${version}_Windows_${arch_alt}_WithoutInstaller.7z" *
    Pop-Location
Pop-Location

#-----------------------------------------------------------------------
# Package installer
Push-Location "${release_directory}"
    makensis.exe "..\Source\Install\MediaConch_GUI_Windows.nsi"
Pop-Location
