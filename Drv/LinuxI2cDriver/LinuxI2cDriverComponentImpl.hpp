// ======================================================================
// \title  LinuxI2cDriverComponentImpl.hpp
// \author tcanham
// \brief  hpp file for LinuxI2cDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef LinuxI2cDriver_HPP
#define LinuxI2cDriver_HPP

#include "Drv/LinuxI2cDriver/LinuxI2cDriverComponentAc.hpp"

namespace Drv {

  class LinuxI2cDriverComponentImpl :
    public LinuxI2cDriverComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object LinuxI2cDriver
      //!
      LinuxI2cDriverComponentImpl(const char *const compName);

      //! Initialize object LinuxI2cDriver
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      bool open(const char* device);
      //! Destroy object LinuxI2cDriver
      //!
      ~LinuxI2cDriverComponentImpl(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for write
      //!
      I2cStatus write_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 addr, 
          Fw::Buffer &serBuffer 
      );

      //! Handler implementation for read
      //!
      I2cStatus read_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 addr, 
          Fw::Buffer &serBuffer 
      );

      NATIVE_INT_TYPE m_fd; //!< i2c file descriptor


    };

} // end namespace Drv

#endif
