module Svc {

  enum SendFileStatus {
    OK = 0
    ERROR = 1
    INVALID = 2
    BUSY = 3
  }

  struct SendFileResponse {
    status: Svc.SendFileStatus
    context: U32
  }

}
