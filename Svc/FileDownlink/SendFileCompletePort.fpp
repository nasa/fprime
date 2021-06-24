module Svc {

  @ FileDownlink response to send file request
  port SendFileComplete(
                         $resp: Svc.SendFileResponse
                       )

}
