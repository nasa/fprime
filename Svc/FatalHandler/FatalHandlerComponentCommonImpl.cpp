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
#include <FpConfig.hpp>

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  FatalHandlerComponentImpl ::
    FatalHandlerComponentImpl(
        const char *const compName
    ) : FatalHandlerComponentBase(compName)
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
    ~FatalHandlerComponentImpl()
  {

  }

} // end namespace Svc
