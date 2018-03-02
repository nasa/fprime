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

    void LinuxSpiDriverComponentImpl::open(NATIVE_INT_TYPE device,
                                           NATIVE_INT_TYPE select,
                                           SpiFrequency clock) {

    }

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    void LinuxSpiDriverComponentImpl::SpiReadWrite_handler(
            const NATIVE_INT_TYPE portNum, Fw::Buffer &WriteBuffer,
            Fw::Buffer &readBuffer) {
        // TODO
    }

    LinuxSpiDriverComponentImpl::~LinuxSpiDriverComponentImpl(void) {

    }

} // end namespace Drv
