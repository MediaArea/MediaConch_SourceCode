#!/bin/sh

UTILS_PATH=$(dirname "$0")
MC_ROOT_PATH=$UTILS_PATH/../../../../
RESOURCE_PATH=$MC_ROOT_PATH/Source/Resource
DATA=

if [ ! -f "$UTILS_PATH/mediaconch.xsd" ]
then
    wget https://github.com/MediaArea/MediaAreaXml/raw/master/mediaconch.xsd -O "$UTILS_PATH/mediaconch.xsd"
fi

if [ ! -f "$UTILS_PATH/mediainfo.xsd" ]
then
    wget https://github.com/MediaArea/MediaAreaXml/raw/master/mediainfo.xsd -O "$UTILS_PATH/mediainfo.xsd"
fi

if [ ! -f "$UTILS_PATH/mediatrace.xsd" ]
then
    wget https://github.com/MediaArea/MediaAreaXml/raw/master/mediatrace.xsd -O "$UTILS_PATH/mediatrace.xsd"
fi

if [ ! -f "$UTILS_PATH/micromediatrace.xsd" ]
then
    wget https://github.com/MediaArea/MediaAreaXml/raw/master/micromediatrace.xsd -O "$UTILS_PATH/micromediatrace.xsd"
fi

if [ ! -f "$UTILS_PATH/mediaarea.xsd" ]
then
    wget https://github.com/MediaArea/MediaAreaXml/raw/master/mediaarea.xsd -O "$UTILS_PATH/mediaarea.xsd"
fi

cmd_is_ok()
{
    if test $? -ne 0
    then
        exit 1;
    fi
}

cmd_is_ko()
{
    if test $? -eq 0
    then
        exit 1;
    fi
}

xml_is_correct()
{
    $(echo "$DATA" | xmllint --noout -)
    if test $? -ne 0
    then
        exit 1;
    fi
}

xml_is_invalid()
{
    $(echo "$DATA" | xmllint --noout -)
    if test $? -eq 0
    then
        exit 1;
    fi
}

output_has_mc_xml()
{
    if [ $(echo "$DATA" | grep -oc "<MediaConch") -eq 0 ]
    then
        exit 1;
    fi
}

output_has_not_mc_xml()
{
    if [ $(echo "$DATA" | grep -oc "<MediaConch") -ne 0 ]
    then
        exit 1;
    fi
}

output_has_mt_xml()
{
    if [ $(echo "$DATA" | grep -oc "<MediaTrace") -eq 0 ]
    then
        exit 1;
    fi
}

output_has_not_mt_xml()
{
    if [ $(echo "$DATA" | grep -oc "<MediaTrace") -ne 0 ]
    then
        exit 1;
    fi
}

output_has_mi_xml()
{
    if [ $(echo "$DATA" | grep -oc "<MediaInfo") -eq 0 ]
    then
        exit 1;
    fi
}

output_has_not_mi_xml()
{
    if [ $(echo "$DATA" | grep -oc "<MediaInfo") -ne 0 ]
    then
        exit 1;
    fi
}

output_xml_is_a_valid_mc()
{
    $(echo "$DATA" | xmllint --noout --schema "$UTILS_PATH/mediaconch.xsd" -)
    if test $? -ne 0
    then
        exit 1;
    fi
}

output_xml_is_a_valid_mi()
{
    $(echo "$DATA" | xmllint --noout --schema "$UTILS_PATH/mediainfo.xsd" -)
    if test $? -ne 0
    then
        exit 1;
    fi
}

output_xml_is_a_valid_mt()
{
    $(echo "$DATA" | xmllint --noout --schema "$UTILS_PATH/mediatrace.xsd" -)
    if test $? -ne 0
    then
        exit 1;
    fi
}

output_xml_is_a_valid_ma()
{
    $(echo "$DATA" | xmllint --noout --schema "$UTILS_PATH/mediaarea.xsd" -)
    if test $? -ne 0
    then
        exit 1;
    fi
}
