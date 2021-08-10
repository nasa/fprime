// ======================================================================
// \title  LinuxTimerImpl.cpp
// \author tim
// \brief  cpp file for LinuxTimer component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Svc/LinuxTimer/LinuxTimerComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  LinuxTimerComponentImpl ::
    LinuxTimerComponentImpl(
        const char *const compName
    ) : LinuxTimerComponentBase(compName),
        m_quit(false)
  {

  }

  void LinuxTimerComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    LinuxTimerComponentBase::init(instance);
  }

  LinuxTimerComponentImpl ::
    ~LinuxTimerComponentImpl()
  {

  }

  void LinuxTimerComponentImpl::quit() {
      this->m_quit = true;
  }

} // end namespace Svc
