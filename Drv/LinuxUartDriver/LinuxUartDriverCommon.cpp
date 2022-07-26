/*
 * LinuxUartDriverCommon.cpp
 *
 *  Created on: Nov 29, 2016
 *      Author: tcanham
 */

#include <Drv/LinuxUartDriver/LinuxUartDriver.hpp>

namespace Drv {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

LinuxUartDriver ::LinuxUartDriver(const char* const compName)
    : LinuxUartDriverComponentBase(compName), m_fd(-1), m_device("NOT_EXIST"), m_quitReadThread(false) {
}

void LinuxUartDriver ::init(const NATIVE_INT_TYPE instance) {
    LinuxUartDriverComponentBase::init(instance);
}

}  // namespace Drv
