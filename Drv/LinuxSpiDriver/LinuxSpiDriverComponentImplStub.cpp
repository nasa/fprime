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

    bool LinuxSpiDriverComponentImpl::open(NATIVE_INT_TYPE device,
                                           NATIVE_INT_TYPE select,
                                           SpiFrequency clock) {
        //TODO: fill this function out
        return false;
    }

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    void LinuxSpiDriverComponentImpl::SpiReadWrite_handler(
            const NATIVE_INT_TYPE portNum, Fw::Buffer &WriteBuffer,
            Fw::Buffer &readBuffer) {
        // TODO
    }

    LinuxSpiDriverComponentImpl::~LinuxSpiDriverComponentImpl() {

    }

} // end namespace Drv
