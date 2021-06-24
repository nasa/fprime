module Svc {

  @ Request that FileDownlink downlink a file.
  port SendFileRequest(
                        sourceFileName: string size 100 @< Path of file to downlink
                        destFileName: string size 100 @< Path to store downlinked file at
                        offset: U32 @< Amount of data in bytes to downlink from file. 0 to read until end of file
                        length: U32 @< Amount of data in bytes to downlink from file. 0 to read until end of file
                      ) -> Svc.SendFileResponse

}
