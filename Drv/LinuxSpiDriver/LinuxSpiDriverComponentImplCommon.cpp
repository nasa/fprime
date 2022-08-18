// ======================================================================
// \title  LinuxSpiDriverImpl.cpp
// \author tcanham
// \brief  cpp file for LinuxSpiDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Drv/LinuxSpiDriver/LinuxSpiDriverComponentImpl.hpp>
#include <FpConfig.hpp>

namespace Drv {

    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    LinuxSpiDriverComponentImpl::
    LinuxSpiDriverComponentImpl(const char * const compName) :
        LinuxSpiDriverComponentBase(compName),
        m_fd(-1),
        m_device(-1),
        m_select(-1),
        m_bytes(0)
    {

    }

    void LinuxSpiDriverComponentImpl::init(const NATIVE_INT_TYPE instance) {
        LinuxSpiDriverComponentBase::init(instance);
    }


} // end namespace Drv
