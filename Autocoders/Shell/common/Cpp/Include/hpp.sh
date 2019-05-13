#!/bin/sh

# ----------------------------------------------------------------------
# hpp.sh
# ----------------------------------------------------------------------
# Incorporate optional include directives
# ----------------------------------------------------------------------

name=$1

if test -f Include/$name.hpp
then
  cat Include/$name.hpp
else
  echo '// To add include directives here, put them in a file called Include/'$name'.hpp'
fi
