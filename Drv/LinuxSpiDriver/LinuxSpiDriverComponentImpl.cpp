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

#include <cstdint>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <cerrno>

//#define DEBUG_PRINT(x,...) printf(x,##__VA_ARGS__); fflush(stdout)
#define DEBUG_PRINT(x,...)

namespace Drv {

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    void LinuxSpiDriverComponentImpl::SpiReadWrite_handler(
            const NATIVE_INT_TYPE portNum, Fw::Buffer &writeBuffer,
            Fw::Buffer &readBuffer) {

        if (this->m_fd == -1) {
            return;
        }

        DEBUG_PRINT("Writing %d bytes to SPI\n",writeBuffer.getSize());

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
        return;

    }

    bool LinuxSpiDriverComponentImpl::open(NATIVE_INT_TYPE device,
                                           NATIVE_INT_TYPE select,
                                           SpiFrequency clock) {

        this->m_device = device;
        this->m_select = select;
        NATIVE_INT_TYPE fd;
        NATIVE_INT_TYPE ret;

        // Open:
        char devName[256];
        snprintf(devName,sizeof(devName),"/dev/spidev%d.%d",device,select);
        // null terminate
        devName[sizeof(devName)-1] = 0;
        DEBUG_PRINT("Opening SPI device %s\n",devName);

        fd = ::open(devName, O_RDWR);
        if (fd == -1) {
            DEBUG_PRINT("open SPI device %d.%d failed. %d\n",device,select,errno);
            this->log_WARNING_HI_SPI_OpenError(device,select,fd);
            return false;
        } else {
            DEBUG_PRINT("Successfully opened SPI device %s fd %d\n",devName,fd);
        }

        this->m_fd = fd;

        // Configure:
        /*
         * SPI Mode 0
         */
        U8 mode = SPI_MODE_0; // Mode 0 (CPOL = 0, CPHA = 0)
        ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
        if (ret == -1) {
            DEBUG_PRINT("ioctl SPI_IOC_WR_MODE fd %d failed. %d\n",fd,errno);
            this->log_WARNING_HI_SPI_ConfigError(device,select,ret);
            return false;
        } else {
            DEBUG_PRINT("SPI fd %d WR mode successfully configured to %d\n",fd,mode);
        }

        ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
        if (ret == -1) {
            DEBUG_PRINT("ioctl SPI_IOC_RD_MODE fd %d failed. %d\n",fd,errno);
            this->log_WARNING_HI_SPI_ConfigError(device,select,ret);
            return false;
        } else {
            DEBUG_PRINT("SPI fd %d RD mode successfully configured to %d\n",fd,mode);
        }

        /*
         * 8 bits per word
         */
        U8 bits = 8;
        ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
        if (ret == -1) {
            DEBUG_PRINT("ioctl SPI_IOC_WR_BITS_PER_WORD fd %d failed. %d\n",fd,errno);
            this->log_WARNING_HI_SPI_ConfigError(device,select,ret);
            return false;
        } else {
            DEBUG_PRINT("SPI fd %d WR bits per word successfully configured to %d\n",fd,bits);
        }

        ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
        if (ret == -1) {
            DEBUG_PRINT("ioctl SPI_IOC_RD_BITS_PER_WORD fd %d failed. %d\n",fd,errno);
            this->log_WARNING_HI_SPI_ConfigError(device,select,ret);
            return false;
        } else {
            DEBUG_PRINT("SPI fd %d RD bits per word successfully configured to %d\n",fd,bits);
        }

        /*
         * Max speed in Hz
         */
        ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &clock);
        if (ret == -1) {
            DEBUG_PRINT("ioctl SPI_IOC_WR_MAX_SPEED_HZ fd %d failed. %d\n",fd,errno);
            this->log_WARNING_HI_SPI_ConfigError(device,select,ret);
            return false;
        } else {
            DEBUG_PRINT("SPI fd %d WR freq successfully configured to %d\n",fd,clock);
        }

        ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &clock);
        if (ret == -1) {
           DEBUG_PRINT("ioctl SPI_IOC_RD_MAX_SPEED_HZ fd %d failed. %d\n",fd,errno);
           this->log_WARNING_HI_SPI_ConfigError(device,select,ret);
           return false;
        } else {
           DEBUG_PRINT("SPI fd %d RD freq successfully configured to %d\n",fd,clock);
        }

        return true;

    }

    LinuxSpiDriverComponentImpl::~LinuxSpiDriverComponentImpl() {
        DEBUG_PRINT("Closing SPI device %d\n",this->m_fd);
        (void) close(this->m_fd);
    }

} // end namespace Drv
