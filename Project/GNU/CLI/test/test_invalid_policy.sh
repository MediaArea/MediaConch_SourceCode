#!/bin/sh

PATH_SCRIPT=$(dirname "$0")
FILES_DIRECTORY="$PATH_SCRIPT/SampleFiles/ImplementationTestFiles/Matroska"
MC="${PWD}/mediaconch"
MEDIA_FILE="$FILES_DIRECTORY/tiny.mkv"
SCHEMA_FILE="$PATH_SCRIPT/../../../../Source/Resource/schema/MatroskaSchema.xml"

TMP_DIR=$(mktemp -d "${TMPDIR:-/tmp}/mediaconch-policy.XXXXXX") || exit 1
trap 'rm -rf "$TMP_DIR"' 0

RCODE=0

check_invalid_policy()
{
    POLICY_FILE="$1"
    EXPECTED_ERROR="$2"
    shift 2

    "${MC}" "--ImplementationSchema=$SCHEMA_FILE" -p "$POLICY_FILE" "$@" "$MEDIA_FILE" >"$TMP_DIR/stdout" 2>"$TMP_DIR/stderr"
    STATUS=$?

    if [ "$STATUS" -eq 0 ]; then
        echo "NOK: $POLICY_FILE, exited with status 0" >&9
        RCODE=1
    elif ! grep -F -q "$EXPECTED_ERROR" "$TMP_DIR/stderr"; then
        echo "NOK: $POLICY_FILE, expected error not found" >&9
        RCODE=1
    elif [ -s "$TMP_DIR/stdout" ]; then
        echo "NOK: $POLICY_FILE, unexpected standard output" >&9
        RCODE=1
    else
        echo "OK: $POLICY_FILE" >&9
    fi
}

printf '%s\n' \
    '<?xml version="1.0"?>' \
    '<!-- check -- invalid -->' \
    '<policy type="and" name="MXF">' \
    '  <rule name="Container is MXF" value="Format" tracktype="General" occurrence="*" operator="=">MXF</rule>' \
    '</policy>' >"$TMP_DIR/bad_comment.xml"

printf '%s\n' \
    '<?xml version="1.0"?>' \
    '<foo/>' >"$TMP_DIR/not_policy.xml"

: >"$TMP_DIR/empty.xml"

check_invalid_policy "$TMP_DIR/bad_comment.xml" "The schema given cannot be parsed"
check_invalid_policy "$TMP_DIR/bad_comment.xml" "The schema given cannot be parsed" -fx
check_invalid_policy "$TMP_DIR/not_policy.xml" "The XML policy tag accepted are 'policy' or 'rule'"
check_invalid_policy "$TMP_DIR/not_policy.xml" "The XML policy tag accepted are 'policy' or 'rule'" -fj
check_invalid_policy "$TMP_DIR/empty.xml" "The schema does not exist"

check_invalid_policy "$TMP_DIR/bad_comment.xml" "The schema given cannot be parsed" -ox "$TMP_DIR/report.xml"
if [ -e "$TMP_DIR/report.xml" ]; then
    echo "NOK: report file created for an invalid policy" >&9
    RCODE=1
fi

exit "$RCODE"
