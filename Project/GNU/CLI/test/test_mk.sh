#!/bin/bash

PATH_SCRIPT=$(dirname "${0}")
. "${PATH_SCRIPT}/utils.sh"

FILES_DIRECTORY="${PATH_SCRIPT}/ImplementationChecks/matroska"

MC="${PWD}/mediaconch"

RCODE=0

pushd "${FILES_DIRECTORY}"

for FILE in *.md ; do
    TEST="$(grep -Po '(?<=^# )[0-9A-Z\-]+' ${FILE})"
    TESTFILE="$(grep -Pom1 '[0-9A-Z\-]+\.mkv' ${FILE})"
    TESTNAME="${TESTFILE%.*}"
    SCRIPT="$(sed -n '/```sh/,/```/{/```sh/!{/```/!p}}' ${FILE})"

    if [ "${TESTFILE}" == "" ] || [ "${SCRIPT}" == "" ] ; then
        continue
    fi

    if [ "${TESTNAME}" == "EBML-ELEM-SIZE-7F" ] ; then
        TESTNAME="EBML-ELEM-UNKNOWN-SIZE"
    fi

    if [ "${TESTNAME}" == "MKV-NUMERICAL-TAGS" ] ; then
        TESTNAME="MKV-NUMERICAL-TAG"
    fi

    if [ "${TEST}" == "HEADER-ELEMENTS-WITHIN-MAXIDLENGTH" ] || \
       [ "${TEST}" == "COLOUR-PRIMARY-CLARITY" ] || \
       [ "${TEST}" == "INTERLACEMENT-CLARITY" ] || \
       [ "${TEST}" == "SAMPLE-RANGE-CLARITY" ] || \
       [ "${TEST}" == "SEGMENTUID-PRESENT" ] || \
       [ "${TEST}" == "EBML-ELEM-START" ] || \
       [ "${TEST}" == "EBML-CRC-FIRST" ] ; then
        continue;
    fi

    echo -e "#!/bin/sh\n${SCRIPT}" > "${TESTNAME}.sh"
    sed -i'' 's/^xml /xmlstarlet /g' "${TESTNAME}.sh"
    sh "${TESTNAME}.sh" &>/dev/null
    rm "${TESTNAME}.sh"

    # clean intermediate files
    rm -f "reference.xml" "reference.mkv"

    if [ ! -e "${TESTFILE}" ] || [ "$(stat -c %s ${TESTFILE})" -eq 0 ] ; then
        echo "INVALID: ${TESTFILE}" >&9
        rm -f "${TESTFILE}"
        RCODE=1
        continue
    fi

    DATA="$(${MC} -fx ${TESTFILE})"

    RESULT=`echo "${DATA}" | xmllint --xpath \
    "boolean(//*/*[@icid and @icid=\"${TESTNAME}\" and @fail_count and @fail_count!=\"0\"])" -`
    if [ "${RESULT}" != "true" ] ; then
        echo "NOK: ${TEST}" >&9
        rm -f "${TESTFILE}"
        RCODE=1
        continue
    fi

    RESULT=`echo "${DATA}" | xmllint --xpath \
    "boolean(//*/*[@icid and @icid!=\"${TESTNAME}\" and @fail_count and @fail_count!=\"0\"])" -`
    if [ "${RESULT}" != "false" ] ; then
        echo "NOK: ${TEST}" >&9
        rm -f "${TESTFILE}"
        RCODE=1
        continue
    fi

rm -f "${TESTFILE}"

    echo "OK: ${TEST}" >&9
done

popd

exit ${RCODE}
