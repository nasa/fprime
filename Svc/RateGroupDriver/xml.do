#!/bin/sh

. ./defs.sh

xml_do "$@"
# Put the AcConstants variable back in for now
awk '
/<port name="CycleOut"/ {
  sub("max_number=\"[^\"]*\"", "max_number=\"$RateGroupDriverRateGroupPorts\"", $0)
}
{ print }
' < $3/RateGroupDriverComponentAi.xml > $3/RateGroupDriverComponentAi.tmp.xml
mv $3/RateGroupDriverComponentAi.tmp.xml $3/RateGroupDriverComponentAi.xml
