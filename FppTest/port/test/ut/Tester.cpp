// ======================================================================
// \title  Example.hpp
// \author tchieu
// \brief  cpp file for Example test harness implementation class
// ======================================================================

#include "Fw/Types/SerialBuffer.hpp"

#include "Tester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10


  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  Tester ::
    Tester() :
      ExampleGTestBase("Tester", MAX_HISTORY_SIZE),
      component("Example")
  {
    this->initComponents();
    this->connectPorts();
  }

  Tester ::
    ~Tester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void Tester ::
    toDo()
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Invoke typed input ports
  // ----------------------------------------------------------------------

  void Tester ::
    invoke(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::NoArgsPort& port
    ) 
  {
    this->invoke_to_noArgsIn(portNum);
  }

  void Tester ::
    invoke(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::PrimitiveArgsPort& port
    ) 
  {
    this->invoke_to_primitiveArgsIn(
      portNum, 
      port.args.u32, 
      port.args.u32Ref, 
      port.args.f32,
      port.args.f32Ref,
      port.args.b,
      port.args.bRef
    );
  }

  void Tester ::
    invoke(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::StringArgsPort& port
    ) 
  {
    this->invoke_to_stringArgsIn(
      portNum,
      port.args.str80,
      port.args.str80Ref,
      port.args.str100,
      port.args.str100Ref
    );
  }

  void Tester ::
    invoke(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::EnumArgsPort& port
    ) 
  {
    this->invoke_to_enumArgsIn(
      portNum, 
      port.args.e, 
      port.args.eRef
    );
  }

  void Tester ::
    invoke(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::ArrayArgsPort& port
    ) 
  {
    this->invoke_to_arrayArgsIn(
      portNum, 
      port.args.a, 
      port.args.aRef
    );
  }

  void Tester ::
    invoke(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::StructArgsPort& port
    ) 
  {
    this->invoke_to_structArgsIn(
      portNum, 
      port.args.s, 
      port.args.sRef
    );
  }

  void Tester ::
    invoke(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::NoArgsReturnPort& port
    ) 
  {
    bool returnVal = this->invoke_to_noArgsReturnIn(portNum);

    ASSERT_EQ(returnVal, this->noArgsReturnVal.val);
  }

  void Tester ::
    invoke(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::PrimitiveReturnPort& port
    ) 
  {
    U32 returnVal = this->invoke_to_primitiveReturnIn(
      portNum, 
      port.args.u32, 
      port.args.u32Ref, 
      port.args.f32,
      port.args.f32Ref,
      port.args.b,
      port.args.bRef
    );

    ASSERT_EQ(returnVal, this->primitiveReturnVal.val);
  }

  void Tester ::
    invoke(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::EnumReturnPort& port
    ) 
  {
    PortEnum returnVal = this->invoke_to_enumReturnIn(
      portNum,
      port.args.e,
      port.args.eRef
    );

    ASSERT_EQ(returnVal, this->enumReturnVal.val);
  }

  void Tester ::
    invoke(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::ArrayReturnPort& port
    ) 
  {
    PortArray returnVal = this->invoke_to_arrayReturnIn(
      portNum,
      port.args.a,
      port.args.aRef
    );

    ASSERT_EQ(returnVal, this->arrayReturnVal.val);
  }

  void Tester ::
    invoke(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::StructReturnPort& port
    ) 
  {
    PortStruct returnVal = this->invoke_to_structReturnIn(
      portNum,
      port.args.s,
      port.args.sRef
    );

    ASSERT_EQ(returnVal, this->structReturnVal.val);
  }

  // ----------------------------------------------------------------------
  // Check history of typed output ports
  // ----------------------------------------------------------------------

  void Tester ::
    check_history(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::NoArgsPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_noArgsOut_SIZE(1);
  }

  void Tester ::
    check_history(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::PrimitiveArgsPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_primitiveArgsOut_SIZE(1);
    ASSERT_from_primitiveArgsOut(
      portNum, 
      port.args.u32, 
      port.args.u32Ref, 
      port.args.f32,
      port.args.f32Ref,
      port.args.b,
      port.args.bRef
    );
  }

  void Tester ::
    check_history(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::StringArgsPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_stringArgsOut_SIZE(1);
    ASSERT_from_stringArgsOut(
      portNum, 
      port.args.str80,
      port.args.str80Ref,
      port.args.str100,
      port.args.str100Ref
    );
  }

  void Tester ::
    check_history(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::EnumArgsPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_enumArgsOut_SIZE(1);
//    ASSERT_from_enumArgsOut(
//      portNum, 
//      port.args.e,
//      port.args.eRef
//    );
  }

  void Tester ::
    check_history(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::ArrayArgsPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_arrayArgsOut_SIZE(1);
    ASSERT_from_arrayArgsOut(
      portNum, 
      port.args.a,
      port.args.aRef
    );
  }

  void Tester ::
    check_history(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::StructArgsPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_structArgsOut_SIZE(1);
    ASSERT_from_structArgsOut(
      portNum, 
      port.args.s,
      port.args.sRef
    );
  }

  void Tester ::
    check_history(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::NoArgsReturnPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_noArgsReturnOut_SIZE(1);
  }

  void Tester ::
    check_history(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::PrimitiveReturnPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_primitiveReturnOut_SIZE(1);
    ASSERT_from_primitiveReturnOut(
      portNum, 
      port.args.u32, 
      port.args.u32Ref, 
      port.args.f32,
      port.args.f32Ref,
      port.args.b,
      port.args.bRef
    );
  }

  void Tester ::
    check_history(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::EnumReturnPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_enumReturnOut_SIZE(1);
//    ASSERT_from_enumReturnOut(
//      portNum, 
//      port.args.e,
//      port.args.eRef
//    );
  }

  void Tester ::
    check_history(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::ArrayReturnPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_arrayReturnOut_SIZE(1);
    ASSERT_from_arrayReturnOut(
      portNum, 
      port.args.a,
      port.args.aRef
    );
  }

  void Tester ::
    check_history(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::StructReturnPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_structReturnOut_SIZE(1);
    ASSERT_from_structReturnOut(
      portNum, 
      port.args.s,
      port.args.sRef
    );
  }

  // ----------------------------------------------------------------------
  // Invoke serial input ports
  // ----------------------------------------------------------------------
    
  void Tester ::
    invoke_from_serial(
      NATIVE_INT_TYPE portNum,
      FppTest::Port::NoArgsPort& port
    )
  {
    U8 data[0];
    Fw::SerialBuffer buf(data, sizeof(data));

    this->invoke_to_serialToNoArgs(portNum, buf);
  }
    
  void Tester ::
    invoke_from_serial(
      NATIVE_INT_TYPE portNum,
      FppTest::Port::PrimitiveArgsPort& port
    )
  {
    U8 data[sizeof(FppTest::Port::PrimitiveArgs) * 6];
    Fw::SerialBuffer buf(data, sizeof(data));

    buf.serialize(port.args.u32);
    buf.serialize(port.args.u32Ref);
    buf.serialize(port.args.f32);
    buf.serialize(port.args.f32Ref);
    buf.serialize(port.args.b);
    buf.serialize(port.args.bRef);

    this->invoke_to_serialToPrimitiveArgs(portNum, buf);
  }
    
  void Tester ::
    invoke_from_serial(
      NATIVE_INT_TYPE portNum,
      FppTest::Port::StringArgsPort& port
    )
  {
    U8 data[
      StringArgsPortStrings::StringSize80::SERIALIZED_SIZE * 2 + 
      StringArgsPortStrings::StringSize100::SERIALIZED_SIZE * 2
    ];
    Fw::SerialBuffer buf(data, sizeof(data));

    buf.serialize(port.args.str80);
    buf.serialize(port.args.str80Ref);
    buf.serialize(port.args.str100);
    buf.serialize(port.args.str100Ref);

    this->invoke_to_serialToStringArgs(portNum, buf);
  }
    
  void Tester ::
    invoke_from_serial(
      NATIVE_INT_TYPE portNum,
      FppTest::Port::EnumArgsPort& port
    )
  {
    U8 data[PortEnum::SERIALIZED_SIZE * 2];
    Fw::SerialBuffer buf(data, sizeof(data));

    buf.serialize(port.args.e);
    buf.serialize(port.args.eRef);

    this->invoke_to_serialToEnumArgs(portNum, buf);
  }
    
  void Tester ::
    invoke_from_serial(
      NATIVE_INT_TYPE portNum,
      FppTest::Port::ArrayArgsPort& port
    )
  {
    U8 data[PortArray::SERIALIZED_SIZE * 2];
    Fw::SerialBuffer buf(data, sizeof(data));

    buf.serialize(port.args.a);
    buf.serialize(port.args.aRef);

    this->invoke_to_serialToArrayArgs(portNum, buf);
  }
    
  void Tester ::
    invoke_from_serial(
      NATIVE_INT_TYPE portNum,
      FppTest::Port::StructArgsPort& port
    )
  {
    U8 data[PortStruct::SERIALIZED_SIZE * 2];
    Fw::SerialBuffer buf(data, sizeof(data));

    buf.serialize(port.args.s);
    buf.serialize(port.args.sRef);

    this->invoke_to_serialToStructArgs(portNum, buf);
  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void Tester ::
    from_arrayArgsOut_handler(
        const NATIVE_INT_TYPE portNum,
        const PortArray &a,
        PortArray &aRef
    )
  {
    this->pushFromPortEntry_arrayArgsOut(a, aRef);
  }

  PortArray Tester ::
    from_arrayReturnOut_handler(
        const NATIVE_INT_TYPE portNum,
        const PortArray &a,
        PortArray &aRef
    )
  {
    this->pushFromPortEntry_arrayReturnOut(a, aRef);

    return this->arrayReturnVal.val;
  }

  void Tester ::
    from_enumArgsOut_handler(
        const NATIVE_INT_TYPE portNum,
        const PortEnum &e,
        PortEnum &eRef
    )
  {
    this->pushFromPortEntry_enumArgsOut(e, eRef);
  }

  PortEnum Tester ::
    from_enumReturnOut_handler(
        const NATIVE_INT_TYPE portNum,
        const PortEnum &e,
        PortEnum &eRef
    )
  {
    this->pushFromPortEntry_enumReturnOut(e, eRef);

    return this->enumReturnVal.val;
  }

  void Tester ::
    from_noArgsOut_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    this->pushFromPortEntry_noArgsOut();
  }

  bool Tester ::
    from_noArgsReturnOut_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    this->pushFromPortEntry_noArgsReturnOut();

    return this->noArgsReturnVal.val;
  }

  void Tester ::
    from_primitiveArgsOut_handler(
        const NATIVE_INT_TYPE portNum,
        U32 u32,
        U32 &u32Ref,
        F32 f32,
        F32 &f32Ref,
        bool b,
        bool &bRef
    )
  {
    this->pushFromPortEntry_primitiveArgsOut(u32, u32Ref, f32, f32Ref, b, bRef);
  }

  U32 Tester ::
    from_primitiveReturnOut_handler(
        const NATIVE_INT_TYPE portNum,
        U32 u32,
        U32 &u32Ref,
        F32 f32,
        F32 &f32Ref,
        bool b,
        bool &bRef
    )
  {
    this->pushFromPortEntry_primitiveReturnOut(u32, u32Ref, f32, f32Ref, b, bRef);

    return this->primitiveReturnVal.val;
  }

  void Tester ::
    from_stringArgsOut_handler(
        const NATIVE_INT_TYPE portNum,
        const str80String &str80,
        str80RefString &str80Ref,
        const str100String &str100,
        str100RefString &str100Ref
    )
  {
    this->pushFromPortEntry_stringArgsOut(str80, str80Ref, str100, str100Ref);
  }

  void Tester ::
    from_structArgsOut_handler(
        const NATIVE_INT_TYPE portNum,
        const PortStruct &s,
        PortStruct &sRef
    )
  {
    this->pushFromPortEntry_structArgsOut(s, sRef);
  }

  PortStruct Tester ::
    from_structReturnOut_handler(
        const NATIVE_INT_TYPE portNum,
        const PortStruct &s,
        PortStruct &sRef
    )
  {
    this->pushFromPortEntry_structReturnOut(s, sRef);

    return this->structReturnVal.val;
  }

  // ----------------------------------------------------------------------
  // Handlers for serial from ports
  // ----------------------------------------------------------------------

  void Tester ::
    from_serialOut_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts()
  {

    // arrayArgsIn
    this->connect_to_arrayArgsIn(
        0,
        this->component.get_arrayArgsIn_InputPort(0)
    );

    // arrayArgsToSerial
    this->connect_to_arrayArgsToSerial(
        0,
        this->component.get_arrayArgsToSerial_InputPort(0)
    );

    // arrayReturnIn
    this->connect_to_arrayReturnIn(
        0,
        this->component.get_arrayReturnIn_InputPort(0)
    );

    // enumArgsIn
    this->connect_to_enumArgsIn(
        0,
        this->component.get_enumArgsIn_InputPort(0)
    );

    // enumArgsToSerial
    this->connect_to_enumArgsToSerial(
        0,
        this->component.get_enumArgsToSerial_InputPort(0)
    );

    // enumReturnIn
    this->connect_to_enumReturnIn(
        0,
        this->component.get_enumReturnIn_InputPort(0)
    );

    // noArgsIn
    this->connect_to_noArgsIn(
        0,
        this->component.get_noArgsIn_InputPort(0)
    );

    // noArgsReturnIn
    this->connect_to_noArgsReturnIn(
        0,
        this->component.get_noArgsReturnIn_InputPort(0)
    );

    // noArgsToSerial
    this->connect_to_noArgsToSerial(
        0,
        this->component.get_noArgsToSerial_InputPort(0)
    );

    // primitiveArgsIn
    this->connect_to_primitiveArgsIn(
        0,
        this->component.get_primitiveArgsIn_InputPort(0)
    );

    // primitiveArgsToSerial
    this->connect_to_primitiveArgsToSerial(
        0,
        this->component.get_primitiveArgsToSerial_InputPort(0)
    );

    // primitiveReturnIn
    this->connect_to_primitiveReturnIn(
        0,
        this->component.get_primitiveReturnIn_InputPort(0)
    );

    // stringArgsIn
    this->connect_to_stringArgsIn(
        0,
        this->component.get_stringArgsIn_InputPort(0)
    );

    // stringArgsToSerial
    this->connect_to_stringArgsToSerial(
        0,
        this->component.get_stringArgsToSerial_InputPort(0)
    );

    // structArgsIn
    this->connect_to_structArgsIn(
        0,
        this->component.get_structArgsIn_InputPort(0)
    );

    // structArgsToSerial
    this->connect_to_structArgsToSerial(
        0,
        this->component.get_structArgsToSerial_InputPort(0)
    );

    // structReturnIn
    this->connect_to_structReturnIn(
        0,
        this->component.get_structReturnIn_InputPort(0)
    );

    // arrayArgsOut
    this->component.set_arrayArgsOut_OutputPort(
        0,
        this->get_from_arrayArgsOut(0)
    );

    // arrayReturnOut
    this->component.set_arrayReturnOut_OutputPort(
        0,
        this->get_from_arrayReturnOut(0)
    );

    // enumArgsOut
    this->component.set_enumArgsOut_OutputPort(
        0,
        this->get_from_enumArgsOut(0)
    );

    // enumReturnOut
    this->component.set_enumReturnOut_OutputPort(
        0,
        this->get_from_enumReturnOut(0)
    );

    // noArgsOut
    this->component.set_noArgsOut_OutputPort(
        0,
        this->get_from_noArgsOut(0)
    );

    // noArgsReturnOut
    this->component.set_noArgsReturnOut_OutputPort(
        0,
        this->get_from_noArgsReturnOut(0)
    );

    // primitiveArgsOut
    this->component.set_primitiveArgsOut_OutputPort(
        0,
        this->get_from_primitiveArgsOut(0)
    );

    // primitiveReturnOut
    this->component.set_primitiveReturnOut_OutputPort(
        0,
        this->get_from_primitiveReturnOut(0)
    );

    // stringArgsOut
    this->component.set_stringArgsOut_OutputPort(
        0,
        this->get_from_stringArgsOut(0)
    );

    // structArgsOut
    this->component.set_structArgsOut_OutputPort(
        0,
        this->get_from_structArgsOut(0)
    );

    // structReturnOut
    this->component.set_structReturnOut_OutputPort(
        0,
        this->get_from_structReturnOut(0)
    );


  // ----------------------------------------------------------------------
  // Connect serial output ports
  // ----------------------------------------------------------------------
    this->component.set_serialOut_OutputPort(
        0,
        this->get_from_serialOut(0)
    );


  // ----------------------------------------------------------------------
  // Connect serial input ports
  // ----------------------------------------------------------------------
    // serialToArrayArgs
    this->connect_to_serialToArrayArgs(
        0,
        this->component.get_serialToArrayArgs_InputPort(0)
    );

    // serialToEnumArgs
    this->connect_to_serialToEnumArgs(
        0,
        this->component.get_serialToEnumArgs_InputPort(0)
    );

    // serialToNoArgs
    this->connect_to_serialToNoArgs(
        0,
        this->component.get_serialToNoArgs_InputPort(0)
    );

    // serialToPrimitiveArgs
    this->connect_to_serialToPrimitiveArgs(
        0,
        this->component.get_serialToPrimitiveArgs_InputPort(0)
    );

    // serialToSerial
    this->connect_to_serialToSerial(
        0,
        this->component.get_serialToSerial_InputPort(0)
    );

    // serialToStringArgs
    this->connect_to_serialToStringArgs(
        0,
        this->component.get_serialToStringArgs_InputPort(0)
    );

    // serialToStructArgs
    this->connect_to_serialToStructArgs(
        0,
        this->component.get_serialToStructArgs_InputPort(0)
    );


  }

  void Tester ::
    initComponents()
  {
    this->init();
    this->component.init(
        INSTANCE
    );
  }

