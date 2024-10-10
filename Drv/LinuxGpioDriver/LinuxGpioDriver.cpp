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
#include <Os/Posix/File.hpp>
#include <Drv/LinuxGpioDriver/LinuxGpioDriver.hpp>
#include <FpConfig.hpp>
#include <Fw/Types/String.hpp>
#include <Fw/Types/StringUtils.hpp>

#include <linux/gpio.h>
#include <sys/ioctl.h>
#include <cerrno>
#include <unistd.h>

namespace Drv {

#ifdef V2
GPIO_V2_GET_LINE_IOCTL, struct gpio_v2_line_request *request
#else

#endif

Os::File::Status errno_to_status(PlatformIntType errno_input) {
    Os::File::Status status = Os::File::Status::OTHER_ERROR;
    switch (errno_input) {
        case EBADF:
            status = Os::File::Status::NOT_OPENED;
            break;
        case EFAULT:
            break;
        case  EINVAL:
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
        case EWOULDBLOCK:
        case EBUSY:
        case EIO:
        default:
            status = Os::File::Status::OTHER_ERROR;
            break;
    }
    return status;
}

U32 configuration_to_flags(Drv::LinuxGpioDriver::GpioConfiguration configuration) {
    U32 flags = 0;
    switch (configuration) {
        case LinuxGpioDriver::GPIO_INPUT:
            flags = GPIOHANDLE_REQUEST_INPUT;
            break;
        case LinuxGpioDriver::GPIO_OUTPUT:
            flags = GPIOHANDLE_REQUEST_OUTPUT;
            break;
        case LinuxGpioDriver::GPIO_AS_IS:
            flags = GPIOHANDLE_REQUEST_ACTIVE_LOW;
            break;
        default:
            FW_ASSERT(0);
            break;
    }
    return flags;
}

LinuxGpioDriver ::~LinuxGpioDriver() {
    ::close(this->m_fd);
}


// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------


Os::File::Status LinuxGpioDriver ::open(const char* device, U32 gpio, GpioConfiguration configuration, Fw::Logic default_state) {
    Os::File::Status status = Os::File::OP_OK;
    Os::File chip_file; // TODO: how to force posix file safely

    // Open chip file and check for success
    status = chip_file.open(device, Os::File::Mode::OPEN_WRITE);
    if (status != Os::File::OP_OK) {
        this->log_WARNING_HI_OpenChipError(Fw::String(device), Os::FileStatus(static_cast<Os::FileStatus::T>(status)));
        return status;
    }
    // Read chip information and check for correctness
    PlatformIntType chip_descriptor = reinterpret_cast<Os::Posix::File::PosixFileHandle*>(chip_file.getHandle())->m_file_descriptor;
    struct gpiochip_info chip_info;
    PlatformIntType return_value = ioctl(chip_descriptor, GPIO_GET_CHIPINFO_IOCTL, &chip_info);
    if (return_value != 0) {
        status = errno_to_status(errno);
        this->log_WARNING_HI_OpenChipError(Fw::String(device), Os::FileStatus(static_cast<Os::FileStatus::T>(status)));
        return status;
    }
    this->log_DIAGNOSTIC_OpenChip(Fw::String(chip_info.name), Fw::String(chip_info.label), chip_info.lines);
    // Check if the GPIO line exists
    if (gpio >= chip_info.lines) {
        this->log_WARNING_HI_OpenPinError(Fw::String(device), gpio, Os::FileStatus(static_cast<Os::FileStatus::T>(status)));
        return status;
    }
    // Set up the GPIO request
    struct gpiohandle_request request;
    request.lineoffsets[0] = gpio;
    Fw::StringUtils::string_copy(request.consumer_label, this->getObjName(),
        static_cast<FwSizeType>(sizeof request.consumer_label));
    request.default_values[0] = (default_state == Fw::Logic::HIGH) ? 1 : 0;
    request.fd = -1;
    request.lines = 1;
    request.flags = configuration_to_flags(configuration);
    return_value = ioctl(chip_descriptor, GPIO_GET_LINEHANDLE_IOCTL, &request);
    if (return_value != 0) {
        status = errno_to_status(errno);
        this->log_WARNING_HI_OpenPinError(Fw::String(device), gpio, Os::FileStatus(static_cast<Os::FileStatus::T>(status)));
        return status;
    }
    this->m_chip = device;
    this->m_gpio = gpio;
    this->m_fd = request.fd;
    return status;
}

void LinuxGpioDriver ::gpioRead_handler(const NATIVE_INT_TYPE portNum, Fw::Logic &state) {
    struct gpiohandle_data values;
    PlatformIntType return_value = ioctl(this->m_fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, values);
    if (return_value != 0) {
        Os::File::Status status = errno_to_status(errno);
        this->log_WARNING_HI_PinReadError(this->m_chip, this->m_gpio, Os::FileStatus(static_cast<Os::FileStatus::T>(status)));
    } else {
        state = values.values[0] ? Fw::Logic::HIGH : Fw::Logic::LOW;
    }
}

void LinuxGpioDriver ::gpioWrite_handler(const NATIVE_INT_TYPE portNum, const Fw::Logic& state){
    struct gpiohandle_data values;
    values.values[0] = (state == Fw::Logic::HIGH) ? 1 : 0;
    PlatformIntType return_value = ioctl(this->m_fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, values);
    if (return_value != 0) {
        Os::File::Status status = errno_to_status(errno);
        this->log_WARNING_HI_PinWriteError(this->m_chip, this->m_gpio, Os::FileStatus(static_cast<Os::FileStatus::T>(status)));

    }
}

} // end namespace Drv
