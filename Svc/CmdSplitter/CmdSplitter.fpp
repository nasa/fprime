
module Svc {

  @ A component for splitting incoming commands to local or remote
  passive component CmdSplitter {

    # ----------------------------------------------------------------------
    # Upstream connections: uplink commanding and command sequencers
    # ----------------------------------------------------------------------

    @ Input port for local or remote commands
    sync input port CmdBuff: [CmdSplitterPorts] Fw.Com

    @ Output port for forwarding the Command status
    output port forwardSeqCmdStatus: [CmdSplitterPorts] Fw.CmdResponse

    # ----------------------------------------------------------------------
    # Downstream connections: local and remote command sequencers
    # ----------------------------------------------------------------------

    @ Input port for receiving the command status
    sync input port seqCmdStatus: [CmdSplitterPorts] Fw.CmdResponse

    @ Output port for local commands
    output port LocalCmd: [CmdSplitterPorts] Fw.Com

    @ Output port for remote commands
    output port RemoteCmd: [CmdSplitterPorts] Fw.Com

  }
}