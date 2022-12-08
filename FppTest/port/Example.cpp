// ======================================================================
// \title  Example.cpp
// \author tiffany
// \brief  cpp file for Example component implementation class
// ======================================================================

#include "Fw/Types/SerialBuffer.hpp"

#include "FppTest/port/Example.hpp"
#include "FpConfig.hpp"


  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  Example ::
    Example(
        const char *const compName
    ) : ExampleComponentBase(compName)
  {

  }

  void Example ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    ExampleComponentBase::init(instance);
  }

  Example ::
    ~Example()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void Example ::
    arrayArgsIn_handler(
        const NATIVE_INT_TYPE portNum,
        const PortArray &a,
        PortArray &aRef
    )
  {
    this->arrayArgsOut_out(portNum, a, aRef);
  }

  void Example ::
    arrayArgsToSerial_handler(
        const NATIVE_INT_TYPE portNum,
        const PortArray &a,
        PortArray &aRef
    )
  {
    U8 data[PortArray::SERIALIZED_SIZE * 2];
    Fw::SerialBuffer buf(data, sizeof(data));

    buf.serialize(a);
    buf.serialize(aRef);

    this->serialOut_out(portNum, buf);
  }

  PortArray Example ::
    arrayReturnIn_handler(
        const NATIVE_INT_TYPE portNum,
        const PortArray &a,
        PortArray &aRef
    )
  {
    return this->arrayReturnOut_out(portNum, a, aRef);
  }

  void Example ::
    enumArgsIn_handler(
        const NATIVE_INT_TYPE portNum,
        const PortEnum &e,
        PortEnum &eRef
    )
  {
    this->enumArgsOut_out(portNum, e, eRef);
  }

  void Example ::
    enumArgsToSerial_handler(
        const NATIVE_INT_TYPE portNum,
        const PortEnum &e,
        PortEnum &eRef
    )
  {
    // TODO
  }

  PortEnum Example ::
    enumReturnIn_handler(
        const NATIVE_INT_TYPE portNum,
        const PortEnum &e,
        PortEnum &eRef
    )
  {
    return this->enumReturnOut_out(portNum, e, eRef);
  }

  void Example ::
    noArgsIn_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    this->noArgsOut_out(portNum);
  }

  bool Example ::
    noArgsReturnIn_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    return this->noArgsReturnOut_out(portNum);
  }

  void Example ::
    noArgsToSerial_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    // TODO
  }

  void Example ::
    primitiveArgsIn_handler(
        const NATIVE_INT_TYPE portNum,
        U32 u32,
        U32 &u32Ref,
        F32 f32,
        F32 &f32Ref,
        bool b,
        bool &bRef
    )
  {
    this->primitiveArgsOut_out(
      portNum, 
      u32, 
      u32Ref, 
      f32, 
      f32Ref, 
      b, 
      bRef
    );
  }

  void Example ::
    primitiveArgsToSerial_handler(
        const NATIVE_INT_TYPE portNum,
        U32 u32,
        U32 &u32Ref,
        F32 f32,
        F32 &f32Ref,
        bool b,
        bool &bRef
    )
  {
    // TODO
  }

  U32 Example ::
    primitiveReturnIn_handler(
        const NATIVE_INT_TYPE portNum,
        U32 u32,
        U32 &u32Ref,
        F32 f32,
        F32 &f32Ref,
        bool b,
        bool &bRef
    )
  {
    return this->primitiveReturnOut_out(
      portNum, 
      u32, 
      u32Ref, 
      f32, 
      f32Ref, 
      b, 
      bRef
    );
  }

  void Example ::
    stringArgsIn_handler(
        const NATIVE_INT_TYPE portNum,
        const str80String &str80,
        str80RefString &str80Ref,
        const str100String &str100,
        str100RefString &str100Ref
    )
  {
    this->stringArgsOut_out(
      portNum,
      str80,
      str80Ref,
      str100,
      str100Ref
    );
  }

  void Example ::
    stringArgsToSerial_handler(
        const NATIVE_INT_TYPE portNum,
        const str80String &str80,
        str80RefString &str80Ref,
        const str100String &str100,
        str100RefString &str100Ref
    )
  {
    // TODO
  }

  void Example ::
    structArgsIn_handler(
        const NATIVE_INT_TYPE portNum,
        const PortStruct &s,
        PortStruct &sRef
    )
  {
    this->structArgsOut_out(portNum, s, sRef);
  }

  void Example ::
    structArgsToSerial_handler(
        const NATIVE_INT_TYPE portNum,
        const PortStruct &s,
        PortStruct &sRef
    )
  {
    // TODO
  }

  PortStruct Example ::
    structReturnIn_handler(
        const NATIVE_INT_TYPE portNum,
        const PortStruct &s,
        PortStruct &sRef
    )
  {
    return this->structReturnOut_out(portNum, s, sRef);
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined serial input ports
  // ----------------------------------------------------------------------

  void Example ::
    serialToArrayArgs_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    PortArray a, aRef;

    Buffer.deserialize(a);
    Buffer.deserialize(aRef);

    this->arrayArgsOut_out(portNum, a, aRef);
  }

  void Example ::
    serialToEnumArgs_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    PortEnum e, eRef;

    Buffer.deserialize(e);
    Buffer.deserialize(eRef);

    this->enumArgsOut_out(portNum, e, eRef);
  }

  void Example ::
    serialToNoArgs_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    this->noArgsOut_out(portNum);
  }

  void Example ::
    serialToPrimitiveArgs_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    U32 u32, u32Ref;
    F32 f32, f32Ref;
    bool b, bRef;

    Buffer.deserialize(u32);
    Buffer.deserialize(u32Ref);
    Buffer.deserialize(f32);
    Buffer.deserialize(f32Ref);
    Buffer.deserialize(b);
    Buffer.deserialize(bRef);

    this->primitiveArgsOut_out(
      portNum,
      u32,
      u32Ref,
      f32,
      f32Ref,
      b,
      bRef
    );
  }

  void Example ::
    serialToSerial_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    // TODO
  }

  void Example ::
    serialToStringArgs_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    StringArgsPortStrings::StringSize80 str80, str80Ref;
    StringArgsPortStrings::StringSize100 str100, str100Ref;

    Buffer.deserialize(str80);
    Buffer.deserialize(str80Ref);
    Buffer.deserialize(str100);
    Buffer.deserialize(str100Ref);

    this->stringArgsOut_out(portNum, str80, str80Ref, str100, str100Ref);
  }

  void Example ::
    serialToStructArgs_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    PortStruct s, sRef;

    Buffer.deserialize(s);
    Buffer.deserialize(sRef);

    this->structArgsOut_out(portNum, s, sRef);
  }

