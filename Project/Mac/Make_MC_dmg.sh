#!/bin/sh

if test -d ~/Qt/5.3/clang_64/bin; then
    PATH=$PATH:~/Qt/5.3/clang_64/bin
fi

if [ $# != 2 ]; then
    echo
    echo "Usage: Make_MC_dmg.sh kind version"
    echo
    exit 1
fi

KIND="$1"
VERSION="$2"

if [ "$KIND" = "CLI" ] || [ "$KIND" = "cli" ]; then
    KIND="CLI"
elif [ "$KIND" = "Server" ] || [ "$KIND" = "server" ]; then
    KIND="Server"
elif [ "$KIND" = "GUI" ] || [ "$KIND" = "gui" ]; then
    KIND="GUI"
else
    echo
    echo "KIND must be either [CLI | cli] or [Server | server ] or [GUI | gui]"
    echo
    exit 1
fi

APPNAME="MediaConch"
APPNAME_lower=`echo ${APPNAME} |awk '{print tolower($0)}'`
KIND_lower=`echo ${KIND} |awk '{print tolower($0)}'`
SIGNATURE="MediaArea.net"
FILES="tmp-${APPNAME}_${KIND}"
TEMPDMG="tmp-${APPNAME}_${KIND}.dmg"
FINALDMG="${APPNAME}_${KIND}_${VERSION}_Mac.dmg"
WD=`pwd`

# Clean up
rm -fr "${FILES}-Root"
rm -fr "${FILES}"
rm -f "${APPNAME}.pkg"
rm -f "${TEMPDMG}"
rm -f "${FINALDMG}"

echo
echo ========== Create the package ==========
echo

mkdir -p "${FILES}/.background"
cp ../../License.html "${FILES}"
cp Logo_White.icns "${FILES}/.background"
if [ "$KIND" = "CLI" ] || [ "$KIND" = "GUI" ]; then
    cp "../../Release/ReadMe_${KIND}_Mac.txt" "${FILES}/ReadMe.txt"
    cp "../../History_${KIND}.txt" "${FILES}/History.txt"
fi

if [ "$KIND" = "CLI" ]; then

    cd ../GNU/CLI
    if test -e ".libs/${APPNAME_lower}"; then
        mv -f ".libs/${APPNAME_lower}" .
    fi
    if ! test -x "${APPNAME_lower}"; then
        echo
        echo "${APPNAME_lower} can’t be found, or this file isn’t a executable."
        echo
        exit 1
    fi
    strip -u -r "${APPNAME_lower}"
    cd ../../Mac

    mkdir -p "${FILES}-Root/usr/local/bin"
    cp "../GNU/CLI/${APPNAME_lower}" "${FILES}-Root/usr/local/bin"
    codesign --deep --options=runtime --force --verbose --sign "Developer ID Application: ${SIGNATURE}" "${FILES}-Root/usr/local/bin/${APPNAME_lower}"

    pkgbuild --root "${FILES}-Root" --identifier "net.mediaarea.${APPNAME_lower}.mac-${KIND_lower}" --sign "Developer ID Installer: ${SIGNATURE}" --version "${VERSION}" "${FILES}/${APPNAME_lower}.pkg"
    codesign --deep --options=runtime --force --verbose --sign "Developer ID Application: ${SIGNATURE}" "${FILES}/${APPNAME_lower}.pkg"

fi

if [ "$KIND" = "Server" ]; then

    APPNAME_lower="mediaconchd"

    cd ../GNU/Server
    if test -e ".libs/${APPNAME_lower}"; then
        mv -f ".libs/${APPNAME_lower}" .
    fi
    if ! test -x "${APPNAME_lower}"; then
        echo
        echo "${APPNAME_lower} can’t be found, or this file isn’t an executable."
        echo
        exit 1
    fi
    strip -u -r "${APPNAME_lower}"
    cd ../../Mac

    mkdir -p "${FILES}-Root/usr/local/bin"
    cp "../GNU/Server/${APPNAME_lower}" "${FILES}-Root/usr/local/bin"
    codesign --deep --options=runtime --force --verbose --sign "Developer ID Application: ${SIGNATURE}" "${FILES}-Root/usr/local/bin/${APPNAME_lower}"

    pkgbuild --root "${FILES}-Root" --identifier "net.mediaarea.${APPNAME_lower}.mac-${KIND_lower}" --sign "Developer ID Installer: ${SIGNATURE}" --version "${VERSION}" "${FILES}/${APPNAME_lower}.pkg"
    codesign --deep --options=runtime --force --verbose --sign "Developer ID Application: ${SIGNATURE}" "${FILES}/${APPNAME_lower}.pkg"

fi

if [ "$KIND" = "GUI" ]; then

    strip -u -r "../Qt/${APPNAME}.app/Contents/MacOS/${APPNAME}"
    cp -r "../Qt/${APPNAME}.app" "${FILES}/"
    sed -i '' -e "s/VERSION/${VERSION}/g" "${FILES}/${APPNAME}.app/Contents/Info.plist"
    macdeployqt "${FILES}/${APPNAME}.app"

    echo
    echo
    echo Signing the application...
    echo

    codesign --deep --options=runtime --force --verbose --sign "Developer ID Application: ${SIGNATURE}" "${FILES}/${APPNAME}.app/Contents/Frameworks/QtWebEngineCore.framework/Helpers/QtWebEngineProcess.app/Contents/MacOS/QtWebEngineProcess"
    codesign --deep --options=runtime --force --verbose --sign "Developer ID Application: ${SIGNATURE}" -i "net.mediaarea.${APPNAME_lower}.mac-${KIND_lower}" "${FILES}/${APPNAME}.app/Contents/Frameworks/QtWebEngineCore.framework/Helpers/QtWebEngineProcess.app"

    for FRAMEWORK in $(ls "${FILES}/${APPNAME}.app/Contents/Frameworks" | grep framework | sed 's/\.framework//') ; do
        codesign --deep --options=runtime --force --verbose --sign "3rd Party Mac Developer Application: ${SIGNATURE}" "${FILES}/${APPNAME}.app/Contents/Frameworks/${FRAMEWORK}.framework/Versions/5/${FRAMEWORK}"
    done

    find "${FILES}/${APPNAME}.app/Contents/PlugIns" -name "*.dylib" -exec codesign --deep --options=runtime --force --verbose --sign "3rd Party Mac Developer Application: $SIGNATURE" '{}' \;

    codesign --deep --options=runtime --deep --no-strict --force --verbose --sign "Developer ID Application: ${SIGNATURE}" "${FILES}/${APPNAME}.app/Contents/MacOS/${APPNAME}"
    echo
    echo
    echo Verifying the signature...
    echo
    codesign --deep --options=runtime --verify --deep --verbose=1 "${FILES}/${APPNAME}.app/Contents/MacOS/${APPNAME}"

    echo
    echo
fi

echo
echo ========== Create the disk image ==========
echo

# Check if an old image isn't already attached
DEVICE=$(hdiutil info |grep -B 1 "/Volumes/${APPNAME}" |egrep '^/dev/' | sed 1q | awk '{print $1}')
test -e "$DEVICE" && hdiutil detach -force "${DEVICE}"

hdiutil create "${TEMPDMG}" -ov -fs HFS+ -format UDRW -volname "${APPNAME}" -srcfolder "${FILES}"
DEVICE=$(hdiutil attach -readwrite -noverify "${TEMPDMG}" | egrep '^/dev/' | sed 1q | awk '{print $1}')
sleep 2

cd "/Volumes/${APPNAME}"
if [ "$KIND" = "GUI" ]; then
    ln -s /Applications
fi
test -e .DS_Store && rm -fr .DS_Store
cd - >/dev/null

if [ "$KIND" = "CLI" ] || [ "$KIND" = "GUI" ]; then
    . Osascript_${KIND}.sh
    osascript_Function
fi

hdiutil detach "${DEVICE}"
sleep 2

echo
echo ========== Convert to compressed image ==========
echo
hdiutil convert "${TEMPDMG}" -format UDBZ -o "${FINALDMG}"

# Useless since the dmg will transit on no HFS+ partition (at least
# on the linux server)
codesign --deep --options=runtime --force --verbose --sign "Developer ID Application: ${SIGNATURE}" "${FINALDMG}"

unset -v APPNAME APPNAME_lower KIND KIND_lower VERSION SIGNATURE
unset -v FILES TEMPDMG FINALDMG WD FRAMEWORK DEVICE TMP
