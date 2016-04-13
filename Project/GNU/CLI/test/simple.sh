#!/bin/sh

PATH_SCRIPT=$(dirname "$0")
. "$PATH_SCRIPT/utils.sh"

cmd_is_ok "./mediaconch -v"
cmd_is_ok "./mediaconch -h"
cmd_is_ko "./mediaconch"
