// ======================================================================
// \title  MathSender/test/ut/Tester.hpp
// \author asloan
// \brief  hpp file for MathSender test harness implementation class
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "MathSenderGTestBase.hpp"
#include "MathProject/Components/MathSender/MathSender.hpp"

namespace MathProject
{

  class MathSenderTester : public MathSenderGTestBase
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

    //! Construct object MathSenderTester
    //!
    MathSenderTester();

    //! Destroy object MathSenderTester
    //!
    ~MathSenderTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! To do
    //!
    void toDo();

    void testDoMath(MathOp op);

    void testAddCommand();

    void testResult();

  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler for from_mathOpOut
    //!
    void from_mathOpOut_handler(
        const FwIndexType portNum,     /*!< The port number*/
        F32 val1,                      /*!<
                         The first operand
                         */
        const MathProject::MathOp &op, /*!<
    The operation
    */
        F32 val2                       /*!<
                          The second operand
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
    MathSender component;
  };

} // end namespace MathProject

#endif
