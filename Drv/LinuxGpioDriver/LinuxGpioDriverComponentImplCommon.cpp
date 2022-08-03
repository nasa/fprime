// ======================================================================
// \title  LinuxGpioDriverImpl.cpp
// \author tcanham
// \brief  cpp file for LinuxGpioDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Drv/LinuxGpioDriver/LinuxGpioDriverComponentImpl.hpp>
#include <FpConfig.hpp>

namespace Drv {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  LinuxGpioDriverComponentImpl ::
    LinuxGpioDriverComponentImpl(
        const char *const compName
    ) : LinuxGpioDriverComponentBase(compName),
      m_gpio(-1),
      m_direction(GPIO_IN),
      m_fd(-1),
      m_quitThread(false)
  {

  }

  void LinuxGpioDriverComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    LinuxGpioDriverComponentBase::init(instance);
  }

} // end namespace Drv
