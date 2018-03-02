// ======================================================================
// \title  LinuxGpioDriverImpl.cpp
// \author tcanham
// \brief  cpp file for LinuxGpioDriver component implementation class
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


#include <Drv/LinuxGpioDriver/LinuxGpioDriverComponentImpl.hpp>
#include <Fw/Types/BasicTypes.hpp>

namespace Drv {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  LinuxGpioDriverComponentImpl ::
#if FW_OBJECT_NAMES == 1
    LinuxGpioDriverComponentImpl(
        const char *const compName
    ) :
      LinuxGpioDriverComponentBase(compName)
#else
    LinuxGpioDriverImpl(void)
#endif
    ,m_gpio(-1)
    ,m_direction(GPIO_IN)
    ,m_fd(-1)
    ,m_quitThread(false)
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
