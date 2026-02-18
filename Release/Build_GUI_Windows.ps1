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

#-----------------------------------------------------------------------
# Build
Push-Location -Path "${release_directory}\..\..\libevent\Contrib\VC17\event"
    MSBuild "/p:Configuration=Release;Platform=${arch}" event.vcxproj
Pop-Location

Push-Location -Path "${release_directory}\..\..\jansson\Contrib\VC17\Jansson"
    MSBuild "/p:Configuration=Release;Platform=${arch}" Jansson.vcxproj
Pop-Location

Push-Location -Path "${release_directory}\..\..\libxml2\win32\VC17"
    MSBuild "/p:Configuration=Release;Platform=${arch}" libxml2.vcxproj
Pop-Location

Push-Location -Path "${release_directory}\..\..\libxslt\win32\VC17"
    MSBuild "/p:Configuration=Release;Platform=${arch}" libxslt/libxslt.vcxproj
    MSBuild "/p:Configuration=Release;Platform=${arch}" libexslt/libexslt.vcxproj
Pop-Location

Push-Location -Path "${release_directory}\..\..\MediaInfoLib\Project\MSVC2022"
    MSBuild "/p:Configuration=Release;Platform=${arch}" MediaInfoLib.sln
Pop-Location

Push-Location -Path "${release_directory}\..\Project\Qt"
    New-Item -Force -ItemType Directory "${arch}"
    Push-Location -Path "${arch}"
        qmake ..
        nmake
        windeployqt MediaConch.exe
    Pop-Location
Pop-Location
