#ifndef DRV_BLOCK_DRIVER_IMPL_HPP
#define DRV_BLOCK_DRIVER_IMPL_HPP

#include <Drv/BlockDriver/BlockDriverComponentAc.hpp>

namespace Drv {

    class BlockDriverImpl : public BlockDriverComponentBase  {

    public:

        // Only called by derived class
        BlockDriverImpl(const char* compName);

        void init(NATIVE_INT_TYPE queueDepth, NATIVE_INT_TYPE instanceId = 0);
        ~BlockDriverImpl();
        // a little hack to get the reference running
        void callIsr();

    private:

        // downcalls for input ports
        void InterruptReport_internalInterfaceHandler(U32 ip);
        void BufferIn_handler(NATIVE_INT_TYPE portNum, Drv::DataBuffer& buffer);
        void Sched_handler(NATIVE_INT_TYPE portNum, U32 context);
        //! Handler implementation for PingIn
        //!
        void PingIn_handler(
            const NATIVE_INT_TYPE portNum, /*!< The port number*/
            U32 key /*!< Value to return to pinger*/
        );

        // static ISR callback
        static void s_driverISR(void* arg);

        // cycle count
        U32 m_cycles;

    };
}

#endif
