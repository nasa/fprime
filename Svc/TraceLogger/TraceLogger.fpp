module Svc {

  @ Component to record Trace Data
  active component TraceLogger {

    # ----------------------------------------------------------------------
    # Types
    # ----------------------------------------------------------------------
    enum Enable {
      DISABLE = 0 @< Disabled state
      ENABLE = 1 @< Enabled state
    }
    
    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------
    @ Logging Port
    async input port TraceBufferLogger: Fw.Trace @< Input Trace port to write to file
   
    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------
    @ Enable or disable trace
    async command EnableTrace ( 
        $enable : Enable 
        )\
        opcode 0x00
    
    @ Dump Trace to a data product
    async command DumpTraceDp\ 
        opcode 0x01

    @Select which trace types to be logged
    async command FilterTrace ( 
        bitmask: U16 @< TraceTypes to filter on
        $enable : Enable @< enable or disable filtering
        )\
        opcode 0x02
    
    # ----------------------------------------------------------------------
    # Events 
    # ----------------------------------------------------------------------
    @ Trace logging status
    event TraceStatus( 
            $status: Enable @< Status of Trace
    ) \
        severity diagnostic \
        id 0x00 \
        format "Trace has been {}."

    @Trace File open error
    event TraceFileOpenError(
        $fileName: string @<print out file name
        )\
        severity warning low \ 
        id 0x01 \
        format "Failed to open Trace Log File {} for recording."

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Command registration port
    command reg port cmdRegOut

    @ Command received port
    command recv port cmdIn

    @ Command response port
    command resp port cmdResponseOut

    @ Event port
    event port logOut

    @ Text event port
    text event port LogText

    @ Time get port
    time get port timeCaller


  }
}