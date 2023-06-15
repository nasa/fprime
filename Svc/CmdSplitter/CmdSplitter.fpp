
module Svc {

  @ A component for splitting incoming commands to local or remote
  passive component CmdSplitter {

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    @ Input port for local or remote commands
    sync input port CmdBuff: Fw.Com

    @ Input port for receiving the command status
    sync input port seqCmdStatus: Fw.CmdResponse

    @ Output port for forwarding the Command status
    output port forwardSeqCmdStatus: Fw.CmdResponse

    @ Output port for local commands
    output port LocalCmd: Fw.Com

    @ Output port for remote commands
    output port RemoteCmd: Fw.Com

  }
}