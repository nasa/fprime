#include <Fw/Types/FwAssert.hpp>
#include <Drv/BlockDriver/DrvTimingSignalPort.hpp>
#include <stdio.h>

extern I32 debug_flag;


namespace Drv {

    namespace {

        class TimingSignalPortBuffer : public Fw::SerializeBufferBase {

            public:
                NATIVE_INT_TYPE getBuffCapacity(void) const {
                    return sizeof(m_buff);
                }

                U8* getBuffAddr(void) {
                    return m_buff;
                }

                const U8* getBuffAddr(void) const {
                    return m_buff;
                }

            private:

                U8 m_buff[sizeof(I32)];

        };

    }

    InputTimingSignalPort::InputTimingSignalPort(void) :
            m_func(0) {
    }

    void InputTimingSignalPort::init(void) {
        Fw::InputPortBase::init();
    }

    void InputTimingSignalPort::addCallComp(Fw::ComponentBase* callComp, CompFuncPtr funcPtr) {

        FW_ASSERT(callComp);
        FW_ASSERT(funcPtr);

        this->m_comp = callComp;
        this->m_func = funcPtr;
        this->m_connObj = callComp;
    }

    void InputTimingSignalPort::invoke(TimingSignal signal) {

#if FW_PORT_TRACING
        this->trace();
#endif

        FW_ASSERT(this->m_comp);
        FW_ASSERT(this->m_func);

        this->m_func(this->m_comp, this->m_portNum, signal);
    }

#if FW_PORT_SERIALIZATION
    void InputTimingSignalPort::invokeSerial(Fw::SerializeBufferBase &buffer) {

#if FW_PORT_TRACING
        this->trace();
#endif

        I32 val;
        Fw::SerializeStatus status = buffer.deserialize(val);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status);
        TimingSignal signal = static_cast<TimingSignal>(val);

        FW_ASSERT(this->m_comp);
        FW_ASSERT(this->m_func);

        this->m_func(this->m_comp, this->m_portNum, signal);
    }
#endif

    OutputTimingSignalPort::OutputTimingSignalPort(void) :
            m_port(0) {
    }

    void OutputTimingSignalPort::init(void) {
        Fw::OutputPortBase::init();
    }

    void OutputTimingSignalPort::addCallPort(InputTimingSignalPort* callPort) {
        FW_ASSERT(callPort);

        this->m_port = callPort;
        this->m_connObj = callPort;
    }

    void OutputTimingSignalPort::invoke(TimingSignal signal) {

#if FW_PORT_TRACING == 1
        this->trace();
#endif

        if (this->m_port) {
            this->m_port->invoke(signal);
#if FW_PORT_SERIALIZATION
        } else if (this->m_serPort) {
            TimingSignalPortBuffer buffer;
            Fw::SerializeStatus status = buffer.serialize((I32)signal);
            FW_ASSERT(Fw::FW_SERIALIZE_OK == status);
            this->m_serPort->invokeSerial(buffer);
#endif
        } else {
            FW_ASSERT(0);
        }
    }

}
