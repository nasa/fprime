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

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Store an offset by command and assert the resulting event and response
    void setOffset(const TimeBase& from, const TimeBase& to, I64 offset_us);

    //! Convert a time into a time base and assert the returned status
    Fw::Time convert(const Fw::Time& in_time, const TimeBase& out_tb, Svc::ConvertTimeStatus status);

    //! Request an offset by command and assert the reported offset
    void assertGetOffset(const TimeBase& from, const TimeBase& to, I64 offset_us);

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
