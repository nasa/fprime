// ======================================================================
// \title  ActiveTest/test/ut/ActiveTestTester.hpp
// \author tiffany
// \brief  hpp file for ActiveTest test harness implementation class
// ======================================================================

#ifndef ACTIVE_TEST_TESTER_HPP
#define ACTIVE_TEST_TESTER_HPP

#include "ActiveTestGTestBase.hpp"
#include "FppTest/component/active/ActiveTest.hpp"
#include "FppTest/component/active/SerialPortIndexEnumAc.hpp"
#include "FppTest/component/active/TypedPortIndexEnumAc.hpp"
#include "FppTest/component/tests/CmdTests.hpp"
#include "FppTest/component/tests/EventTests.hpp"
#include "FppTest/component/tests/ExternalParamTests.hpp"
#include "FppTest/component/tests/InternalInterfaceTests.hpp"
#include "FppTest/component/tests/ParamTests.hpp"
#include "FppTest/component/tests/PortTests.hpp"
#include "FppTest/component/tests/TlmTests.hpp"
#include "FppTest/component/types/FormalParamTypes.hpp"

class ActiveTestTester : public ActiveTestGTestBase {
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

  public:
    // Maximum size of histories storing events, telemetry, and port outputs
    static const U32 MAX_HISTORY_SIZE = 100;
    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;
    // Queue depth supplied to component instance under test
    static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;

    //! Construct object ActiveTestTester
    //!
    ActiveTestTester();

    //! Destroy object ActiveTestTester
    //!
    ~ActiveTestTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    PORT_TEST_DECLS
    PORT_TEST_DECLS_ASYNC

    CMD_TEST_DECLS
    CMD_TEST_DECLS_ASYNC

    EVENT_TEST_DECLS

    TLM_TEST_DECLS

    void testParam();
    PARAM_CMD_TEST_DECLS

    void testExternalParam();
    EXTERNAL_PARAM_CMD_TEST_DECLS

    INTERNAL_INT_TEST_DECLS

    void testTime();

    void testOverflowAssert();

    void testOverflowDrop();

    void testOverflowHook();

  private:
#include "FppTest/component/common/tester.hpp"

  private:
    // ----------------------------------------------------------------------
    // Handlers for serial from ports
    // ----------------------------------------------------------------------

    //! Handler for from_serialOut
    //!
    void from_serialOut_handler(FwIndexType portNum,             //!< The port number
                                Fw::SerializeBufferBase& Buffer  //!< The serialization buffer
                                ) final;

  public:
    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    //! Connect ports
    //!
    void connectPorts();

    //! Connect async ports
    void connectAsyncPorts();

    //! Connect prmSetIn port
    void connectPrmSetIn();

    //! Connect timeGetOut port
    void connectTimeGetOut();

    //! Connect serial ports to special ports
    void connectSpecialPortsSerial();

    //! Set prmValid
    void setPrmValid(Fw::ParamValid valid);

    //! Call doDispatch() on component under test
    Fw::QueuedComponentBase::MsgDispatchStatus doDispatch();

    //! Initialize components
    //!
    void initComponents();

    //! Check successful status of a serial port invocation
    void checkSerializeStatusSuccess();

    //! Check unsuccessful status of a serial port invocation
    void checkSerializeStatusBufferEmpty();

  private:
    // ----------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------

    //! The component under test
    //!
    ActiveTest component;

    // Values returned by typed output ports
    FppTest::Types::BoolType noParamReturnVal;
    FppTest::Types::U32Type primitiveReturnVal;
    FppTest::Types::EnumType enumReturnVal;
    FppTest::Types::ArrayType arrayReturnVal;
    FppTest::Types::StructType structReturnVal;
    FppTest::Types::StringType stringReturnVal;
    FppTest::Types::StringType stringAliasReturnVal;
    FppTest::Types::AliasStringArrayType arrayStringAliasReturnVal;

