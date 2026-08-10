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

    @ Where a data product generation failure occurred
    enum GenerateDpStage {
      @ Opening the source file
      OPEN = 0
      @ Querying the size of the source file
      SIZE = 1
      @ Seeking to the requested begin offset
      SEEK = 2
      @ Reading a chunk from the source file
      READ = 3
      @ Serializing a chunk into the container
      SERIALIZE = 4
      @ A request arrived while another was in progress
      BUSY = 5
    }

    @ How the chunks of a file are emitted
    enum GenerateDpMode {
      @ Emit one chunk per rate group tick, spreading the work over time
      PACED = 0
      @ Emit all chunks in the command handler, completing immediately
      IMMEDIATE = 1
    }

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
    product container FileDpContainer id 0 default priority FileManagerCfg.DEFAULT_DP_PRIORITY

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
