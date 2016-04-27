#!/bin/sh

PATH_SCRIPT=$(dirname "$0")
. "$PATH_SCRIPT/utils.sh"

FILES_DIRECTORY="$PATH_SCRIPT/SampleFiles/PolicyTestFiles/FFV1/"
FILES="`ls "$FILES_DIRECTORY"`"

for i in $FILES
do

    OUTPUT="`./mediaconch \"--implementationschema=$RESOURCE_PATH/schema/MatroskaSchema.xml\" \"$FILES_DIRECTORY/$i\"`"
    cmd_is_ok
    xml_is_invalid "$OUTPUT"
    output_has_not_mc_xml "$OUTPUT"
    output_has_not_mi_xml "$OUTPUT"
    output_has_not_mt_xml "$OUTPUT"

    OUTPUT="`./mediaconch -mc -fx \"--implementationschema=$RESOURCE_PATH/schema/MatroskaSchema.xml\" \"$FILES_DIRECTORY/$i\"`"
    cmd_is_ok
    xml_is_correct "$OUTPUT"
    output_has_mc_xml "$OUTPUT"
    output_has_not_mi_xml "$OUTPUT"
    output_has_not_mt_xml "$OUTPUT"

    OUTPUT="`./mediaconch -mi -fx \"--implementationschema=$RESOURCE_PATH/schema/MatroskaSchema.xml\" \"$FILES_DIRECTORY/$i\"`"
    cmd_is_ok
    xml_is_correct "$OUTPUT"
    output_has_not_mc_xml "$OUTPUT"
    output_has_mi_xml "$OUTPUT"
    output_has_not_mt_xml "$OUTPUT"

    OUTPUT="`./mediaconch -mt -fx \"--implementationschema=$RESOURCE_PATH/schema/MatroskaSchema.xml\" \"$FILES_DIRECTORY/$i\"`"
    cmd_is_ok
    xml_is_correct "$OUTPUT"
    output_has_not_mc_xml "$OUTPUT"
    output_has_not_mi_xml "$OUTPUT"
    output_has_mt_xml "$OUTPUT"

    OUTPUT="`./mediaconch -mt -mi -fx \"--implementationschema=$RESOURCE_PATH/schema/MatroskaSchema.xml\" \"$FILES_DIRECTORY/$i\"`"
    cmd_is_ok
    xml_is_correct "$OUTPUT"
    output_has_not_mc_xml "$OUTPUT"
    output_has_mi_xml "$OUTPUT"
    output_has_mt_xml "$OUTPUT"

    OUTPUT="`./mediaconch -mc -mi -fx \"--implementationschema=$RESOURCE_PATH/schema/MatroskaSchema.xml\" \"$FILES_DIRECTORY/$i\"`"
    cmd_is_ok
    xml_is_correct "$OUTPUT"
    output_has_mc_xml "$OUTPUT"
    output_has_mi_xml "$OUTPUT"
    output_has_not_mt_xml "$OUTPUT"

    OUTPUT="`./mediaconch -mc -mt -fx \"--implementationschema=$RESOURCE_PATH/schema/MatroskaSchema.xml\" \"$FILES_DIRECTORY/$i\"`"
    cmd_is_ok
    xml_is_correct "$OUTPUT"
    output_has_mc_xml "$OUTPUT"
    output_has_not_mi_xml "$OUTPUT"
    output_has_mt_xml "$OUTPUT"

    OUTPUT="`./mediaconch -mt -mi -mc -fx \"--implementationschema=$RESOURCE_PATH/schema/MatroskaSchema.xml\" \"$FILES_DIRECTORY/$i\"`"
    cmd_is_ok
    xml_is_correct "$OUTPUT"
    output_has_mc_xml "$OUTPUT"
    output_has_mi_xml "$OUTPUT"
    output_has_mt_xml "$OUTPUT"

done
