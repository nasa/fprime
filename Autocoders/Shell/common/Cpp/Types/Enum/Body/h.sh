#!/bin/sh

# ----------------------------------------------------------------------
# h.sh
# ----------------------------------------------------------------------

namespace=$1
name=$2

echo 'typedef enum {'
awk 'BEGIN { FS="\n"; RS="" }
NR > 1 {
  printf("  %s", $1)
  if ($2 != "" && $2 != "~")
    printf(" = %s", $2)
  printf(",");
  if ($3 != "" && $3 != "~")
    printf(" //!< %s", $3)
  printf("\n")
}' < ${name}EnumAi.txt
if test -n "$namespace"
then
  echo '} '$namespace'_'$name';'
else
  echo '} '$name';'
fi
