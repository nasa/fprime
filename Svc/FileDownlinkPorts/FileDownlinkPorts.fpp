module Svc {

  @ Send file status enum
  enum SendFileStatus {
    STATUS_OK
    STATUS_ERROR
    STATUS_INVALID
    STATUS_BUSY
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

}
