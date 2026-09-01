// ======================================================================
// \title  LinuxUartDriver.hpp
// \brief  Deprecated compatibility header for Drv::PosixUartDriver
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef LinuxUartDriver_HPP
#define LinuxUartDriver_HPP

#include <Drv/PosixUartDriver/PosixUartDriver.hpp>

namespace Drv {

//! Deprecated alias for backwards compatibility. Use Drv::PosixUartDriver.
using LinuxUartDriver [[deprecated("Drv::LinuxUartDriver was renamed to Drv::PosixUartDriver")]] = PosixUartDriver;

}  // end namespace Drv

#endif
