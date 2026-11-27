module Svc {

  @ Fatal announce port with FATAL Event ID
  port FatalEvent(
                   Id: FwEventIdType @< The ID of the FATAL event
                   args: Fw.LogBuffer @< args of the event (i.e line number, file)
                 )

}
