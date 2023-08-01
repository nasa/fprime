// ======================================================================
// \title  ActiveTest/test/ut/Tester.hpp
// \author tiffany
// \brief  hpp file for ActiveTest test harness implementation class
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "FppTest/component/active/ActiveTest.hpp"
#include "FppTest/component/active/SerialPortIndexEnumAc.hpp"
#include "FppTest/component/active/TypedPortIndexEnumAc.hpp"
#include "FppTest/component/tests/CmdTests.hpp"
#include "FppTest/component/tests/EventTests.hpp"
#include "FppTest/component/tests/InternalInterfaceTests.hpp"
#include "FppTest/component/tests/ParamTests.hpp"
#include "FppTest/component/tests/PortTests.hpp"
#include "FppTest/component/tests/TlmTests.hpp"
#include "FppTest/component/types/FormalParamTypes.hpp"
#include "GTestBase.hpp"

class Tester : public ActiveTestGTestBase {
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

  public:
    // Maximum size of histories storing events, telemetry, and port outputs
    static const NATIVE_INT_TYPE MAX_HISTORY_SIZE = 100;
    // Instance ID supplied to the component instance under test
    static const NATIVE_INT_TYPE TEST_INSTANCE_ID = 0;
    // Queue depth supplied to component instance under test
    static const NATIVE_INT_TYPE TEST_INSTANCE_QUEUE_DEPTH = 10;

    //! Construct object Tester
    //!
    Tester();

    //! Destroy object Tester
    //!
    ~Tester();

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

    INTERNAL_INT_TEST_DECLS

    void testTime();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler for from_arrayArgsOut
    //!
    void from_arrayArgsOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                   const FormalParamArray& a,     /*!<
                                   An array
                                   */
                                   FormalParamArray& aRef         /*!<
                                       An array ref
                                       */
    );

    //! Handler for from_arrayReturnOut
    //!
    FormalParamArray from_arrayReturnOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                                 const FormalParamArray& a,     /*!<
                                                 An array
                                                 */
                                                 FormalParamArray& aRef         /*!<
                                                     An array ref
                                                     */
    );

    //! Handler for from_enumArgsOut
    //!
    void from_enumArgsOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                  const FormalParamEnum& en,     /*!<
                                  An enum
                                  */
                                  FormalParamEnum& enRef         /*!<
                                      An enum ref
                                      */
    );

    //! Handler for from_enumReturnOut
    //!
    FormalParamEnum from_enumReturnOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                               const FormalParamEnum& en,     /*!<
                                               An enum
                                               */
                                               FormalParamEnum& enRef         /*!<
                                                   An enum ref
                                                   */
    );

    //! Handler for from_noArgsOut
    //!
    void from_noArgsOut_handler(const NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Handler for from_noArgsReturnOut
    //!
    bool from_noArgsReturnOut_handler(const NATIVE_INT_TYPE portNum /*!< The port number*/
    );

    //! Handler for from_primitiveArgsOut
    //!
    void from_primitiveArgsOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                       U32 u32,
                                       U32& u32Ref,
                                       F32 f32,
                                       F32& f32Ref,
                                       bool b,
                                       bool& bRef);

    //! Handler for from_primitiveReturnOut
    //!
    U32 from_primitiveReturnOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                        U32 u32,
                                        U32& u32Ref,
                                        F32 f32,
                                        F32& f32Ref,
                                        bool b,
                                        bool& bRef);

    //! Handler for from_prmGetIn
    //!
    Fw::ParamValid from_prmGetIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                         FwPrmIdType id,                /*!<
                                                    Parameter ID
                                                    */
                                         Fw::ParamBuffer& val           /*!<
                                               Buffer containing serialized parameter value
                                               */
    );

    //! Handler for from_prmGetIn
    //!
    void from_prmSetIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                               FwPrmIdType id,                /*!<
                                          Parameter ID
                                          */
                               Fw::ParamBuffer& val           /*!<
                                     Buffer containing serialized parameter value
                                     */
    );

    //! Handler for from_stringArgsOut
    //!
    void from_stringArgsOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                    const str80String& str80,      /*!<
                                     A string of size 80
                                     */
                                    str80RefString& str80Ref,
                                    const str100String& str100, /*!<
                                A string of size 100
                                */
                                    str100RefString& str100Ref);

    //! Handler for from_structArgsOut
    //!
    void from_structArgsOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                    const FormalParamStruct& s,    /*!<
                                   A struct
                                   */
                                    FormalParamStruct& sRef        /*!<
                                       A struct ref
                                       */
    );

    //! Handler for from_structReturnOut
    //!
    FormalParamStruct from_structReturnOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                                   const FormalParamStruct& s,    /*!<
                                                  A struct
                                                  */
                                                   FormalParamStruct& sRef        /*!<
                                                      A struct ref
                                                      */
    );

    void cmdResponseIn(const FwOpcodeType opCode, const U32 cmdSeq, const Fw::CmdResponse response);

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handlers for serial from ports
    // ----------------------------------------------------------------------

    //! Handler for from_serialOut
    //!
    void from_serialOut_handler(NATIVE_INT_TYPE portNum,        /*!< The port number*/
                                Fw::SerializeBufferBase& Buffer /*!< The serialization buffer*/
    );

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

  PRIVATE:
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

    // Command test values
    Fw::CmdResponse cmdResp;

    // Parameter test values
    FppTest::Types::BoolParam boolPrm;
    FppTest::Types::U32Param u32Prm;
    FppTest::Types::PrmStringParam stringPrm;
    FppTest::Types::EnumParam enumPrm;
    FppTest::Types::ArrayParam arrayPrm;
    FppTest::Types::StructParam structPrm;
    Fw::ParamValid prmValid;

    // Time test values
    Fw::Time time;
};

#endif
