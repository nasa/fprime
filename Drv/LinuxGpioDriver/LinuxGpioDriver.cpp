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
#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/String.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <Os/Posix/File.hpp>

#include <linux/gpio.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <type_traits>

namespace Drv {

Os::File::Status errno_to_file_status(int errno_input) {
    Os::File::Status status = Os::File::Status::OTHER_ERROR;
    switch (errno_input) {
        case 0:
            status = Os::File::Status::OP_OK;
            break;
        case EBADF:
            status = Os::File::Status::NOT_OPENED;
            break;
        case EINVAL:
            status = Os::File::Status::INVALID_ARGUMENT;
            break;
        case ENODEV:
            status = Os::File::Status::DOESNT_EXIST;
            break;
        case ENOMEM:
            status = Os::File::Status::NO_SPACE;
            break;
        case EPERM:
            status = Os::File::Status::NO_PERMISSION;
            break;
        case ENXIO:
            status = Os::File::Status::INVALID_MODE;
            break;
        // Cascades intended
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

Drv::GpioStatus errno_to_gpio_status(int errno_input) {
    Drv::GpioStatus status = Drv::GpioStatus::T::UNKNOWN_ERROR;
    switch (errno_input) {
        case EBADF:
            status = Drv::GpioStatus::T::NOT_OPENED;
            break;
        case ENXIO:
            status = Drv::GpioStatus::INVALID_MODE;
            break;
        // Cascades intended
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

U32 configuration_to_handler_flags(Drv::LinuxGpioDriver::GpioConfiguration configuration) {
    U32 flags = 0;
    switch (configuration) {
        case LinuxGpioDriver::GPIO_OUTPUT:
            flags = GPIOHANDLE_REQUEST_OUTPUT;
            break;
        // Cascade intended
        case LinuxGpioDriver::GPIO_INPUT:
        case LinuxGpioDriver::GPIO_INTERRUPT_RISING_EDGE:
        case LinuxGpioDriver::GPIO_INTERRUPT_FALLING_EDGE:
        case LinuxGpioDriver::GPIO_INTERRUPT_BOTH_RISING_AND_FALLING_EDGES:
            flags = GPIOHANDLE_REQUEST_INPUT;
            break;
        default:
            FW_ASSERT(false, static_cast<FwAssertArgType>(configuration));
            break;
    }
    return flags;
}

#ifdef GPIO_V2_GET_LINE_IOCTL
U64 configuration_to_line_flags_v2(Drv::LinuxGpioDriver::GpioConfiguration configuration) {
    U64 flags = 0;
    switch (configuration) {
        case LinuxGpioDriver::GPIO_OUTPUT:
            flags = GPIO_V2_LINE_FLAG_OUTPUT;
            break;
        case LinuxGpioDriver::GPIO_INPUT:
            flags = GPIO_V2_LINE_FLAG_INPUT;
            break;
        case LinuxGpioDriver::GPIO_INTERRUPT_RISING_EDGE:
            flags = GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_EDGE_RISING;
            break;
        case LinuxGpioDriver::GPIO_INTERRUPT_FALLING_EDGE:
            flags = GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_EDGE_FALLING;
            break;
        case LinuxGpioDriver::GPIO_INTERRUPT_BOTH_RISING_AND_FALLING_EDGES:
            flags = GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_EDGE_RISING | GPIO_V2_LINE_FLAG_EDGE_FALLING;
            break;
        default:
            FW_ASSERT(false, static_cast<FwAssertArgType>(configuration));
            break;
    }
    return flags;
}
#endif

U32 configuration_to_event_flags(Drv::LinuxGpioDriver::GpioConfiguration configuration) {
    U32 flags = 0;
    switch (configuration) {
        case LinuxGpioDriver::GPIO_INTERRUPT_RISING_EDGE:
            flags = GPIOEVENT_REQUEST_RISING_EDGE;
            break;
        case LinuxGpioDriver::GPIO_INTERRUPT_FALLING_EDGE:
            flags = GPIOEVENT_REQUEST_FALLING_EDGE;
            break;
        case LinuxGpioDriver::GPIO_INTERRUPT_BOTH_RISING_AND_FALLING_EDGES:
            flags = GPIOEVENT_REQUEST_RISING_EDGE | GPIOEVENT_REQUEST_FALLING_EDGE;
            break;
        default:
            FW_ASSERT(false, static_cast<FwAssertArgType>(configuration));
            break;
    }
    return flags;
}

LinuxGpioDriver ::~LinuxGpioDriver() {
    if (this->m_fd >= 0) {
        (void)::close(this->m_fd);
    }
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

Os::File::Status LinuxGpioDriver ::setupLineRequestV2(const int chip_descriptor,
                                                      const U32 gpio,
                                                      const GpioConfiguration& configuration,
                                                      const Fw::Logic& default_state,
                                                      int& fd) {
    fd = -1;
#ifdef GPIO_V2_GET_LINE_IOCTL
    Os::File::Status status = Os::File::OP_OK;
    // Set up the GPIO v2 line request
    struct gpio_v2_line_request request;
    (void)::memset(&request, 0, sizeof request);
    request.offsets[0] = gpio;
    (void)Fw::StringUtils::string_copy(request.consumer, FW_OPTIONAL_NAME(this->getObjName()),
                                       static_cast<FwSizeType>(sizeof request.consumer));
    request.num_lines = 1;
    request.fd = -1;
    request.config.flags = configuration_to_line_flags_v2(configuration);
    // For outputs, set the default state via an output-values config attribute
    if (configuration == GPIO_OUTPUT) {
        request.config.num_attrs = 1;
        request.config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES;
        request.config.attrs[0].attr.values = (default_state == Fw::Logic::HIGH) ? 1 : 0;
        request.config.attrs[0].mask = 1;
    }

    errno = 0;
    int return_value = ioctl(chip_descriptor, GPIO_V2_GET_LINE_IOCTL, &request);
    fd = request.fd;
    if (return_value != 0) {
        status = errno_to_file_status(errno);
        fd = -1;
    }
    return status;
#else
    // Headers do not provide the v2 uAPI: report unsupported to trigger the v1 fallback
    return Os::File::Status::NOT_SUPPORTED;
#endif
}

Os::File::Status LinuxGpioDriver ::setupLineHandle(const int chip_descriptor,
                                                   const U32 gpio,
                                                   const GpioConfiguration& configuration,
                                                   const Fw::Logic& default_state,
                                                   int& fd) {
    Os::File::Status status = Os::File::OP_OK;
    // Set up the GPIO request
    struct gpiohandle_request request;
    (void)::memset(&request, 0, sizeof request);
    request.lineoffsets[0] = gpio;
    (void)Fw::StringUtils::string_copy(request.consumer_label, FW_OPTIONAL_NAME(this->getObjName()),
                                       static_cast<FwSizeType>(sizeof request.consumer_label));
    request.default_values[0] = (default_state == Fw::Logic::HIGH) ? 1 : 0;
    request.fd = -1;
    request.lines = 1;
    request.flags = configuration_to_handler_flags(configuration);

    errno = 0;
    int return_value = ioctl(chip_descriptor, GPIO_GET_LINEHANDLE_IOCTL, &request);
    fd = request.fd;
    if (return_value != 0) {
        status = errno_to_file_status(errno);
        fd = -1;
    }
    return status;
}

Os::File::Status LinuxGpioDriver ::setupLineEvent(const int chip_descriptor,
                                                  const U32 gpio,
                                                  const GpioConfiguration& configuration,
                                                  int& fd) {
    Os::File::Status status = Os::File::OP_OK;
    // Set up the GPIO request
    struct gpioevent_request event;
    (void)::memset(&event, 0, sizeof event);
    event.lineoffset = gpio;
    (void)Fw::StringUtils::string_copy(event.consumer_label, FW_OPTIONAL_NAME(this->getObjName()),
                                       static_cast<FwSizeType>(sizeof event.consumer_label));
    event.fd = -1;
    event.handleflags = configuration_to_handler_flags(configuration);
    event.eventflags = configuration_to_event_flags(configuration);
    errno = 0;
    int return_value = ioctl(chip_descriptor, GPIO_GET_LINEEVENT_IOCTL, &event);
    fd = event.fd;
    if (return_value != 0) {
        status = errno_to_file_status(errno);
        fd = -1;
    }
    return status;
}

Os::File::Status LinuxGpioDriver ::open(const char* device,
                                        const U32 gpio,
                                        const GpioConfiguration& configuration,
                                        const Fw::Logic& default_state) {
    Os::File::Status status = Os::File::OP_OK;
    Os::File chip_file;
    FW_ASSERT(device != nullptr);
    FW_ASSERT(configuration < MAX_GPIO_CONFIGURATION and configuration >= 0,
              static_cast<FwAssertArgType>(configuration));

    // Open chip file and check for success
    status = chip_file.open(device, Os::File::Mode::OPEN_WRITE);
    if (status != Os::File::OP_OK) {
        this->log_WARNING_HI_OpenChipError(Fw::String(device), Os::FileStatus(static_cast<Os::FileStatus::T>(status)));
        return status;
    }
    // Read chip information and check for correctness
    int chip_descriptor = reinterpret_cast<Os::Posix::File::PosixFileHandle*>(chip_file.getHandle())->m_file_descriptor;
    struct gpiochip_info chip_info;
    (void)::memset(&chip_info, 0, sizeof chip_info);
    int return_value = ioctl(chip_descriptor, GPIO_GET_CHIPINFO_IOCTL, &chip_info);
    if (return_value != 0) {
        status = errno_to_file_status(errno);
        this->log_WARNING_HI_OpenChipError(Fw::String(device), Os::FileStatus(static_cast<Os::FileStatus::T>(status)));
        return status;
    }
    // Check if the GPIO line exists
    if (gpio >= chip_info.lines) {
        this->log_WARNING_HI_OpenPinError(Fw::String(device), gpio, Fw::String("Does Not Exist"),
                                          Os::FileStatus(static_cast<Os::FileStatus::T>(status)));
        return status;
    }
    Fw::String pin_message("Unknown");
    bool has_pin_info = false;
#ifdef GPIO_V2_GET_LINEINFO_IOCTL
    struct gpio_v2_line_info pin_info_v2;
    (void)::memset(&pin_info_v2, 0, sizeof pin_info_v2);
    pin_info_v2.offset = gpio;
    return_value = ioctl(chip_descriptor, GPIO_V2_GET_LINEINFO_IOCTL, &pin_info_v2);
    if (return_value == 0) {
        const bool has_consumer = pin_info_v2.consumer[0] != '\0';
        (void)pin_message.format("%s%s%s", pin_info_v2.name, has_consumer ? " with current consumer " : "",
                                 has_consumer ? pin_info_v2.consumer : "");
        has_pin_info = true;
    }
#endif
    if (not has_pin_info) {
        struct gpioline_info pin_info;
        (void)::memset(&pin_info, 0, sizeof pin_info);
        pin_info.line_offset = gpio;
        return_value = ioctl(chip_descriptor, GPIO_GET_LINEINFO_IOCTL, &pin_info);
        if (return_value == 0) {
            const bool has_consumer = pin_info.consumer[0] != '\0';
            (void)pin_message.format("%s%s%s", pin_info.name, has_consumer ? " with current consumer " : "",
                                     has_consumer ? pin_info.consumer : "");
        }
    }

    // Set up pin and set file descriptor for it. The v2 uAPI is attempted first and the deprecated v1 uAPI is
    // used as a fallback for kernels without v2 support.
    int pin_fd = -1;
    ApiVersion api_version = ApiVersion::API_V2;
    status = this->setupLineRequestV2(chip_descriptor, gpio, configuration, default_state, pin_fd);
    if (status != Os::File::Status::OP_OK) {
        api_version = ApiVersion::API_V1;
        switch (configuration) {
            // Cascade intended
            case GPIO_OUTPUT:
            case GPIO_INPUT:
                status = this->setupLineHandle(chip_descriptor, gpio, configuration, default_state, pin_fd);
                break;
            // Cascade intended
            case GPIO_INTERRUPT_RISING_EDGE:
            case GPIO_INTERRUPT_FALLING_EDGE:
            case GPIO_INTERRUPT_BOTH_RISING_AND_FALLING_EDGES:
                status = this->setupLineEvent(chip_descriptor, gpio, configuration, pin_fd);
                break;
            default:
                FW_ASSERT(false);
                break;
        }
    }
    // Final status check
    if (status != Os::File::Status::OP_OK) {
        this->log_WARNING_HI_OpenPinError(Fw::String(device), gpio, pin_message,
                                          Os::FileStatus(static_cast<Os::FileStatus::T>(status)));
    } else {
        this->log_DIAGNOSTIC_OpenChip(Fw::String(chip_info.name), Fw::String(chip_info.label), gpio, pin_message);
        this->m_fd = pin_fd;
        this->m_configuration = configuration;
        this->m_apiVersion = api_version;
    }
    return status;
}

Drv::GpioStatus LinuxGpioDriver ::gpioRead_handler(const FwIndexType portNum, Fw::Logic& state) {
    Drv::GpioStatus status = Drv::GpioStatus::INVALID_MODE;
    if (this->m_configuration == GpioConfiguration::GPIO_INPUT) {
#ifdef GPIO_V2_GET_LINE_IOCTL
        if (this->m_apiVersion == ApiVersion::API_V2) {
            struct gpio_v2_line_values values;
            (void)::memset(&values, 0, sizeof values);
            values.mask = 1;
            int return_value = ioctl(this->m_fd, GPIO_V2_LINE_GET_VALUES_IOCTL, &values);
            if (return_value != 0) {
                status = errno_to_gpio_status(errno);
            } else {
                state = (values.bits & 1) ? Fw::Logic::HIGH : Fw::Logic::LOW;
                status = Drv::GpioStatus::OP_OK;
            }
            return status;
        }
#endif
        struct gpiohandle_data values;
        (void)::memset(&values, 0, sizeof values);
        int return_value = ioctl(this->m_fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &values);
        if (return_value != 0) {
            status = errno_to_gpio_status(errno);
        } else {
            state = values.values[0] ? Fw::Logic::HIGH : Fw::Logic::LOW;
            status = Drv::GpioStatus::OP_OK;
        }
    }
    return status;
}

Drv::GpioStatus LinuxGpioDriver ::gpioWrite_handler(const FwIndexType portNum, const Fw::Logic& state) {
    Drv::GpioStatus status = Drv::GpioStatus::INVALID_MODE;
    if (this->m_configuration == GpioConfiguration::GPIO_OUTPUT) {
#ifdef GPIO_V2_GET_LINE_IOCTL
        if (this->m_apiVersion == ApiVersion::API_V2) {
            struct gpio_v2_line_values values;
            (void)::memset(&values, 0, sizeof values);
            values.mask = 1;
            values.bits = (state == Fw::Logic::HIGH) ? 1 : 0;
            int return_value = ioctl(this->m_fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &values);
            if (return_value != 0) {
                status = errno_to_gpio_status(errno);
            } else {
                status = Drv::GpioStatus::OP_OK;
            }
            return status;
        }
#endif
        struct gpiohandle_data values;
        (void)::memset(&values, 0, sizeof values);
        values.values[0] = (state == Fw::Logic::HIGH) ? 1 : 0;
        int return_value = ioctl(this->m_fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &values);
        if (return_value != 0) {
            status = errno_to_gpio_status(errno);
        } else {
            status = Drv::GpioStatus::OP_OK;
        }
    }
    return status;
}

void LinuxGpioDriver ::pollLoop() {
    // Ensure size of FwSizeType is large enough to fit the necessary ranges
    // NOTE: casts to unsigned types for int and ssize_t are made to avoid sign-compare warning;
    //       in both cases the cast is safe because max() returns nonnegative value.
    static_assert(GPIO_POLL_TIMEOUT < static_cast<unsigned int>(std::numeric_limits<int>::max()),
                  "Poll timeout would overflow");
    static_assert(sizeof(struct gpioevent_data) < std::numeric_limits<FwSizeType>::max(), "FwSizeType too small");
#ifdef GPIO_V2_GET_LINE_IOCTL
    static_assert(sizeof(struct gpio_v2_line_event) < std::numeric_limits<FwSizeType>::max(), "FwSizeType too small");
#endif
    using unsigned_ssize_t = std::make_unsigned<ssize_t>::type;
    static_assert(
        static_cast<unsigned_ssize_t>(std::numeric_limits<ssize_t>::max()) <= std::numeric_limits<FwSizeType>::max(),
        "FwSizeType too small");
    // Setup poll information
    pollfd file_descriptors[1];
    // Loop forever
    // @non-terminating@: polling thread runs until stopped
    while (this->getRunning()) {
        // Setup polling
        (void)::memset(file_descriptors, 0, sizeof file_descriptors);
        file_descriptors[0].fd = this->m_fd;
        file_descriptors[0].events = POLLIN;  // Ask for read data available
        // Poll for fd bing ready
        int status = ::poll(file_descriptors, 1, static_cast<int>(GPIO_POLL_TIMEOUT));
        // Check for some file descriptor to be ready
        if (status > 0) {
            union {
                struct gpioevent_data v1;
#ifdef GPIO_V2_GET_LINE_IOCTL
                struct gpio_v2_line_event v2;
#endif
            } event_data;
            FwSizeType expected_bytes = sizeof event_data.v1;
#ifdef GPIO_V2_GET_LINE_IOCTL
            if (this->m_apiVersion == ApiVersion::API_V2) {
                expected_bytes = sizeof event_data.v2;
            }
#endif
            FwSizeType read_bytes = static_cast<FwSizeType>(::read(this->m_fd, &event_data, expected_bytes));
            if (read_bytes == expected_bytes) {
                Os::RawTime timestamp;
                Os::RawTime::Status timeStatus = timestamp.now();
                if (timeStatus != Os::RawTime::Status::OP_OK) {
                    this->log_WARNING_HI_InterruptTimeError(
                        Os::RawTimeStatus(static_cast<Os::RawTimeStatus::T>(timeStatus)));
                }
                this->gpioInterrupt_out(0, timestamp);
            }
            // A read error occurred
            else {
                this->log_WARNING_HI_InterruptReadError(static_cast<U32>(expected_bytes), static_cast<U32>(read_bytes));
            }
        }
        // An error of some kind occurred
        else if (status < 0) {
            this->log_WARNING_HI_PollingError(static_cast<I32>(errno));
        }
    }
}

}  // end namespace Drv
