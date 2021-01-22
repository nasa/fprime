#!/bin/sh -e

export FPRIME_ROOT=`cd ../..; echo $PWD`

export FPP_FILES="
Log.fpp
"

deps=`fpp-depend $FPRIME_ROOT/defs.fpp $FPP_FILES`
export DEPS=`echo $deps | sed 's/ /,/g'`
