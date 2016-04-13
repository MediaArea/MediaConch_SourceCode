#!/bin/sh

PATH_SCRIPT=$(dirname "$0")
. "$PATH_SCRIPT/utils.sh"

FILES=`ls $PATH_SCRIPT/SampleFiles/PolicyTestFiles/FFV1/`

for i in $FILES
do
    cmd_is_ok "./mediaconch --implementationschema=$RESOURCE_PATH/schema/MatroskaSchema.xml $i"
done
