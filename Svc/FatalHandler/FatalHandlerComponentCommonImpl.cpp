// ====================================================================== 
// \title  FatalHandlerImpl.cpp
// \author tcanham
// \brief  cpp file for FatalHandler component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 


#include <Svc/FatalHandler/FatalHandlerComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  FatalHandlerComponentImpl ::
#if FW_OBJECT_NAMES == 1
    FatalHandlerComponentImpl(
        const char *const compName
    ) :
      FatalHandlerComponentBase(compName)
#else
    FatalHandlerImpl(void)
#endif
  {

  }

  void FatalHandlerComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    ) 
  {
    FatalHandlerComponentBase::init(instance);
  }

  FatalHandlerComponentImpl ::
    ~FatalHandlerComponentImpl(void)
  {

  }

} // end namespace Svc
