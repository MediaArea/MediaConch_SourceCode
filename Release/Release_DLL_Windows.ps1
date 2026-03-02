##  Copyright (c) MediaArea.net SARL. All Rights Reserved.
##
##  Use of this source code is governed by a BSD-style license that can
##  be found in the License.html file in the root of the source tree.
##

Param([parameter(Mandatory=$true)][String]$arch)

$ErrorActionPreference = "Stop"

#-----------------------------------------------------------------------
# Setup
$release_directory = Split-Path -Parent $MyInvocation.MyCommand.Path
$version = (Get-Content "${release_directory}\..\Project\version.txt" -Raw).Trim()
$arch_alt="${arch}"
if ("${arch}" -eq "Win32" ) {
    $arch_alt="i386"
}

#-----------------------------------------------------------------------
# Cleanup
$artifact = "${release_directory}\MediaConch_DLL_${version}_Windows_${arch_alt}"
if (Test-Path "${artifact}") {
    Remove-Item -Force -Recurse "${artifact}"
}

$artifact = "${release_directory}\MediaConch_DLL_${version}_Windows_${arch_alt}.zip"
if (Test-Path "${artifact}") {
    Remove-Item -Force "${artifact}"
}

#-----------------------------------------------------------------------
# Package DLL
Push-Location "${release_directory}"
    New-Item -Force -ItemType Directory -Path "MediaConch_DLL_${version}_Windows_${arch_alt}"
    Push-Location "MediaConch_DLL_${version}_Windows_${arch_alt}"
        ### Copying: Dll ###
        Copy-Item -Force "..\..\Project\MSVC2022\${arch}\Release\MediaConch.dll" .
        New-Item -ItemType Directory -Path "Developers"
        Push-Location -Path "Developers"
            ### Copying: Include ###
            Copy-Item -Force "..\..\..\Source\Lib\MediaConchDLL.h" .
            ### Copying: Python API ###
            Copy-Item -Force "..\..\..\Source\Lib\MediaConchLib.py" .
        Pop-Location
        ### Copying: libCURL ###
        Copy-Item -Force "..\..\..\libcurl\${arch}\Release\LIBCURL.DLL" .
        ### Copying: Information files ###
        Copy-Item -Force "..\..\License.html" .
        ### Archive
        7za.exe a -r -tzip -mx9 "..\MediaConch_DLL_${version}_Windows_${arch_alt}.zip" *
    Pop-Location
Pop-Location
