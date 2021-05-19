#!/bin/sh

# ----------------------------------------------------------------------
# enum.sh
# Generate an enum type
# ----------------------------------------------------------------------



name=$1

echo 'from models.serialize.IsfTypeExceptions import *
from models.serialize.type_base import *
from models.serialize.isf_enum_type import *
from models.serialize.isf_string_type import *
from models.serialize.isf_serializable_type import *

def '$name'():
  return IsfEnumType(
    "'$name'",
    {'
awk 'BEGIN {
  FS="\n"; RS=""
  value = 0
}
NR > 1 {
  if ($2 != "" && $2 != "~")
    value = $2
  printf("      \"%s\":%s,\n", $1, value)
  ++value
}' < ${name}EnumAi.txt
echo '    }
  )'
