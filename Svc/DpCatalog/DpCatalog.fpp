
module Svc {


  # This structure will use to store the state
  # in the component and to generate a catalog
  # data product

  @ Header validation error
  enum DpHdrField {
    DESCRIPTOR,
    ID,
    PRIORITY,
    CRC
  }

  @ Data structure representing a data product.
  struct DpRecord {
    $id: U32
    tSec: U32
    tSub: U32
    $priority: U32
    $size: U64
    state: Fw.DpState
  }


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
                            stat: I32 @< status
                          ) \
      severity warning high \
      id 0 \
      format "Unable to process directory {} status {}"

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

    @ Component not intialized error
    event ComponentNotIntialized \
      severity warning high \
      id 12 \
      format "DpCatalog not initialized!" \
      throttle 10

    @ Component didn't get memory error
    event ComponentNoMemory \
      severity warning high \
      id 13 \
      format "DpCatalog couldn't get memory" \
      throttle 10

    @ Catalog is full
    event CatalogFull (
                            dir: string size 80 @< last directory read 
                          ) \
      severity warning high \
      id 14 \
      format "DpCatalog full during directory {}" \
      throttle 10

    @ Error opening file
    event FileOpenError(
                            loc: string size 80 @< The directory
                            stat: I32 @< status
                          ) \
      severity warning high \
      id 15 \
      format "Unable to open DP file {} status {}" \
      throttle 10

    @ Error opening file
    event FileReadError(
                            file: string size 80 @< The file
                            stat: I32 @< status
                          ) \
      severity warning high \
      id 16 \
      format "Error reading DP file {} status {}" \
      throttle 10

    @ Error reading header data from DP file
    event FileHdrError(
                            file: string size 80 @< The file
                            field: DpHdrField, @< incorrect value
                            exp: U32 @< expected value
                            act: U32 @< expected value
                          ) \
      severity warning high \
      id 17 \
      format "Error reading DP {} header {} field. Expected: {} Actual: {}" \
      throttle 10

    @ Error deserializing header data
    event FileHdrDesError(
                            file: string size 80 @< The file
                            stat: I32
                          ) \
      severity warning high \
      id 18 \
      format "Error deserializing DP {} header stat: {}" \
      throttle 10

    @ Error inserting entry into list
    event DpInsertError(
                            dp: DpRecord @< The DP
                          ) \
      severity warning high \
      id 19 \
      format "Error deserializing DP {}" \
      throttle 10

    @ Error inserting entry into list
    event DpDuplicate(
                            dp: DpRecord @< The DP
                          ) \
      severity diagnostic \
      id 20 \
      format "DP {} already in catalog" \
      throttle 10

    @ Error inserting entry into list
    event DpCatalogFull(
                            dp: DpRecord @< The DP
                          ) \
      severity warning high \
      id 21 \
      format "Catalog full trying to insert DP {}" \
      throttle 10

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    @ Number of data products in catalog
    telemetry CatlogDps: U32 id 0 

    @ Number of data products sent
    telemetry DpsSent: U32 id 1



  }

}
