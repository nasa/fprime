module Svc {

  @ Send a notification that a data product was written
  port DpWritten(
      fileName: string size FileNameStringSize @< The file name
      $priority: FwDpPriorityType @< The priority
      $size: FwSizeType @< The file size
  )

}
