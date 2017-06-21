#!/bin/bash

PATH_SCRIPT=$(dirname "${0}")
. "${PATH_SCRIPT}/utils.sh"

FILES_DIRECTORY="${PATH_SCRIPT}/ImplementationChecks/matroska"

MC="${PWD}/mediaconch"

RCODE=0

pushd "${FILES_DIRECTORY}"

for FILE in *.md ; do
    TEST="$(grep -Eom1 '^# [0-9A-Z\-]+[0-9A-Z]' ${FILE} | grep -Eom1 '[0-9A-Z\-]+[0-9A-Z]')"
    SCRIPT="$(perl -0777lne 'print for grep !/make/,/^```sh\s+(.*?)^```/mgs' ${FILE})"

    if [ -z "${SCRIPT}" ] || grep -iq "policy" ${FILE} ; then
        continue
    fi

    if [ "${TEST}" == "MKV-NUMERICAL-TAGS" ] ; then
        TEST="MKV-NUMERICAL-TAG"
    fi

    if [ "${TEST}" == "HEADER-ELEMENTS-WITHIN-MAXIDLENGTH" ] || \
       [ "${TEST}" == "SEGMENTUID-PRESENT" ] || \
       [ "${TEST}" == "EBML-ELEM-START" ] || \
       [ "${TEST}" == "EBML-CRC-FIRST" ] ; then
        continue;
    fi

    echo "${SCRIPT}" > "${TEST}.sh"
    sed -i'' 's/^xml /xmlstarlet /g' "${TEST}.sh"
    sh "${TEST}.sh" &>/dev/null
    rm "${TEST}.sh"

    # clean intermediate files
    rm -f "reference.xml" "reference.mkv"

    for TESTFILE in ${TEST}*.mkv ; do
        if [ ! -e "${TESTFILE}" ] || [ "$(stat -c %s ${TESTFILE})" -eq 0 ] ; then
            echo "INVALID: ${TESTFILE}" >&9
            rm -f "${TESTFILE}"
            RCODE=1
            continue
        fi

        DATA="$(${MC} -fx ${TESTFILE})"

        # test if the check fail as expected
        T1=`echo "${DATA}" | xmllint --xpath \
        "boolean(//*/*[@icid and @icid=\"${TEST}\" and @fail_count and @fail_count!=\"0\"])" -`

        # test if other check fail
        T2=`echo "${DATA}" | xmllint --xpath \
        "boolean(//*/*[@icid and @icid!=\"${TEST}\" and @fail_count and @fail_count!=\"0\"])" -`

        if [ "${T1}" != "true" ] || [ "${T2}" != "false" ] ; then
            echo "NOK: ${TEST}" >&9
            rm -f "${TESTFILE}"
            RCODE=1
            continue
        fi

        rm -f "${TESTFILE}"
        echo "OK: ${TEST}" >&9
    done
done

popd

exit ${RCODE}
