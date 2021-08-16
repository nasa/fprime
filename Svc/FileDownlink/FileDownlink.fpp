module Svc {

  @ Send file status enum
  enum SendFileStatus {
    OK
    ERROR
    INVALID
    BUSY
  }

  @ Send file response struct
  struct SendFileResponse {
    status: Svc.SendFileStatus
    context: U32
  }

  @ FileDownlink response to send file request
  port SendFileComplete(
                         $resp: Svc.SendFileResponse
                       )

  @ Request that FileDownlink downlink a file
  port SendFileRequest(
                        sourceFileName: string size 100 @< Path of file to downlink
                        destFileName: string size 100 @< Path to store downlinked file at
                        offset: U32 @< Amount of data in bytes to downlink from file. 0 to read until end of file
                        length: U32 @< Amount of data in bytes to downlink from file. 0 to read until end of file
                      ) -> Svc.SendFileResponse


  @ A component for downlinking files
  active component FileDownlink {

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    @ Run input port
    async input port Run: Svc.Sched

    @ Mutexed Sendfile input port
    guarded input port SendFile: Svc.SendFileRequest

    @ File complete output port
    output port FileComplete: [FileDownCompletePorts] Svc.SendFileComplete

    @ Buffer return input port
    async input port bufferReturn: Fw.BufferSend

    @ Buffer send output port
    output port bufferSendOut: Fw.BufferSend

    @ Ping input port
    async input port pingIn: Svc.Ping

    @ Ping output port
    output port pingOut: Svc.Ping

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Time get port
    time get port timeCaller

    @ Command registration port
    command reg port cmdRegOut

    @ Command receive port
    command recv port cmdIn

    @ Command response port
    command resp port cmdResponseOut

    @ Event port
    event port eventOut

    @ Text event port
    text event port textEventOut

    @ Telemetry port
    telemetry port tlmOut

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
