#!/bin/sh

. ./defs.sh

xml_do "$@"
# Put the AcConstants variable back in for now
awk '
/<port name="portOut"/ {
  sub("max_number=\"[^\"]*\"", "max_number=\"$GenericRepeaterOutputPorts\"", $0)
}
{ print }
' < $3/GenericRepeaterComponentAi.xml > $3/GenericRepeaterComponentAi.tmp.xml
mv $3/GenericRepeaterComponentAi.tmp.xml $3/GenericRepeaterComponentAi.xml
