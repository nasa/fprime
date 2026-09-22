// ======================================================================
// \title  TimeConverterTester.hpp
// \author devin
// \brief  hpp file for TimeConverter component test harness implementation class
// ======================================================================

#ifndef Svc_TimeConverterTester_HPP
#define Svc_TimeConverterTester_HPP

#include "Svc/TimeConverter/TimeConverter.hpp"
#include "Svc/TimeConverter/TimeConverterGTestBase.hpp"

namespace Svc {

class TimeConverterTester : public TimeConverterGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    //! Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 32;

    //! Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object TimeConverterTester
    TimeConverterTester();

    //! Destroy object TimeConverterTester
    ~TimeConverterTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Conversion within a single time base copies the time
    void sameTimeBaseTest();

    //! Conversion in the stored direction and its reverse
    void convertBothDirectionsTest();

    //! Conversion preserves the time context of the input
    void contextPreservedTest();

    //! Conversion of an unknown pair of time bases
    void unknownTimeBaseTest();

    //! Conversion whose result falls outside the representable time range
    void invalidTimeTest();

    //! Offsets supplied over the port are stored without an event
    void portUpdateTest();

    //! A second offset for a pair replaces the first
    void replaceOffsetTest();

    //! SET_OFFSET with a single time base supplied as both ends
    void setOffsetIdenticalTest();

    //! SET_OFFSET with an offset larger than the representable time range
    void setOffsetOutOfRangeTest();

    //! CLEAR_OFFSETS discards every stored offset
    void clearOffsetsTest();

    //! GET_OFFSET in both directions and for an unknown pair
    void getOffsetTest();

    //! DUMP_OFFSETS with a populated and with an empty table
    void dumpOffsetsTest();

    //! An offset is stored for every distinct pair the table holds
    void fillTableTest();

    //! An offset beyond the table capacity is rejected
    void tableFullTest();

    //! Offsets at the limits of the representable range are accepted
    void offsetBoundariesTest();

    //! Offsets rejected on the port are not stored and report no status
    void portUpdateRejectedTest();

    //! Every throttled warning resumes after the offsets are cleared
    void throttleResetTest();

    //! A time base naming no clock rejected at every entry point
    void unusableTimeBaseTest();

    //! A conversion landing on either end of the representable range
    void conversionRangeEndsTest();

    //! An offset beyond the table capacity supplied over the port
    void portUpdateTableFullTest();

  public:
    // ----------------------------------------------------------------------
    // Accessor methods for protected members
    // ----------------------------------------------------------------------

    //! Get the throttle count of a warning event
    static constexpr U32 getNoConversionAvailableThrottle() {
        return static_cast<U32>(TimeConverterComponentBase::EVENTID_NOCONVERSIONAVAILABLE_THROTTLE);
    }
    static constexpr U32 getInvalidTimeThrottle() {
        return static_cast<U32>(TimeConverterComponentBase::EVENTID_INVALIDTIME_THROTTLE);
    }
    static constexpr U32 getIdenticalTimeBasesThrottle() {
        return static_cast<U32>(TimeConverterComponentBase::EVENTID_IDENTICALTIMEBASES_THROTTLE);
    }
    static constexpr U32 getOffsetOutOfRangeThrottle() {
        return static_cast<U32>(TimeConverterComponentBase::EVENTID_OFFSETOUTOFRANGE_THROTTLE);
    }
    static constexpr U32 getOffsetTableFullThrottle() {
        return static_cast<U32>(TimeConverterComponentBase::EVENTID_OFFSETTABLEFULL_THROTTLE);
    }
    static constexpr U32 getUnusableTimeBaseThrottle() {
        return static_cast<U32>(TimeConverterComponentBase::EVENTID_UNUSABLETIMEBASE_THROTTLE);
    }

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Store an offset by command and assert the resulting event and response
    void setOffset(const TimeBase& from, const TimeBase& to, I64 offset_us);

    //! Convert a time into a time base and assert the returned status
    Fw::Time convert(const Fw::Time& in_time, const TimeBase& out_tb, Svc::ConvertTimeStatus status);

    //! Store an offset for as many distinct pairs as the table holds
    void fillToCapacity();

    //! Request an offset by command and assert the reported offset
    void assertGetOffset(const TimeBase& from, const TimeBase& to, I64 offset_us);

    //! Send a SET_OFFSET expected to be rejected and assert the command response
    void assertSetOffsetRejected(const TimeBase& from, const TimeBase& to, I64 offset_us, Fw::CmdResponse response);

    //! Clear the offsets by command, discarding the resulting events
    void clearOffsets();

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    //! Number of the next command sent
    U32 m_cmdSeq;

    //! The component under test
    TimeConverter component;
};

}  // namespace Svc

#endif
