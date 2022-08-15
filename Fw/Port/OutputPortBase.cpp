#include <FpConfig.hpp>
#include <Fw/Port/OutputPortBase.hpp>
#include <Os/Log.hpp>
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
    void OutputPortBase::toString(char* buffer, NATIVE_INT_TYPE size) {
#if FW_OBJECT_NAMES == 1
        FW_ASSERT(size > 0);
        if (snprintf(buffer, size, "OutputPort: %s %s->(%s)", this->m_objName, this->isConnected() ? "C" : "NC",
                     this->isConnected() ? this->m_connObj->getObjName() : "None") < 0) {
            buffer[0] = 0;
        }
#else
        (void)snprintf(buffer,size,"%s","OutputPort");
#endif

    }
#endif


}

