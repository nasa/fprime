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
#include <Fw/FPrimeBasicTypes.hpp>

namespace Drv {

bool LinuxSpiDriverComponentImpl::open(FwIndexType device, FwIndexType select, SpiFrequency clock, SpiMode spiMode) {
    return false;
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

SpiStatus LinuxSpiDriverComponentImpl::SpiWriteRead_handler(const FwIndexType portNum,
                                                            Fw::Buffer& WriteBuffer,
                                                            Fw::Buffer& readBuffer) {
    return SpiStatus::SPI_OK;
}

// @ DEPRECATED: Use SpiWriteRead port instead (same operation with a return value)
void LinuxSpiDriverComponentImpl::SpiReadWrite_handler(const FwIndexType portNum,
                                                       Fw::Buffer& WriteBuffer,
                                                       Fw::Buffer& readBuffer) {}

LinuxSpiDriverComponentImpl::~LinuxSpiDriverComponentImpl() {}

}  // end namespace Drv
