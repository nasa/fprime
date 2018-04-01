#!/bin/sh

# ----------------------------------------------------------------------
# defs.sh
# ----------------------------------------------------------------------

export DIR=`pwd`/../..
export ISF=$DIR/../..
. $ISF/defs-root.sh

export NAMESPACE=Svc
dir=`echo $DIR | sed 's;/test/ut/../..;;'`
export COMPONENT=`basename $dir`
