// ====================================================================== 
// \title  FatalHandlerImpl.cpp
// \author tcanham
// \brief  cpp file for FatalHandler component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
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
