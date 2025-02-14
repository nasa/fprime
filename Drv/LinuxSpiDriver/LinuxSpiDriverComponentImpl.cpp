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

#include <Drv/LinuxSpiDriver/LinuxSpiDriverComponentImpl.hpp>
#include <FpConfig.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/FileNameString.hpp>
#include <cstdint>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <cerrno>

static_assert(FW_USE_PRINTF_FAMILY_FUNCTIONS_IN_STRING_FORMATTING, "Cannot use SPI driver without full string formatting");

namespace Drv {

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    void LinuxSpiDriverComponentImpl::SpiReadWrite_handler(
            const FwIndexType portNum, Fw::Buffer &writeBuffer,
            Fw::Buffer &readBuffer) {

        if (this->m_fd == -1) {
            return;
        }

        spi_ioc_transfer tr;
        // Zero for unused fields:
        memset(&tr, 0, sizeof(tr));
        tr.tx_buf = reinterpret_cast<__u64>(writeBuffer.getData());
        tr.rx_buf = reinterpret_cast<__u64>(readBuffer.getData());
        tr.len = writeBuffer.getSize();
/*
            .speed_hz = 0,
            .delay_usecs = 0,
            .bits_per_word = 0,
            .cs_change = 0,
            .tx_nbits = 0, // on more-recent kernel versions;
            .rx_nbits = 0, // on more-recent kernel versions;
            .pad = 0
*/

        NATIVE_INT_TYPE stat = ioctl(this->m_fd, SPI_IOC_MESSAGE(1), &tr);

        if (stat < 1) {
            this->log_WARNING_HI_SPI_WriteError(this->m_device,this->m_select,stat);
        }
        this->m_bytes += readBuffer.getSize();
        this->tlmWrite_SPI_Bytes(this->m_bytes);
    }

    bool LinuxSpiDriverComponentImpl::open(NATIVE_INT_TYPE device,
                                           NATIVE_INT_TYPE select,
                                           SpiFrequency clock,
                                           SpiMode spiMode) {

        this->m_device = device;
        this->m_select = select;
        NATIVE_INT_TYPE fd;
        NATIVE_INT_TYPE ret;

        // Open:
        Fw::FileNameString devString;
        Fw::FormatStatus formatStatus = devString.format("/dev/spidev%d.%d", device, select);
        FW_ASSERT(formatStatus == Fw::FormatStatus::SUCCESS);

        fd = ::open(devString.toChar(), O_RDWR);
        if (fd == -1) {
            this->log_WARNING_HI_SPI_OpenError(device,select,fd);
            return false;
        }

        this->m_fd = fd;

        // Configure:
        /*
         * SPI Mode 0, 1, 2, 3
         */

        U8 mode; // Mode Select (CPOL = 0/1, CPHA = 0/1)
        switch(spiMode) {
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
                //Assert if the device SPI Mode is not in the correct range
                FW_ASSERT(0, spiMode);                
                break;
        }

        ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
        if (ret == -1) {
            this->log_WARNING_HI_SPI_ConfigError(device,select,ret);
            return false;
        }

        ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
        if (ret == -1) {
            this->log_WARNING_HI_SPI_ConfigError(device,select,ret);
            return false;
        }

        /*
         * 8 bits per word
         */
        U8 bits = 8;
        ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
        if (ret == -1) {
            this->log_WARNING_HI_SPI_ConfigError(device,select,ret);
            return false;
        }

        ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
        if (ret == -1) {
            this->log_WARNING_HI_SPI_ConfigError(device,select,ret);
            return false;
        }

        /*
         * Max speed in Hz
         */
        ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &clock);
        if (ret == -1) {
            this->log_WARNING_HI_SPI_ConfigError(device,select,ret);
            return false;
        }

        ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &clock);
        if (ret == -1) {
           this->log_WARNING_HI_SPI_ConfigError(device,select,ret);
           return false;
        }

        return true;

    }

    LinuxSpiDriverComponentImpl::~LinuxSpiDriverComponentImpl() {
        (void) close(this->m_fd);
    }

} // end namespace Drv
