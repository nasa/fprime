#!/bin/sh

. ./defs.sh

xml_do "$@"
# Put the AcConstants variable back in for now
awk '
/<port name="portIn"/ {
  sub("max_number=\"[^\"]*\"", "max_number=\"$GenericHubInputPorts\"", $0)
}
/<port name="portOut"/ {
  sub("max_number=\"[^\"]*\"", "max_number=\"$GenericHubOutputPorts\"", $0)
}
/<port name="buffersIn"/ {
  sub("max_number=\"[^\"]*\"", "max_number=\"$GenericHubInputBuffers\"", $0)
}
/<port name="buffersOut"/ {
  sub("max_number=\"[^\"]*\"", "max_number=\"$GenericHubOutputBuffers\"", $0)
}
{ print }
' < $3/GenericHubComponentAi.xml > $3/GenericHubComponentAi.tmp.xml
mv $3/GenericHubComponentAi.tmp.xml $3/GenericHubComponentAi.xml
