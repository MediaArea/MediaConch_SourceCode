#!/bin/bash

PATH_SCRIPT=$(dirname "${0}")
. "${PATH_SCRIPT}/utils.sh"

FILES_DIRECTORY="${PATH_SCRIPT}/ImplementationChecks/matroska"

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

    # exclude policy tests
    if [ -z "${SCRIPT}" ] || grep -iq "policy" ${FILE} ; then
        continue
    fi

    if [ "${TEST}" == "MKV-NUMERICAL-TAGS" ] ; then
        TEST="MKV-NUMERICAL-TAG"
    fi

    if [ "${TEST}" == "EBML-HEADER-ELEMENTS-WITHIN-MAXIDLENGTH" ] || \
       [ "${TEST}" == "EBML-NO-JUNK-IN-FIXEDSIZE" ] || \
       [ "${TEST}" == "MKV-ELEMENT-VALID-PARENT" ] || \
       [ "${TEST}" == "EBML-ELEM-START" ] || \
       [ "${TEST}" == "EBML-CRC-FIRST" ] ; then
        continue;
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

        DATA="$(${MC} -fx ${TESTFILE})"

        if [ "${TEST}" == "Correct" ] ; then
            # skip this test for correct files
            T1="true"
        else
            # test if the check fail as expected
            T1=`echo "${DATA}" | xmllint --xpath \
            "boolean(//*/*[@icid and @icid=\"${TEST}\" and @fail_count and @fail_count!=\"0\"])" -`
        fi

        # test if other check fail
        T2=`echo "${DATA}" | xmllint --xpath \
        "boolean(//*/*[@icid and @icid!=\"${TEST}\" and @fail_count and @fail_count!=\"0\"])" -`

        if [ "${T1}" != "true" ] || [ "${T2}" != "false" ] ; then
            echo "NOK: ${TESTFILE}" >&9
            rm -f "${TESTFILE}"
            RCODE=1
            continue
        fi

        rm -f "${TESTFILE}"
        echo "OK: ${TESTFILE}" >&9
    done
done

popd

exit ${RCODE}
