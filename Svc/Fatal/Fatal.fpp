module Svc {

  @ Fatal announce port with FATAL Event ID
  port FatalEvent(
                   Id: FwEventIdType @< The ID of the FATAL event
                 )

}
