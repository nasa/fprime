#!/bin/sh

# ----------------------------------------------------------------------
# defst.sh
# Root definitions for Shell autocoders
# ----------------------------------------------------------------------

# Import source scripts
files=`ls $SHELL_AUTOCODER_DIR/src/*.sh`
if test -d src
then
  files="$files `find src -name '*.sh'`"
fi

for file in $files
do
  . $file
done
