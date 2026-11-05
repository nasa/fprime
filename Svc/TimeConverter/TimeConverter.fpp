module Svc {

    @ Converts Fw::Time objects between time bases using offsets supplied at runtime
    passive component TimeConverter {

        # ----------------------------------------------------------------------
        # Conversion interface
        #
        # Guarded to protect the offset table from concurrent access
        # ----------------------------------------------------------------------

        @ Port receiving offsets between pairs of time bases
        guarded input port offsetUpdate: TimeOffsetSend

        @ Port converting a time into the time base carried by out_time
        guarded input port convertTime: ConvertTime

        # ----------------------------------------------------------------------
        # Framework ports
        # ----------------------------------------------------------------------

        @ Port for receiving commands
        command recv port cmdIn

        @ Port for sending command registration requests
        command reg port cmdRegOut

        @ Port for sending command responses
        command resp port cmdResponseOut

        @ Event port
        event port eventOut

        @ Text event port
        text event port textEventOut

        @ Time get port
        time get port timeCaller

        # ----------------------------------------------------------------------
        # Commands
        # ----------------------------------------------------------------------

        @ Set the offset between a pair of time bases
        guarded command SET_OFFSET(
            from: TimeBase      @< time base the offset converts from
            to: TimeBase        @< time base the offset converts to
            offset_us: I64      @< microseconds added to a "from" time to produce a "to" time
        ) opcode 0x00

        @ Discard every stored offset
        guarded command CLEAR_OFFSETS() opcode 0x01

        @ Report the offset stored for a pair of time bases as an event
        guarded command GET_OFFSET(
            from: TimeBase      @< time base the offset converts from
            to: TimeBase        @< time base the offset converts to
        ) opcode 0x02

        @ Report every stored offset as events
        guarded command DUMP_OFFSETS() opcode 0x03

        # ----------------------------------------------------------------------
        # Events
        # ----------------------------------------------------------------------

        @ No offset is stored for the requested pair of time bases
        event NoConversionAvailable(
            in_tb: TimeBase     @< time base converted from
            out_tb: TimeBase    @< time base converted to
        ) \
        severity warning low \
        id 0x00 \
        format "No known conversion from {} to {}" \
        throttle 5

        @ Conversion result is not representable as an Fw::Time
        event InvalidTime(
            in_tb: TimeBase     @< time base converted from
            out_tb: TimeBase    @< time base converted to
            offset_us: I64      @< offset applied
            in_time_us: I64     @< time converted, in microseconds
        ) \
        severity warning low \
        id 0x01 \
        format "Conversion from {} to {} (offset {} us) of time {} us is out of range" \
        throttle 5

        @ An offset was stored for a pair of time bases by command
        event OffsetSet(
            from: TimeBase      @< time base the offset converts from
            to: TimeBase        @< time base the offset converts to
            offset_us: I64      @< offset stored
        ) \
        severity activity high \
        id 0x02 \
        format "Stored offset {} us from {} to {}"

        @ An offset could not be stored because the offset table is full
        event OffsetTableFull(
            from: TimeBase      @< time base the offset converts from
            to: TimeBase        @< time base the offset converts to
            capacity: U32       @< configured offset table capacity
        ) \
        severity warning high \
        id 0x03 \
        format "Cannot store offset from {} to {}: table is full at {} entries" \
        throttle 5

        @ Every stored offset was discarded
        event OffsetsCleared(
            entries: U32        @< number of entries discarded
        ) \
        severity activity high \
        id 0x04 \
        format "Discarded {} stored offsets"

        @ Report of a stored offset
        event OffsetReport(
            from: TimeBase      @< time base the offset converts from
            to: TimeBase        @< time base the offset converts to
            offset_us: I64      @< offset stored
        ) \
        severity activity high \
        id 0x05 \
        format "Offset from {} to {} is {} us"

        @ No offset is stored for the requested pair of time bases
        event NoOffsetStored(
            from: TimeBase      @< time base the offset converts from
            to: TimeBase        @< time base the offset converts to
        ) \
        severity warning low \
        id 0x06 \
        format "No offset stored from {} to {}"

        @ The offset table holds no entries
        event OffsetTableEmpty() \
        severity activity high \
        id 0x07 \
        format "No offsets are stored"

        @ An offset was supplied for a single time base rather than a pair
        event IdenticalTimeBases(
            tb: TimeBase        @< time base supplied as both ends of the pair
        ) \
        severity warning low \
        id 0x08 \
        format "Time base {} was supplied as both ends of an offset" \
        throttle 5

        @ An offset was rejected because no time can be shifted that far
        event OffsetOutOfRange(
            from: TimeBase      @< time base the offset converts from
            to: TimeBase        @< time base the offset converts to
            offset_us: I64      @< offset supplied
        ) \
        severity warning high \
        id 0x09 \
        format "Offset {} us from {} to {} exceeds the representable time range" \
        throttle 5

        @ A time base that denotes no clock was supplied as an end of a pair
        event UnusableTimeBase(
            tb: TimeBase        @< time base supplied
        ) \
        severity warning high \
        id 0x0A \
        format "Time base {} denotes no clock and cannot be converted" \
        throttle 5

    }
}
