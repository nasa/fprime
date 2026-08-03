module Svc {

  @ A component for managing files
  active component FileManager {

    # ----------------------------------------------------------------------
    # General Ports
    # ----------------------------------------------------------------------

    @ Ping input port
    async input port pingIn: Svc.Ping

    @ Scheduler input port for rate group operations
    sync input port schedIn: Sched

    @ Internal port for delegating schedIn calls in a more controlled fashion
    internal port run drop

    @ Ping output port
    output port pingOut: Svc.Ping

    # ----------------------------------------------------------------------
    # Data products
    # ----------------------------------------------------------------------

    @ Data product ports (synchronous get/send)
    import Fw.DataProductSync

    @ Metadata for one chunk of a file data product
    struct FileChunkHeader {
      fileName: string size FileNameStringSize @< The name of the source file
      offset: U64 @< The offset of this chunk within the source file
      dataSize: U32 @< The number of data bytes in this chunk
    }

    @ Chunk metadata record; each instance is followed by a FileChunkDataRecord
    product record FileChunkHeaderRecord: FileChunkHeader id 0

    @ Chunk data record; carries the actual file bytes for the preceding header
    product record FileChunkDataRecord: U8 array id 1

    @ Container for file data products
    product container FileDpContainer id 0 default priority 10

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Command receive port
    command recv port cmdIn

    @ Command registration port
    command reg port cmdRegOut

    @ Command response port
    command resp port cmdResponseOut

    @ Event port
    event port eventOut

    @ Text event port
    text event port LogText

    @ Time get port
    time get port timeCaller

    @ Telemetry port
    telemetry port tlmOut

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------

    include "Commands.fppi"

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    include "Telemetry.fppi"

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    include "Events.fppi"

  }
}
