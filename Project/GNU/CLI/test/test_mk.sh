#!/bin/bash

PATH_SCRIPT=$(dirname "${0}")
. "${PATH_SCRIPT}/utils.sh"

FILES_DIRECTORY="${PATH_SCRIPT}/ImplementationChecks/matroska"

BUILD_NUMBER="${TRAVIS_BUILD_NUMBER:-0}"

RUN_REPORT="$(date +%Y%m%d)_MediaArea_${BUILD_NUMBER:-0}.txt"
RUN_META="$(date +%Y%m%d)_MediaArea_${BUILD_NUMBER:-0}.meta.txt"

MC="${PWD}/mediaconch"

RCODE=0

pushd "${FILES_DIRECTORY}"

# get and configure ffmpeg sources
git clone --depth=1 https://github.com/FFmpeg/FFmpeg.git
mv FFmpeg/* .
rm -fr FFmpeg

./configure --disable-doc --disable-x86asm --disable-ffserver --disable-ffplay --disable-ffprobe --enable-static --disable-bzlib --disable-libopenjpeg --disable-iconv --disable-zlib --disable-everything --enable-indev=lavfi --enable-filter=scale,color,testsrc2 --enable-decoder=rawvideo,ffv1 --enable-encoder=rawvideo,ffv1 --enable-demuxer=matroska --enable-muxer=matroska --enable-protocol=file

for FILE in $(ls -v *.md) ; do
    if ! echo "${FILE}" | grep -Eq '^[0-9]+\.md$' ; then
        continue
    fi

    TEST="$(grep -Eom1 '^# [^[:space:]]+$' ${FILE} | grep -Eom1 '[^[:space:]]+$')"
    SCRIPT="$(perl -0777lne 'print for grep !//,/^```sh\s+(.*?)^```/mgs' ${FILE})"

    POLICY="$(perl -0777lne 'print for grep !//,/^```xml\s+(.*?)^```/mgs' ${FILE})"
    if [ -n "${POLICY}" ] ; then
        echo "${POLICY}" > "${TEST}.xml"
    fi

    echo "${SCRIPT}" > "${TEST}.sh"
    sh "${TEST}.sh"
    rm "${TEST}.sh"

    # clean intermediate files
    rm -f "reference.xml" "reference.mkv"

    for TESTFILE in ${TEST}*.mkv ; do
        if [ ! -e "${TESTFILE}" ] ; then
            continue
        fi

        DATA="$(${MC}${POLICY/$POLICY/ -p $TEST.xml} -fx ${TESTFILE})"

        # test if the check fail as expected
        if [ "${TEST}" == "Correct" ] ; then
            # skip this test for correct files
            T1="true"
        elif [ -n "${POLICY}" ] ; then
            # test against a policy
            T1=`echo "${DATA}" | xmllint --xpath \
            "boolean(//*/*[@name and @name=\"${TEST}\" and @fail_count and @fail_count!=\"0\"])" -`
        else
            T1=`echo "${DATA}" | xmllint --xpath \
            "boolean(//*/*[@icid and @icid=\"${TEST}\" and @fail_count and @fail_count!=\"0\"])" -`
        fi

        # test if other check fail
        if [ -n "${POLICY}" ] ; then
            # skip this test for policies
            T2="false"
        else
            T2=`echo "${DATA}" | xmllint --xpath \
            "boolean(//*/*[@icid and @icid!=\"${TEST}\" and @fail_count and @fail_count!=\"0\"])" -`
        fi

        CLASSID="AVC$(printf '%03d' ${FILE%.*})"
        COL2="Q0"
        DOC_UID="${TESTFILE}"
        COL4="0"

        if [ "${T1}" != "true" ] || [ "${T2}" != "false" ] ; then
            echo "NOK: ${TESTFILE}" >&9
            CLASS_BELONG="0"
            echo -e "${CLASSID}\t${COL2}\t${DOC_UID}\t${COL4}\t${CLASS_BELONG}\t${BUILD_NUMBER}" >> "${RUN_REPORT}"
            rm -f "${TESTFILE}"
            RCODE=1
            continue
        fi

        rm -f "${TESTFILE}"
        echo "OK: ${TESTFILE}" >&9
        CLASS_BELONG="1"
        echo -e "${CLASSID}\t${COL2}\t${DOC_UID}\t${COL4}\t${CLASS_BELONG}\t${BUILD_NUMBER}" >> "${RUN_REPORT}"
    done

    rm -f "${TEST}.xml"

done

if [ -n "${TRAVIS_COMMIT_MESSAGE}" ] ; then
    echo -e "${TRAVIS_COMMIT_MESSAGE}" >> "${RUN_META}"
fi

popd

exit ${RCODE}
