#!/bin/sh

. ./defs.sh

xml_do "$@"
# Put the AcConstants variable back in for now
awk '
/<port name="PingSend"/ {
  sub("max_number=\"[^\"]*\"", "max_number=\"$HealthPingPorts\"", $0)
}
/<port name="PingReturn"/ {
  sub("max_number=\"[^\"]*\"", "max_number=\"$HealthPingPorts\"", $0)
}
{ print }
' < $3/HealthComponentAi.xml > $3/HealthComponentAi.tmp.xml
mv $3/HealthComponentAi.tmp.xml $3/HealthComponentAi.xml
