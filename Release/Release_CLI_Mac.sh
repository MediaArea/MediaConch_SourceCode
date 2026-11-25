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
rm -f "${release_directory}/MediaConch_CLI_${version}_Mac.dmg"

#-----------------------------------------------------------------------
# Build dependencies and MediaConch CLI
pushd "${workspace_directory}"
    ./CLI_Compile.sh --enable-arch x86_64 --enable-arch arm64
popd

pushd "${release_directory}/../Project/GNU/CLI"
    binary=".libs/mediaconch"
    if [[ ! -f "${binary}" ]]; then
        binary="mediaconch"
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
    ./Make_MC_dmg.sh CLI "${version}"
popd

#-----------------------------------------------------------------------
# Copy artifacts
cp -a "${release_directory}/../Project/Mac/MediaConch_CLI_${version}_Mac.dmg" "${release_directory}/"
