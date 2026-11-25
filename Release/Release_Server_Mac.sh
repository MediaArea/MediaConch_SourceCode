#!/bin/bash

##  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 #
 #  Use of this source code is governed by a BSD-style license that can
 #  be found in the License.html file in the root of the source tree.
 ##

set -e # fail on any error

#-----------------------------------------------------------------------
# Setup
release_directory="$(realpath -- "$(dirname -- "${BASH_SOURCE[0]}")")"
workspace_directory="$(realpath -- "${release_directory}/../..")"
version="$(<"${release_directory}/../Project/version.txt")"

export MAKEOPTS=-j$(($(sysctl -n hw.logicalcpu)+1))

#-----------------------------------------------------------------------
# Cleanup
rm -f "${release_directory}/MediaConch_Server_${version}_Mac.dmg"

#-----------------------------------------------------------------------
# Build dependencies and MediaConch Server
pushd "${workspace_directory}"
    ./Server_Compile.sh --enable-arch x86_64 --enable-arch arm64
popd

pushd "${release_directory}/../Project/GNU/Server"
    binary=".libs/mediaconchd"
    if [[ ! -f "${binary}" ]]; then
        binary="mediaconchd"
    fi
    architectures="$(lipo -archs "${binary}")"
    if [[ "${architectures}" != *x86_64* || "${architectures}" != *arm64* ]]; then
        echo "Error: ${binary} is not universal (architectures: ${architectures})" >&2
        exit 1
    fi
popd

#-----------------------------------------------------------------------
# Package .pkg and .dmg
pushd "${release_directory}/../Project/Mac"
    sh ./Make_MC_dmg.sh Server "${version}"
popd

#-----------------------------------------------------------------------
# Copy artifacts
cp -a "${release_directory}/../Project/Mac/MediaConch_Server_${version}_Mac.dmg" "${release_directory}/"
