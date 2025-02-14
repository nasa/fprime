#include <Fw/Cfg/FwConfig.hpp>
#include <Fw/Types/FwAssert.hpp>
#include <Fw/Types/FwSerializable.hpp>

#include <Ports/Msg1PortAc.hpp>

namespace Ports {

    namespace {

        class Msg1PortBuffer : public Fw::SerializeBufferBase {

            public:
                I32 getBuffCapacity() const {
                    return sizeof(m_buff);
                }

                U8* getBuffAddr() {
                    return m_buff;
                }

                const U8* getBuffAddr() const {
                    return m_buff;
                }

        private:

            U8 m_buff[InputMsg1Port::SERIALIZED_SIZE];

        };

    }
    InputMsg1Port::InputMsg1Port() :
        Fw::InputPortBase(),
        m_func(0) {
    }

    void InputMsg1Port::init() {
        Fw::InputPortBase::init();
    }

    void InputMsg1Port::addCallComp(Fw::ComponentBase* callComp, CompFuncPtr funcPtr) {
        FW_ASSERT(callComp);
        FW_ASSERT(funcPtr);

        this->m_comp = callComp;
        this->m_func = funcPtr;
        this->m_connObj = callComp;
    }

    // call virtual logging function for component
    void InputMsg1Port::invoke(U32 arg1, U32 *arg2, U32 &arg3) {

#if FW_PORT_TRACING == 1
        this->trace();
#endif
        FW_ASSERT(this->m_comp);
        FW_ASSERT(this->m_func);
        this->m_func(this->m_comp, this->m_portNum, arg1, arg2, arg3);

    }

#if FW_PORT_SERIALIZATION == 1
    void InputMsg1Port::invokeSerial(Fw::SerializeBufferBase &buffer) {
        Fw::SerializeStatus status;
#if FW_PORT_TRACING == 1
        this->trace();
#endif
        FW_ASSERT(this->m_comp);
        FW_ASSERT(this->m_func);

        U32 arg3;
        status = buffer.deserialize(arg3);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<FwAssertArgType>(status));
        U32 *arg2;
        status = buffer.deserialize(static_cast<void *>(arg2));
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<FwAssertArgType>(status));
        U32 arg1;
        status = buffer.deserialize(arg1);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<FwAssertArgType>(status));

        this->m_func(this->m_comp, this->m_portNum, arg1, arg2, arg3);
    }
#endif

OutputMsg1Port::OutputMsg1Port() :
            Fw::OutputPortBase(),
    m_port(0) {
}

void OutputMsg1Port::init() {
    Fw::OutputPortBase::init();
}

void OutputMsg1Port::addCallPort(InputMsg1Port* callPort) {
    FW_ASSERT(callPort);

    this->m_port = callPort;
    this->m_connObj = callPort;
#if FW_PORT_SERIALIZATION == 1
    this->m_serPort = 0;
#endif
}

void OutputMsg1Port::invoke(U32 arg1, U32 *arg2, U32 &arg3) {
#if FW_PORT_TRACING == 1
    this->trace();
#endif

    if (this->m_port) {
        this->m_port->invoke(arg1, arg2, arg3);
#if FW_PORT_SERIALIZATION
    } else if (this->m_serPort) {
        Fw::SerializeStatus status;
        Msg1PortBuffer _buffer;
        status = _buffer.serialize(arg1);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<FwAssertArgType>(status));

        status = _buffer.serialize(static_cast<void *>(arg2));
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<FwAssertArgType>(status));

        status = _buffer.serialize(arg3);
        FW_ASSERT(Fw::FW_SERIALIZE_OK == status,static_cast<FwAssertArgType>(status));

        this->m_serPort->invokeSerial(_buffer);
#endif
    } else {
        FW_ASSERT(0);
    }
}

};

