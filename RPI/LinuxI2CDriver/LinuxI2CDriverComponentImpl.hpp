// ======================================================================
// \title  LinuxI2CDriverComponentImpl.hpp
// \author aaron
// \brief  hpp file for LinuxI2CDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef LinuxI2CDriver_HPP
#define LinuxI2CDriver_HPP

#include "RPI/LinuxI2CDriver/LinuxI2CDriverComponentAc.hpp"

#include "Fw/Types/Assert.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <errno.h>

#if defined(__linux__) || defined(__CYGWIN__)
# include<linux/i2c-dev.h>
#else
# warning Platform not supported: defining I2C_SLAVE value as 0x073 (based on source)
#define I2C_SLAVE 0x073
#endif


namespace RPI {

  class LinuxI2CDriverComponentImpl :
    public LinuxI2CDriverComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object LinuxI2CDriver
      //!
      LinuxI2CDriverComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object LinuxI2CDriver
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object LinuxI2CDriver
      //!
      ~LinuxI2CDriverComponentImpl(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for I2CWrite
      //!
      void I2CWrite_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 address, 
          Fw::Buffer &writeBuffer 
      );

      //! Handler implementation for I2CRead
      //!
      void I2CRead_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 address, 
          Fw::Buffer &readBuffer, 
          U32 length 
      );


    };

} // end namespace RPI

#endif
