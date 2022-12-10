// ======================================================================
// \title  Example.hpp
// \author tchieu
// \brief  cpp file for Example test harness implementation class
// ======================================================================

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
    switch (portNum) {
      case TypedPortIndex::TYPED:
        this->invoke_to_arrayArgsIn(
          portNum, 
          port.args.a, 
          port.args.aRef
        );
        break;

      case TypedPortIndex::SERIAL: {
        U8 data[PortArray::SERIALIZED_SIZE * 2];
        Fw::SerialBuffer buf(data, sizeof(data));
        Fw::SerializeStatus status;

        status = buf.serialize(port.args.a);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

        status = buf.serialize(port.args.aRef);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        
        this->invoke_to_serialIn(
          SerialPortIndex::ARRAY,
          buf
        );
        break;
      }
    }
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
    // TODO
  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts()
  {

    // arrayArgsIn
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
      this->connect_to_arrayArgsIn(
          i,
          this->component.get_arrayArgsIn_InputPort(i)
      );
    }

    // arrayReturnIn
    this->connect_to_arrayReturnIn(
        0,
        this->component.get_arrayReturnIn_InputPort(0)
    );

    // enumArgsIn
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
      this->connect_to_enumArgsIn(
          i,
          this->component.get_enumArgsIn_InputPort(i)
      );
    }

    // enumReturnIn
    this->connect_to_enumReturnIn(
        0,
        this->component.get_enumReturnIn_InputPort(0)
    );

    // noArgsIn
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
      this->connect_to_noArgsIn(
          i,
          this->component.get_noArgsIn_InputPort(i)
      );
    }

    // noArgsReturnIn
    this->connect_to_noArgsReturnIn(
        0,
        this->component.get_noArgsReturnIn_InputPort(0)
    );

    // primitiveArgsIn
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
      this->connect_to_primitiveArgsIn(
          i,
          this->component.get_primitiveArgsIn_InputPort(i)
      );
    }

    // primitiveReturnIn
    this->connect_to_primitiveReturnIn(
        0,
        this->component.get_primitiveReturnIn_InputPort(0)
    );

    // stringArgsIn
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
      this->connect_to_stringArgsIn(
          i,
          this->component.get_stringArgsIn_InputPort(i)
      );
    }

    // structArgsIn
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
      this->connect_to_structArgsIn(
          i,
          this->component.get_structArgsIn_InputPort(i)
      );
    }

    // structReturnIn
    this->connect_to_structReturnIn(
        0,
        this->component.get_structReturnIn_InputPort(0)
    );

    // arrayArgsOut
    this->component.set_arrayArgsOut_OutputPort(
        TypedPortIndex::TYPED,
        this->get_from_arrayArgsOut(TypedPortIndex::TYPED)
    );

    // arrayReturnOut
    this->component.set_arrayReturnOut_OutputPort(
        0,
        this->get_from_arrayReturnOut(0)
    );

    // enumArgsOut
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
      this->component.set_enumArgsOut_OutputPort(
          i,
          this->get_from_enumArgsOut(i)
      );
    }

    // enumReturnOut
    this->component.set_enumReturnOut_OutputPort(
        0,
        this->get_from_enumReturnOut(0)
    );

    // noArgsOut
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
      this->component.set_noArgsOut_OutputPort(
          i,
          this->get_from_noArgsOut(i)
      );
    }

    // noArgsReturnOut
    this->component.set_noArgsReturnOut_OutputPort(
        0,
        this->get_from_noArgsReturnOut(0)
    );

    // primitiveArgsOut
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
      this->component.set_primitiveArgsOut_OutputPort(
          i,
          this->get_from_primitiveArgsOut(i)
      );
    }

    // primitiveReturnOut
    this->component.set_primitiveReturnOut_OutputPort(
        0,
        this->get_from_primitiveReturnOut(0)
    );

    // stringArgsOut
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
      this->component.set_stringArgsOut_OutputPort(
          i,
          this->get_from_stringArgsOut(i)
      );
    }

    // structArgsOut
    for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
      this->component.set_structArgsOut_OutputPort(
          i,
          this->get_from_structArgsOut(i)
      );
    }

    // structReturnOut
    this->component.set_structReturnOut_OutputPort(
        0,
        this->get_from_structReturnOut(0)
    );


  // ----------------------------------------------------------------------
  // Connect serial output ports
  // ----------------------------------------------------------------------
    for (NATIVE_INT_TYPE i = 0; i < 7; ++i) {
      this->component.set_serialOut_OutputPort(
          i,
          this->get_from_serialOut(i)
      );
    }


  // ----------------------------------------------------------------------
  // Connect serial input ports
  // ----------------------------------------------------------------------
    // serialIn
    // for (NATIVE_INT_TYPE i = 0; i < 7; ++i) {
    //   this->connect_to_serialIn(
    //       i,
    //       this->component.get_serialIn_InputPort(i)
    //   );
    // }

    this->component.set_arrayArgsOut_OutputPort(
      TypedPortIndex::SERIAL,
      this->get_from_serialOut(SerialPortIndex::ARRAY)
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

