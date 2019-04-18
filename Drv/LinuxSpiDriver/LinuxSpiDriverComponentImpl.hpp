// ======================================================================
// \title  LinuxSpiDriverImpl.hpp
// \author tcanham
// \brief  hpp file for LinuxSpiDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef LinuxSpiDriver_HPP
#define LinuxSpiDriver_HPP

#include "Drv/LinuxSpiDriver/LinuxSpiDriverComponentAc.hpp"

namespace Drv {

    /**
     * This was taken from the dspal_tester example
     *
     * Supported SPI frequency to talk to MPU9x50 slave device
     * MPU9x50 SPI interface supports upto 20MHz frequency. However 20MHz is not
     * reliable in our test and corrupted data is observed.
     */
    enum SpiFrequency
    {
       SPI_FREQUENCY_1MHZ = 1000000UL,
       SPI_FREQUENCY_5MHZ = 5000000UL,
       SPI_FREQUENCY_10MHZ = 10000000UL,
       SPI_FREQUENCY_15MHZ = 15000000UL,
       SPI_FREQUENCY_20MHZ = 20000000UL,
    };

    class LinuxSpiDriverComponentImpl: public LinuxSpiDriverComponentBase {

        public:

            // ----------------------------------------------------------------------
            // Construction, initialization, and destruction
            // ----------------------------------------------------------------------

            //! Construct object LinuxSpiDriver
            //!
            LinuxSpiDriverComponentImpl(
#if FW_OBJECT_NAMES == 1
                    const char * const compName /*!< The component name*/
#else
                    void
#endif
                    );

            //! Initialize object LinuxSpiDriver
            //!
            void init(const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
            );

            //! Destroy object LinuxSpiDriver
            //!
            ~LinuxSpiDriverComponentImpl(void);

            //! Open device
            bool open(NATIVE_INT_TYPE device,
                      NATIVE_INT_TYPE select,
                      SpiFrequency clock);

        PRIVATE:

            // ----------------------------------------------------------------------
            // Handler implementations for user-defined typed input ports
            // ----------------------------------------------------------------------

            //! Handler implementation for SpiReadWrite
            //!
            void SpiReadWrite_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
            Fw::Buffer &WriteBuffer, Fw::Buffer &readBuffer);

            NATIVE_INT_TYPE m_fd;
            NATIVE_INT_TYPE m_device;
            NATIVE_INT_TYPE m_select;
            U32 m_bytes;

    };

} // end namespace Drv

#endif
