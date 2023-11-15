#!/bin/bash

PATH_SCRIPT=$(dirname "${0}")
. "${PATH_SCRIPT}/utils.sh"

FILES_DIRECTORY="${PATH_SCRIPT}/SampleFiles/Groundtruth"

MC="${PWD}/mediaconch"

RCODE=0

pushd "${FILES_DIRECTORY}" >/dev/null
    for NAME in $(ls -v) ; do
        if [ ! -d "${NAME}" ] ; then
            continue
        fi

        pushd "${NAME}" >/dev/null
            OPTIONS=
            if [ -f "policy.xml" ] ; then
                OPTIONS=" -p policy.xml"
            fi

            for FILE in $(ls -v *.mkv) ; do
                DATA="$(${MC}${OPTIONS} -fx ${FILE})"

                # test if the check fail as expected
                if [ "${NAME}" == "Correct" ] ; then
                    # skip this test for correct files
                    T1="true"
                elif [ -f "policy.xml" ] ; then
                    # test against a policy
                    T1=`echo "${DATA}" | xmllint --xpath \
                    "boolean(//*/*[@name and @name=\"${NAME}\" and @fail_count and @fail_count!=\"0\"])" -`
                else
                    T1=`echo "${DATA}" | xmllint --xpath \
                    "boolean(//*/*[@icid and @icid=\"${NAME}\" and @fail_count and @fail_count!=\"0\"])" -`
                fi

                # test if other check fail
                if [ -f "policy.xml" ] ; then
                    # skip this test for policies
                    T2="false"
                else
                    T2=`echo "${DATA}" | xmllint --xpath \
                    "boolean(//*/*[@icid and @icid!=\"${NAME}\" and @fail_count and @fail_count!=\"0\"])" -`
                fi

                if [ "${T1}" != "true" ] || [ "${T2}" != "false" ] ; then
                    echo "NOK: ${FILE}" >&9
                    RCODE=1
                    continue
                fi
                echo "OK: ${FILE}" >&9
            done
        popd >/dev/null
    done
popd >/dev/null

exit ${RCODE}
