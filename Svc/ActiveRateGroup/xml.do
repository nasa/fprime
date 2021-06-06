#!/bin/sh

. ./defs.sh

xml_do "$@"
# Put the AcConstants variable back in for now
# This is a temporary solution so we can commit the XML
# It will go away when we integrate XML code gen into the F Prime build
awk '
/<port name="RateGroupMemberOut"/ {
  sub("max_number=\"[^\"]*\"", "max_number=\"$ActiveRateGroupOutputPorts\"", $0)
}
{ print }
' < $3/ActiveRateGroupComponentAi.xml > $3/ActiveRateGroupComponentAi.tmp.xml
mv $3/ActiveRateGroupComponentAi.tmp.xml $3/ActiveRateGroupComponentAi.xml
