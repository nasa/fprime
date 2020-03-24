// ======================================================================
// \title  LinuxI2CDriverComponentImpl.cpp
// \author aaron
// \brief  cpp file for LinuxI2CDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <RPI/LinuxI2CDriver/LinuxI2CDriverComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

#define I2C_FILE (char*)"/dev/i2c-1"


int init_address(int addr) {
  int fd = open(I2C_FILE, O_CLOEXEC | O_RDWR);
  FW_ASSERT(fd >= 0);
  int ioctlRes = ioctl(fd, I2C_SLAVE, addr);
  FW_ASSERT(ioctlRes >= 0);
  return fd;
}


namespace RPI {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  LinuxI2CDriverComponentImpl ::
#if FW_OBJECT_NAMES == 1
    LinuxI2CDriverComponentImpl(
        const char *const compName
    ) :
      LinuxI2CDriverComponentBase(compName)
#else
    LinuxI2CDriverComponentImpl(void)
#endif
  {

  }

  void LinuxI2CDriverComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    LinuxI2CDriverComponentBase::init(instance);
  }

  LinuxI2CDriverComponentImpl ::
    ~LinuxI2CDriverComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void LinuxI2CDriverComponentImpl ::
    I2CWrite_handler(
        const NATIVE_INT_TYPE portNum,
        U32 address,
        Fw::Buffer &writeBuffer
    )
  {
    U32 fd = init_address(address);
    U32 writeRes = write(fd, (void*)writeBuffer.getdata(), writeBuffer.getsize());
    FW_ASSERT(writeRes == writeBuffer.getsize());
  }

  void LinuxI2CDriverComponentImpl ::
    I2CRead_handler(
        const NATIVE_INT_TYPE portNum,
        U32 address,
        Fw::Buffer &readBuffer,
        U32 length
    )
  {
    U32 fd = init_address(address);
    U32 readRes = read(fd, (void*)readBuffer.getdata(), length);
    FW_ASSERT(readRes == length);
  }

} // end namespace RPI
