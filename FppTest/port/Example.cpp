// ======================================================================
// \title  Example.cpp
// \author joshuaa
// \brief  cpp file for Example component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include "FppTest/port/no_arg/Example.hpp"
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
  noArgsIn_handler(
      const NATIVE_INT_TYPE portNum
  )
{
}

void Example ::
  noArgsOut_handler(
      const NATIVE_INT_TYPE portNum
  )
{
}

U32 Example ::
  returnU32In_handler(
      const NATIVE_INT_TYPE portNum
  )
{
  return 0;
}

U32 Example ::
  returnU32Out_handler(
      const NATIVE_INT_TYPE portNum
  )
{
  return 0;
}
