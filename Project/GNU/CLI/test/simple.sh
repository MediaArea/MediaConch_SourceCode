#!/bin/sh

PATH_SCRIPT=$(dirname "$0")
. "$PATH_SCRIPT/utils.sh"

OUTPUT="`./mediaconch -v`"
cmd_is_ok

OUTPUT="`./mediaconch --version`"
cmd_is_ok

OUTPUT="`./mediaconch --help`"
cmd_is_ok

OUTPUT="`./mediaconch -h`"
cmd_is_ok

OUTPUT="`./mediaconch`"
cmd_is_ko
