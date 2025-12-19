
module Svc {


  # This structure will use to store the state
  # in the component and to generate a catalog
  # data product

  @ Header validation error
  enum DpHdrField : U8 {
    DESCRIPTOR,
    ID,
    PRIORITY,
    CRC
  }

  @ Data structure representing a data product.
  struct DpRecord {
    $id: FwDpIdType # The ID of the data product
    tSec: U32 # Generation time in seconds
    tSub: U32 # Generation time in subseconds
    $priority: U32 # Priority of the data product
    $size: U64 # Overall size of the data product
    blocks: U32 # Number of blocks transmitted
    $state: Fw.DpState # Transmission state of the data product
  }


  @ A component for managing downlink of data products
  active component DpCatalog {

    # Component specific ports

    @ Ping input port
    async input port pingIn: Svc.Ping

    @ Ping output port
    output port pingOut: Svc.Ping

    @ File Downlink send port
    output port fileOut: Svc.SendFileRequest

    @ File Downlink send complete port
    async input port fileDone: SendFileComplete

    @ DP Writer Add File to Cat
    async input port addToCat: DpWritten

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

    @ Build catalog from data product directory. Will block until complete
    async command BUILD_CATALOG \
      opcode 0

    @ Start transmitting catalog
    async command START_XMIT_CATALOG (
                                    wait: Fw.Wait, @< have START_XMIT command complete wait for catalog to complete transmitting
                                    remainActive: bool @< should the catalog resume transmission when Dps are added at runtime
                                  ) \
      opcode 1

    @ Stop transmitting catalog
    async command STOP_XMIT_CATALOG \
      opcode 2

    @ clear existing catalog
    async command CLEAR_CATALOG \
      opcode 3

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    @ Error opening directory
    event DirectoryOpenError(
                            loc: string size FileNameStringSize @< The directory
                            stat: I32 @< status
                          ) \
      severity warning high \
      id 0 \
      format "Unable to process directory {} status {}"

    @ Processing directory
    event ProcessingDirectory (
                            directory: string size FileNameStringSize @< The directory
                          ) \
      severity activity low \
      id 1 \
      format "Processing directory {}"

    @ Processing directory
    event ProcessingFile (
                            file: string size FileNameStringSize @< The file
                          ) \
      severity activity low \
      id 2 \
      format "Processing file {}"


    @ Directory Processing complete
    event ProcessingDirectoryComplete (
                            loc: string size FileNameStringSize @< The directory
                            total: U32 @< total data products
                            pending: U32 @< pending data products
                            pending_bytes: U64 @< pending data product volume
                          ) \
      severity activity high \
      id 3 \
      format "Completed processing directory {}. Total products: {} Pending products: {} Pending bytes: {}"

    @ Catalog processing complete
    event CatalogBuildComplete \
      severity activity high \
      id 4 \
      format "Catalog build complete"

    @ Error opening directory
    event DirectoryNotManaged(
                            file: string size FileNameStringSize @< The file
                          ) \
      severity warning high \
      id 5 \
      format "Unable to add file {}; directory not managed"

    @ Catalog transmission started
    event CatalogXmitStarted \
      severity activity high \
      id 10 \
      format "Catalog transmission started"

    @ Catalog transmission stopped
    event CatalogXmitStopped (
                            bytes: U64 @< data transmitted
                          ) \
      severity activity high \
      id 11 \
      format "Catalog transmission stopped. {} bytes transmitted."

    @ Catalog transmission completed
    event CatalogXmitCompleted (
                            bytes: U64 @< data transmitted
                          ) \
      severity activity high \
      id 12 \
      format "Catalog transmission completed.  {} bytes transmitted."

    @ Sending product
    event SendingProduct (
                            file: string size FileNameStringSize @< The file
                            bytes: U32 @< file size
                            prio: U32 @< DP priority
                          ) \
      severity activity low \
      id 13 \
      format "Sending product {} of size {} priority {}"

    @ Product send complete
    event ProductComplete (
                            file: string size FileNameStringSize @< The file
                            pending: U32 @< pending data products
                            pending_bytes: U64 @< pending data product volume
                          ) \
      severity activity low \
      id 14 \
      format "Product {} complete. Pending products: {} Pending bytes: {}"

    @ Component not initialized error
    event ComponentNotInitialized \
      severity warning high \
      id 20 \
      format "DpCatalog not initialized!" \
      throttle 10

    @ Component didn't get memory error
    event ComponentNoMemory \
      severity warning high \
      id 21 \
      format "DpCatalog couldn't get memory" \
      throttle 10

    @ Catalog is full
    event CatalogFull (
                            dir: string size FileNameStringSize @< last directory read
                          ) \
      severity warning high \
      id 22 \
      format "DpCatalog full during directory {}" \
      throttle 10

    @ Error opening file
    event FileOpenError(
                            loc: string size FileNameStringSize @< The directory
                            stat: I32 @< status
                          ) \
      severity warning high \
      id 23 \
      format "Unable to open DP file {} status {}" \
      throttle 10

    @ Error opening file
    event FileReadError(
                            file: string size FileNameStringSize @< The file
                            stat: I32 @< status
                          ) \
      severity warning high \
      id 24 \
      format "Error reading DP file {} status {}" \
      throttle 10

    @ Error reading header data from DP file
    event FileHdrError(
                            file: string size FileNameStringSize @< The file
                            field: DpHdrField, @< incorrect value
                            exp: U32 @< expected value
                            act: U32 @< expected value
                          ) \
      severity warning high \
      id 25 \
      format "Error reading DP {} header {} field. Expected: {} Actual: {}" \
      throttle 10

    @ Error deserializing header data
    event FileHdrDesError(
                            file: string size FileNameStringSize @< The file
                            stat: I32
                          ) \
      severity warning high \
      id 26 \
      format "Error deserializing DP {} header stat: {}" \
      throttle 10

    @ Error inserting entry into list
    event DpInsertError(
                            dp: DpRecord @< The DP
                          ) \
      severity warning high \
      id 27 \
      format "Error deserializing DP {}" \
      throttle 10

    @ Error inserting entry into list
    event DpDuplicate(
                            dp: DpRecord @< The DP
                          ) \
      severity diagnostic \
      id 28 \
      format "DP {} already in catalog" \
      throttle 10

    @ Error inserting entry into list
    event DpCatalogFull(
                            dp: DpRecord @< The DP
                          ) \
      severity warning high \
      id 29 \
      format "Catalog full trying to insert DP {}" \
      throttle 10

    @ Tried to build catalog while downlink process active
    event DpXmitInProgress \
      severity warning low \
      id 30 \
      format "Cannot build new catalog while DPs are being transmitted" \
      throttle 10

    @ Error getting file size
    event FileSizeError(
                            file: string size FileNameStringSize @< The file
                            stat: I32
                          ) \
      severity warning high \
      id 31 \
      format "Error getting file {} size. stat: {}" \
      throttle 10

    event NoDpMemory \
      severity warning high \
      id 32 \
      format "No memory for DP"

    event XmitNotActive \
      severity warning low \
      id 34 \
      format "DpCatalog transmit not active"

    event StateFileOpenError(
                            file: string size FileNameStringSize @< The file
                            stat: I32
                          ) \
      severity warning high \
      id 35 \
      format "Error opening state file {}, stat: {}"

    event StateFileReadError(
                            file: string size FileNameStringSize @< The file
                            stat: I32
                            offset: I32
                          ) \
      severity warning high \
      id 36 \
      format "Error reading state file {}, stat {}, offset: {}"

    event StateFileTruncated(
                            file: string size FileNameStringSize @< The file
                            offset: I32
                            $size: I32
                          ) \
      severity warning high \
      id 37 \
      format "Truncated state file {} size. offset: {} size: {}"

    event NoStateFileSpecified \
      severity warning low \
      id 38 \
      format "No specified state file"

    event StateFileWriteError(
                            file: string size FileNameStringSize @< The file
                            stat: I32
                          ) \
      severity warning high \
      id 39 \
      format "Error writing state file {}, stat {}"

    event NoStateFile(
                            file: string size FileNameStringSize @< The file
                          ) \
      severity warning low \
      id 40 \
      format "State file {} doesn't exist"

    event DpFileXmitError(
                            file: string size FileNameStringSize @< The file
                            stat: Svc.SendFileStatus
                          ) \
      severity warning high \
      id 41 \
      format "Error transmitting DP file {}, stat {}. Halting xmit." \
      throttle 10

    event DpFileSendError(
                            file: string size FileNameStringSize @< The file
                            stat: Svc.SendFileStatus
                          ) \
      severity warning high \
      id 42 \
      format "Error sending DP file {}, stat {}. Halting xmit." \
      throttle 10

    @ File added
    event DpFileAdded(
                          file: string size FileNameStringSize @< The file
      ) \
      severity activity high \
      id 43 \
      format "DP file {} added at runtime"

    event NotLoaded(
                            file: string size FileNameStringSize @< The file
    ) \
      severity activity high \
      id 44 \
      format "Not adding file {} now; Catalog not yet loaded"

    @ Skipped a transmitted file
    event DpFileSkipped(
                          file: string size FileNameStringSize @< The file
      ) \
      severity activity high \
      id 45 \
      format "Already Transmitted DP file {} not added"

    event XmitUnbuiltCatalog() \
      severity warning high \
      id 46 \
      format "Cannot Transmit a Catalog before Building"

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    @ Number of data products in catalog
    telemetry CatalogDps: U32 id 0

    @ Number of data products sent
    telemetry DpsSent: U32 id 1



  }

}
