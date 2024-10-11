// ======================================================================
// \title  LinuxGpioDriver.hpp
// \author lestarch
// \brief  hpp file for LinuxGpioDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef LinuxGpioDriver_HPP
#define LinuxGpioDriver_HPP

#include "Drv/LinuxGpioDriver/LinuxGpioDriverComponentAc.hpp"
#include <Os/Task.hpp>
#include <Os/File.hpp>

namespace Drv {

  class LinuxGpioDriver :
    public LinuxGpioDriverComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object LinuxGpioDriver
      //!
      LinuxGpioDriver(
          const char *const compName /*!< The component name*/
      );

      //! Destroy object LinuxGpioDriver
      //!
      ~LinuxGpioDriver();

      //! \brief configure the GPIO pin
      //!
      //! Configure the GPIO pin for use in this driver. Only one mode may be selected as a time.
      enum GpioConfiguration {
          GPIO_OUTPUT, //!< Output GPIO pin for direct writing
          GPIO_INPUT, //!< Input GPIO pin for direct reading without altering the existing electrical configuration
          GPIO_INPUT_PULL_HIGH, //!< Input GPIO pin for direct reading pulling unconnected values high
          GPIO_INPUT_PULL_LOW, //!< Input GPIO pin for direct reading pulling unconnected values low
          GPIO_INTERRUPT_RISING_EDGE, //!< Input GPIO pin triggers interrupt port on rising edge
          GPIO_INTERRUPT_FALLING_EDGE, //!< Input GPIO pin triggers interrupt port on falling edge
      };

      //! open GPIO
      Os::File::Status open(const char* device, U32 gpio, GpioConfiguration direction, Fw::Logic default_state=Fw::Logic::LOW);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for gpioRead
      //!
      void gpioRead_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Logic &state
      );

      //! Handler implementation for gpioWrite
      //!
      void gpioWrite_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const Fw::Logic& state
      );
      //! Pin configuration
      GpioConfiguration m_configuration;

      //! Keep the chip
      Fw::String m_chip;

      //! keep GPIO ID
      FwSizeType m_gpio;
      //! file descriptor for GPIO
      PlatformIntType m_fd;
    };

} // end namespace Drv

#endif
