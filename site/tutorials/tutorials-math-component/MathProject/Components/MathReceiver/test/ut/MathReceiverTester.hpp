// ======================================================================
// \title  MathReceiver/test/ut/Tester.hpp
// \author asloan
// \brief  hpp file for MathReceiver test harness implementation class
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "MathReceiverGTestBase.hpp"
#include "MathProject/Components/MathReceiver/MathReceiver.hpp"

namespace MathProject
{

  class MathReceiverTester : public MathReceiverGTestBase
  {

    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

  public:
    // Maximum size of histories storing events, telemetry, and port outputs
    static const U32 MAX_HISTORY_SIZE = 10;
    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;
    // Queue depth supplied to component instance under test
    static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;

    //! Construct object MathReceiverTester
    //!
    MathReceiverTester();

    //! Destroy object MathReceiverTester
    //!
    ~MathReceiverTester();

  private:
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    enum class ThrottleState
    {
      THROTTLED,
      NOT_THROTTLED
    };

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! To do
    //!
    void toDo();

    F32 pickF32Value();
    void setFactor(
        F32 factor,
        ThrottleState throttleState);

    F32 computeResult(
        F32 val1,
        MathOp op,
        F32 val2,
        F32 factor);

    void doMathOp(
        MathOp op,
        F32 factor);

    void testAdd();

    void testSub();

    void testThrottle();

  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler for from_mathResultOut
    //!
    void from_mathResultOut_handler(
        const FwIndexType portNum, /*!< The port number*/
        F32 result                 /*!<
                    the result of the operation
                    */
    );

  private:
    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    //! Connect ports
    //!
    void connectPorts();

    //! Initialize components
    //!
    void initComponents();

  private:
    // ----------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------

    //! The component under test
    //!
    MathReceiver component;
  };

} // end namespace MathProject

#endif
