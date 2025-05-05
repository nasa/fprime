module Svc {

  @ A component for running command sequences
  active component CmdSequencer {

    # ----------------------------------------------------------------------
    # Types
    # ----------------------------------------------------------------------

    @ The sequencer mode
    enum SeqMode {
      STEP = 0
      AUTO = 1
    }

    @ Sequencer blocking state
    enum BlockState {
        BLOCK = 0
        NO_BLOCK = 1
    }

    @ The stage of the file read operation
    enum FileReadStage {
      READ_HEADER
      READ_HEADER_SIZE
      DESER_SIZE
      DESER_NUM_RECORDS
      DESER_TIME_BASE
      DESER_TIME_CONTEXT
      READ_SEQ_CRC
      READ_SEQ_DATA
      READ_SEQ_DATA_SIZE
    }

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Command receive port
    command recv port cmdIn

    @ Command registration port
    command reg port cmdRegOut

    @ Command response port
    command resp port cmdResponseOut

    @ Event port
    event port logOut

    @ Telemetry port
    telemetry port tlmOut

    @ Text event port
    text event port LogText

    @ Time get port
    time get port timeCaller

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    @ Sequence cancel port
    async input port seqCancelIn: Svc.CmdSeqCancel

    @ Command response in port
    async input port cmdResponseIn: Fw.CmdResponse

    @ Ping in port
    async input port pingIn: Svc.Ping

    @ Ping out port
    output port pingOut: Svc.Ping

    @ Port for indicating sequence done
    output port seqDone: Fw.CmdResponse

    @ Port for requests to run sequences
    async input port seqRunIn: Svc.CmdSeqIn

    @ Port for sending sequence commands
    output port comCmdOut: Fw.Com

    @ Schedule in port
    async input port schedIn: Svc.Sched

    @ Notifies that a sequence has started running
    output port seqStartOut: Svc.CmdSeqIn

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------

    include "Commands.fppi"

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    include "Telemetry.fppi"

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    include "Events.fppi"

  }

}
