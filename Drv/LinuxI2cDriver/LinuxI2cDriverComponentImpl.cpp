// ======================================================================
// \title  LinuxI2cDriverComponentImpl.cpp
// \author tcanham
// \brief  cpp file for LinuxI2cDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Drv/LinuxI2cDriver/LinuxI2cDriverComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include "Fw/Types/Assert.hpp"

#include <unistd.h> // required for I2C device access
#include <fcntl.h>  // required for I2C device configuration
#include <sys/ioctl.h> // required for I2C device usage
#include <linux/i2c-dev.h> // required for constant definitions
#include <stdio.h>  // required for printf statements
#include <errno.h>

#define DEBUG_PRINT 0

namespace Drv {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  LinuxI2cDriverComponentImpl ::
    LinuxI2cDriverComponentImpl(
        const char *const compName
    ) : LinuxI2cDriverComponentBase(compName),
        m_fd(-1)
  {

  }

  void LinuxI2cDriverComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    LinuxI2cDriverComponentBase::init(instance);
  }

  LinuxI2cDriverComponentImpl ::
    ~LinuxI2cDriverComponentImpl(void)
  {
    if (-1 != this->m_fd) { // check if file is open
      ::close(this->m_fd);
    }
  }

  bool LinuxI2cDriverComponentImpl::open(const char* device) {
      FW_ASSERT(device);
      this->m_fd = ::open(device, O_RDWR);
      return (-1 != this->m_fd);
  }


  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  // Note this this port handler is guarded, so we can make the ioctl call

  Drv::I2cStatus LinuxI2cDriverComponentImpl ::
    write_handler(
        const NATIVE_INT_TYPE portNum,
        U32 addr,
        Fw::Buffer &serBuffer
    )
  {
      // Make sure file has been opened
      FW_ASSERT(-1 != this->m_fd);

#if DEBUG_PRINT
      printf("I2c addr: 0x%02X\n",addr);
      for (U32 byte = 0; byte < serBuffer.getSize(); byte++) {
    	  printf("0x%02X ",serBuffer.getData()[byte]);

      }
      printf("\n");
#endif
      // select slave address
      int stat = ioctl(this->m_fd, I2C_SLAVE, addr);
      if (stat == -1) {
#if DEBUG_PRINT
          printf("Status: %d Errno: %d\n", stat, errno);
#endif
	  return I2cStatus::I2C_ADDRESS_ERR;
      }
      // make sure it isn't a null pointer
      FW_ASSERT(serBuffer.getData());
      // write data
      stat = write(this->m_fd, serBuffer.getData(), serBuffer.getSize());
      if (stat == -1) {
#if DEBUG_PRINT
          printf("Status: %d Errno: %d\n", stat, errno);
#endif
	  return I2cStatus::I2C_WRITE_ERR;
      }
      return I2cStatus::I2C_OK;
  }

  Drv::I2cStatus LinuxI2cDriverComponentImpl ::
    read_handler(
        const NATIVE_INT_TYPE portNum,
        U32 addr,
        Fw::Buffer &serBuffer
    )
  {
      // Make sure file has been opened
      FW_ASSERT(-1 != this->m_fd);

#if DEBUG_PRINT
      printf("I2c addr: 0x%02X\n",addr);
#endif
      // select slave address
      int stat = ioctl(this->m_fd, I2C_SLAVE, addr);
      if (stat == -1) {
#if DEBUG_PRINT
          printf("Status: %d Errno: %d\n", stat, errno);
#endif
	  return I2cStatus::I2C_ADDRESS_ERR;
      }
      // make sure it isn't a null pointer
      FW_ASSERT(serBuffer.getData());
      // read data
      stat = read(this->m_fd, serBuffer.getData(), serBuffer.getSize());
      if (stat == -1) {
#if DEBUG_PRINT
          printf("Status: %d Errno: %d\n", stat, errno);
#endif
	  return I2cStatus::I2C_READ_ERR;
      }
#if DEBUG_PRINT
      for (U32 byte = 0; byte < serBuffer.getSize(); byte++) {
    	  printf("0x%02X ",serBuffer.getData()[byte]);

      }
      printf("\n");
#endif
      return I2cStatus::I2C_OK;
  }

} // end namespace Drv
