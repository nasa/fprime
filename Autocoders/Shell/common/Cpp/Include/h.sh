#!/bin/sh

# ----------------------------------------------------------------------
# h.sh
# ----------------------------------------------------------------------
# Incorporate optional include directives
# ----------------------------------------------------------------------

name=$1

if test -f Include/$name.h
then
  cat Include/$name.h
else
  echo '// To add include directives here, put them in a file called Include/'$name'.h'
fi
