#include <FpConfig.hpp>
#include <Fw/Port/OutputPortBase.hpp>
#include <Os/Console.hpp>
#include <cstdio>
#include <Fw/Types/Assert.hpp>


namespace Fw {

    OutputPortBase::OutputPortBase() : PortBase()
#if FW_PORT_SERIALIZATION == 1
        ,m_serPort(nullptr)
#endif
    {

    }

    OutputPortBase::~OutputPortBase() {
    }

    void OutputPortBase::init() {
        PortBase::init();
    }
#if FW_PORT_SERIALIZATION == 1
    void OutputPortBase::registerSerialPort(InputPortBase* port) {
        FW_ASSERT(port);
        this->m_connObj = port;
        this->m_serPort = port;
    }

    SerializeStatus OutputPortBase::invokeSerial(SerializeBufferBase &buffer) {
        FW_ASSERT(this->m_serPort);
        return this->m_serPort->invokeSerial(buffer);
    }
#endif

#if FW_OBJECT_TO_STRING == 1
    const char* OutputPortBase::getToStringFormatString() {
        return "Output Port: %s %s->(%s)";
    }
#endif


}

