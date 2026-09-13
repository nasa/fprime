#####
# TimeConverterPorts:
#
# Types and ports used to supply time base offsets to, and request time base
# conversions from, Svc::TimeConverter
#####

module Svc {

    @ An offset between a pair of time bases. A time in the "from" base is converted
    @ into the "to" base by adding offset_us microseconds
    struct TimeOffset {
        from: TimeBase      @< time base the offset converts from
        to: TimeBase        @< time base the offset converts to
        offset_us: I64      @< microseconds added to a "from" time to produce a "to" time
    }

    @ Result of a time base conversion
    enum ConvertTimeStatus {
        OK                  @< conversion succeeded
        UNKNOWN_TIMEBASE    @< no offset is stored for the requested pair of time bases
        INVALID_TIME        @< conversion result is not representable as an Fw::Time
    }

    @ Supply an offset between a pair of time bases
    port TimeOffsetSend(
        offset: TimeOffset
    )

    @ Convert in_time into the time base carried by out_time
    port ConvertTime(
        in_time: Fw.Time,       @< time to convert
        ref out_time: Fw.Time   @< carries the requested time base in; carries the converted time out
    ) -> ConvertTimeStatus

}
