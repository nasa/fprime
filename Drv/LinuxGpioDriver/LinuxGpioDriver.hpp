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

      //! Start interrupt task
      Os::Task::Status startIntTask(Os::Task::ParamType priority = Os::Task::TASK_DEFAULT,
                                    Os::Task::ParamType stackSize = Os::Task::TASK_DEFAULT,
                                    Os::Task::ParamType cpuAffinity = Os::Task::TASK_DEFAULT);



      //! \brief configure the GPIO pin
      //!
      //! Configure the GPIO pin for use in this driver. Only one mode may be selected as a time.
      enum GpioConfiguration {
          GPIO_INPUT, //!< Input GPIO pin for direct reading
          GPIO_OUTPUT, //!< Output GPIO pin for direct writing
          GPIO_AS_IS, //!< Input GPIO pin for reading without altering the existing electrical configuration
          GPIO_INTERRUPT, //!< Input GPIO pin triggering interrupt
      };

      //! open GPIO
      Os::File::Status open(const char* dev, FwSizeType gpio, GpioConfiguration direction);

      //! exit thread
      void exitThread();

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

      //! keep GPIO ID
      NATIVE_INT_TYPE m_gpio;

      //! device direction
      GpioDirection m_direction;

      //! Entry point for task waiting for interrupt
      static void intTaskEntry(void * ptr);

      //! Task object for RTI task
      Os::Task m_intTask;
      //! file descriptor for GPIO
      NATIVE_INT_TYPE m_fd;
      //! flag to quit thread
      bool m_quitThread;

    };

} // end namespace Drv

#endif
