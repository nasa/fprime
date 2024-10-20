// ======================================================================
// \title  LinuxGpioDriverImpl.cpp
// \author tcanham
// \brief  cpp file for LinuxGpioDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Drv/LinuxGpioDriver/LinuxGpioDriver.hpp>
#include <FpConfig.hpp>
#include <Fw/Types/String.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <Os/Posix/File.hpp>

#include <linux/gpio.h>      // GPIO ioctl definitions
#include <poll.h>            // poll() system call
#include <sys/ioctl.h>      // ioctl() system call
#include <unistd.h>         // close() and other POSIX functions
#include <cerrno>           // errno definitions

namespace Drv {

// ----------------------------------------------------------------------
// Utility function to convert errno values to Os::File::Status
// ----------------------------------------------------------------------
Os::File::Status errno_to_file_status(PlatformIntType errno_input) {
    Os::File::Status status = Os::File::Status::OTHER_ERROR; // Default to OTHER_ERROR
    switch (errno_input) {
        case 0:
            status = Os::File::Status::OP_OK; // No error
            break;
        case EBADF:
            status = Os::File::Status::NOT_OPENED; // Bad file descriptor
            break;
        case EINVAL:
            status = Os::File::Status::INVALID_ARGUMENT; // Invalid argument
            break;
        case ENODEV:
            status = Os::File::Status::DOESNT_EXIST; // No such device
            break;
        case ENOMEM:
            status = Os::File::Status::NO_SPACE; // Not enough memory
            break;
        case EPERM:
            status = Os::File::Status::NO_PERMISSION; // Operation not permitted
            break;
        case ENXIO:
            status = Os::File::Status::INVALID_MODE; // No such device or address
            break;
        // Cascades intended for other error cases
        case EFAULT:
        case EWOULDBLOCK:
        case EBUSY:
        case EIO:
        default:
            status = Os::File::Status::OTHER_ERROR;
            break;
    }
    return status;
}

// ----------------------------------------------------------------------
// Utility function to convert errno values to Drv::GpioStatus
// ----------------------------------------------------------------------
Drv::GpioStatus errno_to_gpio_status(PlatformIntType errno_input) {
    Drv::GpioStatus status = Drv::GpioStatus::T::UNKNOWN_ERROR; // Default to UNKNOWN_ERROR
    switch (errno_input) {
        case EBADF:
            status = Drv::GpioStatus::T::NOT_OPENED; // Bad file descriptor
            break;
        case ENXIO:
            status = Drv::GpioStatus::INVALID_MODE; // Invalid mode
            break;
        // Cascades intended for other error cases
        case EFAULT:
        case EINVAL:
        case EWOULDBLOCK:
        case EBUSY:
        case EIO:
        default:
            status = Drv::GpioStatus::T::UNKNOWN_ERROR;
            break;
    }
    return status;
}

// ----------------------------------------------------------------------
// Maps GpioConfiguration to GPIO handle request flags
// ----------------------------------------------------------------------
U32 configuration_to_handler_flags(Drv::LinuxGpioDriver::GpioConfiguration configuration) {
    U32 flags = 0;
    switch (configuration) {
        case LinuxGpioDriver::GPIO_OUTPUT:
            flags = GPIOHANDLE_REQUEST_OUTPUT; // Request output handle
            break;
        // Cascade intended: Input and various interrupt configurations use input flags
        case LinuxGpioDriver::GPIO_INPUT:
        case LinuxGpioDriver::GPIO_INTERRUPT_RISING_EDGE:
        case LinuxGpioDriver::GPIO_INTERRUPT_FALLING_EDGE:
        case LinuxGpioDriver::GPIO_INTERRUPT_BOTH_RISING_AND_FALLING_EDGES:
            flags = GPIOHANDLE_REQUEST_INPUT; // Request input handle
            break;
        default:
            FW_ASSERT(0, static_cast<FwAssertArgType>(configuration)); // Assert invalid configuration
            break;
    }
    return flags;
}

// ----------------------------------------------------------------------
// Maps GpioConfiguration to GPIO event request flags
// ----------------------------------------------------------------------
U32 configuration_to_event_flags(Drv::LinuxGpioDriver::GpioConfiguration configuration) {
    U32 flags = 0;
    switch (configuration) {
        case LinuxGpioDriver::GPIO_INTERRUPT_RISING_EDGE:
            flags = GPIOEVENT_REQUEST_RISING_EDGE; // Request rising edge event
            break;
        case LinuxGpioDriver::GPIO_INTERRUPT_FALLING_EDGE:
            flags = GPIOEVENT_REQUEST_FALLING_EDGE; // Request falling edge event
            break;
        case LinuxGpioDriver::GPIO_INTERRUPT_BOTH_RISING_AND_FALLING_EDGES:
            flags = GPIOEVENT_REQUEST_RISING_EDGE | GPIOEVENT_REQUEST_FALLING_EDGE; // Request both edges
            break;
        default:
            FW_ASSERT(0, static_cast<FwAssertArgType>(configuration)); // Assert invalid configuration
            break;
    }
    return flags;
}

// ----------------------------------------------------------------------
// Destructor: Closes the file descriptor associated with the GPIO
// ----------------------------------------------------------------------
LinuxGpioDriver::~LinuxGpioDriver() {
    (void) ::close(this->m_fd); // Close the file descriptor
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Sets up a GPIO line handle based on the provided configuration
// ----------------------------------------------------------------------
Os::File::Status LinuxGpioDriver::setupLineHandle(
    const PlatformIntType chip_descriptor, // File descriptor for the GPIO chip
    const U32 gpio,                        // GPIO line number
    const GpioConfiguration& configuration, // GPIO configuration mode
    const Fw::Logic& default_state,        // Default state for output
    PlatformIntType& fd                    // Output file descriptor for the GPIO line
) {
    Os::File::Status status = Os::File::OP_OK; // Initialize status
    // Set up the GPIO handle request structure
    struct gpiohandle_request request;
    (void) ::memset(&request, 0, sizeof request); // Zero out the structure
    request.lineoffsets[0] = gpio; // Specify the GPIO line offset
    // Copy the consumer label (object name) into the request
    Fw::StringUtils::string_copy(
        request.consumer_label, 
        this->getObjName(),
        static_cast<FwSizeType>(sizeof request.consumer_label)
    );
    // Set the default value for the GPIO line if it's configured as output
    request.default_values[0] = (default_state == Fw::Logic::HIGH) ? 1 : 0;
    request.fd = -1; // Initialize file descriptor
    request.lines = 1; // Number of lines requested
    request.flags = configuration_to_handler_flags(configuration); // Set request flags based on configuration

    errno = 0; // Reset errno before ioctl
    // Issue the ioctl to get the line handle
    PlatformIntType return_value = ioctl(chip_descriptor, GPIO_GET_LINEHANDLE_IOCTL, &request);
    fd = request.fd; // Retrieve the file descriptor from the request
    if (return_value != 0) { // Check for ioctl failure
        status = errno_to_file_status(errno); // Convert errno to status
        fd = -1; // Reset file descriptor on failure
    }
    return status; // Return the status
}

// ----------------------------------------------------------------------
// Sets up a GPIO line event based on the provided configuration
// ----------------------------------------------------------------------
Os::File::Status LinuxGpioDriver::setupLineEvent(
    const PlatformIntType chip_descriptor, // File descriptor for the GPIO chip
    const U32 gpio,                        // GPIO line number
    const GpioConfiguration& configuration, // GPIO configuration mode
    PlatformIntType& fd                    // Output file descriptor for the GPIO event
) {
    Os::File::Status status = Os::File::OP_OK; // Initialize status
    // Set up the GPIO event request structure
    struct gpioevent_request event;
    (void) ::memset(&event, 0, sizeof event); // Zero out the structure
    event.lineoffset = gpio; // Specify the GPIO line offset
    // Copy the consumer label (object name) into the event request
    Fw::StringUtils::string_copy(
        event.consumer_label, 
        this->getObjName(),
        static_cast<FwSizeType>(sizeof event.consumer_label)
    );
    event.fd = -1; // Initialize file descriptor
    event.handleflags = configuration_to_handler_flags(configuration); // Set handle flags based on configuration
    event.eventflags = configuration_to_event_flags(configuration);   // Set event flags based on configuration
    errno = 0; // Reset errno before ioctl
    // Issue the ioctl to get the line event
    PlatformIntType return_value = ioctl(chip_descriptor, GPIO_GET_LINEEVENT_IOCTL, &event);
    fd = event.fd; // Retrieve the file descriptor from the event
    if (return_value != 0) { // Check for ioctl failure
        status = errno_to_file_status(errno); // Convert errno to status
        fd = -1; // Reset file descriptor on failure
    }
    return status; // Return the status
}

// ----------------------------------------------------------------------
// Opens a GPIO line with the specified configuration and default state
// ----------------------------------------------------------------------
Os::File::Status LinuxGpioDriver::open(
    const char* device,                    // Path to the GPIO chip device
    const U32 gpio,                        // GPIO line number
    const GpioConfiguration& configuration, // GPIO configuration mode
    const Fw::Logic& default_state          // Default state for output
) {
    Os::File::Status status = Os::File::OP_OK; // Initialize status
    Os::File chip_file; // File object for the GPIO chip
    FW_ASSERT(device != nullptr); // Ensure device path is not null
    FW_ASSERT(
        configuration < MAX_GPIO_CONFIGURATION && configuration >= 0, 
        static_cast<FwAssertArgType>(configuration)
    ); // Validate configuration

    // Open the GPIO chip device file in write mode
    status = chip_file.open(device, Os::File::Mode::OPEN_WRITE);
    if (status != Os::File::OP_OK) { // Check if opening the chip file failed
        this->log_WARNING_HI_OpenChipError(
            Fw::String(device), 
            Os::FileStatus(static_cast<Os::FileStatus::T>(status))
        ); // Log a warning
        return status; // Return the error status
    }

    // Retrieve the file descriptor from the opened chip file
    PlatformIntType chip_descriptor =
        reinterpret_cast<Os::Posix::File::PosixFileHandle*>(chip_file.getHandle())->m_file_descriptor;

    // Get information about the GPIO chip
    struct gpiochip_info chip_info;
    (void) ::memset(&chip_info, 0, sizeof chip_info); // Zero out the structure
    PlatformIntType return_value = ioctl(chip_descriptor, GPIO_GET_CHIPINFO_IOCTL, &chip_info);
    if (return_value != 0) { // Check for ioctl failure
        status = errno_to_file_status(errno); // Convert errno to status
        this->log_WARNING_HI_OpenChipError(
            Fw::String(device), 
            Os::FileStatus(static_cast<Os::FileStatus::T>(status))
        ); // Log a warning
        return status; // Return the error status
    }

    // Verify that the requested GPIO line exists on the chip
    if (gpio >= chip_info.lines) {
        this->log_WARNING_HI_OpenPinError(
            Fw::String(device), 
            gpio, 
            Fw::String("Does Not Exist"),
            Os::FileStatus(static_cast<Os::FileStatus::T>(status))
        ); // Log a warning if GPIO does not exist
        return status; // Return the error status
    }

    // Retrieve information about the specific GPIO line
    Fw::String pin_message("Unknown"); // Initialize pin message
    struct gpioline_info pin_info;
    (void) ::memset(&pin_info, 0, sizeof pin_info); // Zero out the structure
    pin_info.line_offset = gpio; // Set the line offset
    return_value = ioctl(chip_descriptor, GPIO_GET_LINEINFO_IOCTL, &pin_info);
    if (return_value == 0) { // If ioctl succeeded
        const bool has_consumer = pin_info.consumer[0] != '\0'; // Check if the line has a consumer
        pin_message.format(
            "%s%s%s", 
            pin_info.name, 
            has_consumer ? " with current consumer " : "",
            has_consumer ? pin_info.consumer : ""
        ); // Format the pin message with name and consumer info
    }

    // Initialize the GPIO line file descriptor
    PlatformIntType pin_fd = -1;
    switch (configuration) { // Determine how to set up the GPIO based on configuration
        case GPIO_OUTPUT:
        case GPIO_INPUT:
            // Set up a GPIO handle for input or output
            status = this->setupLineHandle(chip_descriptor, gpio, configuration, default_state, pin_fd);
            break;
        case GPIO_INTERRUPT_RISING_EDGE:
        case GPIO_INTERRUPT_FALLING_EDGE:
        case GPIO_INTERRUPT_BOTH_RISING_AND_FALLING_EDGES:
            // Set up a GPIO event for interrupt-based configurations
            status = this->setupLineEvent(chip_descriptor, gpio, configuration, pin_fd);
            break;
        default:
            FW_ASSERT(0); // Assert invalid configuration
            break;
    }

    // Final status check after setting up the GPIO line
    if (status != Os::File::Status::OP_OK) { // If there was an error
        this->log_WARNING_HI_OpenPinError(
            Fw::String(device), 
            gpio, 
            pin_message,
            Os::FileStatus(static_cast<Os::FileStatus::T>(status))
        ); // Log a warning about the pin error
    } else { // If setup was successful
        this->log_DIAGNOSTIC_OpenChip(
            Fw::String(chip_info.name), 
            Fw::String(chip_info.label),
            gpio, 
            pin_message
        ); // Log diagnostic information about the opened chip and pin
        this->m_fd = pin_fd; // Store the file descriptor
        this->m_configuration = configuration; // Store the configuration
    }
    return status; // Return the status
}

// ----------------------------------------------------------------------
// Handler for reading the GPIO state
// ----------------------------------------------------------------------
Drv::GpioStatus LinuxGpioDriver::gpioRead_handler(
    const NATIVE_INT_TYPE portNum, // Port number (unused)
    Fw::Logic& state               // Output state
) {
    Drv::GpioStatus status = Drv::GpioStatus::INVALID_MODE; // Initialize status to INVALID_MODE
    if (this->m_configuration == GpioConfiguration::GPIO_INPUT) { // Ensure the GPIO is configured as input
        struct gpiohandle_data values;
        (void) ::memset(&values, 0, sizeof values); // Zero out the structure
        // Issue ioctl to get the current GPIO line values
        PlatformIntType return_value = ioctl(this->m_fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &values);
        if (return_value != 0) { // Check for ioctl failure
            status = errno_to_gpio_status(errno); // Convert errno to GpioStatus
        } else { // If ioctl succeeded
            state = values.values[0] ? Fw::Logic::HIGH : Fw::Logic::LOW; // Set the state based on the GPIO value
            status = Drv::GpioStatus::OP_OK; // Operation successful
        }
    }
    return status; // Return the GPIO status
}

// ----------------------------------------------------------------------
// Handler for writing the GPIO state
// ----------------------------------------------------------------------
Drv::GpioStatus LinuxGpioDriver::gpioWrite_handler(
    const NATIVE_INT_TYPE portNum, // Port number (unused)
    const Fw::Logic& state         // Desired state to set
) {
    Drv::GpioStatus status = Drv::GpioStatus::INVALID_MODE; // Initialize status to INVALID_MODE
    if (this->m_configuration == GpioConfiguration::GPIO_OUTPUT) { // Ensure the GPIO is configured as output
        struct gpiohandle_data values;
        (void) ::memset(&values, 0, sizeof values); // Zero out the structure
        values.values[0] = (state == Fw::Logic::HIGH) ? 1 : 0; // Set the desired GPIO value
        // Issue ioctl to set the GPIO line values
        PlatformIntType return_value = ioctl(this->m_fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &values);
        if (return_value != 0) { // Check for ioctl failure
            status = errno_to_gpio_status(errno); // Convert errno to GpioStatus
        } else { // If ioctl succeeded
            status = Drv::GpioStatus::OP_OK; // Operation successful
        }
    }
    return status; // Return the GPIO status
}

// ----------------------------------------------------------------------
// Polling loop to monitor GPIO events (e.g., interrupts)
// ----------------------------------------------------------------------
void LinuxGpioDriver::pollLoop() {
    // Compile-time assertions to ensure types are large enough
    static_assert(GPIO_POLL_TIMEOUT < std::numeric_limits<int>::max(), "Poll timeout would overflow");
    static_assert(sizeof(struct gpioevent_data) < std::numeric_limits<FwSizeType>::max(), "FwSizeType too small");
    static_assert(std::numeric_limits<ssize_t>::max() <= std::numeric_limits<FwSizeType>::max(), "FwSizeType too small");

    // Setup pollfd structure for polling the GPIO file descriptor
    pollfd file_descriptors[1];
    
    // Enter the polling loop
    while (this->getRunning()) { // Continue while the driver is running
        // Initialize the pollfd structure
        (void) ::memset(file_descriptors, 0, sizeof file_descriptors);
        file_descriptors[0].fd = this->m_fd;    // Set the GPIO file descriptor
        file_descriptors[0].events = POLLIN;    // Monitor for readable data (events)

        // Perform the poll with a specified timeout
        PlatformIntType status = ::poll(file_descriptors, 1, static_cast<int>(GPIO_POLL_TIMEOUT));

        if (status > 0) { // If at least one file descriptor is ready
            struct gpioevent_data event_data;
            // Attempt to read the GPIO event data
            FwSizeType read_bytes = static_cast<FwSizeType>(
                ::read(this->m_fd, &event_data, sizeof event_data)
            );
            if (read_bytes == sizeof event_data) { // Successful read of event data
                Os::RawTime timestamp;
                timestamp.now(); // Capture the current timestamp
                this->gpioInterrupt_out(0, timestamp); // Trigger the GPIO interrupt output
            }
            else { // Read error or incomplete read
                this->log_WARNING_HI_InterruptReadError(
                    static_cast<U32>(sizeof event_data),
                    static_cast<U32>(read_bytes)
                ); // Log a warning about the read error
            }
        }
        else if (status < 0) { // An error occurred during poll
            this->log_WARNING_HI_PollingError(static_cast<I32>(errno)); // Log a polling error
        }
        // If status == 0, the poll timed out without any events; continue looping
    }
}

}  // end namespace Drv
