#!/bin/bash

PATH_SCRIPT=$(dirname "${0}")
. "${PATH_SCRIPT}/utils.sh"

FILES_DIRECTORY="${PATH_SCRIPT}/SampleFiles/PolicyTestFiles"

MC="${PWD}/mediaconch"

RCODE=0

while read LINE ; do
    POLICY="$(echo "${LINE}" | cut -d':' -f1)"
    FILE="$(echo "${LINE}" | cut -d':' -f2)"
    OUTCOME="$(echo "${LINE}" | cut -d':' -f3)"
    TEST="${POLICY}:$(basename "${FILE}"):${OUTCOME}"

    if [ ! -e "${FILES_DIRECTORY}/Policy/${POLICY}" ] ; then
        echo "NOK: ${TEST}/${POLICY}, file not found" >&9
        RCODE=1
        continue
    fi

    if [ ! -e "${FILES_DIRECTORY}/${FILE}" ] ; then
        echo "NOK: ${TEST}/${FILE}, file not found" >&9
        RCODE=1
        continue
    fi

    pushd "${FILES_DIRECTORY}" >/dev/null 2>&1
    DATA="$(${MC} -p Policy/${POLICY} -fx ${FILE})"

    T1=`echo "${DATA}" | xmllint --xpath \
    "string(/*[local-name()='MediaConch']/*[local-name()='media']/*[local-name()='policy']/@outcome)" -`

    if [ "${T1}" == "${OUTCOME}" ] ; then
        echo "OK: ${TEST}" >&9
    else
        echo "NOK: ${TEST}, failed" >&9
        RCODE=1
    fi
    popd >/dev/null 2>&1
done < "${PATH_SCRIPT}/test_policy.txt"

exit ${RCODE}
