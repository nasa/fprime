// ======================================================================
// \title  Example.cpp
// \author tiffany
// \brief  cpp file for Example component implementation class
// ======================================================================


#include <FppTest/port/Example.hpp>
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
    this->primitiveArgsOut_out(portNum, u32, u32Ref, f32, f32Ref, b, bRef);
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
    return this->primitiveReturnOut_out(portNum, u32, u32Ref, f32, f32Ref, b, bRef);
  }

