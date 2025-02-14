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

#ifndef DRV_LINUX_GPIO_DRIVER_HPP
#define DRV_LINUX_GPIO_DRIVER_HPP

#include <Os/File.hpp>
#include <Os/Mutex.hpp>
#include <Os/Task.hpp>
#include "Drv/LinuxGpioDriver/LinuxGpioDriverComponentAc.hpp"

namespace Drv {

class LinuxGpioDriver : public LinuxGpioDriverComponentBase {
  public:
    static constexpr FwSizeType GPIO_POLL_TIMEOUT = 500;  // Timeout looking for interrupts to check for shutdown
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object LinuxGpioDriver
    //!
    LinuxGpioDriver(const char* const compName /*!< The component name*/
    );

    //! Destroy object LinuxGpioDriver
    //!
    ~LinuxGpioDriver();

    //! \brief configure the GPIO pin
    //!
    //! Configure the GPIO pin for use in this driver. Only one mode may be selected as a time.
    enum GpioConfiguration {
        GPIO_OUTPUT,                                   //!< Output GPIO pin for direct writing
        GPIO_INPUT,                                    //!< Input GPIO pin for direct reading
        GPIO_INTERRUPT_RISING_EDGE,                    //!< Input GPIO pin triggers interrupt port on rising edge
        GPIO_INTERRUPT_FALLING_EDGE,                   //!< Input GPIO pin triggers interrupt port on falling edge
        GPIO_INTERRUPT_BOTH_RISING_AND_FALLING_EDGES,  //!< Input GPIO pin triggers interrupt port on both edges
        MAX_GPIO_CONFIGURATION
    };

    //! \brief open a GPIO pin for use in the system
    //!
    //! This function opens and configures a GPIO pin. User must supply the device path and the gpio pin registered to
    //! that device. Pin configuration is also accepted and supports: input, output, and interrupts on either edge.
    //!
    //! If the user selects output, a default state can be set with the default_state flag.
    //!
    //! \param device: /dev/gpiochip* path for this pin's bank
    //! \param gpio: pin or line number on the above chip
    //! \param configuration: pin configuration
    //! \param default_state: default state when using output configuration
    //! \return status of the gpio open OP_OK on success, something else on error
    Os::File::Status open(const char* device,
                          const U32 gpio,
                          const GpioConfiguration& configuration,
                          const Fw::Logic& default_state = Fw::Logic::LOW);

    //! \brief start interrupt detection thread
    //!
    Drv::GpioStatus start(const FwSizeType priority = Os::Task::TASK_DEFAULT,
                          const FwSizeType stackSize = Os::Task::TASK_DEFAULT,
                          const FwSizeType cpuAffinity = Os::Task::TASK_DEFAULT,
                          const PlatformUIntType identifier = static_cast<PlatformUIntType>(Os::Task::TASK_DEFAULT));

    //! \brief stop interrupt detection thread
    //!
    void stop();

    //! \brief join interrupt detection thread
    //!
    void join();

  PRIVATE:
    //! \brief helper to setup a line handle (read or write).
    Os::File::Status setupLineHandle(const PlatformIntType chip_descriptor,
                                     const U32 gpio,
                                     const GpioConfiguration& configuration,
                                     const Fw::Logic& default_state,
                                     PlatformIntType& fd);

    //! \brief helper to setup a line event (interrupt)
    Os::File::Status setupLineEvent(const PlatformIntType chip_descriptor,
                                    const U32 gpio,
                                    const GpioConfiguration& configuration,
                                    PlatformIntType& fd);

    //! \brief poll for interrupt loop helper
    //!
    void pollLoop();

    //! \brief helper to get running state
    //!
    bool getRunning();

    //! \brief interrupt function
    //!
    static void interruptFunction(void* self);

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for gpioRead
    //!
    Drv::GpioStatus gpioRead_handler(const FwIndexType portNum, /*!< The port number*/
                                     Fw::Logic& state);

    //! Handler implementation for gpioWrite
    //!
    Drv::GpioStatus gpioWrite_handler(const FwIndexType portNum, /*!< The port number*/
                                      const Fw::Logic& state);
    //! Task to run interrupt polling
    Os::Task m_poller;

    //! Mutex for locking m_running state
    Os::Mutex m_lock;

    //! Pin configuration
    GpioConfiguration m_configuration = GpioConfiguration::MAX_GPIO_CONFIGURATION;

    //! File descriptor for GPIO
    PlatformIntType m_fd = -1;

    //! Determine if the interrupt polling thread is running
    bool m_running = false;
};

}  // end namespace Drv

#endif  // DRV_LINUX_GPIO_DRIVER_HPP
