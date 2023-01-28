// ======================================================================
// \title  Example.cpp
// \author T. Chieu
// \brief  cpp file for Example component implementation class
// ======================================================================


#include <Example.hpp>
#include <FpConfig.hpp>


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
    structArgsIn_handler(
        const NATIVE_INT_TYPE portNum,
        const PortStruct &s,
        PortStruct &sRef
    )
  {
    this->structArgsOut_out(portNum, s, sRef);
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
    serialIn_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    this->serializeStatus = this->serialOut_out(portNum, Buffer);
  }

