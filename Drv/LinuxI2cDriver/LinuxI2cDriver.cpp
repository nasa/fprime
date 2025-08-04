// ======================================================================
// \title  LinuxI2cDriverComponentImpl.cpp
// \author tcanham
// \brief  cpp file for LinuxI2cDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Drv/LinuxI2cDriver/LinuxI2cDriver.hpp>
#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Logger/Logger.hpp>
#include "Fw/Types/Assert.hpp"

#include <fcntl.h>          // required for I2C device configuration
#include <linux/i2c-dev.h>  // required for constant definitions
#include <linux/i2c.h>      // required for struct / constant definitions
#include <sys/ioctl.h>      // required for I2C device usage
#include <unistd.h>         // required for I2C device access
#include <cerrno>

namespace Drv {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

LinuxI2cDriver ::LinuxI2cDriver(const char* const compName) : LinuxI2cDriverComponentBase(compName), m_fd(-1) {}

LinuxI2cDriver::~LinuxI2cDriver() {
    if (-1 != this->m_fd) {  // check if file is open
        ::close(this->m_fd);
    }
}

bool LinuxI2cDriver::open(const char* device) {
    FW_ASSERT(device);
    this->m_fd = ::open(device, O_RDWR);
    return (-1 != this->m_fd);
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

// Note this port handler is guarded, so we can make the ioctl call

Drv::I2cStatus LinuxI2cDriver ::write_handler(const FwIndexType portNum, U32 addr, Fw::Buffer& serBuffer) {
    // Make sure file has been opened
    if (-1 == this->m_fd) {
        return I2cStatus::I2C_OPEN_ERR;
    }

    // select slave address
    int stat = ioctl(this->m_fd, I2C_SLAVE, addr);
    if (stat == -1) {
        return I2cStatus::I2C_ADDRESS_ERR;
    }
    // make sure it isn't a null pointer
    FW_ASSERT(serBuffer.getData());
    FW_ASSERT_NO_OVERFLOW(serBuffer.getSize(), size_t);
    // write data
    ssize_t status_write = write(this->m_fd, serBuffer.getData(), static_cast<size_t>(serBuffer.getSize()));
    if (status_write == -1) {
        return I2cStatus::I2C_WRITE_ERR;
    }
    return I2cStatus::I2C_OK;
}

Drv::I2cStatus LinuxI2cDriver ::read_handler(const FwIndexType portNum, U32 addr, Fw::Buffer& serBuffer) {
    // Make sure file has been opened
    if (-1 == this->m_fd) {
        return I2cStatus::I2C_OPEN_ERR;
    }

    // select slave address
    int stat = ioctl(this->m_fd, I2C_SLAVE, addr);
    if (stat == -1) {
        return I2cStatus::I2C_ADDRESS_ERR;
    }
    // make sure it isn't a null pointer
    FW_ASSERT(serBuffer.getData());
    // read data
    FW_ASSERT_NO_OVERFLOW(serBuffer.getSize(), size_t);
    ssize_t status_read = read(this->m_fd, serBuffer.getData(), static_cast<size_t>(serBuffer.getSize()));
    if (status_read == -1) {
        return I2cStatus::I2C_READ_ERR;
    }
    return I2cStatus::I2C_OK;
}

Drv::I2cStatus LinuxI2cDriver ::writeRead_handler(const FwIndexType portNum, /*!< The port number*/
                                                  U32 addr,
                                                  Fw::Buffer& writeBuffer,
                                                  Fw::Buffer& readBuffer) {
    // Make sure file has been opened
    if (-1 == this->m_fd) {
        return I2cStatus::I2C_OPEN_ERR;
    }
    FW_ASSERT(-1 != this->m_fd);

    // make sure they are not null pointers
    FW_ASSERT(writeBuffer.getData());
    FW_ASSERT(readBuffer.getData());
    // make sure downcasts are safe
    FW_ASSERT_NO_OVERFLOW(addr, U16);
    FW_ASSERT_NO_OVERFLOW(writeBuffer.getSize(), U16);
    FW_ASSERT_NO_OVERFLOW(readBuffer.getSize(), U16);

    struct i2c_msg rdwr_msgs[2];

    // Start address
    rdwr_msgs[0].addr = static_cast<U16>(addr);
    rdwr_msgs[0].flags = 0;  // write
    rdwr_msgs[0].len = static_cast<U16>(writeBuffer.getSize());
    rdwr_msgs[0].buf = writeBuffer.getData();

    // Read buffer
    rdwr_msgs[1].addr = static_cast<U16>(addr);
    rdwr_msgs[1].flags = I2C_M_RD;  // read
    rdwr_msgs[1].len = static_cast<U16>(readBuffer.getSize());
    rdwr_msgs[1].buf = readBuffer.getData();

    struct i2c_rdwr_ioctl_data rdwr_data;
    rdwr_data.msgs = rdwr_msgs;
    rdwr_data.nmsgs = 2;

    // Use ioctl to perform the combined write/read transaction
    int stat = ioctl(this->m_fd, I2C_RDWR, &rdwr_data);

    if (stat == -1) {
        // Because we're using ioctl to perform the transaction we dont know exactly the type of error that occurred
        return I2cStatus::I2C_OTHER_ERR;
    }

    return I2cStatus::I2C_OK;
}

}  // end namespace Drv
