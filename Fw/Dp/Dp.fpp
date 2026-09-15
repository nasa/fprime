module Fw {

  # ----------------------------------------------------------------------
  # Types
  # ----------------------------------------------------------------------

  enum DpState: U8 {
    @ The untransmitted state
    UNTRANSMITTED
    @ The partially transmitted state
    @ A data product is in this state from the start of transmission
    @ until transmission is complete.
    PARTIAL
    @ The transmitted state
    TRANSMITTED
  } default UNTRANSMITTED

  # ----------------------------------------------------------------------
  # Ports
  # ----------------------------------------------------------------------

  @ Port for synchronously getting a data product buffer
  @ Returns the status
  @
  @ On return, buffer should be set to a valid buffer large enough
  @ to hold a data product packet with the requested data size (if
  @ status is SUCCESS) or an invalid buffer (if status is FAILURE).
  port DpGet(
      @ The container ID (input)
      $id: FwDpIdType
      @ The data size of the requested buffer (input)
      dataSize: FwSizeType
      @ The buffer (output)
      ref buffer: Fw.Buffer
  ) -> Fw.Success

  @ Port for sending a request for a data product buffer to
  @ back a data product container. The request is for a buffer
  @ large enough to hold a data product packet with the requested
  @ data size.
  port DpRequest(
      @ The container ID
      $id: FwDpIdType
      @ The data size of the requested buffer
      dataSize: FwSizeType
  )

  @ Port for receiving a response to a buffer request
  port DpResponse(
      @ The container ID
      $id: FwDpIdType
      @ The buffer
      buffer: Fw.Buffer
      @ The status
      status: Fw.Success
  )

  @ Port for sending a data product buffer
  port DpSend(
      @ The container ID
      $id: FwDpIdType
      @ The buffer
      buffer: Fw.Buffer
  )

}
