#!/bin/sh

. ./defs.sh

xml_do "$@"
# Put the AcConstants variable back in for now
awk '
/<port name="bufferAllocate"/ {
  sub("max_number=\"[^\"]*\"", "max_number=\"$StaticMemoryAllocations\"", $0)
}
/<port name="bufferDeallocate"/ {
  sub("max_number=\"[^\"]*\"", "max_number=\"$StaticMemoryAllocations\"", $0)
}
{ print }
' < $3/StaticMemoryComponentAi.xml > $3/StaticMemoryComponentAi.tmp.xml
mv $3/StaticMemoryComponentAi.tmp.xml $3/StaticMemoryComponentAi.xml
