#ifndef DRV_BIU_RB_INT_SIG_RECEIVE_PORT_HPP
#define DRV_BIU_RB_INT_SIG_RECEIVE_PORT_HPP

#include <Fw/Port/FwInputPortBase.hpp>
#include <Fw/Port/FwOutputPortBase.hpp>
#include <Fw/Comp/FwCompBase.hpp>
#include <FpConfig.hpp>

namespace Drv {

    typedef enum {
            REAL_TIME_INTERRUPT
    } TimingSignal ;

    class InputTimingSignalPort : public Fw::InputPortBase {
      public:
        typedef void (*CompFuncPtr)(Fw::ComponentBase* callComp, NATIVE_INT_TYPE portNum, TimingSignal signal);

        InputTimingSignalPort();
        void init();
        void addCallComp(Fw::ComponentBase* callComp, CompFuncPtr funcPtr);
        void invoke(TimingSignal signal);
      protected:
      private:
        CompFuncPtr m_func;
#if FW_PORT_SERIALIZATION
        void invokeSerial(Fw::SerializeBufferBase &buffer);
#endif
    };

    class OutputTimingSignalPort : public Fw::OutputPortBase {
      public:
        OutputTimingSignalPort();
        void init();
        void addCallPort(Drv::InputTimingSignalPort* callPort);
        void invoke(TimingSignal signal);
      protected:
      private:
        Drv::InputTimingSignalPort* m_port;
    };

}
#endif
