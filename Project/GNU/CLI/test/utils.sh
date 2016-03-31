#!/bin/sh

UTILS_PATH=$(dirname "$0")
MC_ROOT_PATH=$UTILS_PATH/../../../
RESOURCE_PATH=$MC_ROOT_PATH/Resource

cmd_is_ok()
{
    CMD=$1
    RESULT=`$CMD`

    if test $? -ne 0
    then
        exit 1;
    fi
}

cmd_is_ko()
{
    CMD=$1
    RESULT=`$CMD`

    if test $? -eq 0
    then
        exit 1;
    fi
}
