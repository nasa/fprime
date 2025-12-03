// ======================================================================
// \title  LinuxSpiDriverImpl.cpp
// \author tcanham
// \brief  cpp file for LinuxSpiDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <Drv/LinuxSpiDriver/LinuxSpiDriverComponentImpl.hpp>
#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/FileNameString.hpp>
#include <Fw/Types/String.hpp>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static_assert(FW_USE_PRINTF_FAMILY_FUNCTIONS_IN_STRING_FORMATTING,
              "Cannot use SPI driver without full string formatting");

namespace Drv {

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

// @ DEPRECATED: Use SpiWriteRead port instead (same operation with a return value)
void LinuxSpiDriverComponentImpl::SpiReadWrite_handler(const FwIndexType portNum,
                                                       Fw::Buffer& writeBuffer,
                                                       Fw::Buffer& readBuffer) {
    FW_ASSERT(portNum >= 0, static_cast<FwAssertArgType>(portNum));
    FW_ASSERT(writeBuffer.isValid());
    FW_ASSERT(readBuffer.isValid());
    (void)SpiWriteRead_handler(portNum, writeBuffer, readBuffer);
}

SpiStatus LinuxSpiDriverComponentImpl::SpiWriteRead_handler(const FwIndexType portNum,
                                                            Fw::Buffer& writeBuffer,
                                                            Fw::Buffer& readBuffer) {
    FW_ASSERT(portNum >= 0, static_cast<FwAssertArgType>(portNum));
    FW_ASSERT(writeBuffer.isValid());
    FW_ASSERT(readBuffer.isValid());
    FW_ASSERT(writeBuffer.getSize() == readBuffer.getSize());

    if (this->m_fd == -1) {
        return SpiStatus::SPI_OPEN_ERR;
    }

    spi_ioc_transfer tr;
    // Zero for unused fields:
    memset(&tr, 0, sizeof(tr));
    tr.tx_buf = reinterpret_cast<__u64>(writeBuffer.getData());
    tr.rx_buf = reinterpret_cast<__u64>(readBuffer.getData());
    FW_ASSERT_NO_OVERFLOW(writeBuffer.getSize(), __u32);
    tr.len = static_cast<__u32>(writeBuffer.getSize());
    /*
                .speed_hz = 0,
                .delay_usecs = 0,
                .bits_per_word = 0,
                .cs_change = 0,
                .tx_nbits = 0, // on more-recent kernel versions;
                .rx_nbits = 0, // on more-recent kernel versions;
                .pad = 0
    */

    int stat = ioctl(this->m_fd, SPI_IOC_MESSAGE(1), &tr);

    if (stat < 1) {
        this->log_WARNING_HI_SPI_WriteError(this->m_device, this->m_select, stat);
        return SpiStatus::SPI_OTHER_ERR;
    }
    this->m_bytes += readBuffer.getSize();
    this->tlmWrite_SPI_Bytes(this->m_bytes);

    return SpiStatus::SPI_OK;
}

bool LinuxSpiDriverComponentImpl::open(FwIndexType device, FwIndexType select, SpiFrequency clock, SpiMode spiMode) {
    FW_ASSERT(device >= 0, static_cast<FwAssertArgType>(device));
    FW_ASSERT(select >= 0, static_cast<FwAssertArgType>(select));

    this->m_device = device;
    this->m_select = select;
    int fd;
    int ret;

    // Open:
    Fw::FileNameString devString;
    Fw::FormatStatus formatStatus =
        devString.format("/dev/spidev%" PRI_FwIndexType ".%" PRI_FwIndexType, device, select);
    FW_ASSERT(formatStatus == Fw::FormatStatus::SUCCESS);

    fd = ::open(devString.toChar(), O_RDWR);
    if (fd == -1) {
        this->log_WARNING_HI_SPI_OpenError(device, select, fd);
        return false;
    }

    this->m_fd = fd;

    // Configure:
    /*
     * SPI Mode 0, 1, 2, 3
     */

    U8 mode;  // Mode Select (CPOL = 0/1, CPHA = 0/1)
    switch (spiMode) {
        case SpiMode::SPI_MODE_CPOL_LOW_CPHA_LOW:
            mode = SPI_MODE_0;
            break;
        case SpiMode::SPI_MODE_CPOL_LOW_CPHA_HIGH:
            mode = SPI_MODE_1;
            break;
        case SpiMode::SPI_MODE_CPOL_HIGH_CPHA_LOW:
            mode = SPI_MODE_2;
            break;
        case SpiMode::SPI_MODE_CPOL_HIGH_CPHA_HIGH:
            mode = SPI_MODE_3;
            break;
        default:
            // Assert if the device SPI Mode is not in the correct range
            FW_ASSERT(0, spiMode);
            break;
    }

    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1) {
        this->log_WARNING_HI_SPI_ConfigError(device, select, ret);
        return false;
    }

    U8 read_mode = 0;
    ret = ioctl(fd, SPI_IOC_RD_MODE, &read_mode);
    if (ret == -1) {
        this->log_WARNING_HI_SPI_ConfigError(device, select, ret);
        return false;
    }

    if (mode != read_mode) {
        this->log_WARNING_LO_SPI_ConfigMismatch(device, select, Fw::String("MODE"), mode, read_mode);
    }

    /*
     * 8 bits per word
     */
    U8 bits = 8;
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1) {
        this->log_WARNING_HI_SPI_ConfigError(device, select, ret);
        return false;
    }

    U8 read_bits = 0;
    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &read_bits);
    if (ret == -1) {
        this->log_WARNING_HI_SPI_ConfigError(device, select, ret);
        return false;
    }

    if (bits != read_bits) {
        this->log_WARNING_LO_SPI_ConfigMismatch(device, select, Fw::String("BITS_PER_WORD"), bits, read_bits);
    }

    /*
     * Max speed in Hz
     */
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &clock);
    if (ret == -1) {
        this->log_WARNING_HI_SPI_ConfigError(device, select, ret);
        return false;
    }

    SpiFrequency read_clock;
    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &read_clock);
    if (ret == -1) {
        this->log_WARNING_HI_SPI_ConfigError(device, select, ret);
        return false;
    }

    if (clock != read_clock) {
        this->log_WARNING_LO_SPI_ConfigMismatch(device, select, Fw::String("MAX_SPEED_HZ"), clock, read_clock);
    }

    return true;
}

LinuxSpiDriverComponentImpl::~LinuxSpiDriverComponentImpl() {
    (void)close(this->m_fd);
}

}  // end namespace Drv
