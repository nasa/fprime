// ======================================================================
// \title  LinuxSpiDriverImpl.cpp
// \author tcanham
// \brief  cpp file for LinuxSpiDriver component implementation class
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

#include <Drv/LinuxSpiDriver/LinuxSpiDriverComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Drv {

    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    LinuxSpiDriverComponentImpl::
#if FW_OBJECT_NAMES == 1
    LinuxSpiDriverComponentImpl(const char * const compName) :
            LinuxSpiDriverComponentBase(compName)
#else
    LinuxSpiDriverImpl(void)
#endif
    ,m_fd(-1),m_device(-1),m_select(-1),m_bytes(0)
    {

    }

    void LinuxSpiDriverComponentImpl::init(const NATIVE_INT_TYPE instance) {
        LinuxSpiDriverComponentBase::init(instance);
    }


} // end namespace Drv
