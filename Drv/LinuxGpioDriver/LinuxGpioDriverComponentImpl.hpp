// ======================================================================
// \title  LinuxGpioDriverImpl.hpp
// \author tcanham
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

namespace Drv {

  class LinuxGpioDriverComponentImpl :
    public LinuxGpioDriverComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object LinuxGpioDriver
      //!
      LinuxGpioDriverComponentImpl(
          const char *const compName /*!< The component name*/
      );

      //! Initialize object LinuxGpioDriver
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object LinuxGpioDriver
      //!
      ~LinuxGpioDriverComponentImpl();

      //! Start interrupt task
      Os::Task::TaskStatus startIntTask(NATIVE_UINT_TYPE priority = Os::Task::TASK_DEFAULT, NATIVE_UINT_TYPE cpuAffinity = Os::Task::TASK_DEFAULT);

      //! configure GPIO
      enum GpioDirection {
          GPIO_IN, //!< input
          GPIO_OUT, //!< output
          GPIO_INT //!< interrupt
      };

      //! open GPIO
      bool open(NATIVE_INT_TYPE gpio, GpioDirection direction);

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
