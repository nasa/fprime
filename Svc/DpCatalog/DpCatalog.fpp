
module Svc {

  @ A component for managing downling of data products
  active component DpCatalog {

    # Component specific ports

    @ Ping input port
    async input port pingIn: Svc.Ping

    @ Ping output port
    output port pingOut: Svc.Ping

    # ----------------------------------------------------------------------
    # F Prime infrastructure ports
    # ----------------------------------------------------------------------

    @ Command receive port
    command recv port CmdDisp

    @ Command registration port
    command reg port CmdReg

    @ Command response port
    command resp port CmdStatus

    @ Event port
    event port Log

    @ Text event port
    text event port LogText

    @ Time get port
    time get port Time

    @ Telemetry port
    telemetry port Tlm

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------

    @ Build catalog from data product directory
    async command BUILD_CATALOG \
      opcode 0

    @ Start transmitting catalog
    async command START_XMIT_CATALOG \
      opcode 1

    @ Stop transmitting catalog
    async command STOP_XMIT_CATALOG \
      opcode 2

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    @ Error opening directory
    event DirectoryOpenError(
                            loc: string size 80 @< The directory 
                          ) \
      severity warning high \
      id 0 \
      format "Unable to process directory {}"

    @ Processing directory
    event ProcessingDirectory (
                            loc: string size 80 @< The directory 
                          ) \
      severity activity low \
      id 1 \
      format "Processing directory {}"

    @ Processing complete
    event ProcessingComplete (
                            loc: string size 80 @< The directory 
                            total: U32 @< total data products
                            pending: U32 @< pending data products
                            pending_bytes: U32 @< pending data product volume
                          ) \
      severity activity high \
      id 2 \
      format "Completed processing directory {}. Total products: {} Pending products: {} Pending bytes: {}"

    @ Catalog transmission started
    event CatalogXmitStarted \
      severity activity high \
      id 5 \
      format "Catalog transmission started"

    @ Catalog transmission stopped
    event CatalogXmitStopped (
                            bytes: U32 @< data transmitted
                          ) \
      severity activity high \
      id 6 \
      format "Catalog transmission stopped. {} bytes transmitted."

    @ Catalog transmission completed
    event CatalogXmitCompleted (
                            bytes: U32 @< data transmitted
                          ) \
      severity activity high \
      id 7 \
      format "Catalog transmission completed.  {} bytes transmitted."

    @ Sending product
    event SendingProduct (
                            file: string size 80 @< The file 
                            bytes: U32 @< file size
                            prio: U32 @< DP priority
                          ) \
      severity activity low \
      id 10 \
      format "Sending product {} of size {} priority {}"

    @ Sending product
    event ProductComplete (
                            file: string size 80 @< The file 
                          ) \
      severity activity low \
      id 11 \
      format "Product {} complete"

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    @ Number of data products in catalog
    telemetry CatlogDps: U32 id 0 

    @ Number of data products sent
    telemetry DpsSent: U32 id 1



  }

}
