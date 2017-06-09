#!/bin/bash

PATH_SCRIPT=$(dirname "${0}")
. "${PATH_SCRIPT}/utils.sh"

FILES_DIRECTORY="${PATH_SCRIPT}/ImplementationChecks/matroska"

MC="${PWD}/mediaconch"

RCODE=0

pushd "${FILES_DIRECTORY}"

for FILE in *.md ; do
    # skip test if there no .md file
    if [ "${FILE}" == "*.md" ] ; then
        exit 77
    fi

    TEST="$(grep -Eom1 '^# [0-9A-Z\-]+' ${FILE} | grep -Eom1 '[0-9A-Z\-]+')"
    TESTFILE="$(grep -Eom1 '[0-9A-Z\-]+\.mkv' ${FILE})"
    TESTNAME="${TESTFILE%.*}"
    SCRIPT="$(sed -n '/```sh/,/```/{/```sh/!{/```/!p;};}' ${FILE})"

    if [ "${TESTFILE}" == "" ] || [ "${SCRIPT}" == "" ] ; then
        continue
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

    # clean intermediate files
    rm -f "${TESTNAME}.sh" "reference.xml" "reference.mkv"

    if [ ! -e "${TESTFILE}" ] || [ "$(stat -c %s ${TESTFILE})" -eq 0 ] ; then
        echo "INVALID: ${TESTFILE}" >&9
        rm -f "${TESTFILE}"
        RCODE=1
        continue
    fi

    DATA="$(${MC} -fx ${TESTFILE})"

    # test if the check fails as expected
    T1=`echo "${DATA}" | xmllint --xpath \
    "boolean(//*/*[@icid and @icid=\"${TESTNAME}\" and @fail_count and @fail_count!=\"0\"])" -`

    # test if other check fails
    T2=`echo "${DATA}" | xmllint --xpath \
    "boolean(//*/*[@icid and @icid!=\"${TESTNAME}\" and @fail_count and @fail_count!=\"0\"])" -`

    if [ "${T1}" != "true" ] || [ "${T2}" != "false" ] ; then
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
