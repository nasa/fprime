#!/bin/sh -e

redo-ifchange $REF_ROOT/defs-root.sh

export REF_ROOT=`cd $REF_ROOT; echo $PWD`
export FPRIME_ROOT=`cd $REF_ROOT/..; echo $PWD`
export CLIENT_FPP_LOCS=$REF_ROOT/locs.fpp

. $FPRIME_ROOT/defs-root.sh
