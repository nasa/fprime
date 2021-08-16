#!/bin/sh

. ./defs.sh

xml_do "$@"
# Put the AcConstants variable back in for now
awk '
/<port name="FileComplete"/ {
  sub("max_number=\"[^\"]*\"", "max_number=\"$FileDownCompletePorts\"", $0)
}
{ print }
' < $3/FileDownlinkComponentAi.xml > $3/FileDownlinkComponentAi.tmp.xml
mv $3/FileDownlinkComponentAi.tmp.xml $3/FileDownlinkComponentAi.xml