    // Buffers from serial output ports;
    U8 primitiveData[InputPrimitiveArgsPort::SERIALIZED_SIZE];
    U8 stringData[InputStringArgsPort::SERIALIZED_SIZE];
    U8 enumData[InputEnumArgsPort::SERIALIZED_SIZE];
    U8 arrayData[InputArrayArgsPort::SERIALIZED_SIZE];
    U8 structData[InputStructArgsPort::SERIALIZED_SIZE];
    U8 serialData[SERIAL_ARGS_BUFFER_CAPACITY];

    Fw::SerialBuffer primitiveBuf;
    Fw::SerialBuffer stringBuf;
    Fw::SerialBuffer enumBuf;
    Fw::SerialBuffer arrayBuf;
    Fw::SerialBuffer structBuf;
    Fw::SerialBuffer serialBuf;

    // Parameter test values
    FppTest::Types::BoolParam boolPrm;
    FppTest::Types::U32Param u32Prm;
    FppTest::Types::PrmStringParam stringPrm;
    FppTest::Types::EnumParam enumPrm;
    FppTest::Types::ArrayParam arrayPrm;
    FppTest::Types::StructParam structPrm;
    Fw::ParamValid prmValid;

    Fw::Time time;

    //! External Parameter Delegate
    class ActiveTestComponentBaseParamExternalDelegate : public Fw::ParamExternalDelegate {
      public:
        // ----------------------------------------------------------------------
        // Parameter validity flags
        // ----------------------------------------------------------------------

        //! True if ParamBoolExternal was successfully received
        Fw::ParamValid m_param_ParamBoolExternal_valid;

        //! True if ParamI32External was successfully received
        Fw::ParamValid m_param_ParamI32External_valid;

        //! True if ParamStringExternal was successfully received
        Fw::ParamValid m_param_ParamStringExternal_valid;

        //! True if ParamEnumExternal was successfully received
        Fw::ParamValid m_param_ParamEnumExternal_valid;

        //! True if ParamArrayExternal was successfully received
        Fw::ParamValid m_param_ParamArrayExternal_valid;

        //! True if ParamStructExternal was successfully received
        Fw::ParamValid m_param_ParamStructExternal_valid;

      public:
        // ----------------------------------------------------------------------
        // Parameter variables
        // ----------------------------------------------------------------------

        //! Parameter ParamBoolExternal
        bool m_param_ParamBoolExternal;

        //! Parameter ParamI32External
        I32 m_param_ParamI32External;

        //! Parameter ParamStringExternal
        Fw::ParamString m_param_ParamStringExternal;

        //! Parameter ParamEnumExternal
        FormalParamEnum m_param_ParamEnumExternal;

        //! Parameter ParamArrayExternal
        FormalParamArray m_param_ParamArrayExternal;

        //! Parameter ParamStructExternal
        FormalParamStruct m_param_ParamStructExternal;

      public:
        // ----------------------------------------------------------------------
        // Unit test implementation of external parameter delegate serialization/deserialization
        // ----------------------------------------------------------------------

        //! Parameter deserialization function for external parameter unit testing
        Fw::SerializeStatus deserializeParam(
            const FwPrmIdType base_id,     //!< The component base parameter ID to deserialize
            const FwPrmIdType local_id,    //!< The parameter local ID to deserialize
            const Fw::ParamValid prmStat,  //!< The parameter validity status
            Fw::SerializeBufferBase& buff  //!< The buffer containing the parameter to deserialize
            ) override;

        //! Parameter serialization function for external parameter unit testing
        Fw::SerializeStatus serializeParam(
            const FwPrmIdType base_id,     //!< The component base parameter ID to serialize
            const FwPrmIdType local_id,    //!< The parameter local ID to serialize
            Fw::SerializeBufferBase& buff  //!< The buffer to serialize the parameter into
        ) const override;
    };

    // ----------------------------------------------------------------------
    // Parameter delegates
    // ----------------------------------------------------------------------

    //! Delegate to serialize/deserialize an externally stored parameter
    ActiveTestComponentBaseParamExternalDelegate paramTesterDelegate;
};

typedef ActiveTestTester Tester;

#endif
