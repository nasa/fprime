#!/bin/sh -e

export FPRIME_ROOT=`cd ../..; echo $PWD`

export FPP_FILES="
Cmd.fpp
"

. $FPRIME_ROOT/defs-root.sh
