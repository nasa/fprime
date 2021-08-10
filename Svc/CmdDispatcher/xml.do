#!/bin/sh

. ./defs.sh

xml_do "$@"
# Put the AcConstants variable back in for now
awk '
/<port name="compCmdSend"/ {
  sub("max_number=\"[^\"]*\"", "max_number=\"$CmdDispatcherComponentCommandPorts\"", $0)
}
/<port name="compCmdReg"/ {
  sub("max_number=\"[^\"]*\"", "max_number=\"$CmdDispatcherComponentCommandPorts\"", $0)
}
/<port name="seqCmdStatus"/ {
  sub("max_number=\"[^\"]*\"", "max_number=\"$CmdDispatcherSequencePorts\"", $0)
}
/<port name="seqCmdBuff"/ {
  sub("max_number=\"[^\"]*\"", "max_number=\"$CmdDispatcherSequencePorts\"", $0)
}
{ print }
' < $3/CommandDispatcherComponentAi.xml > $3/CommandDispatcherComponentAi.tmp.xml
mv $3/CommandDispatcherComponentAi.tmp.xml $3/CommandDispatcherComponentAi.xml
