#!/bin/sh

PATH_SCRIPT=$(dirname "$0")
. "$PATH_SCRIPT/utils.sh"

FILES_DIRECTORY="$PATH_SCRIPT/SampleFiles/ImplementationTestFiles/Matroska"

cp "$FILES_DIRECTORY/tiny.mkv" "ti&ny.mkv"
DATA="`./mediaconch -fx ti\&ny.mkv`"
cmd_is_ok
xml_is_correct

mv "ti&ny.mkv" "ti<ny.mkv"
DATA="`./mediaconch -fx 'ti<ny.mkv'`"
cmd_is_ok
xml_is_correct

mv "ti<ny.mkv" "ti>ny.mkv"
DATA="`./mediaconch -fx 'ti>ny.mkv'`"
cmd_is_ok
xml_is_correct

mv "ti>ny.mkv" "ti'ny.mkv"
DATA="`./mediaconch -fx ti\'ny.mkv`"
cmd_is_ok
xml_is_correct

mv "ti'ny.mkv" "ti\"ny.mkv"
DATA="`./mediaconch -fx ti\\\"ny.mkv`"
cmd_is_ok
xml_is_correct

rm "ti\"ny.mkv"
