// ======================================================================
// \title  Tester.cpp
// \author T. Chieu
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
      component("Example"),
      primitiveBuf(primitiveData, sizeof(primitiveData)),
      stringBuf(stringData, sizeof(stringData)),
      enumBuf(enumData, sizeof(enumData)),
      arrayBuf(arrayData, sizeof(arrayData)),
      structBuf(structData, sizeof(structData)),
      serialBuf(serialData, sizeof(serialData))
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
      port.args.en, 
      port.args.enRef
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
      FppTest::Port::SerialArgsPort& port
    )
  {
    this->invoke_to_serialIn(
      portNum,
      port.args.buf
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
      port.args.en,
      port.args.enRef
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
  // Invoke serial input ports
  // ----------------------------------------------------------------------

  void Tester ::
    invoke_serial(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::NoArgsPort& port
    ) 
  {
    U8 data[0];
    Fw::SerialBuffer buf(data, sizeof(data));

    this->invoke_to_serialIn(
      SerialPortIndex::NO_ARGS,
      buf
    );
  }

  void Tester ::
    invoke_serial(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::PrimitiveArgsPort& port
    ) 
  {
    Fw::SerializeStatus status;

    // Check unsuccessful deserialization of first parameter
    U8 invalidData1[0];
    Fw::SerialBuffer invalidBuf1(invalidData1, sizeof(invalidData1));

    this->invoke_to_serialIn(
      SerialPortIndex::PRIMITIVE,
      invalidBuf1
    );

    this->checkSerializeStatusBufferEmpty();

    // Check unsuccessful deserialization of second parameter
    U8 invalidData2[sizeof(U32)];
    Fw::SerialBuffer invalidBuf2(invalidData2, sizeof(invalidData2));

    status = invalidBuf2.serialize(port.args.u32);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    this->invoke_to_serialIn(
      SerialPortIndex::PRIMITIVE,
      invalidBuf2
    );

    this->checkSerializeStatusBufferEmpty();

    // Check unsuccessful deserialization of third parameter
    U8 invalidData3[sizeof(U32) * 2];
    Fw::SerialBuffer invalidBuf3(invalidData3, sizeof(invalidData3));

    status = invalidBuf3.serialize(port.args.u32);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = invalidBuf3.serialize(port.args.u32Ref);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    this->invoke_to_serialIn(
      SerialPortIndex::PRIMITIVE,
      invalidBuf3
    );

    this->checkSerializeStatusBufferEmpty();

    // Check unsuccessful deserialization of fourth parameter
    U8 invalidData4[
      (sizeof(U32) * 2) + 
      sizeof(F32)
    ];
    Fw::SerialBuffer invalidBuf4(invalidData4, sizeof(invalidData4));

    status = invalidBuf4.serialize(port.args.u32);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = invalidBuf4.serialize(port.args.u32Ref);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = invalidBuf4.serialize(port.args.f32);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    this->invoke_to_serialIn(
      SerialPortIndex::PRIMITIVE,
      invalidBuf4
    );

    this->checkSerializeStatusBufferEmpty();

    // Check unsuccessful deserialization of fifth parameter
    U8 invalidData5[
      (sizeof(U32) * 2) + 
      (sizeof(F32) * 2)
    ];
    Fw::SerialBuffer invalidBuf5(invalidData5, sizeof(invalidData5));

    status = invalidBuf5.serialize(port.args.u32);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = invalidBuf5.serialize(port.args.u32Ref);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = invalidBuf5.serialize(port.args.f32);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = invalidBuf5.serialize(port.args.f32Ref);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    this->invoke_to_serialIn(
      SerialPortIndex::PRIMITIVE,
      invalidBuf5
    );

    this->checkSerializeStatusBufferEmpty();

    // Check unsuccessful deserialization of sixth parameter
    U8 invalidData6[
      (sizeof(U32) * 2) + 
      (sizeof(F32) * 2) +
      sizeof(U8)
    ];
    Fw::SerialBuffer invalidBuf6(invalidData6, sizeof(invalidData6));

    status = invalidBuf6.serialize(port.args.u32);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = invalidBuf6.serialize(port.args.u32Ref);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = invalidBuf6.serialize(port.args.f32);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = invalidBuf6.serialize(port.args.f32Ref);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = invalidBuf6.serialize(port.args.b);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    this->invoke_to_serialIn(
      SerialPortIndex::PRIMITIVE,
      invalidBuf6
    );

    this->checkSerializeStatusBufferEmpty();

    // Check successful serialization
    U8 data[InputPrimitiveArgsPort::SERIALIZED_SIZE];
    Fw::SerialBuffer buf(data, sizeof(data));

    status = buf.serialize(port.args.u32);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = buf.serialize(port.args.u32Ref);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = buf.serialize(port.args.f32);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = buf.serialize(port.args.f32Ref);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = buf.serialize(port.args.b);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = buf.serialize(port.args.bRef);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    this->invoke_to_serialIn(
      SerialPortIndex::PRIMITIVE,
      buf
    );

    this->checkSerializeStatusSuccess();
  }

  void Tester ::
    invoke_serial(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::StringArgsPort& port
    ) 
  {
    Fw::SerializeStatus status;

    // Check unsuccessful deserialization of first parameter
    U8 invalidData1[0];
    Fw::SerialBuffer invalidBuf1(invalidData1, sizeof(invalidData1));

    this->invoke_to_serialIn(
      SerialPortIndex::STRING,
      invalidBuf1
    );

    this->checkSerializeStatusBufferEmpty();

    // Check unsuccessful deserialization of second parameter
    U8 invalidData2[StringArgsPortStrings::StringSize80::SERIALIZED_SIZE];
    Fw::SerialBuffer invalidBuf2(invalidData2, sizeof(invalidData2));

    status = invalidBuf2.serialize(port.args.str80);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    this->invoke_to_serialIn(
      SerialPortIndex::STRING,
      invalidBuf2
    );

    this->checkSerializeStatusBufferEmpty();

    // Check unsuccessful deserialization of third parameter
    U8 invalidData3[StringArgsPortStrings::StringSize80::SERIALIZED_SIZE * 2];
    Fw::SerialBuffer invalidBuf3(invalidData3, sizeof(invalidData3));

    status = invalidBuf3.serialize(port.args.str80);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = invalidBuf3.serialize(port.args.str80Ref);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    this->invoke_to_serialIn(
      SerialPortIndex::STRING,
      invalidBuf3
    );

    this->checkSerializeStatusBufferEmpty();

    // Check unsuccessful deserialization of fourth parameter
    U8 invalidData4[
      (StringArgsPortStrings::StringSize80::SERIALIZED_SIZE * 2) +
      StringArgsPortStrings::StringSize100::SERIALIZED_SIZE
    ];
    Fw::SerialBuffer invalidBuf4(invalidData4, sizeof(invalidData4));

    status = invalidBuf4.serialize(port.args.str80);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = invalidBuf4.serialize(port.args.str80Ref);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = invalidBuf4.serialize(port.args.str100);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    this->invoke_to_serialIn(
      SerialPortIndex::STRING,
      invalidBuf4
    );

    this->checkSerializeStatusBufferEmpty();

    // Check successful serialization
    U8 data[InputStringArgsPort::SERIALIZED_SIZE];
    Fw::SerialBuffer buf(data, sizeof(data));

    status = buf.serialize(port.args.str80);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = buf.serialize(port.args.str80Ref);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = buf.serialize(port.args.str100);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = buf.serialize(port.args.str100Ref);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    this->invoke_to_serialIn(
      SerialPortIndex::STRING,
      buf
    );

    this->checkSerializeStatusSuccess();
  }

  void Tester ::
    invoke_serial(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::EnumArgsPort& port
    ) 
  {
    Fw::SerializeStatus status;

    // Check unsuccessful deserialization of first parameter
    U8 invalidData1[0];
    Fw::SerialBuffer invalidBuf1(invalidData1, sizeof(invalidData1));

    this->invoke_to_serialIn(
      SerialPortIndex::ENUM,
      invalidBuf1
    );

    this->checkSerializeStatusBufferEmpty();

    // Check unsuccessful deserialization of second parameter
    U8 invalidData2[PortEnum::SERIALIZED_SIZE];
    Fw::SerialBuffer invalidBuf2(invalidData2, sizeof(invalidData2));

    status = invalidBuf2.serialize(port.args.en);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    this->invoke_to_serialIn(
      SerialPortIndex::ENUM,
      invalidBuf2
    );

    this->checkSerializeStatusBufferEmpty();

    // Check successful serialization
    U8 data[InputEnumArgsPort::SERIALIZED_SIZE];
    Fw::SerialBuffer buf(data, sizeof(data));

    status = buf.serialize(port.args.en);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = buf.serialize(port.args.enRef);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    this->invoke_to_serialIn(
      SerialPortIndex::ENUM,
      buf
    );

    this->checkSerializeStatusSuccess();
  }

  void Tester ::
    invoke_serial(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::ArrayArgsPort& port
    ) 
  {
    Fw::SerializeStatus status;

    // Check unsuccessful deserialization of first parameter
    U8 invalidData1[0];
    Fw::SerialBuffer invalidBuf1(invalidData1, sizeof(invalidData1));

    this->invoke_to_serialIn(
      SerialPortIndex::ARRAY,
      invalidBuf1
    );

    this->checkSerializeStatusBufferEmpty();

    // Check unsuccessful deserialization of second parameter
    U8 invalidData2[PortArray::SERIALIZED_SIZE];
    Fw::SerialBuffer invalidBuf2(invalidData2, sizeof(invalidData2));

    status = invalidBuf2.serialize(port.args.a);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    this->invoke_to_serialIn(
      SerialPortIndex::ARRAY,
      invalidBuf2
    );

    this->checkSerializeStatusBufferEmpty();

    U8 data[InputArrayArgsPort::SERIALIZED_SIZE];
    Fw::SerialBuffer buf(data, sizeof(data));

    status = buf.serialize(port.args.a);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = buf.serialize(port.args.aRef);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    this->invoke_to_serialIn(
      SerialPortIndex::ARRAY,
      buf
    );

    this->checkSerializeStatusSuccess();
  }

  void Tester ::
    invoke_serial(
        NATIVE_INT_TYPE portNum,
        FppTest::Port::StructArgsPort& port
    ) 
  {
    Fw::SerializeStatus status;

    // Check unsuccessful deserialization of first parameter
    U8 invalidData1[0];
    Fw::SerialBuffer invalidBuf1(invalidData1, sizeof(invalidData1));

    this->invoke_to_serialIn(
      SerialPortIndex::STRUCT,
      invalidBuf1
    );

    this->checkSerializeStatusBufferEmpty();

    // Check unsuccessful deserialization of second parameter
    U8 invalidData2[PortStruct::SERIALIZED_SIZE];
    Fw::SerialBuffer invalidBuf2(invalidData2, sizeof(invalidData2));

    status = invalidBuf2.serialize(port.args.s);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    this->invoke_to_serialIn(
      SerialPortIndex::STRUCT,
      invalidBuf2
    );

    this->checkSerializeStatusBufferEmpty();

    U8 data[InputStructArgsPort::SERIALIZED_SIZE];
    Fw::SerialBuffer buf(data, sizeof(data));

    status = buf.serialize(port.args.s);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = buf.serialize(port.args.sRef);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    this->invoke_to_serialIn(
      SerialPortIndex::STRUCT,
      buf
    );

    this->checkSerializeStatusSuccess();
  }

  void Tester :: 
    invoke_serial(
      NATIVE_INT_TYPE portNum,
      FppTest::Port::SerialArgsPort& port
    )
  {
    this->invoke_to_serialIn(
      portNum,
      port.args.buf
    );

    ASSERT_EQ(
      component.serializeStatus, 
      Fw::FW_SERIALIZE_OK
    );
  }

  // ----------------------------------------------------------------------
  // Check history of typed output ports
  // ----------------------------------------------------------------------

  void Tester ::
    check_history(
        FppTest::Port::NoArgsPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_noArgsOut_SIZE(1);
  }

  void Tester ::
    check_history(
        FppTest::Port::PrimitiveArgsPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_primitiveArgsOut_SIZE(1);
    ASSERT_from_primitiveArgsOut(
      0, 
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
        FppTest::Port::StringArgsPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_stringArgsOut_SIZE(1);
    ASSERT_from_stringArgsOut(
      0, 
      port.args.str80,
      port.args.str80Ref,
      port.args.str100,
      port.args.str100Ref
    );
  }

  void Tester ::
    check_history(
        FppTest::Port::EnumArgsPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_enumArgsOut_SIZE(1);
    ASSERT_from_enumArgsOut(
      0, 
      port.args.en,
      port.args.enRef
    );
  }

  void Tester ::
    check_history(
        FppTest::Port::ArrayArgsPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_arrayArgsOut_SIZE(1);
    ASSERT_from_arrayArgsOut(
      0, 
      port.args.a,
      port.args.aRef
    );
  }

  void Tester ::
    check_history(
        FppTest::Port::StructArgsPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_structArgsOut_SIZE(1);
    ASSERT_from_structArgsOut(
      0, 
      port.args.s,
      port.args.sRef
    );
  }

  void Tester ::
    check_history(
        FppTest::Port::NoArgsReturnPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_noArgsReturnOut_SIZE(1);
  }

  void Tester ::
    check_history(
        FppTest::Port::PrimitiveReturnPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_primitiveReturnOut_SIZE(1);
    ASSERT_from_primitiveReturnOut(
      0, 
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
        FppTest::Port::EnumReturnPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_enumReturnOut_SIZE(1);
    ASSERT_from_enumReturnOut(
      0, 
      port.args.en,
      port.args.enRef
    );
  }

  void Tester ::
    check_history(
        FppTest::Port::ArrayReturnPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_arrayReturnOut_SIZE(1);
    ASSERT_from_arrayReturnOut(
      0, 
      port.args.a,
      port.args.aRef
    );
  }

  void Tester ::
    check_history(
        FppTest::Port::StructReturnPort& port
    ) 
  {
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_structReturnOut_SIZE(1);
    ASSERT_from_structReturnOut(
      0, 
      port.args.s,
      port.args.sRef
    );
  }

  // ----------------------------------------------------------------------
  // Check serial output ports
  // ----------------------------------------------------------------------

  void Tester ::
    check_serial(
      FppTest::Port::NoArgsPort& port
    )
  {
  }

  void Tester ::
    check_serial(
      FppTest::Port::PrimitiveArgsPort& port
    )
  {
    Fw::SerializeStatus status;
    U32 u32, u32Ref;
    F32 f32, f32Ref;
    bool b, bRef;

    status = this->primitiveBuf.deserialize(u32);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = this->primitiveBuf.deserialize(u32Ref);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = this->primitiveBuf.deserialize(f32);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = this->primitiveBuf.deserialize(f32Ref);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = this->primitiveBuf.deserialize(b);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = this->primitiveBuf.deserialize(bRef);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    ASSERT_EQ(u32, port.args.u32);
    ASSERT_EQ(u32Ref, port.args.u32Ref);
    ASSERT_EQ(f32, port.args.f32);
    ASSERT_EQ(f32Ref, port.args.f32Ref);
    ASSERT_EQ(b, port.args.b);
    ASSERT_EQ(bRef, port.args.bRef);
  }

  void Tester ::
    check_serial(
      FppTest::Port::StringArgsPort& port
    )
  {
    Fw::SerializeStatus status;
    StringArgsPortStrings::StringSize80 str80, str80Ref;
    StringArgsPortStrings::StringSize100 str100, str100Ref;

    status = this->stringBuf.deserialize(str80);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = this->stringBuf.deserialize(str80Ref);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = this->stringBuf.deserialize(str100);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = this->stringBuf.deserialize(str100Ref);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    ASSERT_EQ(str80, port.args.str80);
    ASSERT_EQ(str80Ref, port.args.str80Ref);
    ASSERT_EQ(str100, port.args.str100);
    ASSERT_EQ(str100Ref, port.args.str100Ref);
  }

  void Tester ::
    check_serial(
      FppTest::Port::EnumArgsPort& port
    )
  {
    Fw::SerializeStatus status;
    PortEnum en, enRef;

    status = this->enumBuf.deserialize(en);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = this->enumBuf.deserialize(enRef);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    ASSERT_EQ(en, port.args.en);
    ASSERT_EQ(enRef, port.args.enRef);
  }

  void Tester ::
    check_serial(
      FppTest::Port::ArrayArgsPort& port
    )
  {
    Fw::SerializeStatus status;
    PortArray a, aRef;

    status = this->arrayBuf.deserialize(a);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = this->arrayBuf.deserialize(aRef);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    ASSERT_EQ(a, port.args.a);
    ASSERT_EQ(aRef, port.args.aRef);
  }

  void Tester ::
    check_serial(
      FppTest::Port::StructArgsPort& port
    )
  {
    Fw::SerializeStatus status;
    PortStruct s, sRef;

    status = this->structBuf.deserialize(s);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = this->structBuf.deserialize(sRef);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    ASSERT_EQ(s, port.args.s);
    ASSERT_EQ(sRef, port.args.sRef);
  }

  void Tester ::
    check_serial(
      FppTest::Port::SerialArgsPort& port
    )
  {
    ASSERT_EQ(this->serialBuf, port.args.buf);
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
        const PortEnum &en,
        PortEnum &enRef
    )
  {
    this->pushFromPortEntry_enumArgsOut(en, enRef);
  }

  PortEnum Tester ::
    from_enumReturnOut_handler(
        const NATIVE_INT_TYPE portNum,
        const PortEnum &en,
        PortEnum &enRef
    )
  {
    this->pushFromPortEntry_enumReturnOut(en, enRef);

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
    Fw::SerializeStatus status;

    switch (portNum) {
      case SerialPortIndex::NO_ARGS:
        status = Fw::FW_SERIALIZE_OK;
        break;

      case SerialPortIndex::PRIMITIVE:
        status = Buffer.copyRaw(
          this->primitiveBuf,
          Buffer.getBuffCapacity()
        );
        break;

      case SerialPortIndex::STRING:
        status = Buffer.copyRaw(
          this->stringBuf,
          Buffer.getBuffCapacity()
        );
        break;

      case SerialPortIndex::ENUM:
        status = Buffer.copyRaw(
          this->enumBuf,
          Buffer.getBuffCapacity()
        );
        break;

      case SerialPortIndex::ARRAY:
        status = Buffer.copyRaw(
          this->arrayBuf, 
          Buffer.getBuffCapacity()
        );
        break;

      case SerialPortIndex::STRUCT:
        status = Buffer.copyRaw(
          this->structBuf,
          Buffer.getBuffCapacity()
        );
        break;

      case SerialPortIndex::SERIAL:
        status = Buffer.copyRaw(
          this->serialBuf,
          Buffer.getBuffCapacity()
        );
        break;
    }

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
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
    this->component.set_enumArgsOut_OutputPort(
        TypedPortIndex::TYPED,
        this->get_from_enumArgsOut(TypedPortIndex::TYPED)
    );

    // enumReturnOut
    this->component.set_enumReturnOut_OutputPort(
        0,
        this->get_from_enumReturnOut(0)
    );

    // noArgsOut
    this->component.set_noArgsOut_OutputPort(
        TypedPortIndex::TYPED,
        this->get_from_noArgsOut(TypedPortIndex::TYPED)
    );

    // noArgsReturnOut
    this->component.set_noArgsReturnOut_OutputPort(
        0,
        this->get_from_noArgsReturnOut(0)
    );

    // primitiveArgsOut
    this->component.set_primitiveArgsOut_OutputPort(
        TypedPortIndex::TYPED,
        this->get_from_primitiveArgsOut(TypedPortIndex::TYPED)
    );

    // primitiveReturnOut
    this->component.set_primitiveReturnOut_OutputPort(
        0,
        this->get_from_primitiveReturnOut(0)
    );

    // stringArgsOut
    this->component.set_stringArgsOut_OutputPort(
        TypedPortIndex::TYPED,
        this->get_from_stringArgsOut(TypedPortIndex::TYPED)
    );

    // structArgsOut
    this->component.set_structArgsOut_OutputPort(
        TypedPortIndex::TYPED,
        this->get_from_structArgsOut(TypedPortIndex::TYPED)
    );

    // structReturnOut
    this->component.set_structReturnOut_OutputPort(
        0,
        this->get_from_structReturnOut(0)
    );


  // ----------------------------------------------------------------------
  // Connect serial output ports
  // ----------------------------------------------------------------------
    this->component.set_noArgsOut_OutputPort(
      TypedPortIndex::SERIAL,
      this->get_from_serialOut(SerialPortIndex::NO_ARGS)
    );

    this->component.set_primitiveArgsOut_OutputPort(
      TypedPortIndex::SERIAL,
      this->get_from_serialOut(SerialPortIndex::PRIMITIVE)
    );

    this->component.set_stringArgsOut_OutputPort(
      TypedPortIndex::SERIAL,
      this->get_from_serialOut(SerialPortIndex::STRING)
    );

    this->component.set_enumArgsOut_OutputPort(
      TypedPortIndex::SERIAL,
      this->get_from_serialOut(SerialPortIndex::ENUM)
    );

    this->component.set_arrayArgsOut_OutputPort(
      TypedPortIndex::SERIAL,
      this->get_from_serialOut(SerialPortIndex::ARRAY)
    );

    this->component.set_structArgsOut_OutputPort(
      TypedPortIndex::SERIAL,
      this->get_from_serialOut(SerialPortIndex::STRUCT)
    );

    this->component.set_serialOut_OutputPort(
      SerialPortIndex::NO_ARGS,
      this->get_from_noArgsOut(TypedPortIndex::SERIAL)
    );

    this->component.set_serialOut_OutputPort(
      SerialPortIndex::PRIMITIVE,
      this->get_from_primitiveArgsOut(TypedPortIndex::SERIAL)
    );

    this->component.set_serialOut_OutputPort(
      SerialPortIndex::STRING,
      this->get_from_stringArgsOut(TypedPortIndex::SERIAL)
    );

    this->component.set_serialOut_OutputPort(
      SerialPortIndex::ENUM,
      this->get_from_enumArgsOut(TypedPortIndex::SERIAL)
    );

    this->component.set_serialOut_OutputPort(
      SerialPortIndex::ARRAY,
      this->get_from_arrayArgsOut(TypedPortIndex::SERIAL)
    );

    this->component.set_serialOut_OutputPort(
      SerialPortIndex::STRUCT,
      this->get_from_structArgsOut(TypedPortIndex::SERIAL)
    );

    this->component.set_serialOut_OutputPort(
      SerialPortIndex::SERIAL,
      this->get_from_serialOut(SerialPortIndex::SERIAL)
    );


  // ----------------------------------------------------------------------
  // Connect serial input ports
  // ----------------------------------------------------------------------
    for (NATIVE_INT_TYPE i = 0; i < 7; ++i) {
      this->connect_to_serialIn(
          i,
          this->component.get_serialIn_InputPort(i)
      );
    }

  }

  void Tester ::
    initComponents()
  {
    this->init();
    this->component.init(
        INSTANCE
    );
  }

  void Tester ::
    checkSerializeStatusSuccess()
  {
    ASSERT_EQ(
      component.serializeStatus, 
      Fw::FW_SERIALIZE_OK
    );
  }

  void Tester ::
    checkSerializeStatusBufferEmpty()
  {
    ASSERT_EQ(
      component.serializeStatus, 
      Fw::FW_DESERIALIZE_BUFFER_EMPTY
    );
  }

