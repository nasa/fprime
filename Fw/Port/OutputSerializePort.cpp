#include <Fw/Port/OutputSerializePort.hpp>
#include <Fw/Types/Assert.hpp>
#include <stdio.h>

#if FW_PORT_SERIALIZATION

namespace Fw {

    // SerializePort has no call interface. It is to pass through serialized data

    OutputSerializePort::OutputSerializePort() : OutputPortBase() {
    }

    OutputSerializePort::~OutputSerializePort() {
    }

    void OutputSerializePort::init() {
        OutputPortBase::init();
    }

#if FW_OBJECT_TO_STRING == 1
    void OutputSerializePort::toString(char* buffer, NATIVE_INT_TYPE size) {
#if FW_OBJECT_NAMES == 1
        (void)snprintf(buffer, size, "Output Serial Port: %s %s->(%s)", this->m_objName, this->isConnected() ? "C" : "NC",
                        this->isConnected() ? this->m_connObj->getObjName() : "None");
        buffer[size-1] = 0;
#else
        (void)snprintf(buffer,size,"%s","OutputSerializePort");
#endif
    }
#endif


}

#endif // FW_PORT_SERIALIZATION
